#pragma once

#include <iostream>
#include <functional>
#include <unordered_map>
#include <vector>
#include <memory>


class Event {
public:
    virtual ~Event() = default;
};  

enum class KeyCode {
    Left, Right, Down, Up, Escape, Ctrl, Alt
    // Add other keys as needed
};

class KeyPressedEvent : public Event {
public:
    KeyCode key;

    explicit KeyPressedEvent(KeyCode key) : key(key) {}
};

// Type alias for Event Callbacks
using EventCallback = std::function<void(std::shared_ptr<Event>)>;

class EventDispatcher {
private:

    std::unordered_map<std::size_t, std::vector<EventCallback>> listeners;

    // Helper function to get a unique event type ID
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