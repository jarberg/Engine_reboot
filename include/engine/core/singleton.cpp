#include "singleton.h"


// Static member initialization
template <typename T>
std::unique_ptr<T> Singleton<T>::pinstance_ = nullptr;

template <typename T>
std::once_flag Singleton<T>::initFlag;


template<typename T>
inline T* Singleton<T>::GetInstance()
{
    std::call_once(initFlag, []() {
        pinstance_.reset(new T());
        });
    return pinstance_.get();
}


