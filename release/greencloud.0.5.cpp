
// C include ------------------------------------------------------------------
#include <signal.h>			// sigaction
#include <errno.h>      	// errno

// C++ include ----------------------------------------------------------------
#include <memory>			// std::unique_ptr, std::shared_ptr
#include <iostream>			// std::cerr
#include <thread>			// std::thread

#include <boost/property_tree/json_parser.hpp>	// boost::property_tree::ptree
#include <boost/property_tree/ptree.hpp> // boost::property_tree::ptree

// Local include --------------------------------------------------------------
#include "handleton.hpp"
#include "logger.hpp"
#include "globals.hpp"
#include "nbd_tasks.hpp"
#include "request_engine.hpp"
#include "gateway.hpp"
#include "stdin_tasks.hpp"
#include "stdin_gateway.hpp"
#include "driver_proxy_gateway.hpp"
#include "storage.hpp"
#include "cloud_storage.hpp"
#include "slave_gateway.hpp"

#define LOG(lvl, msg) s_log->Write(lvl, msg, __FILE__, __LINE__)

using namespace hrd11;

// Declarations ---------------------------------------------------------------
static void PrintUsege();
static void ExitLoop(int sig);
static void SigAdd(struct sigaction& act, int sig);
static void Sigaction(struct sigaction& act, int sig);
static void SigEmpty(struct sigaction& act);
static void SetSigMask();
static void MainThreadSetSignals();
static void RunGreenCloude(std::shared_ptr<DriverProxy> driver,
							std::shared_ptr<CloudStorage> storage);
static void AddTasksToReqEngine(REQUEST_ENGINE* req_eng);


enum ExitStatus
{
	SUCCESS = 0,
	ARGUMENTS = 1,
	EXCEPTION = 2
};

// Globals --------------------------------------------------------------------
static Logger* s_log = Handleton<Logger>::GetInstance(LOG_PATH, LOG_LVL);
static const unsigned int CONVTER_TO_MB = 1024 * 1024;
static const unsigned int MAX_EVENTS_TO_WAKE = 1;
static const unsigned int BUFF_SIZE = 100;
static const unsigned int END_PROG = 1;
static boost::property_tree::ptree* g_root;


// Main -----------------------------------------------------------------------
int main(int ac, char* av[])
{
	std::shared_ptr<DriverProxy> driver;
	std::shared_ptr<CloudStorage> storage;

	boost::property_tree::ptree tmp;
	boost::property_tree::read_json(JSON_PATH, tmp);
	g_root = &tmp;

	size_t storage_size = g_root->get<size_t>("storage_size");
	size_t num_of_slaves = g_root->get<size_t>("num_of_slaves");

	if (1 == ac)
	{
		PrintUsege();
		exit(ARGUMENTS);
	}
	try
	{
		storage_size *= CONVTER_TO_MB;

		driver.reset(new NBDDriverProxy(storage_size, av[1]));
		storage.reset(new CloudStorage(num_of_slaves, storage_size));

		MainThreadSetSignals();
		RunGreenCloude(driver, storage);

	}
	catch (EpollError& e)
	{
		// Epoll throws exception when a signal is recived
		if (EINTR == e.m_errno)
		{
			driver->Disconnect();
			goto exit_safe;
		}
	}
	catch (std::runtime_error& e)
	{
		std::cerr << e.what();
		exit(EXCEPTION);
	}

	exit_safe:

	return SUCCESS;
}


// Static Functions -----------------------------------------------------------

static void RunGreenCloude(std::shared_ptr<DriverProxy> driver,
							std::shared_ptr<CloudStorage> storage)
{
	std::unique_ptr<GateWay<TEMPLATE>> stdin_gateway(
									new StdinGateWay(driver, storage));
	std::unique_ptr<GateWay<TEMPLATE>> nbd_gateway(
									new DriverProxyGateWay(driver, storage));

	std::string tmp = g_root->get<std::string>("plugins_dir_path");
	size_t threads = g_root->get<size_t>("num_of_threads");

	RequestEngine<TEMPLATE>* req_eng = Handleton<RequestEngine<TEMPLATE>>
			::GetInstance(tmp.c_str(), threads);

	AddTasksToReqEngine(req_eng);


	req_eng->AddGateWay(std::move(stdin_gateway));
	req_eng->AddGateWay(std::move(nbd_gateway));

	std::vector<int> fds(storage->GetAllFds());

	for (auto& iter : fds)
	{
		req_eng->AddGateWay(std::unique_ptr<GateWay<TEMPLATE>>(
			new SlaveGateway(driver, storage, iter)));
	}


	req_eng->Run();
	Handleton<RequestEngine<TEMPLATE>>::ReleaseResources();
}

static void AddTasksToReqEngine(REQUEST_ENGINE* req_eng)
{
	req_eng->AddTask(FactoryKey::Q_EXIT, &STDINExit::Create);
	req_eng->AddTask(FactoryKey::IGNORE, &STDINIgnore::Create);

	req_eng->AddTask(FactoryKey::READ, &NBDRead::Create);
	req_eng->AddTask(FactoryKey::WRITE, &NBDWrite::Create);
	req_eng->AddTask(FactoryKey::DISCONNECT, &NBDDisconnect::Create);
	req_eng->AddTask(FactoryKey::FLUSH, &NBDFlush::Create);
	req_eng->AddTask(FactoryKey::TRIM, &NBDTrim::Create);
	req_eng->AddTask(FactoryKey::BAD_REQUEST, &NBDBadRequest::Create);
	req_eng->AddTask(FactoryKey::REPLAY, &NBDReplay::Create);
}

static void PrintUsege()
{
	std::cerr << "-- usage green_cloud.0.1.out:\t";
	std::cerr << "path to nbd" << std::endl;
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
		LOG(LOG_ERROR, "SigAdd() fail to set signals");

		throw std::runtime_error("SigAdd() - fail to set signals");
	}
}

static void Sigaction(struct sigaction& act, int sig)
{
	int stt = 0;

	stt = sigaction(sig, &act, NULL);
	if (stt)
	{
		LOG(LOG_ERROR, "Sigaction() fail to set signals");

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
		LOG(LOG_ERROR, "SigEmpty() fail to set signals");

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
		LOG(LOG_ERROR, "SetSigMask() fail to set signals");

		throw std::runtime_error("SetSigMask() fail to set signals");
	}
}

static void MainThreadSetSignals()
{
	LOG(LOG_DEBUG, "setting main thread signls");

	struct sigaction act;

	SetSigMask();

	SigEmpty(act);
	SigAdd(act, SIGINT);
	SigAdd(act, SIGTERM);
	Sigaction(act, SIGINT);
	Sigaction(act, SIGTERM);

	LOG(LOG_INFO, "main thread signls are set");
}
