
#ifndef __HRD11_MMAP_STORAGE_HPP__
#define __HRD11_MMAP_STORAGE_HPP__

#include "storage.hpp"
#include "driver_data.hpp"

namespace hrd11
{

class MmapStorage : public Storage
{
public:
    MmapStorage(const std::string& file_name, size_t size);
    virtual ~MmapStorage() override;

    virtual std::unique_ptr<DriverData> Write(
                                    std::unique_ptr<DriverData> data) override;
    virtual std::unique_ptr<DriverData> Read(
                                    std::unique_ptr<DriverData> data) override;
    virtual size_t GetSize() override;
    virtual void Disconnect() override;

private:
    size_t m_size;
    char* m_mmap;

};

}	// end namespace hrd11

#endif // __HRD11_MMAP_STORAGE_HPP__
