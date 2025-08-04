#pragma once

#include "core/singleton.h"
#include <core/Events.h>

class InputHandler : public Singleton<InputHandler> {
    friend class Singleton<InputHandler>;

public:
    static unsigned int keyStates[256];
    static EventDispatcher* inputDispatcher;

    InputHandler() {
        if (!inputDispatcher) {
            inputDispatcher = new EventDispatcher();
           
        }
    }

    ~InputHandler() {
        delete inputDispatcher;
    }
    static void clearKeyStates();
};

void initInputHandlers();
void shutdownInputHandlers(); // optional
void pollInput();