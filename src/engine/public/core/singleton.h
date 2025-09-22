#pragma once

#include <mutex>
#include <memory>

// Template Singleton class
template <typename T>
class Singleton {
private:
    static std::unique_ptr<T> pinstance_;
    static std::once_flag initFlag;

protected:
    // Protected constructor to prevent direct instantiation
    Singleton() = default;
    virtual ~Singleton() = default;

public:
    // Delete copy constructor and assignment operator to prevent copying
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    // Public method to get the singleton instance
    static T* GetInstance();
};

// Static member initialization in the header to avoid multiple definition errors
template <typename T>
std::unique_ptr<T> Singleton<T>::pinstance_ = nullptr;

template <typename T>
std::once_flag Singleton<T>::initFlag;

template <typename T>
T* Singleton<T>::GetInstance() {
    std::call_once(initFlag, []() {
        pinstance_.reset(new T());
        });
    return pinstance_.get();
}
