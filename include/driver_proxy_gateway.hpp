
/*
    DriverProxyGateWay -

    Discription:    This class will Read request from NBD fd and send
                    them to RequestEngine.

    Usage:          Create an instance and use AddGateWat().
                    see request_engine.hpp

    Date:           12.2.2020

    Ver 1.0
*/

#ifndef __HRD11_NBD_GATEWAY_HPP__
#define __HRD11_NBD_GATEWAY_HPP__

#include <memory>			// std::unique_ptr

#include "gateway.hpp"
#include "globals.hpp"
#include "task_args.hpp"

namespace hrd11
{

class DriverProxyGateWay : public GateWay<TEMPLATE>
{

public:
    DriverProxyGateWay(std::shared_ptr<DriverProxy> driver,
				    std::shared_ptr<Storage> storage);
    ~DriverProxyGateWay() = default;

    virtual int GetFd() override;
    virtual std::pair<TEMPLATE> Read(RequestEngine<TEMPLATE>* req_eng) override;

private:
    std::shared_ptr<DriverProxy> m_driver;
    std::shared_ptr<Storage> m_storage;
};


}	// end namespace hrd11

#endif // __HRD11_NBD_GATEWAY_HPP__
