

#include <sys/epoll.h>  // epoll
#include <unistd.h>     // close()
#include <errno.h>      // errno

#include <stdexcept>  // runtime_rerror

#include "epoll.hpp"

namespace hrd11
{

static int InitEpoll()
{
    errno = 0;
    return epoll_create1(0);
}

Epoll::Epoll(size_t max_events) :
    m_events(max_events, {0, 0}),
    m_max_events(max_events),
    m_epoll_fd(InitEpoll())
{
    if (-1 == m_epoll_fd)
    {
        throw EpollError("Epoll::Epoll() - epoll_create1()" ,errno);
    }
}

Epoll::~Epoll()
{
    close(m_epoll_fd);
}

void Epoll::Add(int fd, unsigned int event_type)
{
    struct epoll_event event = {0, 0};
    int stt = 0;

    event.events = event_type;
    event.data.fd = fd;

    errno = 0;
    stt = epoll_ctl(m_epoll_fd, EPOLL_CTL_ADD, fd, &event);
    if (-1 == stt)
    {
        throw EpollError(" Epoll::Add() - epoll_ctl()" ,errno);
    }
}

void Epoll::Remove(int fd)
{
    int stt = 0;

    errno = 0;
    stt = epoll_ctl(m_epoll_fd, EPOLL_CTL_DEL, fd, NULL);
    if (-1 == stt)
    {
        throw EpollError("Epoll::Remove() - epoll_ctl()" ,errno);
    }
}

int Epoll::Wait()
{
    int ret = 0;

    errno = 0;

    ret = epoll_wait(m_epoll_fd, m_events.data(), m_max_events, -1);
    if (-1 == ret)
    {
        throw EpollError("Epoll::Wait() - epoll_wait()" ,errno);
    }

    return ret;
}

int Epoll::WaitTimeOut(int timeout)
{
    int ret = 0;

    errno = 0;

    ret = epoll_wait(m_epoll_fd, m_events.data(), m_max_events, timeout);
    if (-1 == ret)
    {
        throw EpollError("Epoll::WaitTimeOut() - epoll_wait()" ,errno);
    }

    return ret;
}

int Epoll::operator[](size_t index)
{
    return m_events[index].data.fd;
}

} //end namespace hrd11
