#include "core/input.h"

#include <emscripten/html5.h>
#include <emscripten/emscripten.h>

#include <algorithm>  
#include <iostream>



bool canvasFocused = false;


EM_BOOL on_mouse_down(int eventType, const EmscriptenMouseEvent* e, void* userData) {
    if (!canvasFocused) {
        canvasFocused = true;
        std::cout << "[focus] Focusing canvas after user click\n";
        emscripten_run_script("document.getElementById('canvas').focus();");
    }
    return EM_TRUE;
}

EM_BOOL key_down(int, const EmscriptenKeyboardEvent* e, void*) {
    int k = (int)e->keyCode;
    if (k >= 0 && k < 256) {
        InputHandler::keyStates[k] = 1;
    }
    return EM_TRUE;
}

EM_BOOL key_up(int, const EmscriptenKeyboardEvent* e, void*) {
    int k = (int)e->keyCode;
    if (k >= 0 && k < 256) {
        InputHandler::keyStates[k] = 0;
        InputHandler::GetInstance()->inputDispatcher->Dispatch(
            std::make_shared<KeyReleasedEvent>(static_cast<KeyCode>(k)));
    }
    return EM_TRUE;
}

EM_BOOL on_focus(int eventType, const EmscriptenFocusEvent* e, void* userData) {
    std::cout << "[focus] Canvas gained focus\n";
    canvasFocused = true;
    return EM_TRUE;
}

EM_BOOL on_blur(int eventType, const EmscriptenFocusEvent* e, void* userData) {
    std::cout << "[focus] Canvas lost focus\n";
    canvasFocused = false;
	InputHandler::clearKeyStates(); // Clear key states on blur
    return EM_TRUE;
}

EM_BOOL on_document_keydown(int eventType, const EmscriptenKeyboardEvent* e, void* userData) {
    if (!canvasFocused) {
        std::cout << "[keydown] Detected key while canvas is not focused. Refocusing canvas...\n";
        emscripten_run_script("document.getElementById('canvas').focus();");
    }
    return EM_TRUE;  // Let the event propagate to the canvas if it gains focus
}


void initInputHandlers(WindowHandle /*unused*/) {
    std::cout << "[initInputHandlers] Setting up input...\n";

    // Ensure canvas is focusable
    emscripten_run_script("document.getElementById('canvas').setAttribute('tabindex', '0');");

    // Register event callbacks
    emscripten_set_keydown_callback("#canvas", nullptr, EM_TRUE, key_down);
    emscripten_set_keyup_callback("#canvas", nullptr, EM_TRUE, key_up);

    emscripten_set_focus_callback("#canvas", nullptr, EM_TRUE, on_focus);
    emscripten_set_blur_callback("#canvas", nullptr, EM_TRUE, on_blur);

    emscripten_set_mousedown_callback("#canvas", nullptr, EM_TRUE, on_mouse_down);

    emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, EM_TRUE, on_document_keydown);

}

