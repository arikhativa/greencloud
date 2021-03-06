
#include "driver_proxy_gateway.hpp"

#include "handleton.hpp"
#include "logger.hpp"

#define LOG(lvl, msg) s_log->Write(lvl, msg, __FILE__, __LINE__)

namespace hrd11
{

// Globals --------------------------------------------------------------------
static Logger* s_log = Handleton<Logger>::GetInstance(LOG_PATH, LOG_LVL);


// Members --------------------------------------------------------------------
DriverProxyGateWay::DriverProxyGateWay(std::shared_ptr<DriverProxy> driver,
                            std::shared_ptr<Storage> storage) :
                            m_driver(driver),
                            m_storage(storage)
{
    LOG(LOG_INFO, "DriverProxyGateWay()");
}

int DriverProxyGateWay::GetFd()
{
    return m_driver->GetReqFd();
}

std::pair<TEMPLATE> DriverProxyGateWay::Read(RequestEngine<TEMPLATE>* req_eng)
{
    std::pair<TEMPLATE> ret;
    std::unique_ptr<DriverData> data(m_driver->ReceiveRequest());

    ret.first = data->m_key;
    ret.second = std::unique_ptr<TaskInfo>(
                        new TaskInfo(m_driver, m_storage, std::move(data)));

	if(FactoryKey::DISCONNECT == ret.first)
    {
        LOG(LOG_INFO, "DriverProxyGateWay::Read() - FactoryKey::DISCONNECT");
        req_eng->Stop();
    }

    return ret;
}

}	// end namespace hrd11
