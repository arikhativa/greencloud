
#include <unistd.h>     // open(), write(), lseek()
#include <fcntl.h>      // open()
#include <sys/mman.h>   // mmap()
#include <string.h>     // memcpy()

#include "mmap_storage.hpp"

namespace hrd11
{

static char* InitMmap(const std::string& file_name, size_t size)
{
    char* ret = nullptr;
    int fd = open(file_name.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    lseek (fd, size + 1, SEEK_SET);
    write (fd, "", 1);
    lseek (fd, 0, SEEK_SET);

    ret = (char*)mmap(0, size, PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    return ret;
}

MmapStorage::MmapStorage(const std::string& file_name, size_t size) :
                                            m_size(size),
                                            m_mmap(InitMmap(file_name, size))
{}

MmapStorage::~MmapStorage()
{
    munmap(m_mmap, m_size);
}

std::unique_ptr<DriverData> MmapStorage::Write(
                                            std::unique_ptr<DriverData> data)
{
    // std::lock_guard<std::mutex> lock(m_mutex);
    printf("Request for write of size %u\n", data->m_len);

    char* offset = m_mmap + data->m_offset;

    memcpy(offset, data->m_buff.data(), data->m_len);

    return std::move(data);
}

std::unique_ptr<DriverData> MmapStorage::Read(
                                std::unique_ptr<DriverData> data)
{
    // std::lock_guard<std::mutex> lock(m_mutex);
    printf("Request for read of size %u\n", data->m_len);

    data->m_buff.reserve(data->m_len);

    char* offset = m_mmap + data->m_offset;
    memcpy(data->m_buff.data(), offset, data->m_len);

    return std::move(data);
}


}	// end namespace hrd11
