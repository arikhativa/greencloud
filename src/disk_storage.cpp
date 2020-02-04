
#include <memory>		// std::unique_ptr
#include <vector>		// std::vector
#include <mutex>		// std::mutex
#include <fstream>      // std::fstream

#include "disk_storage.hpp"
#include "driver_data.hpp"

namespace hrd11
{

DiskStorage::DiskStorage(const std::string& file_name, size_t size) :
        m_file(file_name, std::ios::out | std::ios::in | std::ios::trunc)
{
    m_file.seekp(size);
    m_file.write("", 1);
}

DiskStorage::~DiskStorage()
{
    m_file.close();
}

std::unique_ptr<DriverData> DiskStorage::Write(std::unique_ptr<DriverData> data)
{

    printf("Request for write of size %d\n", data->m_len);

    std::lock_guard<std::mutex> lock(m_mutex);
    m_file.seekp(data->m_offset);
    m_file.write(data->m_buff.data(), data->m_len);

    return std::move(data);
}

std::unique_ptr<DriverData> DiskStorage::Read(std::unique_ptr<DriverData> data)
{
    printf("Request for read of size %d\n", data->m_len);

    data->m_buff.reserve(data->m_len);

    std::lock_guard<std::mutex> lock(m_mutex);
    m_file.seekp(data->m_offset);
    m_file.read(data->m_buff.data(), data->m_len);

    return std::move(data);
}

}	// end namespace hrd11
