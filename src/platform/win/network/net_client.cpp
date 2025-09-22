// ws_client_win.cpp
#include "net_client.h"

#include <windows.h>
#include <winhttp.h>
#include <string>
#include <thread>
#include <atomic>
#include <vector>
#include <cstdio>
#include <mutex>

#pragma comment(lib, "winhttp.lib")

namespace {
   
    struct Impl {
        HINTERNET hSession = nullptr;
        HINTERNET hConnect = nullptr;
        HINTERNET hRequest = nullptr;   // only during upgrade
        HINTERNET hWebSock = nullptr;   // WebSocket handle after upgrade
        std::thread rxThread;
        std::atomic<bool> running{ false };
        std::mutex sendMtx;             // WinHTTP WS is thread-safe, but we’ll serialize sends anyway
    } g;

    struct UrlParts {
        bool secure = false;     // wss => true
        std::wstring host;
        INTERNET_PORT port = 0;
        std::wstring path;       // must begin with '/'
    };

    // Minimal parser for ws:// / wss:// URLs
    UrlParts parse_ws_url(const std::string& urlUtf8) {
        UrlParts out{};
        // quick & dirty UTF-8 -> UTF-16
        int wlen = MultiByteToWideChar(CP_UTF8, 0, urlUtf8.c_str(), -1, nullptr, 0);
        std::wstring wurl(wlen ? wlen - 1 : 0, L'\0');
        if (wlen) MultiByteToWideChar(CP_UTF8, 0, urlUtf8.c_str(), -1, wurl.data(), wlen);

        // scheme
        std::wstring_view u{ wurl };
        const std::wstring ws = L"ws://";
        const std::wstring wss = L"wss://";
        if (u.rfind(ws, 0) == 0) {
            out.secure = false;
            u.remove_prefix(ws.size());
        }
        else if (u.rfind(wss, 0) == 0) {
            out.secure = true;
            u.remove_prefix(wss.size());
        }
        else if (u.rfind(L"http://", 0) == 0) { // allow http(s) as convenience
            out.secure = false;
            u.remove_prefix(7);
        }
        else if (u.rfind(L"https://", 0) == 0) {
            out.secure = true;
            u.remove_prefix(8);
        }
        else {
            // fallback: assume host without scheme
        }

        // split host[:port]/path
        size_t slash = u.find(L'/');
        std::wstring_view hostport = u.substr(0, slash);
        out.path = (slash == std::wstring_view::npos) ? L"/" : std::wstring(u.substr(slash));

        size_t colon = hostport.find(L':');
        if (colon == std::wstring_view::npos) {
            out.host = std::wstring(hostport);
            out.port = out.secure ? 443 : 80;
        }
        else {
            out.host = std::wstring(hostport.substr(0, colon));
            out.port = static_cast<INTERNET_PORT>(std::stoi(std::wstring(hostport.substr(colon + 1))));
        }
        if (out.path.empty() || out.path.front() != L'/') out.path.insert(out.path.begin(), L'/');
        return out;
    }

    // Convert UTF-16 to UTF-8
    std::string to_utf8(std::wstring_view w) {
        if (w.empty()) return {};
        int len = WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(), nullptr, 0, nullptr, nullptr);
        std::string out(len, '\0');
        WideCharToMultiByte(CP_UTF8, 0, w.data(), (int)w.size(), out.data(), len, nullptr, nullptr);
        return out;
    }
}

bool WsClient::connect(const std::string& url)
{
    // Cleanup if reconnecting
    close();

    auto parts = parse_ws_url(url);

    // WinHTTP uses http(s) handles, even for WebSocket
    g.hSession = WinHttpOpen(L"WsClient/1.0",
        WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY,
        WINHTTP_NO_PROXY_NAME,
        WINHTTP_NO_PROXY_BYPASS,
        0);
    if (!g.hSession) {
        if (onError) onError("WinHttpOpen failed");
        return false;
    }

    g.hConnect = WinHttpConnect(g.hSession, parts.host.c_str(), parts.port, 0);
    if (!g.hConnect) {
        if (onError) onError("WinHttpConnect failed");
        close();
        return false;
    }

    DWORD flags = parts.secure ? WINHTTP_FLAG_SECURE : 0;
    g.hRequest = WinHttpOpenRequest(g.hConnect,
        L"GET",
        parts.path.c_str(),
        nullptr,
        WINHTTP_NO_REFERER,
        WINHTTP_DEFAULT_ACCEPT_TYPES,
        flags);
    if (!g.hRequest) {
        if (onError) onError("WinHttpOpenRequest failed");
        close();
        return false;
    }

    // Tell WinHTTP we’re upgrading this request to WebSocket
    if (!WinHttpSetOption(g.hRequest, WINHTTP_OPTION_UPGRADE_TO_WEB_SOCKET, nullptr, 0)) {
        if (onError) onError("WinHttpSetOption(UPGRADE_TO_WEB_SOCKET) failed");
        close();
        return false;
    }

    if (!WinHttpSendRequest(g.hRequest, WINHTTP_NO_ADDITIONAL_HEADERS, 0, WINHTTP_NO_REQUEST_DATA, 0, 0, 0) ||
        !WinHttpReceiveResponse(g.hRequest, nullptr)) {
        if (onError) onError("WinHttpSendRequest/ReceiveResponse failed");
        close();
        return false;
    }

    // Complete the upgrade – returns the WebSocket handle
    g.hWebSock = WinHttpWebSocketCompleteUpgrade(g.hRequest, 0);
    // After successful upgrade, the request handle must be closed; keep session/connect/websocket.
    WinHttpCloseHandle(g.hRequest);
    g.hRequest = nullptr;

    if (!g.hWebSock) {
        if (onError) onError("WinHttpWebSocketCompleteUpgrade failed");
        close();
        return false;
    }

    // Fire onOpen
    if (onOpen) onOpen();

    // Start receive loop
    g.running = true;
    g.rxThread = std::thread([this]() {
        std::vector<uint8_t> buf(8192);
        for (;;) {
            if (!g.running.load()) break;

            DWORD bytesRead = 0;
            WINHTTP_WEB_SOCKET_BUFFER_TYPE type = WINHTTP_WEB_SOCKET_BINARY_MESSAGE_BUFFER_TYPE;
            DWORD hr = WinHttpWebSocketReceive(g.hWebSock, buf.data(), (DWORD)buf.size(), &bytesRead, &type);

            if (hr != NO_ERROR) {
                if (onError) onError("WinHttpWebSocketReceive failed");
                break;
            }

            if (type == WINHTTP_WEB_SOCKET_CLOSE_BUFFER_TYPE) {
                // Server initiated close; acknowledge and exit
                WinHttpWebSocketClose(g.hWebSock, WINHTTP_WEB_SOCKET_SUCCESS_CLOSE_STATUS, nullptr, 0);
                break;
            }

            if (type == WINHTTP_WEB_SOCKET_UTF8_MESSAGE_BUFFER_TYPE ||
                type == WINHTTP_WEB_SOCKET_UTF8_FRAGMENT_BUFFER_TYPE) {
                std::string s(reinterpret_cast<char*>(buf.data()), bytesRead);
                if (onMessage) onMessage(s);
            }
            else {
                // Ignore binary; add a hook if you need it
            }
        }

        // Fire onClose after loop ends
        if (onClose) onClose();
        });

    return true;
}

void WsClient::send(const std::string& text)
{
    if (!g.hWebSock) {
        if (onError) onError("send() called with no socket");
        return;
    }
    std::lock_guard<std::mutex> lock(g.sendMtx);

    DWORD hr = WinHttpWebSocketSend(g.hWebSock,
        WINHTTP_WEB_SOCKET_UTF8_MESSAGE_BUFFER_TYPE,
        (PVOID)text.data(),
        (DWORD)text.size());
    if (hr != NO_ERROR) {
        if (onError) onError("WinHttpWebSocketSend failed");
    }
}

void WsClient::close()
{
    // Signal receive loop to stop
    g.running = false;

    if (g.hWebSock) {
        // Try a graceful close
        WinHttpWebSocketClose(g.hWebSock, WINHTTP_WEB_SOCKET_SUCCESS_CLOSE_STATUS, nullptr, 0);
		DWORD waitMs = 5; // wait up to 5s for server to close
        WinHttpWebSocketShutdown(g.hWebSock, WINHTTP_WEB_SOCKET_SUCCESS_CLOSE_STATUS, 0, waitMs);
        WinHttpCloseHandle(g.hWebSock);
        g.hWebSock = nullptr;
    }

    if (g.rxThread.joinable()) {
        // If the thread is blocked in Receive, closing the socket will unblock it
        g.rxThread.join();
    }

    if (g.hConnect) {
        WinHttpCloseHandle(g.hConnect);
        g.hConnect = nullptr;
    }
    if (g.hSession) {
        WinHttpCloseHandle(g.hSession);
        g.hSession = nullptr;
    }
}

void WsClient::poll()
{
    // Not required on Windows; background thread handles receive.
    // Keep as no-op to match your interface.
}
