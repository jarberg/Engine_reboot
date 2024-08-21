#pragma once

#include <mutex>
#include <memory>
#include <engine/modelLookup.h>
#include <engine/resourceManager.h>

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

template class Singleton<model_datatable>;
template class Singleton<ResourceManager>;