#include "core/input.h"

#include <emscripten/html5.h>
#include <emscripten/emscripten.h>

#include <iostream>

EventDispatcher* InputHandler::inputDispatcher = new EventDispatcher();


EM_BOOL on_key_down(int eventType, const EmscriptenKeyboardEvent* e, void* userData) {

    auto* dispatcher = InputHandler::GetInstance()->inputDispatcher;

    switch (e->keyCode) {
	    case 38: dispatcher->Dispatch(std::make_shared<KeyPressedEvent>(KeyCode::Up));
            std::cout << "Up arrow pressed\n";
		    break;
        case 40: dispatcher->Dispatch(std::make_shared<KeyPressedEvent>(KeyCode::Down));
		    std::cout << "Down arrow pressed\n";
            break;
	    case 37: dispatcher->Dispatch(std::make_shared<KeyPressedEvent>(KeyCode::Left));
		    std::cout << "Left arrow pressed\n";
		    break;
	    case 39: dispatcher->Dispatch(std::make_shared<KeyPressedEvent>(KeyCode::Right));
		    std::cout << "Right arrow pressed\n";
		    break;
        }
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

