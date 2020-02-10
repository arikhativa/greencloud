
// C include ------------------------------------------------------------------
#include <unistd.h>			// read()
#include <signal.h>			// sigaction
#include <errno.h>      	// errno

// C++ include ----------------------------------------------------------------
#include <memory>			// std::unique_ptr, std::shared_ptr
#include <iostream>			// std::cerr
#include <thread>			// std::thread

// local include --------------------------------------------------------------
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


#define LOG(lvl, msg) s_log->Write(lvl, msg, __FILE__, __LINE__)

using namespace hrd11;

// Declarations ---------------------------------------------------------------
static void PrintUsege();
static int HundelInputFD();

static void ExitLoop(int sig);
static void SigAdd(struct sigaction& act, int sig);
static void Sigaction(struct sigaction& act, int sig);
static void SigEmpty(struct sigaction& act);
static void SetSigMask();
static void MainThreadSetSignals();
static void RunGreenCloude(std::shared_ptr<DriverProxy> driver,
							std::shared_ptr<Storage> storage);
static void AddTasksToFactory(FACTORY* factory);

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


// Static funcs ---------------------------------------------------------------

static void RunGreenCloude(std::shared_ptr<DriverProxy> driver,
							std::shared_ptr<Storage> storage)
{
	std::unique_ptr<Monitor> monitor(new Epoll(MAX_EVENTS_TO_WAKE));
	std::unique_ptr<DriverData> data;
	std::unique_ptr<ThreadInfo> current_info;
	ThreadPool thread_pool;
	DirMonitor subject(PLUGINS_PATH);
	DL_Loader observer(subject.GetDispatcher());

	FACTORY* factory = Handleton<FACTORY>::GetInstance();

	AddTasksToFactory(factory);

	monitor->Add(STDIN_FILENO, Epoll::READ_FD);
	monitor->Add(driver->GetReqFd(), Epoll::READ_FD);

	while (1)
	{
		monitor->Wait();

		if (STDIN_FILENO == (*monitor)[0])
		{
			if (END_PROG == HundelInputFD())
			{
				driver->Disconnect();
				goto exit_safe;
			}
		}
		else
		{
			data = driver->ReceiveRequest();

			DataType current_type = data->m_type;
			if (DISCONNECT == current_type)
			{
				driver->Disconnect();
				goto exit_safe;
			}

			current_info.reset(
							new ThreadInfo(driver, storage, std::move(data)));

			thread_pool.AddTask(
						factory->Create(current_type, std::move(current_info)));
		}
	}

	exit_safe:

	printf("RunGreenCloude()\n");
	return ;
}

static void AddTasksToFactory(FACTORY* factory)
{
	factory->Add(READ, NBDRead::Create);
	factory->Add(READ, NBDRead::Create);
	factory->Add(WRITE, NBDWrite::Create);
	factory->Add(FLUSH, NBDFlush::Create);
	factory->Add(TRIM, NBDTrim::Create);
	factory->Add(BAD_REQUEST, NBDBadRequest::Create);
}

static void PrintUsege()
{
	std::cerr << "-- usage green_cloud.0.1.out:\t";
	std::cerr << "size_of_storage, path_to_fd" << std::endl;
	std::cerr << "-- usage green_cloud.0.1.out:\t";
	std::cerr << "size_of_block num_of_blocks path_to_fd" << std::endl;
}

static int HundelInputFD()
{
	char input[BUFF_SIZE];
	int stt = read(STDIN_FILENO, &input, BUFF_SIZE);

	if (0 > stt)
	{
		Logger* log = Handleton<Logger>::GetInstance(LOG_PATH, LOG_LVL);
		log->Write(LOG_ERROR, "HundelInputFD() fail to read from stdin",
								__FILE__, __LINE__);

		throw std::runtime_error("HundelInputFD() fail to read from stdin");
	}

	return ('q' == input[0]);
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
