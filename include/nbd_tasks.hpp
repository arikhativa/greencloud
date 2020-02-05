
#ifndef __HRD11_NBD_TASKS_HPP__
#define __HRD11_NBD_TASKS_HPP__

#include <memory>		// std::unique_ptr

#include "globals.hpp"
#include "handleton.hpp"
#include "logger.hpp"
#include "tptask.hpp"
#include "nbd_driver_proxy.hpp"
#include "disk_storage.hpp"
#include "driver_data.hpp"

#define LOG(lvl, msg) s_log->Write(lvl, msg, __FILE__, __LINE__)


namespace hrd11
{

static Logger* s_log = Handleton<Logger>::GetInstance(LOG_PATH, LOG_LVL);

struct ThreadInfo
{
	ThreadInfo(std::shared_ptr<DriverProxy> driver,
				std::shared_ptr<Storage> storage,
				std::unique_ptr<DriverData> data) :
		m_driver(driver),
		m_storage(storage),
		m_data(std::move(data))
	{
        m_i = '5';
    }
    ~ThreadInfo() = default;

    char m_i;
	std::shared_ptr<DriverProxy> m_driver;
	std::shared_ptr<Storage> m_storage;
	std::unique_ptr<DriverData> m_data;
};

class NBDRead : public TPTask
{
public:
    static std::unique_ptr<NBDRead> Create(std::unique_ptr<ThreadInfo> info)
    {
        LOG(LOG_DEBUG, "NBDRead::Create()");
        return std::unique_ptr<NBDRead>(new NBDRead(std::move(info)));
    }

private:
    NBDRead(std::unique_ptr<ThreadInfo> info) : m_info(std::move(info))
    {
        LOG(LOG_DEBUG, "NBDRead()");
    }

    void Execute() override
    {
        LOG(LOG_DEBUG, "NBDRead::Execute()");
        m_info->m_data = m_info->m_storage->Read(std::move(m_info->m_data));
        m_info->m_driver->SendReply(std::move(m_info->m_data));
    }

    std::unique_ptr<ThreadInfo> m_info;
};

class NBDWrite : public TPTask
{
public:
    static std::unique_ptr<NBDWrite> Create(std::unique_ptr<ThreadInfo> info)
    {
        LOG(LOG_DEBUG, "NBDWrite::Create()");
        return std::unique_ptr<NBDWrite>(new NBDWrite(std::move(info)));
    }

private:
    NBDWrite(std::unique_ptr<ThreadInfo> info) : m_info(std::move(info))
    {
        LOG(LOG_DEBUG, "NBDWrite()");
    }

    void Execute() override
    {
        LOG(LOG_DEBUG, "NBDWrite::Execute()");
        m_info->m_data = m_info->m_storage->Write(std::move(m_info->m_data));
        m_info->m_driver->SendReply(std::move(m_info->m_data));
    }

    std::unique_ptr<ThreadInfo> m_info;
};

class NBDFlush : public TPTask
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

class NBDTrim : public TPTask
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

class NBDBadRequest : public TPTask
{
public:
    static std::unique_ptr<NBDBadRequest> Create(std::shared_ptr<ThreadInfo> info)
    {
        LOG(LOG_DEBUG, "NBDBadRequest::Create()");
        return std::unique_ptr<NBDBadRequest>(new NBDBadRequest(info));
    }

private:
    NBDBadRequest(std::shared_ptr<ThreadInfo> info) :
        m_driver(info->m_driver),
        m_storage(info->m_storage),
        m_data(std::move(info->m_data))
    {
        ii = info->m_i;
        LOG(LOG_DEBUG, "NBDBadRequest()");
    }

    void Execute() override
    {
        LOG(LOG_DEBUG, "NBDBadRequest::Execute()");
        LOG(LOG_DEBUG, &(ii));

        // m_driver->SendReply(std::move(m_data));
    }
    char ii;
    std::shared_ptr<DriverProxy> m_driver;
	std::shared_ptr<Storage> m_storage;
	std::unique_ptr<DriverData> m_data;
};

}	// end namespace hrd11

#endif // __HRD11_NBD_TASKS_HPP__
