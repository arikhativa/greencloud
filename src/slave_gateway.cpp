
#include <cstring>      // strcpy()
#include <netdb.h>      // struct sockaddr_in

#include "slave_gateway.hpp"
#include "cloud_storage.hpp"


#define BUFF_SIZE 4136


namespace hrd11
{

SlaveGateway::SlaveGateway(std::shared_ptr<DriverProxy> driver,
                            std::shared_ptr<CloudStorage> storage,
                            int socket) :
    m_driver(driver),
    m_storage(storage),
    m_socket(socket)
{

}


int SlaveGateway::GetFd()
{
    return m_socket;
}

std::pair<TEMPLATE> SlaveGateway::Read(RequestEngine<TEMPLATE>* req_eng)
{
    struct sockaddr_in nothing;
    socklen_t len = 0;
    char buf[BUFF_SIZE] = {0};
    ssize_t stt = 0;

    printf("SlaveGateway::Read()\n");

    (void)req_eng;

    std::pair<TEMPLATE> ret;

    stt = recvfrom(m_socket, buf, BUFF_SIZE, MSG_WAITALL,
        (struct sockaddr *)&nothing, &len);

    if (-1 == stt)
    {
        perror("SlaveGateway::Read() recvfrom()");
        exit(1);
    }

    std::unique_ptr<DriverData> data(CreateDataFromBuff(buf));

    ret.second.reset(new TaskInfo(m_driver, m_storage, std::move(data)));
    ret.first = FactoryKey::REPLAY;

    return ret;
}


std::unique_ptr<DriverData> SlaveGateway::CreateDataFromBuff(char* src)
{
    static const int BASE_TEN = 10;
    bool stt = 0;

    std::unique_ptr<DriverData> data(new DriverData);

    char* runner = src;

    data->m_offset = 0;
    data->m_len = strtoul(runner, &runner, BASE_TEN);

    stt = m_storage->IdToHandler(strtol((runner + 1), &runner, BASE_TEN),
    data->m_handler);

    if (false == stt)
    {
        perror("CreateDataFromBuff() - IdToHandler()");
        exit(1);
    }

    data->m_key = static_cast<FactoryKey>((*(runner + 1)) - '0');

    if (FactoryKey::READ == data->m_key)
    {
        printf("read -%s\n\n", runner);
        data->m_buff.reserve(BUFF_SIZE + 1);
        strcpy(data->m_buff.data(), (runner + 4));
    }

    return std::move(data);
}

}	// end namespace hrd11
