// InputHandler_windows.cpp

#include "core/input.h"
#include <windows.h>
#include <iostream>

#include "core/entity.h" 
#include <core/components.h>
#include <core/world.h>

EventDispatcher* InputHandler::inputDispatcher = new EventDispatcher();


void initInputHandlers() {
    std::cout << "Windows input initialized. Press ESC to quit.\n";
	
}

bool isKeyPressed(int vkey) {
    return (GetAsyncKeyState(vkey) & 0x8000) != 0;
}

void pollInput() {
    if (isKeyPressed(VK_LEFT)) InputHandler::GetInstance()->inputDispatcher->Dispatch(std::make_shared<KeyPressedEvent>(KeyCode::Left));
    if (isKeyPressed(VK_RIGHT))InputHandler::GetInstance()->inputDispatcher->Dispatch(std::make_shared<KeyPressedEvent>(KeyCode::Right));
    if (isKeyPressed(VK_UP)) InputHandler::GetInstance()->inputDispatcher->Dispatch(std::make_shared<KeyPressedEvent>(KeyCode::Up));
    if (isKeyPressed(VK_DOWN)) InputHandler::GetInstance()->inputDispatcher->Dispatch(std::make_shared<KeyPressedEvent>(KeyCode::Down));
    if (isKeyPressed(VK_ESCAPE)) {
        std::cout << "Escape pressed, quitting...\n";
        exit(0);
    }
}

void shutdownInputHandlers() {
    // Optional cleanup
}
