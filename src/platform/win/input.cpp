// InputHandler_windows.cpp

#include "core/input.h"
#include <windows.h>
#include <iostream>

void initInputHandlers() {
    std::cout << "Windows input initialized. Press ESC to quit.\n";

    // You may want to launch a loop in a thread or tie this into your game loop
}

bool isKeyPressed(int vkey) {
    return (GetAsyncKeyState(vkey) & 0x8000) != 0;
}

void pollInput() {
    if (isKeyPressed(VK_LEFT)) std::cout << "Left arrow pressed\n";
    if (isKeyPressed(VK_RIGHT)) std::cout << "Right arrow pressed\n";
    if (isKeyPressed(VK_UP)) std::cout << "Up arrow pressed\n";
    if (isKeyPressed(VK_DOWN)) std::cout << "Down arrow pressed\n";
    if (isKeyPressed(VK_ESCAPE)) {
        std::cout << "Escape pressed, quitting...\n";
        exit(0);
    }
}

void shutdownInputHandlers() {
    // Optional cleanup
}
