
#include "net_client.h"
#include <emscripten/websocket.h>
#include <string>

namespace {
    struct Impl { EMSCRIPTEN_WEBSOCKET_T sock = 0; } g;
}

static EM_BOOL on_open(int, const EmscriptenWebSocketOpenEvent*, void* u) {
    auto* self = static_cast<WsClient*>(u);
    if (self->onOpen) self->onOpen();
    return EM_TRUE;
}
static EM_BOOL on_close(int, const EmscriptenWebSocketCloseEvent*, void* u) {
    auto* self = static_cast<WsClient*>(u);
    if (self->onClose) self->onClose();
    return EM_TRUE;
}
static EM_BOOL on_error(int, const EmscriptenWebSocketErrorEvent*, void* u) {
    auto* self = static_cast<WsClient*>(u);
    if (self->onError) self->onError("WebSocket error");
    return EM_TRUE;
}
static EM_BOOL on_message(int, const EmscriptenWebSocketMessageEvent* e, void* u) {
    if (!e->isText) return EM_TRUE;
    auto* self = static_cast<WsClient*>(u);
    std::string s(reinterpret_cast<const char*>(e->data), static_cast<size_t>(e->numBytes));
    if (self->onMessage) self->onMessage(s);
    return EM_TRUE;
}

bool WsClient::connect(const std::string& url) {
    if (!emscripten_websocket_is_supported()) {
        if (onError) onError("WebSocket not supported");
        return false;
    }
    EmscriptenWebSocketCreateAttributes a{};
    a.url = url.c_str();
    a.protocols = nullptr;
    a.createOnMainThread = EM_TRUE;

    g.sock = emscripten_websocket_new(&a);
    if (g.sock <= 0) {
        if (onError) onError("Failed to create socket");
        return false;
    }

    emscripten_websocket_set_onopen_callback(g.sock, this, on_open);
    emscripten_websocket_set_onmessage_callback(g.sock, this, on_message);
    emscripten_websocket_set_onclose_callback(g.sock, this, on_close);
    emscripten_websocket_set_onerror_callback(g.sock, this, on_error);
    return true;
}

void WsClient::send(const std::string& text) {
    if (g.sock) emscripten_websocket_send_utf8_text(g.sock, text.c_str());
}

void WsClient::close() {
    if (g.sock) {
        emscripten_websocket_close(g.sock, 1000, "bye");
        emscripten_websocket_delete(g.sock);
        g.sock = 0;
    }
}

void WsClient::poll() {/* browser drives callbacks */ }
