#pragma once

#include <core/Events.h>
#include <core/input/input_types.h>


class KeyEvent : public Event {

public:
    KeyCode key;
    KeyAction action;
    unsigned int mod;

    explicit KeyEvent(KeyCode key, KeyAction action, unsigned int mod) : key(key), action(action), mod(mod) {};
};
