
/*
    DriverProxy -

    Discription:    DriverProxy is an abstruct class to represent a driver.
					it would be an interface to work with a disc.

    Date:           27.1.2020

    Ver 1.0
*/

#ifndef __HRD11_DRIVER_PROXY_HPP__
#define __HRD11_DRIVER_PROXY_HPP__

#include <memory>			// std::unique_ptr

#include "driver_data.hpp"

namespace hrd11
{
class DriverData;

class DriverProxy
{
	public:
	DriverProxy() = default;
	virtual ~DriverProxy() = default;
	// uncopyable
	DriverProxy(const DriverProxy& other) = delete;
	DriverProxy& operator=(const DriverProxy& other) = delete;

	virtual std::unique_ptr<DriverData> ReceiveRequest() = 0; // move is better than cpy
	virtual void SendReply(std::unique_ptr<DriverData> data) = 0;
	virtual void Disconnect() = 0;
	virtual int GetReqFd() = 0;
};

}	// namespace hrd11

#endif // __HRD11_DRIVER_PROXY_HPP__
