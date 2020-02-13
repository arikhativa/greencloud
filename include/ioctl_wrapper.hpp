
/*
    Ioctl -

    Discription:    Ioctl is a wrapper to ioctl().

    Usage:          The same way of ioctl().

    Exceptions:     std::bad_alloc
                    IoctlError

    Date:           27.1.2020

    Ver 1.0
*/

#ifndef __HRD11_IOCTL_HPP__
#define __HRD11_IOCTL_HPP__

#include <sys/types.h>      // ssize_t
#include <cstddef>	     	// size_t
#include <sys/ioctl.h>		// ioctl()

#include <stdexcept>		// runtime_error
#include <string>           // std::string, to_string

namespace hrd11
{

class IoctlError : public std::runtime_error
{
public:
    static const int NO_FLAGS = -1;

    IoctlError(int fd, int err ,size_t macro, ssize_t flags);

    virtual const char* what() const throw () override;

private:

    int m_fd;
    int m_errno;
    size_t m_macro;
    ssize_t m_flags;
    mutable std::string m_error;
};

void Ioctl(int fd, size_t macro, ssize_t flags);

} // end namespace hrd11

#endif // __HRD11_IOCTL_HPP__
