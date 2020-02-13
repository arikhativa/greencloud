

#ifndef __HRD11_SLAVE_GATEWAY_HPP__
#define __HRD11_SLAVE_GATEWAY_HPP__

#include <memory>			// std::unique_ptr

#include "gateway.hpp"
#include "globals.hpp"
#include "task_args.hpp"

namespace hrd11
{

class SlaveGateWay : public GateWay<TEMPLATE>
{

public:
    SlaveGateWay(std::shared_ptr<DriverProxy> driver,
				    std::shared_ptr<Storage> storage);
    ~SlaveGateWay() = default;

    virtual int GetFd() override;
    virtual std::pair<TEMPLATE> Read(RequestEngine<TEMPLATE>* req_eng) override;

private:
    std::shared_ptr<DriverProxy> m_driver;
    std::shared_ptr<Storage> m_storage;
};


}	// end namespace hrd11

#endif // __HRD11_SLAVE_GATEWAY_HPP__
