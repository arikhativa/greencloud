// C include ------------------------------------------------------------------
#include <signal.h>					// sigaction
#include <string.h>					// strlen()

// C++ include ----------------------------------------------------------------
#include <iostream>					// std::cerr
#include <memory>					// std::unique_ptr, std::shared_ptr

#include <boost/property_tree/json_parser.hpp>	// boost::property_tree::ptree
#include <boost/property_tree/ptree.hpp> // boost::property_tree::ptree

// Local include --------------------------------------------------------------
#include "storage.hpp"          	// class interface Storage
#include "driver_proxy.hpp"     	// class interface DriverProxy
#include "gateway.hpp"          	// class interface GateWay

#include "mmap_storage.hpp"      	// class MmapStorage
#include "slave_driver_proxy.hpp" 	// class SlaveDriverProxy
#include "driver_proxy_gateway.hpp"	// class NBDGateWay

#include "request_engine.hpp"       // class RequestEngine
#include "slave_tasks.hpp"          // SlaveRead::Create/Write::Create()

#define PORT 1111

using namespace hrd11;

// Declarations ---------------------------------------------------------------

static void ExitLoop(int sig);
static void SigAdd(struct sigaction& act, int sig);
static void Sigaction(struct sigaction& act, int sig);
static void SigEmpty(struct sigaction& act);
static void SetSigMask();
static void MainThreadSetSignals();

static void RunSlave(std::shared_ptr<DriverProxy> driver,
							std::shared_ptr<Storage> storage,
							const std::string& plugins_path);
static void AddTasksToReqEngine(REQUEST_ENGINE* req_eng);
static void PrintUsege();

enum ExitStatus
{
	SUCCESS = 0,
	ARGUMENTS = 1,
	EXCEPTION = 2
};

// Globals --------------------------------------------------------------------
static const unsigned int CONVTER_TO_MB = 1024 * 1024;
static const char *first_msg = "Hi Master I am your slave";
static boost::property_tree::ptree* g_root;

int main()
{
    std::shared_ptr<DriverProxy> driver;
    std::shared_ptr<Storage> storage;

	boost::property_tree::ptree tmp;
	boost::property_tree::read_json(SLAVE_JSON_PATH, tmp);
	g_root = &tmp;

	size_t storage_size = g_root->get<size_t>("storage_size");
	std::string ip = g_root->get<std::string>("ip");
	unsigned short port = g_root->get<unsigned short>("port");
	std::string storage_file_path = g_root->get<std::string>("storage_file_path");
	std::string plugins_path = g_root->get<std::string>("plugins_dir_path");

	if (!storage_size)
	{
		PrintUsege();
		exit(ARGUMENTS);
	}

    try
    {
        MainThreadSetSignals();

        storage_size *= CONVTER_TO_MB;

        driver.reset(new SlaveDriverProxy(ip, port));
        storage.reset(new MmapStorage(storage_file_path, storage_size));

        RunSlave(driver, storage, plugins_path);
    }
    catch (EpollError& e)
    {
        // Epoll throws exception when a signal is recived
        if (EINTR == e.m_errno)
        {
            driver->Disconnect();
        }
    }
    catch (std::runtime_error& e)
    {
        std::cerr << e.what();
        exit(EXCEPTION);
    }

    return SUCCESS;
}


// Static Functions -----------------------------------------------------------

static void RunSlave(std::shared_ptr<DriverProxy> driver,
							std::shared_ptr<Storage> storage,
							const std::string& plugins_path)
{
	std::unique_ptr<GateWay<TEMPLATE>> slave_gateway(
									new DriverProxyGateWay(driver, storage));

	size_t threads = g_root->get<size_t>("num_of_threads");

	RequestEngine<TEMPLATE>* req_eng = Handleton<RequestEngine<TEMPLATE>>
										::GetInstance(plugins_path, threads);

	AddTasksToReqEngine(req_eng);

	req_eng->AddGateWay(std::move(slave_gateway));

    std::unique_ptr<DriverData> data(new DriverData);

    data->m_buff.reserve(strlen(MAGIC_STR) + 1);
    strcpy(data->m_buff.data(), MAGIC_STR);
    data->m_len = 0;
    data->m_req_id = 0;
    data->m_key = FactoryKey::READ;


    driver->SendReply(std::move(data));
	printf("sent\n");

	req_eng->Run();
}

static void AddTasksToReqEngine(REQUEST_ENGINE* req_eng)
{
	req_eng->AddTask(FactoryKey::READ, &SlaveRead::Create);
	req_eng->AddTask(FactoryKey::WRITE, &SlaveWrite::Create);
	req_eng->AddTask(FactoryKey::DISCONNECT, &SlaveDisconnect::Create);
}

static void PrintUsege()
{
	std::cerr << "-- slave.out ERROR\t";
	std::cerr << "size_of_storage it set the 0" << std::endl;
	std::cerr << "see slave_condig.json" << std::endl;
}

// Signals --------------------------------------------------------------------

static void ExitLoop(int sig)
{
	(void)sig;
}

static void SigAdd(struct sigaction& act, int sig)
{
	int stt = 0;

	stt = sigaddset(&act.sa_mask, sig);
	if (stt)
	{
		throw std::runtime_error("SigAdd() - fail to set signals");
	}
}

static void Sigaction(struct sigaction& act, int sig)
{
	int stt = 0;

	stt = sigaction(sig, &act, NULL);
	if (stt)
	{
		throw std::runtime_error("Sigaction() fail to set signals");
	}
}

static void SigEmpty(struct sigaction& act)
{
	int stt = 0;

	act.sa_handler = ExitLoop;
	act.sa_flags = 0;

	stt = sigemptyset(&act.sa_mask);
	if (stt)
	{
		throw std::runtime_error("SigEmpty() fail to set signals");
	}
}

static void SetSigMask()
{
	int stt = 0;

	sigset_t mask;

	stt = sigfillset(&mask);
	stt += sigdelset(&mask, SIGINT);
	stt += sigdelset(&mask, SIGTERM);
	stt += sigprocmask(SIG_SETMASK, &mask, 0);

	if (stt)
	{
				throw std::runtime_error("SetSigMask() fail to set signals");
	}
}

static void MainThreadSetSignals()
{
		struct sigaction act;

	SetSigMask();

	SigEmpty(act);
	SigAdd(act, SIGINT);
	SigAdd(act, SIGTERM);
	Sigaction(act, SIGINT);
	Sigaction(act, SIGTERM);

}
