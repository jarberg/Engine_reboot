// InputHandler_windows.cpp

#include "core/input.h"
#include <windows.h>
#include <iostream>

#include "core/entity.h" 
#include <core/components.h>
#include <core/world.h>

EventDispatcher* InputHandler::inputDispatcher = new EventDispatcher();
unsigned int InputHandler::keyStates[256] = { 0 };

static inline KeyCode WindowsVKToJSKeyCode(int vk){
    switch (vk) {
    default: return  KeyCode(vk);
    }
}

void initInputHandlers() {
    std::cout << "Windows input initialized. Press ESC to quit.\n";
	
}

bool isKeyPressed(int vkey) {
    return (GetAsyncKeyState(vkey) & 0x8000) != 0;
}

void pollInput() {
    KeyCode E_keyCode;
    auto* input = InputHandler::GetInstance();

    if (isKeyPressed(VK_LEFT)) {
        E_keyCode = WindowsVKToJSKeyCode(VK_LEFT);
        InputHandler::setKeyState(E_keyCode);
    }
    if (isKeyPressed(VK_RIGHT)) {
        E_keyCode = WindowsVKToJSKeyCode(VK_RIGHT);
        InputHandler::setKeyState(E_keyCode);
    }
    if (isKeyPressed(VK_UP)) {
        E_keyCode = WindowsVKToJSKeyCode(VK_UP);
        InputHandler::setKeyState(E_keyCode);
    }
    if (isKeyPressed(VK_DOWN)) {
        E_keyCode = WindowsVKToJSKeyCode(VK_DOWN);
        InputHandler::setKeyState(E_keyCode);
    }
    if (isKeyPressed(VK_CONTROL)) {
        E_keyCode = WindowsVKToJSKeyCode(VK_CONTROL);
        InputHandler::setKeyState(E_keyCode);
    }
    if (isKeyPressed(VK_MENU)) {
        E_keyCode = WindowsVKToJSKeyCode(VK_MENU);
        InputHandler::setKeyState(E_keyCode);
    }
 
}

void InputHandler::setKeyState(KeyCode jsKeyCode){

    int state = InputHandler::keyStates[int(jsKeyCode)];
    InputHandler::keyStates[int(jsKeyCode)] = (state + 1) % 2;

    InputHandler::GetInstance()->inputDispatcher->Dispatch(std::make_shared<KeyPressedEvent>(jsKeyCode));
    
}

void InputHandler::clearKeyStates() {
    std::fill(std::begin(InputHandler::keyStates), std::end(InputHandler::keyStates), 0); // Clear the key states after processing  
}

void shutdownInputHandlers() {

}