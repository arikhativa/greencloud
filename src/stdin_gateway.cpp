
#include "stdin_gateway.hpp"
#include "logger.hpp"

#define LOG(lvl, msg) s_log->Write(lvl, msg, __FILE__, __LINE__)

namespace hrd11
{

// Globals --------------------------------------------------------------------
static Logger* s_log = Handleton<Logger>::GetInstance(LOG_PATH, LOG_LVL);


StdinGateWay::StdinGateWay(std::shared_ptr<DriverProxy> driver,
                            std::shared_ptr<Storage> storage) :
                            m_fd(STDIN_FILENO),
                            m_driver(driver),
                            m_storage(storage)
{}

int StdinGateWay::GetFd()
{
    return m_fd;
}

std::pair<TEMPLATE> StdinGateWay::Read(RequestEngine<TEMPLATE>* req_eng)
{
    std::pair<TEMPLATE> ret;
    char input[BUFF_SIZE];
	int stt = read(STDIN_FILENO, &input, BUFF_SIZE);

    ret.first = FactoryKey::IGNORE;
    ret.second = std::unique_ptr<TaskInfo>(nullptr);

	if (0 > stt)
	{
        LOG(LOG_ERROR, "HundelInputFD() fail to read from stdin");

		throw std::runtime_error("HundelInputFD() fail to read from stdin");
	}

	if('q' == input[0])
    {
        ret.first = FactoryKey::Q_EXIT;
        ret.second.reset(new TaskInfo(m_driver, m_storage, nullptr));

        req_eng->Stop();
    }

    return ret;
}

}	// end namespace hrd11
