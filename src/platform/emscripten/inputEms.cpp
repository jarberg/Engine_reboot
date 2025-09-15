
#include "core/input.h"

#include <emscripten/html5.h>
#include <emscripten/emscripten.h>

#include <algorithm>  
#include <iostream>

bool canvasFocused = false;

EM_BOOL on_mouse_down(int eventType, const EmscriptenMouseEvent* e, void* userData) {

	//std::cout << "emscripten key inopyt :" << e->button << "\n";
    if (!canvasFocused) {
        canvasFocused = true;
        std::cout << "[focus] Focusing canvas after user click\n";
        emscripten_run_script("document.getElementById('canvas').focus();");

    }
    if (InputHandler::keyStates[e->button]) {
        InputHandler::setKeyState(e->button, KeyAction::Repeat);
    }
    else {
        InputHandler::setKeyState(e->button, KeyAction::Press);
    }

    return EM_TRUE;
}
EM_BOOL on_mouse_up(int eventType, const EmscriptenMouseEvent* e, void* userData) {
    if (!canvasFocused) {
        canvasFocused = true;
        std::cout << "[focus] Focusing canvas after user click\n";
        emscripten_run_script("document.getElementById('canvas').focus();");

    }
    InputHandler::setKeyState(e->button, KeyAction::Release);

    return EM_TRUE;
}
EM_BOOL key_down(int, const EmscriptenKeyboardEvent* e, void*) {
    if (InputHandler::keyStates[e->keyCode]) {
        InputHandler::setKeyState(e->keyCode, KeyAction::Repeat);
    }
    else{
        InputHandler::setKeyState(e->keyCode, KeyAction::Press);
    }

    return EM_TRUE;
}

EM_BOOL key_up(int, const EmscriptenKeyboardEvent* e, void*) {
    InputHandler::setKeyState(e->keyCode, KeyAction::Release);
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

EM_BOOL on_cursorMove(int eventType, const EmscriptenMouseEvent* e, void* userData) {
    // Handle cursor movement if needed
	//std::cout << "[cursorMove] Cursor moved to: " << e->targetX << ", " << e->targetY << std::endl;
	InputHandler::cursormoveEvent(e->targetX, e->targetY);
    return EM_TRUE;
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

    emscripten_set_mousemove_callback("#canvas", nullptr, EM_TRUE, on_cursorMove);

    emscripten_set_mousedown_callback("#canvas", nullptr, EM_TRUE, on_mouse_down);
    emscripten_set_mouseup_callback("#canvas", nullptr, EM_TRUE, on_mouse_up);

    emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, nullptr, EM_TRUE, on_document_keydown);

}

namespace Input {
    void getWindowSize(int* width, int* height) {
        if (width && height) {
            emscripten_get_canvas_element_size("#canvas", width, height);
        }
    }
    void getMousePosition(int* xPos, int* yPos) {
        
        if (!xPos || !yPos) return;
        *xPos = InputHandler::currentX;
        *yPos = InputHandler::currentY;
    }
}