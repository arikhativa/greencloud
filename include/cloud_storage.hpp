
#ifndef __HRD11_CLOUDE_STORAGE_HPP__
#define __HRD11_CLOUDE_STORAGE_HPP__

#include <memory>           // std::unique_ptr
#include <vector>           // std::vector
#include <unordered_map>    //std::unordered map
#include <netdb.h>
#include <functional>
#include <list>
#include <utility>
#include <mutex>

#include "storage.hpp"
#include "driver_data.hpp"

namespace hrd11
{

class Offset;

struct SlaveData
{
    SlaveData() = default; // dont use this!
    explicit SlaveData(size_t port);
    ~SlaveData() = default;

    SlaveData(const SlaveData& other) = default;
    SlaveData& operator=(const SlaveData& other) = default;

    SlaveData(SlaveData&& other) = default;
    SlaveData& operator=(SlaveData&& other) = default;

    void Disconnect();

    int m_socket;
    struct sockaddr_in m_cliaddr;
};

struct Offset
{
    explicit Offset(off_t offset) : m_offset(offset)
    {
    }

    bool operator==(const Offset &other) const
    {
        (void)other;

        return true;
    }

    off_t m_offset;
};

}	// end namespace hrd11


template<>
struct std::hash<hrd11::Offset>
{
    size_t operator()(const hrd11::Offset& Offset) const
    {
        (void)Offset;

        return 0;
    }
};


namespace hrd11
{

class CloudStorage : public Storage
{


public:
    CloudStorage(size_t num_of_slaves, size_t storage_size);
    virtual ~CloudStorage() override;

    // uncopiable
    CloudStorage(const CloudStorage& other) = delete;
    CloudStorage& operator=(const CloudStorage& other) = delete;

    // unmoveable
    CloudStorage(CloudStorage&& other) = delete;
    CloudStorage& operator=(CloudStorage&& other) = delete;

    void AddSlave(Offset slave_offset, size_t port);

    virtual std::unique_ptr<DriverData> Write(
            std::unique_ptr<DriverData> data) override;
	virtual std::unique_ptr<DriverData> Read(
            std::unique_ptr<DriverData> data) override;

	virtual size_t GetSize() override;
	virtual void Disconnect() override;

    std::vector<int> GetAllFds();
    SlaveData GetSlaveData(off_t offset);
    bool IdToHandler(int req_id, char *ret);

private:
    int HandlerToId(char handler[HANDLE_SIZE]);

    std::unordered_map<Offset, SlaveData> m_slaves;
    std::list< std::pair<int, char[HANDLE_SIZE]> > m_list;

    size_t m_num_of_slaves;
    size_t m_storage_size;
    std::mutex m_mutex;
    int m_req_id;

};

}	// end namespace hrd11



#endif // __HRD11_CLOUDE_STORAGE_HPP__
