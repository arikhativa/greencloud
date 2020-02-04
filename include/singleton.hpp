
/*
    Singleton -

    Discription:    Singleton is a design pattern for ensuring there is
                    only one instance of a class.
                    Multi thread safe
                    Singleton is a template class.
                    Based on DCLP.(design pattern)

    Usage:          T* ptr = Singleton<T>::GetInstance();

                    This way 'ptr' will allways get the same instance.

    Requirement:
                    'T' has to have privte default ctor
                    'T' has to set 'class Singleton' as a 'class friend'

    Date:           27.1.2020
    Folder name:    greencloud/include
    File names:     ingelton.hpp

    Reviewer:

    Ver 1.0
*/

#ifndef __HRD11_SINGELTON_HPP__
#define __HRD11_SINGELTON_HPP__

#include <memory>   // std::unique_ptr
#include <mutex>    // std::mutex
#include <atomic>   // std::atomic

namespace hrd11
{

template<typename T>
class Singleton
{
public:
    Singleton() = delete;
    ~Singleton() = delete;
    Singleton(const Singleton& other) = delete;
    Singleton& operator=(const Singleton& other) = delete;

    Singleton(Singleton&& other) = delete;
    Singleton& operator=(Singleton&& other) = delete;


    template<typename... Args>
    static T* GetInstance(Args... args);

private:

    static std::mutex s_mutex;
    static std::atomic<T*> s_instance;
    static std::unique_ptr<T> s_ptr;

    // using unique_ptr for RAII
};

template<typename T>
std::mutex Singleton<T>::s_mutex;

template<typename T>
std::atomic<T*> Singleton<T>::s_instance;

template<typename T>
std::unique_ptr<T> Singleton<T>::s_ptr;


template <typename T>
template <typename... Args>
inline T* Singleton<T>::GetInstance(Args... args)
{
    T* tmp = s_instance.load(std::memory_order_acquire);

    if (0 == tmp)
    {
        std::lock_guard<std::mutex> lock(s_mutex);
        tmp = s_instance.load(std::memory_order_relaxed);

        if (0 == tmp)
        {
            tmp = new T(args...);
            s_ptr.reset(tmp);
            s_instance.store(tmp, std::memory_order_release);
        }
    }

    return tmp;
}

}   // end namespace hrd11

#endif // __HRD11_SINGELTON_HPP__
