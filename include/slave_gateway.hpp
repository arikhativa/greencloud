
#ifndef __HRD11_SLAVE_GATEWAY_HPP__
#define __HRD11_SLAVE_GATEWAY_HPP__

#include "gateway.hpp"
#include "globals.hpp"
#include "task_args.hpp"
#include "cloud_storage.hpp"

namespace hrd11
{

class SlaveGateway : public GateWay<TEMPLATE>
{

public:
    SlaveGateway(std::shared_ptr<DriverProxy> driver,
				    std::shared_ptr<CloudStorage> storage,
                    int socket);

    ~SlaveGateway() = default;

    virtual int GetFd() override;
    virtual std::pair<TEMPLATE> Read(RequestEngine<TEMPLATE>* req_eng) override;

private:
    std::unique_ptr<DriverData> CreateDataFromBuff(char* src);

    std::shared_ptr<DriverProxy> m_driver;
    std::shared_ptr<CloudStorage> m_storage;
    int m_socket;

};

}	// end namespace hrd11

#endif // __HRD11_SLAVE_GATEWAY_HPP__
