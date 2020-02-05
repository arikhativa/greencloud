
#include <semaphore.h>      // sem_t
#include <cerrno>           // errno

#include "semaphore.hpp"

namespace hrd11
{

Semaphore::Semaphore()
{
    errno = 0;
    // sem start at value '0'
    if (sem_init(&m_sem, 0, 0))
    {
        throw SemError("Semaphore() - sem_init() fail", errno);
    }
}

Semaphore::~Semaphore()
{
    // TODO use logger here
    sem_destroy(&m_sem);
}

void Semaphore::Post()
{
    errno = 0;
    if (sem_post(&m_sem))
    {
        throw SemError("Semaphore::Post() - sem_post() fail", errno);
    }
}

void Semaphore::Wait()
{
    errno = 0;
    if (sem_wait(&m_sem))
    {
        throw SemError("Semaphore::Wait() - sem_wait() fail", errno);
    }
}

void Semaphore::TryWait()
{
    errno = 0;
    int ret = sem_trywait(&m_sem);

    if (ret && EAGAIN != errno)
    {
        throw SemError("Semaphore::TryWait() - sem_trywait() fail", errno);
    }
}

void Semaphore::TimedWait(size_t time)
{
    struct timespec restrict;

    errno = 0;
    if (-1 == clock_gettime(CLOCK_REALTIME, &restrict))
    {
        throw SemError("Semaphore::TimedWait() - clock_gettime() fail", errno);
    }

    restrict.tv_sec += time;

    errno = 0;
    int ret = sem_timedwait(&m_sem, &restrict);

    if (ret && ETIMEDOUT != errno)
    {
        throw SemError("Semaphore::TimedWait() -  sem_timedwait() fail", errno);
    }
    if (ret && ETIMEDOUT == errno)
    {
        throw SemTimeOut("Semaphore::TimedWait() - Timed Out", errno);
    }
}

int Semaphore::GetVal()
{
    int ret = 0;

    if (sem_getvalue(&m_sem, &ret))
    {
        throw SemError("Semaphore::GetVal() -  sem_getvalue() fail", errno);
    }

    return ret;
}

} // namespace hrd11
