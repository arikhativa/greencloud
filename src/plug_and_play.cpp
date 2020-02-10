// C include ------------------------------------------------------------------
#include <sys/inotify.h>    // inotify_init()
#include <unistd.h>         // read()
#include <dlfcn.h>          // dlopen()

// C++ include ----------------------------------------------------------------
#include <string>       // std::string
#include <thread>       // std::thread

// Local include --------------------------------------------------------------
#include "epoll.hpp"
#include "plug_and_play.hpp"
#include "dispatcher_callback.hpp"
#include "handleton.hpp"
#include "logger.hpp"
#include "globals.hpp"

#define LOG(lvl, msg) s_log->Write(lvl, msg, __FILE__, __LINE__)

namespace hrd11
{

// Globals --------------------------------------------------------------------
static Logger* s_log = Handleton<Logger>::GetInstance(LOG_PATH, LOG_LVL);


// DirMonitor -----------------------------------------------------------------
DirMonitor::DirMonitor(const std::string& dir_path) :
        m_thread_stt(DirMonitor::ThreadStatus::INIT),
        m_monitor(&DirMonitor::Monitor, this, dir_path)
{
    LOG(LOG_DEBUG, "DirMonitor()");
}

DirMonitor::~DirMonitor()
{
    LOG(LOG_DEBUG, "~DirMonitor()");

    while (INIT == GetThreadStatus())
    {}

    SetThreadStatus(END_THREAD);

    LOG(LOG_DEBUG, "~DirMonitor() - thread.join()");
    m_monitor.join();
    LOG(LOG_DEBUG, "~DirMonitor() - end");
}

Dispatcher<std::string>* DirMonitor::GetDispatcher()
{
    return &m_dispatcher;
}

DirMonitor::ThreadStatus DirMonitor::GetThreadStatus()
{
    return m_thread_stt;
}

void DirMonitor::SetThreadStatus(DirMonitor::ThreadStatus stt)
{
    m_thread_stt = stt;
}

void DirMonitor::Monitor(const std::string& dir_path)
{
    LOG(LOG_DEBUG, "Monitor()");

    Epoll epoll(1);
    int m_dir_fd;
    int stt = 0;
    // buf is allocated the same way the manual page for inotify alloc it.
    char buf[4096]__attribute__ ((aligned(__alignof__(struct inotify_event))));
    struct inotify_event *event;
    std::string full_name;

    m_dir_fd = inotify_init1(IN_NONBLOCK);
    if (0 > m_dir_fd)
    {
        SetThreadStatus(ERROR);
        LOG(LOG_ERROR, "Monitor() - inotify_init1() fail");
    }

    errno = 0;

    stt = inotify_add_watch(m_dir_fd, dir_path.c_str(), IN_CREATE);
    if (0 > stt)
    {
        SetThreadStatus(ERROR);
        LOG(LOG_ERROR, "Monitor() - inotify_add_watch() fail");
    }

    epoll.Add(m_dir_fd, Epoll::READ_FD);

    SetThreadStatus(RUNING);

    LOG(LOG_DEBUG, "Monitor() - begin to monitor directory");
    while (RUNING == GetThreadStatus())
    {
        stt = epoll.WaitTimeOut(3000);  // timeout in every 3 sec

        if (0 < stt) // Epoll not timed out
        {
            read(m_dir_fd, buf, sizeof(buf));
            event = (struct inotify_event*)buf;

            full_name += dir_path;
            full_name += "/";
            full_name += (char*)event->name;

            m_dispatcher.NotifyAll(full_name);
        }
    }

    close(m_dir_fd);
    SetThreadStatus(SUCCESS);
    LOG(LOG_DEBUG, "Monitor() - exit");
}

typedef void (*fp_t)(void);

// DL_Loader ------------------------------------------------------------------
DL_Loader::DL_Loader(Dispatcher<std::string>* dispatcher) :
        m_callback(dispatcher, *this, &DL_Loader::LoadLib, nullptr)
{
    LOG(LOG_DEBUG, "DL_Loader()");
}

DL_Loader::~DL_Loader()
{
    LOG(LOG_DEBUG, "~DL_Loader()");

    for (size_t i = 0; i < m_fds.size(); ++i)
    {
        if (dlclose(m_fds[i]))
        {
            LOG(LOG_ERROR, "~DL_Loader() - dlclose() fail");
        }
    }

}

void DL_Loader::LoadLib(const std::string& path)
{
    LOG(LOG_DEBUG, "LoadLib()");

    void *handle = 0;

    handle = dlopen(path.c_str(), RTLD_LAZY);
    if (!handle)
    {
        printf("-- can't open - %s\n", path.c_str());
        printf("%s\n", dlerror());
        LOG(LOG_ERROR, "LoadLib() - dlopen() fail");
    }

    m_fds.push_back(handle);
    LOG(LOG_DEBUG, "LoadLib() - end");
}

}   // end namespace hrd11
