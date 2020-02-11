
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
