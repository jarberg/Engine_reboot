#include "core/input.h"

#include <emscripten/html5.h>
#include <emscripten/emscripten.h>

#include <iostream>

EM_BOOL on_key_down(int eventType, const EmscriptenKeyboardEvent* e, void* userData) {
    std::string code(e->code);  // physical key code

    if (code == "ArrowUp") {
        std::cout << "Up arrow pressed\n";
    }
    else if (code == "ArrowDown") {
        std::cout << "Down arrow pressed\n";
    }
    else if (code == "ArrowLeft") {
        std::cout << "Left arrow pressed\n";
    }
    else if (code == "ArrowRight") {
        std::cout << "Right arrow pressed\n";
    }
    else {
        std::cout << "Other key: " << code << "\n";
    }

    return EM_TRUE; // prevent default browser behavior (like scrolling)
}

bool canvasFocused = false;

EM_BOOL on_mouse_down(int eventType, const EmscriptenMouseEvent* e, void* userData) {
    std::cout << "[mouse_down] Button: " << e->button << std::endl;

    if (!canvasFocused) {
        canvasFocused = true;
        std::cout << "[focus] Focusing canvas after user click\n";
        emscripten_run_script("document.getElementById('canvas').focus();");
    }

    return EM_TRUE;
}

void initInputHandlers() {
    std::cout << "[initInputHandlers] Setting up input...\n";

    // Ensure canvas is focusable
    emscripten_run_script("document.getElementById('canvas').setAttribute('tabindex', '0');");

    // Register event callbacks
    emscripten_set_keydown_callback("#canvas", nullptr, EM_TRUE, on_key_down);
    emscripten_set_mousedown_callback("#canvas", nullptr, EM_TRUE, on_mouse_down);
}

void shutdownInputHandlers() {
    // Optional: deregister callbacks if needed
}