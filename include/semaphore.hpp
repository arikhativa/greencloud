
/*
    Semaphore -

    Discription:    Warapper for posix semaphore

    Exception:      bad_alloc
                    SemError
                    SemTimeOut

    Date: 30.1.20
    Ver 1.0
*/

#ifndef __HRD11_SEMAPHORE_HPP__
#define __HRD11_SEMAPHORE_HPP__

#include <stdexcept>        // std::runtime_error
#include <string>           // std::string

#include <semaphore.h>      // sem_t

namespace hrd11
{

class Semaphore
{
public:
    Semaphore();
    ~Semaphore();

    void Post();
    void Wait();
    void TryWait();
    void TimedWait(size_t time);

private:
    // No cctor, copy=
    Semaphore(const Semaphore& other);
    Semaphore& operator=(const Semaphore& other);

    sem_t m_sem;
};

struct SemTimeOut : public std::runtime_error
{
    SemTimeOut(const std::string& msg, int err) :
    std::runtime_error(msg),
    m_errno(err)
    {}

    virtual const char* what() const throw () override
    {
        return std::runtime_error::what();
    }

    int m_errno;
};

struct SemError : public std::runtime_error
{
    SemError(const std::string& msg, int err) :
    std::runtime_error(msg),
    m_errno(err)
    {}

    virtual const char* what() const throw () override
    {
        return std::runtime_error::what();
    }

    int m_errno;
};

} // end namespace hrd11

#endif // __HRD11_SEMAPHORE_HPP__
