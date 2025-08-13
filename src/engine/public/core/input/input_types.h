#pragma once
enum class KeyAction : unsigned int { Release = 0, Press = 1, Repeat = 2 };

enum class KeyCode {
    Left = 37,
    Up = 38,
    Right = 39,
    Down = 40,
    Escape,
    Ctrl = 17,
    Alt = 18,
    Mouse01 = 1, // Left mouse button
    Mouse02 = 2, // Right mouse button
    Mouse03 = 3, // Middle mouse button
    Mouse04 = 4, // Mouse wheel up
    Mouse05 = 5, // Mouse wheel down
    Mouse06 = 6, // Mouse button 4
    Mouse07 = 7, // Mouse button 5
    Mouse08 = 8, // Mouse button 6
    Unknown = -1
};