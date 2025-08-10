// input_windows_glfw.cpp  (Windows + GLFW backend)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include "engine/core/GL.h"   // brings in <GLFW/glfw3.h> in your project
#include "core/input.h"
#include <atomic>

// ---- Mapping: GLFW key -> JS keyCode (extend as needed) ----
static int GLFWToJS(int key) {
    switch (key) {
    case GLFW_KEY_LEFT:             return 37;
    case GLFW_KEY_UP:               return 38;
    case GLFW_KEY_RIGHT:            return 39;
    case GLFW_KEY_DOWN:             return 40;
    case GLFW_KEY_ESCAPE:           return 27;
    case GLFW_KEY_ENTER:            return 13;
    case GLFW_KEY_TAB:              return 9;
    case GLFW_KEY_BACKSPACE:        return 8;
    case GLFW_KEY_DELETE:           return 46;
    case GLFW_KEY_SPACE:            return 32;
	case GLFW_KEY_LEFT_CONTROL:     return 17; // Ctrl
	case GLFW_KEY_LEFT_ALT:         return 18; // Alt
    default:                 return key; // letters/numbers align with JS keyCode
    }
}

static inline KeyAction GLFWActionToKA(int a) {
    if (a == GLFW_PRESS)  return KeyAction::Press;
    if (a == GLFW_REPEAT) return KeyAction::Repeat;
    return KeyAction::Release;
}

// ---- Bridge state ----
static std::atomic<KeyCallback> g_cb{ nullptr };

// GLFW -> engine-agnostic callback
static void GLFW_KeyTrampoline(GLFWwindow* w, int key, int scancode, int action, int mods) {
    auto cb = g_cb.load();
    if (!cb) return;
    cb(static_cast<WindowHandle>(w),
        GLFWToJS(key),
        scancode,
        GLFWActionToKA(action),
        static_cast<std::uint16_t>(mods));
}

void initInputHandlers(WindowHandle window) {
    auto* gw = reinterpret_cast<GLFWwindow*>(window);

    Input::SetKeyCallback([](WindowHandle, int jsKey, int /*scancode*/, KeyAction act, std::uint16_t /*mods*/) {
        InputHandler::setKeyState(jsKey, act);
        });

    Input::InstallBackendKeyHook(window); // this already casts inside to GLFWwindow*
    // (If your InstallBackendKeyHook needs it, you can pass 'gw' after casting.)
}

// Wire into GLFW for this window
namespace Input {
    void SetKeyCallback(KeyCallback cb) { g_cb.store(cb); }
    void InstallBackendKeyHook(WindowHandle window) {
        auto* gw = reinterpret_cast<GLFWwindow*>(window);
        glfwSetKeyCallback(gw, GLFW_KeyTrampoline);
    }
}
