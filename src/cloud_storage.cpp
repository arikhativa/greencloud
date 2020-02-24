

// C include ------------------------------------------------------------------
#include <unistd.h>     // close()
#include <cstring>      // strcmp()


#include <arpa/inet.h>      // inet_pton()

#include "globals.hpp"
#include "cloud_storage.hpp"

#define BUFF_SIZE 4136

namespace hrd11
{


// SlaveData ------------------------------------------------------------------
SlaveData::SlaveData(size_t port)
{
    struct sockaddr_in servaddr;
    char buff[BUFF_SIZE] = {0};
    char *runner = buff;
    int stt = 0;
    socklen_t len = 0;

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&m_cliaddr, 0, sizeof(m_cliaddr));

    m_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (-1 == m_socket)
    {
        perror("SlaveData() socket()");
        exit(1);

    }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    stt = bind(m_socket, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (0 > stt)
    {
	    perror("SlaveData() bind()");
        exit(1);

    }

    len = sizeof(m_cliaddr);

    do
    {
        printf("port %lu waiting for connections...\n", port);
        stt = recvfrom(m_socket, buff, BUFF_SIZE, MSG_WAITALL,
                                            (struct sockaddr *)&m_cliaddr, &len);
        if (0 > stt)
        {
            perror("SlaveData() - recvfrom()\n");
            exit(1);

        }
        while (*runner)
        {
            ++runner;
        }
        ++runner;

    } while(strcmp(runner, MAGIC_STR));
}

void SlaveData::Disconnect()
{
    close(m_socket);
}

// CloudStorage --------------------------------------------------------------
CloudStorage::CloudStorage(size_t num_of_slaves, size_t storage_size) :
    m_num_of_slaves(num_of_slaves),
    m_storage_size(storage_size),
    m_req_id(0)
{
    size_t port = 6666;
    Offset offset(0);

    for (size_t i = 1; (i - 1) < num_of_slaves; ++i)
    {
        AddSlave(offset, port * i);
    }
    printf("added slave \n");
}

CloudStorage::~CloudStorage()
{
    // TODO close all slaves's sockets
}

void CloudStorage::AddSlave(Offset slave_offset, size_t port)
{
    m_slaves[slave_offset] = SlaveData(port);
}

std::unique_ptr<DriverData> CloudStorage::Write(std::unique_ptr<DriverData> data)
{
    ssize_t stt = 0;
    SlaveData slave(GetSlaveData(data->m_offset));

    char buf[BUFF_SIZE] = {0};
    char* runner = buf;

    sprintf(buf, "%lu;%u;%d;%c;",data->m_offset,
                                    data->m_len,
                                    HandlerToId(data->m_handler),
                                    '1');

    while (*runner)
    {
        ++runner;
    }
    memcpy((runner + 1), data->m_buff.data(), data->m_len);

    stt = sendto(slave.m_socket,
            buf, BUFF_SIZE, MSG_CONFIRM,
            (struct sockaddr *)&(slave.m_cliaddr),
            sizeof(struct sockaddr));

    if (-1 == stt)
    {
        perror("CloudStorage::Write() sendto()");
        exit(1);
    }

    return std::unique_ptr<DriverData>(nullptr);
}

std::unique_ptr<DriverData> CloudStorage::Read(std::unique_ptr<DriverData> data)
{
    ssize_t stt = 0;
    SlaveData slave(GetSlaveData(data->m_offset));

    char buf[BUFF_SIZE] = {0};

    sprintf(buf, "%lu;%u;%d;%c;",data->m_offset, data->m_len,
            HandlerToId(data->m_handler), '0');

    stt = sendto(slave.m_socket, buf, BUFF_SIZE, MSG_CONFIRM,
        (struct sockaddr *)&(slave.m_cliaddr), sizeof(struct sockaddr));

    if (-1 == stt)
    {
        perror("CloudStorage::Read() sendto()");

        exit(1);
    }

    return std::unique_ptr<DriverData>(nullptr);
}


size_t CloudStorage::GetSize()
{
    return m_storage_size;
}

void CloudStorage::Disconnect()
{
    ssize_t stt = 0;

    char buf[BUFF_SIZE] = {0};

    strcpy(buf, "0;0;0;2;");

    auto iter = m_slaves.begin();
    auto end = m_slaves.end();

    while (iter != end)
    {
        stt = sendto(iter->second.m_socket, buf, BUFF_SIZE, MSG_CONFIRM,
        (struct sockaddr *)&(iter->second.m_cliaddr), sizeof(struct sockaddr));

        if (-1 == stt)
        {
            perror("CloudStorage::Disconnect() sendto()");
            exit(1);
        }

        ++iter;
    }
}


SlaveData CloudStorage::GetSlaveData(off_t offset)
{
    return m_slaves[Offset(offset)];
}

std::vector<int> CloudStorage::GetAllFds()
{
    std::vector<int> fds(m_num_of_slaves);

    auto iter = m_slaves.begin();

    for (size_t i = 0; i < m_num_of_slaves; ++i)
    {
        fds[i] = iter->second.m_socket;
        ++iter;
    }

    return std::move(fds);
}



int CloudStorage::HandlerToId(char handler[HANDLE_SIZE])
{
    int ret = 0;

    {
        std::lock_guard<std::mutex> lock(m_mutex);
        ret = ++m_req_id;
        std::pair<int, char[HANDLE_SIZE]> pair;
        pair.first = m_req_id;

        // do not use strncpy() in here.
        // handler is not a string! it is 8 beyts.
        // it could have '\0' in the middel.
        memcpy(pair.second, handler, HANDLE_SIZE);

        m_list.push_front(pair);

        // printf("-- HandlerToId() handler \n");
        // PPP(handler);

    }

    return ret;
}

bool CloudStorage::IdToHandler(int req_id, char *ret)
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        auto iter = m_list.begin();
        auto end = m_list.end();



        while (req_id != iter->first && iter != end)
        {
            // printf("-- iter->first %d \n", iter->first);
            // printf("-- req_id %d \n", req_id);

            ++iter;
        }
        if (iter == end)
        {
            return false;
        }

        // do not use strncpy() in here.
        // handler is not a string! it is 8 beyts.
        // it could have '\0' in the middel.
        memcpy(ret, iter->second, HANDLE_SIZE);

        m_list.remove(*iter);
        // printf("-- IdToHandler() handler \n");
        // PPP(ret);
    }

    return true;
}


}	// end namespace hrd11
