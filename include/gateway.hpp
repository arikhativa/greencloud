
/*
    GateWay -

    Discription:    This class is an interface for RequestEngine to read
                    requests from a spacific file discriptor.

    Usage:          The user needs to inhert from this class and override
                    the GetFd() and Read() functions.

        GetFd():    Should return the file discriptor that, when is avilabel,
                    will be Read() from.

        Read():     Will get the data that is needed by the users *RETask,
                    and will put it in Args.
                    And will set the corolating Key to a spasific RETask

                    If Read() wants RequestEngine to stop and exit. It needs
                    to run req_eng->Stop().
                    WARNING: DO NOT RUN ANY OTHER METHOD OF req_eng !

    *RETask:        read retask.hpp

    Date:           12.2.2020

    Ver 1.0
*/

#ifndef __HRD11_GATEWAY_HPP__
#define __HRD11_GATEWAY_HPP__

#include <utility>      // std::pair
#include <unistd.h>     // close()

namespace hrd11
{

template <typename Key, typename Args>
class RequestEngine;

template <typename Key, typename Args>
class GateWay
{

public:
    GateWay() = default;
    virtual ~GateWay() = default;

    GateWay(const GateWay& other) = default;
    GateWay& operator=(const GateWay& other)=  default;
    GateWay(GateWay&& other) = default;
    GateWay& operator=(GateWay&& other)=  default;

    virtual int GetFd() = 0;

    virtual std::pair<Key, Args> Read(RequestEngine<Key, Args>* req_eng) = 0;
};

}	// end namespace hrd11

#endif // __HRD11_GATEWAY_HPP__
