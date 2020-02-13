
#ifndef __HRD11_SLAVE_TASKS_HPP__
#define __HRD11_SLAVE_TASKS_HPP__

#include <memory>		// std::unique_ptr

#include "task_args.hpp"
#include "retask.hpp"

#include "slave_driver_proxy.hpp"
#include "disk_storage.hpp"
#include "driver_data.hpp"

namespace hrd11
{

class SlaveRead : public RETask
{
public:
    static std::unique_ptr<SlaveRead> Create(std::unique_ptr<TaskInfo> info)
    {
        return std::unique_ptr<SlaveRead>(new SlaveRead(std::move(info)));
    }

private:
    SlaveRead(std::unique_ptr<TaskInfo> info) : m_info(std::move(info))
    {}

    void Execute() override
    {
        m_info->m_data = m_info->m_storage->Read(std::move(m_info->m_data));
        m_info->m_driver->SendReply(std::move(m_info->m_data));
    }

    std::unique_ptr<TaskInfo> m_info;
};

class SlaveWrite : public RETask
{
public:
    static std::unique_ptr<SlaveWrite> Create(std::unique_ptr<TaskInfo> info)
    {
        return std::unique_ptr<SlaveWrite>(new SlaveWrite(std::move(info)));
    }

private:
    SlaveWrite(std::unique_ptr<TaskInfo> info) : m_info(std::move(info))
    {}

    void Execute() override
    {
        m_info->m_data = m_info->m_storage->Write(std::move(m_info->m_data));


        if (m_info->m_data->m_offset < 67108864)
        {
            m_info->m_driver->SendReply(std::move(m_info->m_data));
        }
    }

    std::unique_ptr<TaskInfo> m_info;
};

// class SlaveDisconnect : public RETask
// {
// public:
//     static std::unique_ptr<SlaveDisconnect> Create(std::unique_ptr<TaskInfo> info)
//     {
//         return std::unique_ptr<SlaveDisconnect>(new SlaveDisconnect(std::move(info)));
//     }
//
// private:
//     SlaveDisconnect(std::unique_ptr<TaskInfo> info) : m_info(std::move(info))
//     {}
//
//     void Execute() override
//     {
//         m_info->m_driver->Disconnect();
//     }
//
//     std::unique_ptr<TaskInfo> m_info;
// };

}	// end namespace hrd11

#endif // __HRD11_SLAVE_TASKS_HPP__
