
/*
    Monitor -

    Discription:    Monitor is an abstruct class.
                    It is an interface for systems like select() and epoll()

    Date:           27.1.2020

    Ver 1.0
*/

#ifndef __HRD11_MONITOR_HPP__
#define __HRD11_MONITOR_HPP__

#include <cstddef>  // size_t

namespace hrd11
{

class Monitor
{
public:
    Monitor() = default;
    virtual ~Monitor() = default;

    virtual void Add(int fd, unsigned int event_type) = 0;
    virtual void Remove(int fd) = 0;
    virtual int Wait() = 0;
    virtual int operator[](size_t index) = 0;

private:

};

} //end namespace hrd11

#endif // __HRD11_MONITOR_HPP__
