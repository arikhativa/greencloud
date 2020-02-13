
/*
    Epoll -

    Discription:    Epoll is a wrapper to epoll().
                    it block the caller untill one of the fd is ready for use.

    Exceptions:     bad_alloc
                    EpollError

    Date:           27.1.2020

    Ver 1.0
*/

#ifndef __HRD11_EPOLL_HPP__
#define __HRD11_EPOLL_HPP__

#include <sys/epoll.h>  // epoll
#include <vector>       // std::vector
#include <string>       // std::string
#include <stdexcept>    // std::runtime_error

#include "monitor.hpp"

namespace hrd11
{

class Epoll : public Monitor
{
public:
    static const int READ_FD = EPOLLIN;
    static const int WRITE_FD = EPOLLOUT;

    Epoll(size_t num_of_events = 2);
    virtual ~Epoll() override;

    virtual void Add(int fd, unsigned int event_type) override;
    virtual void Remove(int fd) override;
    virtual int Wait() override;
    virtual int WaitTimeOut(int timeout) override;
    virtual int operator[](size_t index) override;

private:
    std::vector<struct epoll_event> m_events;
    size_t m_max_events;
    int m_epoll_fd;

};

struct EpollError : public std::runtime_error
{
    EpollError(const std::string& msg, int err) :
    std::runtime_error(msg),
    m_errno(err)
    {}

    virtual const char* what() const throw () override
    {
        return std::runtime_error::what();
    }

    int m_errno;
};

} //end namespace hrd11

#endif // __HRD11_EPOLL_HPP__
