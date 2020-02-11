
// C include ------------------------------------------------------------------
#include <unistd.h>			// read()
#include <signal.h>			// sigaction
#include <errno.h>      	// errno

// C++ include ----------------------------------------------------------------
#include <memory>			// std::unique_ptr, std::shared_ptr
#include <iostream>			// std::cerr
#include <thread>			// std::thread

// Local include --------------------------------------------------------------
#include "nbd_driver_proxy.hpp"
#include "disk_storage.hpp"
#include "driver_data.hpp"
#include "epoll.hpp"
#include "monitor.hpp"
#include "handleton.hpp"
#include "logger.hpp"
#include "globals.hpp"
#include "thread_pool.hpp"
#include "tptask.hpp"
#include "tp_sys_task.hpp"
#include "factory.hpp"
#include "nbd_tasks.hpp"
#include "plug_and_play.hpp"

#include "request_engine.hpp"
#include "task_args.hpp"
#include "gateway.hpp"
#include "stdin_tasks.hpp"
#include "stdin_gateway.hpp"


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
							std::shared_ptr<Storage> storage);
static void AddTasksToReqEngine(REQUEST_ENGINE* req_eng);


enum ExitStatus
{
	SUCCESS = 0,
	ARGUMENTS = 1,
	EXCEPTION = 2
};

// Globals --------------------------------------------------------------------
static Logger* s_log = Handleton<Logger>::GetInstance(LOG_PATH, LOG_LVL);
static const char* STORAGE_FILE_PATH = "./storage.ignore";
static const unsigned int CONVTER_TO_MB = 1024 * 1024;
static const unsigned int MAX_EVENTS_TO_WAKE = 1;
static const unsigned int BUFF_SIZE = 100;
static const unsigned int END_PROG = 1;


// Main -----------------------------------------------------------------------
int main(int ac, char* av[])
{
	std::shared_ptr<DriverProxy> driver;
	std::shared_ptr<Storage> storage;

	try
	{
		MainThreadSetSignals();

		switch (ac)
		{
			case 3:
			{
				size_t size = std::stoull(av[1]);

				size *= CONVTER_TO_MB;

				driver.reset(new NBDDriverProxy(size, av[2]));
				storage.reset(new DiskStorage(STORAGE_FILE_PATH, size));
				break ;
			}
			case 4:
			{
				size_t blk_size = std::stoull(av[1]);
				size_t num_of_blocks = std::stoull(av[2]);

				driver.reset(new NBDDriverProxy(blk_size, num_of_blocks,
					 							av[3]));
				storage.reset(new DiskStorage(STORAGE_FILE_PATH,
					 							blk_size * num_of_blocks));
				break ;
			}
			default:
				PrintUsege();
				exit(ARGUMENTS);
		}

		RunGreenCloude(driver, storage);
		printf("aaaaa\n");
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
	printf("end\n");

	return SUCCESS;
}


// Static funcs ---------------------------------------------------------------

static void RunGreenCloude(std::shared_ptr<DriverProxy> driver,
							std::shared_ptr<Storage> storage)
{
	std::unique_ptr<GateWay<TEMPLATE>> input(new StdinGateWay(driver, storage));

	RequestEngine<FactoryKey , std::unique_ptr<TaskInfo>> req_eng(PLUGINS_PATH, 1);

	AddTasksToReqEngine(&req_eng);

	req_eng.AddGateWay(std::move(input));

	printf("going into run\n");
	req_eng.Run();

	printf("out run\n");

	return ;
}

static void AddTasksToReqEngine(REQUEST_ENGINE* req_eng)
{
	req_eng->AddTask(FactoryKey::Q_EXIT, STDINExit::Create);
	req_eng->AddTask(FactoryKey::IGNORE, STDINIgnore::Create);

	// req_eng->AddTask(FactoryKey::READ, NBDRead::Create);
	// req_eng->AddTask(FactoryKey::WRITE, NBDWrite::Create);
	// req_eng->AddTask(FactoryKey::FLUSH, NBDFlush::Create);
	// req_eng->AddTask(FactoryKey::TRIM, NBDTrim::Create);
	// req_eng->AddTask(FactoryKey::BAD_REQUEST, NBDBadRequest::Create);
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
		Logger* log = Handleton<Logger>::GetInstance(LOG_PATH, LOG_LVL);
		log->Write(LOG_ERROR, "SigAdd() fail to set signals",
					__FILE__, __LINE__);

		throw std::runtime_error("SigAdd() - fail to set signals");
	}
}

static void Sigaction(struct sigaction& act, int sig)
{
	int stt = 0;

	stt = sigaction(sig, &act, NULL);
	if (stt)
	{
		Logger* log = Handleton<Logger>::GetInstance(LOG_PATH, LOG_LVL);
		log->Write(LOG_ERROR, "Sigaction() fail to set signals",
					__FILE__, __LINE__);

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
		Logger* log = Handleton<Logger>::GetInstance(LOG_PATH, LOG_LVL);
		log->Write(LOG_ERROR, "SigEmpty() fail to set signals",
					__FILE__, __LINE__);

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
		Logger* log = Handleton<Logger>::GetInstance(LOG_PATH, LOG_LVL);
		log->Write(LOG_ERROR, "SetSigMask() fail to set signals",
					__FILE__, __LINE__);

		throw std::runtime_error("SetSigMask() fail to set signals");
	}
}

static void MainThreadSetSignals()
{
	Logger* log = Handleton<Logger>::GetInstance(LOG_PATH, LOG_LVL);
	log->Write(LOG_DEBUG, "setting main thread signls", __FILE__, __LINE__);

	struct sigaction act;

	SetSigMask();

	SigEmpty(act);
	SigAdd(act, SIGINT);
	SigAdd(act, SIGTERM);
	Sigaction(act, SIGINT);
	Sigaction(act, SIGTERM);
	log->Write(LOG_INFO, "main thread signls are set", __FILE__, __LINE__);
}
