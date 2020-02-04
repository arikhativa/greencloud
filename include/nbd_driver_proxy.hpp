
/*
    NBDDriverProxy -

    Discription:    NBDDriverProxy is a wrapper for "network block device" (nbd)
					This mechanisim will redirect data from a spacific block
					device to a process.
					In greencloud it will redirect data from a shared directory
					to the Master process.

	Usage:			Create instace:
						NBDDriverProxy(...) driver;
						std::unique_ptr<DriverData>;

					Get a request ,manipulate it and send a replay:
						data = driver.ReceiveRequest();

						// manipulate data
						read(data) || write (data)

						driver.SendReply(std::move(data));

	Exceptions:		std::bad_alloc
					std::runtime_error
					NBDBadMagic

    Date:           27.1.2020

    Ver 1.0
*/

#ifndef __HRD11_NBD_DRIVER_PROXY_HPP__
#define __HRD11_NBD_DRIVER_PROXY_HPP__

#include <thread>			//std::thread
#include <mutex>			//std::mutex
#include <stdexcept>		//std::runtime_error

#include "driver_proxy.hpp"
#include "driver_data.hpp"

namespace hrd11
{
class NBDDriverProxy : public DriverProxy
{
public:
	// no cctor & cpoy= uncopiable through inheritence.

	// 'storage_size' is by MB
	NBDDriverProxy(size_t storage_size, const std::string& device_name);

	// 'block_size' is by bytes
	NBDDriverProxy(size_t block_size, size_t num_blocks, const std::string& device_name);

	~NBDDriverProxy() override;

	std::unique_ptr<DriverData> ReceiveRequest() override;
	void SendReply(std::unique_ptr<DriverData> data) override;
	void Disconnect() override;
	int GetReqFd() override;

private:
	std::string m_device_name;
	int m_req_fd;
	int m_nbd_fd;
	int m_file_fd;
	std::mutex m_mutex;
	std::thread m_nbd_thread;
	bool m_connected;

};


// Exception Classes --------------------------------------------------------------

struct NBDBadMagic : std::runtime_error
{
	NBDBadMagic(const std::string& msg, unsigned int bad_magic) :
	std::runtime_error(msg),
	m_bad_magic(bad_magic)
	{}

		unsigned int m_bad_magic;
	};

}   // namespace hrd11

#endif // __HRD11_NBD_DRIVER_PROXY_HPP__
