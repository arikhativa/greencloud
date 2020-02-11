
#ifndef __HRD11_STDIN_GATEWAY_HPP__
#define __HRD11_STDIN_GATEWAY_HPP__

#include <memory>			// std::unique_ptr

#include "gateway.hpp"
#include "logger.hpp"
#include "globals.hpp"
#include "request_engine.hpp"
#include "task_args.hpp"

#define LOG(lvl, msg) s_log->Write(lvl, msg, __FILE__, __LINE__)


namespace hrd11
{

// Globals --------------------------------------------------------------------
static Logger* s_log = Handleton<Logger>::GetInstance(LOG_PATH, LOG_LVL);


class StdinGateWay : public GateWay<TEMPLATE>
{

public:
    StdinGateWay(std::shared_ptr<DriverProxy> driver,
				    std::shared_ptr<Storage> storage);
    ~StdinGateWay() = default;

    virtual int GetFd() override;
    virtual std::pair<TEMPLATE> Read(RequestEngine<TEMPLATE>* req_eng) override;

private:
    static const unsigned int BUFF_SIZE = 100;

    int m_fd;
    std::shared_ptr<DriverProxy> m_driver;
    std::shared_ptr<Storage> m_storage;

};

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
		Logger* log = Handleton<Logger>::GetInstance(LOG_PATH, LOG_LVL);
		log->Write(LOG_ERROR, "HundelInputFD() fail to read from stdin",
								__FILE__, __LINE__);


		throw std::runtime_error("HundelInputFD() fail to read from stdin");
	}

	if('q' == input[0])
    {
        ret.first = FactoryKey::Q_EXIT;
        ret.second.reset(new TaskInfo(m_driver, m_storage, nullptr));


        printf("stop\n\n");
        req_eng->Stop();
    }

    return ret;
}

}	// end namespace hrd11

#endif // __HRD11_STDIN_GATEWAY_HPP__
