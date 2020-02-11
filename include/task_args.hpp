
#ifndef __HRD11_TASK_ARGS_HPP__
#define __HRD11_TASK_ARGS_HPP__

#include "driver_proxy.hpp"
#include "globals.hpp"
#include "request_engine.hpp"
#include "storage.hpp"

namespace hrd11
{

struct TaskInfo
{
	TaskInfo(std::shared_ptr<DriverProxy> driver,
				std::shared_ptr<Storage> storage,
				std::unique_ptr<DriverData> data) :
		m_driver(driver),
		m_storage(storage),
		m_data(std::move(data))
	{}
    ~TaskInfo() = default;

	std::shared_ptr<DriverProxy> m_driver;
	std::shared_ptr<Storage> m_storage;
	std::unique_ptr<DriverData> m_data;
};

}	// end namespace hrd11

#endif // __HRD11_TASK_ARGS_HPP__
