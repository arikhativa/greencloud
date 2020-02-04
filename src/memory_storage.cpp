
#include <memory>		// std::unique_ptr
#include <vector>		// std::vector

#include "memory_storage.hpp"
#include "driver_data.hpp"

namespace hrd11
{

MemoryStorage::MemoryStorage(size_t size) : m_disk(size)
{}

std::unique_ptr<DriverData> MemoryStorage::Write(std::unique_ptr<DriverData> data)
{
    printf("Request for write of size %d\n", data->m_len);

    std::vector<char>::iterator disk_ptr = m_disk.begin();

    disk_ptr += data->m_offset;

    std::copy(data->m_buff.begin(), data->m_buff.end(), disk_ptr);

    return std::move(data);
}

std::unique_ptr<DriverData> MemoryStorage::Read(std::unique_ptr<DriverData> data)
{
    printf("Request for read of size %d\n", data->m_len);

    std::vector<char>::iterator disk_start = m_disk.begin();
    std::vector<char>::iterator disk_end = m_disk.begin();

    disk_start += data->m_offset;
    disk_end += data->m_len + data->m_offset;

    data->m_buff.reserve(data->m_len);

    std::copy(disk_start, disk_end, data->m_buff.begin());

    return std::move(data);
}

}	// end namespace hrd11
