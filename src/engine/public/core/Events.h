#pragma once

#include <iostream>
#include <functional>
#include <unordered_map>
#include <vector>
#include <memory>

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

class Event {
public:
    virtual ~Event() = default;
};


class KeyEvent : public Event {

public:
    KeyCode key;
    KeyAction action;
    unsigned int mod;

    explicit KeyEvent(KeyCode key, KeyAction action, unsigned int mod) : key(key), action(action), mod(mod) {};
};


using EventCallback = std::function<void(std::shared_ptr<Event>)>;

class EventDispatcher {
private:

    std::unordered_map<std::size_t, std::vector<EventCallback>> listeners;

    template<typename T>
    static std::size_t GetEventTypeID() {
        return typeid(T).hash_code();
    }

public:

    template<typename T>
    void Subscribe(std::function<void(std::shared_ptr<T>)> callback) {
        listeners[GetEventTypeID<T>()].push_back(
            [callback](std::shared_ptr<Event> e) {
                callback(std::static_pointer_cast<T>(e));
            }
        );
    }

    template<typename T>
    void Dispatch(std::shared_ptr<T> event) {
        auto it = listeners.find(GetEventTypeID<T>());
        if (it != listeners.end()) {
            for (const auto& listener : it->second) {
                listener(event);
            }
        }
    }
};