#include "core/input.h"

#include <emscripten/html5.h>
#include <emscripten/emscripten.h>

#include <iostream>

EventDispatcher* InputHandler::inputDispatcher = new EventDispatcher();
unsigned int InputHandler::keyStates[256] = {0};

EM_BOOL on_key_down(int eventType, const EmscriptenKeyboardEvent* e, void* userData) {

    auto* dispatcher = InputHandler::GetInstance()->inputDispatcher;

    if (e->keyCode == 37) InputHandler::GetInstance()->inputDispatcher->Dispatch(std::make_shared<KeyPressedEvent>(KeyCode::Left));
    if (e->keyCode == 39) InputHandler::GetInstance()->inputDispatcher->Dispatch(std::make_shared<KeyPressedEvent>(KeyCode::Right));
    if (e->keyCode == 38) InputHandler::GetInstance()->inputDispatcher->Dispatch(std::make_shared<KeyPressedEvent>(KeyCode::Up));
    if (e->keyCode == 40) InputHandler::GetInstance()->inputDispatcher->Dispatch(std::make_shared<KeyPressedEvent>(KeyCode::Down));

    return EM_TRUE;
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

EM_BOOL key_down(int eventType, const EmscriptenKeyboardEvent* e, void* userData) {
    InputHandler::keyStates[e->keyCode] = 1;
    return EM_TRUE;
}

EM_BOOL key_up(int eventType, const EmscriptenKeyboardEvent* e, void* userData) {
    InputHandler::keyStates[e->keyCode] = 0;
    return EM_TRUE;
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

    emscripten_set_mousedown_callback("#canvas", nullptr, EM_TRUE, on_mouse_down);
}

void shutdownInputHandlers() {
    // Optional: deregister callbacks if needed
}

