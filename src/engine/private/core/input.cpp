
#include "core/input.h"

#include <core/input/input_types.h>
#include <core/input/input_Events.h>

#include "core/entity.h" 
#include <core/components.h>
#include <core/world.h>

#include <iostream>

EventDispatcher* InputHandler::inputDispatcher = nullptr;
unsigned int InputHandler::keyStates[256] = { 0 };
int InputHandler::lastX = 0;
int InputHandler::lastY = 0;
int InputHandler::currentX = 0;
int InputHandler::currentY = 0;

double InputHandler::cursorDeltaX = 0.0f;
double InputHandler::cursorDeltaY = 0.0f;

void InputHandler::setKeyState(int jsKeyCode, KeyAction action) {
    if (jsKeyCode < 0 || jsKeyCode >= 256) return;

    const bool down = (action == KeyAction::Press || action == KeyAction::Repeat);
    keyStates[jsKeyCode] = down ? 1u : 0u;

    const KeyCode kc = static_cast<KeyCode>(jsKeyCode);

    unsigned int mod = (keyStates[17] ? 1u : 0u) + (keyStates[18] ? 2u : 0u);

    InputHandler::GetInstance()->inputDispatcher
        ->Dispatch(std::make_shared<KeyEvent>(kc, action, mod));
}

void InputHandler::fireHeldPressed() {
    auto* disp = InputHandler::GetInstance()->inputDispatcher;
    unsigned int mod = (keyStates[17] ? 1u : 0u) | (keyStates[18] ? 2u : 0u);
    for (int k = 0; k < 256; ++k) {
        if (keyStates[k]) {
            disp->Dispatch(std::make_shared<KeyEvent>(static_cast<KeyCode>(k), KeyAction::Repeat, mod));
        }
    }
}

void InputHandler::cursormoveEvent(int xPos, int yPos) {
    InputHandler::lastX = InputHandler::currentX;
    InputHandler::lastY = InputHandler::currentY;

    int width = 0, height = 0;
    Input::getWindowSize(&width, &height);
    InputHandler::currentX = xPos;
    InputHandler::currentY = yPos;
    InputHandler::cursorDeltaX = double(InputHandler::currentX - InputHandler::lastX)/width;
    InputHandler::cursorDeltaY = double(InputHandler::currentY - InputHandler::lastY)/height;

    inputDispatcher->Dispatch(std::make_shared<CursorMoveEvent>(InputHandler::cursorDeltaX, InputHandler::cursorDeltaY, InputHandler::currentX, InputHandler::currentY));
}

void InputHandler::clearKeyStates() {
    std::fill(std::begin(InputHandler::keyStates), std::end(InputHandler::keyStates), 0); 
}

void shutdownInputHandlers() {
}
