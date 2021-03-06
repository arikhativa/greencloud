
#ifndef __HRD11_STDIN_GATEWAY_HPP__
#define __HRD11_STDIN_GATEWAY_HPP__

#include <memory>			// std::unique_ptr

#include "gateway.hpp"
#include "globals.hpp"
#include "request_engine.hpp"
#include "task_args.hpp"

namespace hrd11
{

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

}	// end namespace hrd11

#endif // __HRD11_STDIN_GATEWAY_HPP__
