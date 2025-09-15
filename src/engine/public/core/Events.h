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



class msgEvent : public Event {

public:
    std::string msg;

    explicit msgEvent(std::string msg) : msg(msg) {


    }
};

class clientMSGEvent : public Event {
public:
    std::string msg;

    explicit clientMSGEvent(std::string msg) : msg(msg) {


    }
};
class RClientMSGEvent : public Event {
public:
    std::string msg;
    int id;
    explicit RClientMSGEvent(std::string msg, int id) : msg(msg), id(id) {
    }
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