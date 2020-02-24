
#ifndef __HRD11_SLAVE_DRIVER_PROXY_HPP__
#define __HRD11_SLAVE_DRIVER_PROXY_HPP__

#include <memory>			// std::unique_ptr

#include <netdb.h>          // struct sockaddr_in

#include "driver_proxy.hpp"
#include "driver_data.hpp"

namespace hrd11
{

class SlaveDriverProxy : public DriverProxy
{
public:
    SlaveDriverProxy(const std::string& ip, unsigned short port);
	~SlaveDriverProxy() override;

	// uncopyable
	SlaveDriverProxy(const SlaveDriverProxy& other) = delete;
	SlaveDriverProxy& operator=(const SlaveDriverProxy& other) = delete;

	// unmoveable
	SlaveDriverProxy(SlaveDriverProxy&& other) = delete;
	SlaveDriverProxy& operator=(SlaveDriverProxy&& other) = delete;

	std::unique_ptr<DriverData> ReceiveRequest() override;
	void SendReply(std::unique_ptr<DriverData> data) override;
	void Disconnect() override;
	int GetReqFd() override;

private:
    int m_socket;
    unsigned short m_port;
    struct sockaddr_in m_servaddr;

};

}	// end namespace hrd11

#endif // __HRD11_SLAVE_DRIVER_PROXY_HPP__
