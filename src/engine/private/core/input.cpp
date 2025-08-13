// InputHandler_windows.cpp

#include "core/input.h"

#include <iostream>

#include "core/entity.h" 
#include <core/components.h>
#include <core/world.h>

EventDispatcher* InputHandler::inputDispatcher = nullptr;
unsigned int InputHandler::keyStates[256] = { 0 };


void InputHandler::setKeyState(int jsKeyCode, KeyAction action) {
    if (jsKeyCode < 0 || jsKeyCode >= 256) return;

    const bool down = (action == KeyAction::Press || action == KeyAction::Repeat);
    keyStates[jsKeyCode] = down ? 1u : 0u;

    // Cast int -> KeyCode to match your Events.h
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

void InputHandler::clearKeyStates() {
    std::fill(std::begin(InputHandler::keyStates), std::end(InputHandler::keyStates), 0); 
}

void shutdownInputHandlers() {

}
