
/*
    Handleton -

    Discription:    Handleton is based on Handleton design pattern -
                    for ensuring there is only one instance of a class.
                    Handleton is Multi thread safe and can work with
                    explicit load - (run-time loading)
                    Handleton is a template class.
                    Based on DCLP.(design pattern)

    Usage:          T* ptr = Handleton<T>::GetInstance();

                    This way 'ptr' will allways get the same instance.

    Requirement:
                    'T' has to have privte default ctor
                    'T' has to set 'class Handleton' as a 'class friend'

    Date:           27.1.2020
    Folder name:    greencloud/include
    File names:     handleton.hpp

    Reviewer:

    Ver 1.0
*/

#ifndef __HRD11_HANDLETON_HPP__
#define __HRD11_HANDLETON_HPP__

#include <memory>   // std::unique_ptr
#include <mutex>    // std::mutex
#include <atomic>   // std::atomic

namespace hrd11
{

template<typename T>
class Handleton
{
public:
    Handleton() = delete;
    ~Handleton() = delete;
    Handleton(const Handleton& other) = delete;
    Handleton& operator=(const Handleton& other) = delete;

    Handleton(Handleton&& other) = delete;
    Handleton& operator=(Handleton&& other) = delete;


    template<typename... Args>
    static T* GetInstance(Args... args);

private:
    static std::mutex s_mutex;
    static std::atomic<T*> s_instance;
    static std::unique_ptr<T> s_ptr; // using unique_ptr only for RAII
};


#define INIT_HANDLETON(type)                                \
template<>                                                  \
std::mutex Handleton<type>::s_mutex{};                      \
template<>                                                  \
std::atomic<type*> Handleton<type>::s_instance{nullptr};    \
template<>                                                  \
std::unique_ptr<type> Handleton<type>::s_ptr{nullptr};


#ifdef CLANGE
    template<typename T>
    std::mutex Handleton<T>::s_mutex;

    template<typename T>
    std::atomic<T*> Handleton<T>::s_instance;

    template<typename T>
    std::unique_ptr<T> Handleton<T>::s_ptr;
#endif



template <typename T>
template <typename... Args>
inline T* Handleton<T>::GetInstance(Args... args)
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

#endif // __HRD11_HANDLETON_HPP__
