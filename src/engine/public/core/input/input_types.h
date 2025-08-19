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
    Mouse01 = 0, // Left mouse button
    Mouse02 = 1, // Right mouse button
    Mouse03 = 2, // Middle mouse button
    Mouse04 = 3, // Mouse wheel up
    Mouse05 = 4, // Mouse wheel down
    Mouse06 = 6, // Mouse button 4
    Mouse07 = 7, // Mouse button 5
    Mouse08 = 8, // Mouse button 6
    Unknown = -1
};