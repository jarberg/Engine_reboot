
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include "engine/core/GL.h" 
#include "core/input.h"
#include <atomic>

// ---- Bridge state ----
static std::atomic<KeyCallback> g_cb{ nullptr };
static std::atomic<MouseCallback> m_cb{ nullptr };
static std::atomic<MouseMoveCallback> mm_cb{ nullptr };

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
	case GLFW_KEY_LEFT_CONTROL:     return 17;  // Ctrl
	case GLFW_KEY_LEFT_ALT:         return 18;  // Alt
    default:                        return key; // letters/numbers align with JS keyCode
    }
}

static inline KeyAction GLFWActionToKA(int a) {
    if (a == GLFW_PRESS)    return KeyAction::Press;
    if (a == GLFW_REPEAT)   return KeyAction::Repeat;
    if (a == GLFW_RELEASE)  return KeyAction::Release;
    return KeyAction::Release;
}

static void GLFW_KeyTrampoline(GLFWwindow* w, int key, int scancode, int action, int mods) {
    auto cb = g_cb.load();
    if (!cb) return;
    cb(static_cast<WindowHandle>(w),
        GLFWToJS(key),
        scancode,
        GLFWActionToKA(action),
        static_cast<std::uint16_t>(mods));
}

static void GLFW_MouseTrampoline(GLFWwindow* w, int button, int action, int mods) {
    auto cb = m_cb.load();
    if (!cb) return;
    cb(static_cast<WindowHandle>(w),
        GLFWToJS(button),
        GLFWActionToKA(action),
        static_cast<std::uint16_t>(mods));
}

static void GLFW_MouseMoveTrampoline(GLFWwindow* w, double xpos, double ypos) {
    auto cb = mm_cb.load();
    if (!cb) return;
    cb(static_cast<WindowHandle>(w),
        xpos,
        ypos);
}

void initInputHandlers(WindowHandle window) {
    auto* gw = reinterpret_cast<GLFWwindow*>(window);

    Input::SetKeyCallback([](WindowHandle, int jsKey, int /*scancode*/, KeyAction act, std::uint16_t /*mods*/) {
        InputHandler::setKeyState(jsKey, act);
        });
    Input::SetMouseCallback([](WindowHandle, int jsKey, KeyAction act, std::uint16_t /*mods*/) {
        InputHandler::setKeyState(jsKey, act);
        });
    Input::SetCursorCallback([](WindowHandle, double xPos, double yPos) {
        InputHandler::MouseMoved(xPos, yPos);
        });
    Input::InstallBackendKeyHook(window);
}

namespace Input {
    void SetKeyCallback(KeyCallback cb) { g_cb.store(cb); }
    void SetMouseCallback(MouseCallback cb) { m_cb.store(cb); }
    void SetCursorCallback(MouseMoveCallback cb) { mm_cb.store(cb); }

    void InstallBackendKeyHook(WindowHandle window) {
        auto* gw = reinterpret_cast<GLFWwindow*>(window);
        glfwSetKeyCallback(gw, GLFW_KeyTrampoline);
        glfwSetMouseButtonCallback(gw, GLFW_MouseTrampoline);
		glfwSetCursorPosCallback(gw, GLFW_MouseMoveTrampoline);
    }

    void getWindowSize(int* width, int* height) {

        auto* window = glfwGetCurrentContext();
        if (!window || !width || !height) return;
        glfwGetWindowSize(window, width, height);
    }

    void getMousePosition( int *xPos, int *yPos){
        auto* window = glfwGetCurrentContext();
        if (!window) return;
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
		*xPos = static_cast<int>(xpos);
		*yPos = static_cast<int>(ypos);
    }
}
