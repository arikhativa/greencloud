// C include ------------------------------------------------------------------
#include <signal.h>					// sigaction
#include <string.h>					// strlen()

// C++ include ----------------------------------------------------------------
#include <iostream>					// std::cerr
#include <memory>					// std::unique_ptr, std::shared_ptr

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
							std::shared_ptr<Storage> storage);
static void AddTasksToReqEngine(REQUEST_ENGINE* req_eng);
static void PrintUsege();

enum ExitStatus
{
	SUCCESS = 0,
	ARGUMENTS = 1,
	EXCEPTION = 2
};

// Globals --------------------------------------------------------------------
static const char* STORAGE_FILE_PATH = "./storage.ignore";
static const unsigned int CONVTER_TO_MB = 1024 * 1024;
static const char *first_msg = "Hi Master I am your slave";

int main(int ac, char *av[])
{
    std::shared_ptr<DriverProxy> driver;
    std::shared_ptr<Storage> storage;

    try
    {
        MainThreadSetSignals();

        switch (ac)
        {
            case 2:
            {
                size_t size = std::stoull(av[1]);

                size *= CONVTER_TO_MB;

                driver.reset(new SlaveDriverProxy(PORT));
                storage.reset(new MmapStorage(STORAGE_FILE_PATH, size));
                break ;
            }

            default:
                PrintUsege();
                exit(ARGUMENTS);
        }

        RunSlave(driver, storage);

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
							std::shared_ptr<Storage> storage)
{
	std::unique_ptr<GateWay<TEMPLATE>> slave_gateway(
											new DriverProxyGateWay(driver, storage));


                                            // TODO REQ is ONLY 1 THREAD
	RequestEngine<TEMPLATE>* req_eng = Handleton<RequestEngine<TEMPLATE>>
												::GetInstance(PLUGINS_PATH);

	AddTasksToReqEngine(req_eng);

	req_eng->AddGateWay(std::move(slave_gateway));

    std::unique_ptr<DriverData> data(new DriverData);

    // data->mm_buff = new char[strlen(first_msg) + 1];
    data->m_buff.reserve(strlen(first_msg) + 1);
    strcpy(data->m_buff.data(), first_msg);
    data->m_len = 0;
    data->m_req_id = 0;
    data->m_key = FactoryKey::READ;

	printf("sent\n");

    driver->SendReply(std::move(data));

	req_eng->Run();
}

static void AddTasksToReqEngine(REQUEST_ENGINE* req_eng)
{
	req_eng->AddTask(FactoryKey::READ, &SlaveRead::Create);
	req_eng->AddTask(FactoryKey::WRITE, &SlaveWrite::Create);
}

static void PrintUsege()
{
	std::cerr << "-- usage green_cloud.0.1.out:\t";
	std::cerr << "size_of_storage, path_to_fd" << std::endl;
	std::cerr << "-- usage green_cloud.0.1.out:\t";
	std::cerr << "size_of_block num_of_blocks path_to_fd" << std::endl;
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
