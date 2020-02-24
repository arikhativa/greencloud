
#ifndef __HRD11_NBD_TASKS_HPP__
#define __HRD11_NBD_TASKS_HPP__

#include <memory>		// std::unique_ptr

#include "task_args.hpp"
#include "retask.hpp"

#include "nbd_driver_proxy.hpp"
#include "disk_storage.hpp"
#include "driver_data.hpp"

namespace hrd11
{

class NBDRead : public RETask
{
public:
    static std::unique_ptr<NBDRead> Create(std::unique_ptr<TaskInfo> info)
    {
        return std::unique_ptr<NBDRead>(new NBDRead(std::move(info)));
    }

private:
    NBDRead(std::unique_ptr<TaskInfo> info) : m_info(std::move(info))
    {}

    void Execute() override
    {
        m_info->m_storage->Read(std::move(m_info->m_data));
        // m_info->m_data = m_info->m_storage->Read(std::move(m_info->m_data));
        // m_info->m_driver->SendReply(std::move(m_info->m_data));
    }

    std::unique_ptr<TaskInfo> m_info;
};

class NBDWrite : public RETask
{
public:
    static std::unique_ptr<NBDWrite> Create(std::unique_ptr<TaskInfo> info)
    {
        return std::unique_ptr<NBDWrite>(new NBDWrite(std::move(info)));
    }

private:
    NBDWrite(std::unique_ptr<TaskInfo> info) : m_info(std::move(info))
    {}

    void Execute() override
    {
        m_info->m_storage->Write(std::move(m_info->m_data));
        // m_info->m_data = m_info->m_storage->Write(std::move(m_info->m_data));
        // m_info->m_driver->SendReply(std::move(m_info->m_data));
    }

    std::unique_ptr<TaskInfo> m_info;
};

class NBDFlush : public RETask
{
public:
    static std::unique_ptr<NBDFlush> Create(std::unique_ptr<TaskInfo> info)
    {
        return std::unique_ptr<NBDFlush>(new NBDFlush(std::move(info)));
    }

private:
    NBDFlush(std::unique_ptr<TaskInfo> info) : m_info(std::move(info))
    {}

    void Execute() override
    {
        m_info->m_driver->SendReply(std::move(m_info->m_data));
    }

    std::unique_ptr<TaskInfo> m_info;
};

class NBDTrim : public RETask
{
public:
    static std::unique_ptr<NBDTrim> Create(std::unique_ptr<TaskInfo> info)
    {
        return std::unique_ptr<NBDTrim>(new NBDTrim(std::move(info)));
    }

private:
    NBDTrim(std::unique_ptr<TaskInfo> info) : m_info(std::move(info))
    {}

    void Execute() override
    {
        m_info->m_driver->SendReply(std::move(m_info->m_data));
    }

    std::unique_ptr<TaskInfo> m_info;
};

class NBDReplay : public RETask
{
public:
    static std::unique_ptr<NBDReplay> Create(std::unique_ptr<TaskInfo> info)
    {
        return std::unique_ptr<NBDReplay>(new NBDReplay(std::move(info)));
    }

private:
    NBDReplay(std::unique_ptr<TaskInfo> info) : m_info(std::move(info))
    {}

    void Execute() override
    {
        printf("NBDReplay() - Execute()\n");

        m_info->m_driver->SendReply(std::move(m_info->m_data));
    }

    std::unique_ptr<TaskInfo> m_info;
};

class NBDBadRequest : public RETask
{
public:
    static std::unique_ptr<NBDBadRequest> Create(std::unique_ptr<TaskInfo> info)
    {
        return std::unique_ptr<NBDBadRequest>(new NBDBadRequest(std::move(info)));
    }

private:
    NBDBadRequest(std::unique_ptr<TaskInfo> info) : m_info(std::move(info))
    {}

    void Execute() override
    {
        m_info->m_driver->SendReply(std::move(m_info->m_data));
    }

    std::unique_ptr<TaskInfo> m_info;
};

class NBDDisconnect : public RETask
{
public:
    static std::unique_ptr<NBDDisconnect> Create(std::unique_ptr<TaskInfo> info)
    {
        return std::unique_ptr<NBDDisconnect>(new NBDDisconnect(std::move(info)));
    }

private:
    NBDDisconnect(std::unique_ptr<TaskInfo> info) : m_info(std::move(info))
    {}

    void Execute() override
    {
        m_info->m_storage->Disconnect();
        m_info->m_driver->Disconnect();
    }

    std::unique_ptr<TaskInfo> m_info;
};

}	// end namespace hrd11

#endif // __HRD11_NBD_TASKS_HPP__
