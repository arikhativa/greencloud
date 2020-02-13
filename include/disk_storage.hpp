
/*
    DiskStorage -

    Discription:    DiskStorage is used for greencloude.0.1.
					It is a wrapper for interacting with memory on the heep.

    Date:           27.1.2020

    Ver 1.0
*/

#ifndef __HRD11_DISK_STORAGE_HPP__
#define __HRD11_DISK_STORAGE_HPP__

#include <memory>		// std::unique_ptr
#include <vector>		// std::vector
#include <mutex>		// std::mutex
#include <fstream>      // std::fstream
#include <string>       // std::string

#include "storage.hpp"
#include "driver_data.hpp"

namespace hrd11
{

class DiskStorage : public Storage
{
public:
	// uncopiable through inheritence
	DiskStorage(const std::string& file_name, size_t size);
	virtual ~DiskStorage() override;

	virtual std::unique_ptr<DriverData> Write(std::unique_ptr<DriverData> data) override;
	virtual std::unique_ptr<DriverData> Read(std::unique_ptr<DriverData> data) override;

private:
	std::mutex m_mutex;
	std::fstream m_file;
};

}	// end namespace hrd11

#endif // __HRD11_DISK_STORAGE_HPP__
