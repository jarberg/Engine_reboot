#include "core/input.h"

#include <emscripten/html5.h>
#include <emscripten/emscripten.h>

#include <iostream>

EventDispatcher* InputHandler::inputDispatcher = new EventDispatcher();
unsigned int InputHandler::keyStates[256] = {0};

bool canvasFocused = false;

void InputHandler::clearKeyStates() {
    std::fill(std::begin(InputHandler::keyStates), std::end(InputHandler::keyStates), 0); // Clear the key states after processing  
}

EM_BOOL on_mouse_down(int eventType, const EmscriptenMouseEvent* e, void* userData) {
    if (!canvasFocused) {
        canvasFocused = true;
        std::cout << "[focus] Focusing canvas after user click\n";
        emscripten_run_script("document.getElementById('canvas').focus();");
    }
    return EM_TRUE;
}

EM_BOOL key_down(int eventType, const EmscriptenKeyboardEvent* e, void* userData) {
    InputHandler::keyStates[e->keyCode] = 1;
    return EM_TRUE;
}

EM_BOOL key_up(int eventType, const EmscriptenKeyboardEvent* e, void* userData) {
    InputHandler::keyStates[e->keyCode] = 0;
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

void pollInput() {

    InputHandler* Input = InputHandler::GetInstance();
    for (int i = 0; i < 256; i++) {
        if (InputHandler::keyStates[i]==1) {
            if (i == 37) Input->inputDispatcher->Dispatch(std::make_shared<KeyPressedEvent>(KeyCode::Left));
            if (i == 39) Input->inputDispatcher->Dispatch(std::make_shared<KeyPressedEvent>(KeyCode::Right));
            if (i == 38) Input->inputDispatcher->Dispatch(std::make_shared<KeyPressedEvent>(KeyCode::Up));
            if (i == 40) Input->inputDispatcher->Dispatch(std::make_shared<KeyPressedEvent>(KeyCode::Down));
        }
    }
}


void initInputHandlers() {
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

void shutdownInputHandlers() {
    // Optional: deregister callbacks if needed
}

