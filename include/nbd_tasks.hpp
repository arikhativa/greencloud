
#ifndef __HRD11_NBD_TASKS_HPP__
#define __HRD11_NBD_TASKS_HPP__

#include <memory>		// std::unique_ptr

#include "retask.hpp"
#include "nbd_driver_proxy.hpp"
#include "disk_storage.hpp"
#include "driver_data.hpp"

namespace hrd11
{

struct ThreadInfo
{
	ThreadInfo(std::shared_ptr<DriverProxy> driver,
				std::shared_ptr<Storage> storage,
				std::unique_ptr<DriverData> data) :
		m_driver(driver),
		m_storage(storage),
		m_data(std::move(data))
	{}
    ~ThreadInfo() = default;

	std::shared_ptr<DriverProxy> m_driver;
	std::shared_ptr<Storage> m_storage;
	std::unique_ptr<DriverData> m_data;
};

class NBDRead : public RETask
{
public:
    static std::unique_ptr<NBDRead> Create(std::unique_ptr<ThreadInfo> info)
    {
        return std::unique_ptr<NBDRead>(new NBDRead(std::move(info)));
    }

private:
    NBDRead(std::unique_ptr<ThreadInfo> info) : m_info(std::move(info))
    {}

    void Execute() override
    {
        m_info->m_data = m_info->m_storage->Read(std::move(m_info->m_data));
        m_info->m_driver->SendReply(std::move(m_info->m_data));
    }

    std::unique_ptr<ThreadInfo> m_info;
};

class NBDWrite : public RETask
{
public:
    static std::unique_ptr<NBDWrite> Create(std::unique_ptr<ThreadInfo> info)
    {
        return std::unique_ptr<NBDWrite>(new NBDWrite(std::move(info)));
    }

private:
    NBDWrite(std::unique_ptr<ThreadInfo> info) : m_info(std::move(info))
    {}

    void Execute() override
    {
        m_info->m_data = m_info->m_storage->Write(std::move(m_info->m_data));
        m_info->m_driver->SendReply(std::move(m_info->m_data));
    }

    std::unique_ptr<ThreadInfo> m_info;
};

class NBDFlush : public RETask
{
public:
    static std::unique_ptr<NBDFlush> Create(std::unique_ptr<ThreadInfo> info)
    {
        return std::unique_ptr<NBDFlush>(new NBDFlush(std::move(info)));
    }

private:
    NBDFlush(std::unique_ptr<ThreadInfo> info) : m_info(std::move(info))
    {}

    void Execute() override
    {
        m_info->m_driver->SendReply(std::move(m_info->m_data));
    }

    std::unique_ptr<ThreadInfo> m_info;
};

class NBDTrim : public RETask
{
public:
    static std::unique_ptr<NBDTrim> Create(std::unique_ptr<ThreadInfo> info)
    {
        return std::unique_ptr<NBDTrim>(new NBDTrim(std::move(info)));
    }

private:
    NBDTrim(std::unique_ptr<ThreadInfo> info) : m_info(std::move(info))
    {}

    void Execute() override
    {
        m_info->m_driver->SendReply(std::move(m_info->m_data));
    }

    std::unique_ptr<ThreadInfo> m_info;
};

class NBDBadRequest : public RETask
{
public:
    static std::unique_ptr<NBDBadRequest> Create(std::unique_ptr<ThreadInfo> info)
    {
        return std::unique_ptr<NBDBadRequest>(new NBDBadRequest(std::move(info)));
    }

private:
    NBDBadRequest(std::unique_ptr<ThreadInfo> info) : m_info(std::move(info))
    {}

    void Execute() override
    {
        m_info->m_driver->SendReply(std::move(m_info->m_data));
    }

    std::unique_ptr<ThreadInfo> m_info;
};

}	// end namespace hrd11

#endif // __HRD11_NBD_TASKS_HPP__
