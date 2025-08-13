// input.h
#pragma once
#include <cstdint>
#include "core/singleton.h"
#include <core/Events.h>

using WindowHandle = void*;



class InputHandler : public Singleton<InputHandler> {
    friend class Singleton<InputHandler>;
public:
    static unsigned int keyStates[256];
    static EventDispatcher* inputDispatcher;

    InputHandler() {
        if (!inputDispatcher) inputDispatcher = new EventDispatcher();
    }
    ~InputHandler() {
        delete inputDispatcher;
        inputDispatcher = nullptr;
    }

    // Engine-agnostic API
    static void setKeyState(int jsKeyCode, KeyAction action);
    static void clearKeyStates();
    static void fireHeldPressed();
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

// Engine-agnostic wiring points
namespace Input {
    void SetKeyCallback(KeyCallback cb);          // Engine registers its key sink (usually a small shim)
    void SetMouseCallback(MouseCallback cb);
    void InstallBackendKeyHook(WindowHandle win); // Platform hooks into the window
         // Initialize input handlers, called by the engine
}