
// #include <arpa/inet.h>
// #include <netinet/in.h>

#include <unistd.h>         // close()
#include <string.h>         // strcpy(), strcat()
#include <netdb.h>          // struct sockaddr_in
#include <sys/socket.h>     // socket()
#include <sys/types.h>      // recvfrom()
#include <stdlib.h>         // strtoul()
#include <arpa/inet.h>      // inet_pton()

#include <string>           // std::string

#include "globals.hpp"              // FactoryKey
#include "slave_driver_proxy.hpp"


#define BUFF_SIZE 4136

namespace hrd11
{

// Globals --------------------------------------------------------------------

// Declarations ---------------------------------------------------------------
static int InitSocket();

static char* GetNextParse(char* str);
static char* GetSizeT(size_t* l, char* src);
static char* GetUint(unsigned int* ui, char* src);
static char* GetInt(int* i, char* src);
static char* GetReqKey(FactoryKey* key, char* src);

static void AddMessage(char *dest, const char* src);
static char* AddRequestType(char* dest, FactoryKey key);
static char *AddInt(char *dest, const char* src);


SlaveDriverProxy::SlaveDriverProxy(const std::string& ip,
                                    unsigned short port) :
        m_socket(InitSocket())
{
    m_servaddr.sin_family = AF_INET;
    m_servaddr.sin_port = htons(port);
    m_servaddr.sin_addr.s_addr = INADDR_ANY;
    inet_aton(ip.c_str(), &(m_servaddr.sin_addr));

}

SlaveDriverProxy::~SlaveDriverProxy()
{
    if (-1 != m_socket)
    {
        Disconnect();
    }
}

std::unique_ptr<DriverData> SlaveDriverProxy::ReceiveRequest()
{
    std::unique_ptr<DriverData> ret(new DriverData);
    char buf[BUFF_SIZE + 1] = {0};
    char *runner = buf;
    socklen_t len = BUFF_SIZE;
    ssize_t n = 0;

    n = recvfrom(m_socket, buf, BUFF_SIZE, 0,
                    (struct sockaddr *)&m_servaddr, &len);

    if (-1 == n)
    {
        perror("recvfrom() fail");
    }

    runner = GetSizeT(&ret->m_offset, runner);
    runner = GetUint(&ret->m_len, runner);
    runner = GetInt(&ret->m_req_id, runner);
    runner = GetReqKey(&ret->m_key, runner);

    if (FactoryKey::WRITE == ret->m_key)
    {
        ret->m_buff.reserve(BUFF_SIZE + 1);
        strcpy(ret->m_buff.data(), runner);
    }

    return std::move(ret);
}

void SlaveDriverProxy::SendReply(std::unique_ptr<DriverData> data)
{
    char buf[BUFF_SIZE + 1] = {0};
    char *runner = buf;
    std::string str;

    str = std::to_string(data->m_len);
    runner = AddInt(runner, str.c_str());

    str = std::to_string(data->m_req_id);
    runner = AddInt(runner, str.c_str());

    runner = AddRequestType(runner, data->m_key);

    *runner = '\0';
    ++runner;

    if (FactoryKey::READ == data->m_key)
    {
        AddMessage(runner, data->m_buff.data());
    }

    ssize_t tmp = sendto(m_socket, buf, sizeof(buf), 0,
                            (struct sockaddr *)&m_servaddr, sizeof(m_servaddr));
    if (-1 == tmp)
    {
        perror("sendto() fail");
    }
}

void SlaveDriverProxy::Disconnect()
{
    close(m_socket);
    m_socket = -1;
}

int SlaveDriverProxy::GetReqFd()
{
    return m_socket;
}


// Static Functions -----------------------------------------------------------
// Static Functions Ctor ------------------------------------------------------
static int InitSocket()
{
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0)
    {
	    printf("client: error socket\n");
    }

    return sockfd;
}


// Static Functions ReceiveRequest --------------------------------------------

char* GetNextParse(char* str)
{
    while (*str != ';')
    {
        ++str;
    }

    return (str + 1);
}

char* GetSizeT(size_t* l, char* src)
{
    *l = strtoul(src, nullptr, 10);

    return GetNextParse(src);
}

char* GetUint(unsigned int* ui, char* src)
{
    *ui = strtoul(src, nullptr, 10);

    return GetNextParse(src);
}

char* GetInt(int* i, char* src)
{
    *i = strtoul(src, nullptr, 10);
    // *i = std::stoi(src, nullptr, 10);

    return GetNextParse(src);
}

char* GetReqKey(FactoryKey* key, char* src)
{
    *key = static_cast<FactoryKey>(*src - '0');

    return (src + 3); // skip the current char and ';' and '\0'
}


// Static Functions SendReply -------------------------------------------------

static char *AddInt(char *dest, const char* src)
{
    while (*src)
    {
        *dest = *src;
        ++dest;
        ++src;
    }
    *dest = ';';

    return ++dest;
}

static char* AddRequestType(char* dest, FactoryKey key)
{
    char id = '1';

    if (FactoryKey::READ == key)
    {
        id = '0';
    }

    *dest = id;
    ++dest;
    *dest = ';';

    return ++dest;
}

static void AddMessage(char *dest, const char* src)
{
    while (*src)
    {
        *dest = *src;
        ++dest;
        ++src;
    }
}


}	// end namespace hrd11
