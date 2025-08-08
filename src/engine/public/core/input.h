#pragma once

#include "core/singleton.h"
#include <core/Events.h>

enum InputKeyCode {
    KEY_CTRL = 17, 
    KEY_ALT = 18, 
    KEY_SPACE = 32,
    KEY_PRIOR = 33,
    KEY_NEXT = 34,
    KEY_END = 35,
    KEY_HOME = 36,
    KEY_LEFT = 37,
    KEY_UP = 38,
    KEY_RIGHT = 39,
    KEY_DOWN = 40,
    KEY_SELECT = 41,
    KEY_PRINT = 42,
    KEY_SNAPSHOT = 44,
    KEY_INSERT = 45,
    KEY_DELETE = 46,
    KEY_HELP = 47,
};

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
	static void setKeyState(KeyCode jsKeyCode);
    static void clearKeyStates();
};


static inline int VK_to_JS(int vk) { return vk; }

void initInputHandlers();
void shutdownInputHandlers();
void pollInput();