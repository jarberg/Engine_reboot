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

class CursorKeyEvent : public KeyEvent {

public:
    unsigned int posX;
    unsigned int posY;

    explicit CursorKeyEvent(KeyCode key, KeyAction action, unsigned int mod, unsigned int posX, unsigned int posY): KeyEvent(key, action, mod), posX(posX), posY(posY) {};
};

class CursorMoveEvent : public Event {

public:
    unsigned int posX;
    unsigned int posY;
    double deltaX;
    double deltaY;

    explicit CursorMoveEvent(double deltaX, double deltaY, unsigned int posX, unsigned int posY) :deltaX(deltaX), deltaY(deltaY), posX(posX), posY(posY){};
};
