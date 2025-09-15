// input.h
#pragma once
#include <cstdint>
#include "core/singleton.h"
#include <core/events.h>

#include <core/input/input_types.h>

using WindowHandle = void*;


class InputHandler : public Singleton<InputHandler> {
    friend class Singleton<InputHandler>;
public:
    static unsigned int keyStates[256];
    static EventDispatcher* inputDispatcher;
    static int lastX;
    static int lastY;
    static int currentX;
    static int currentY;
    static double cursorDeltaX;
    static double cursorDeltaY;

	static int cursorKeyHeldCode;

    InputHandler() {
        if (!inputDispatcher) inputDispatcher = new EventDispatcher();
        cursorKeyHeldCode = -1;
    }
    ~InputHandler() {
        delete inputDispatcher;
        inputDispatcher = nullptr;
    }

    // Engine-agnostic API
    static void setKeyState(int jsKeyCode, KeyAction action);
    static void clearKeyStates();
    static void fireHeldPressed();
    static void cursormoveEvent(int x, int y);
};

void initInputHandlers(WindowHandle window);

// Engine-agnostic callback signature (no GLFW here)
using KeyCallback = void(*)(WindowHandle window,
    int jsKeyCode,
    int scancode,
    KeyAction action,
    std::uint16_t mods);

using MouseCallback = void(*)(WindowHandle window,
    int jsKeyCode,
    KeyAction action,
    std::uint16_t mods);

using MouseMoveCallback = void(*)(WindowHandle window,
    double xPos,
    double yPos);

// Engine-agnostic wiring points
namespace Input {
    void SetKeyCallback(KeyCallback cb);          // Engine registers its key sink (usually a small shim)
    void SetMouseCallback(MouseCallback cb);
	void SetCursorCallback(MouseMoveCallback cb);
    void InstallBackendKeyHook(WindowHandle win); // Platform hooks into the window
    void getMousePosition(int*xPos, int *yPos);
    void getWindowSize(int* width, int* height);
         // Initialize input handlers, called by the engine
}