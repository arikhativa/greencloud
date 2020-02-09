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

namespace hrd11
{

// Declarations ---------------------------------------------------------------


// DirMonitor -----------------------------------------------------------------
DirMonitor::DirMonitor(const std::string& dir_path) :
        m_exit(DirMonitor::ThreadStatus::INIT),
        m_monitor(&DirMonitor::Monitor, this, dir_path)
{}

DirMonitor::~DirMonitor()
{
    m_monitor.join();
}


Dispatcher<std::string>* DirMonitor::GetDispatcher()
{
    return &m_dispatcher;
}

void DirMonitor::Monitor(const std::string& dir_path)
{
    Epoll epoll(1);
    int m_dir_fd;
    int stt = 0;
    char buf[4096]__attribute__ ((aligned(__alignof__(struct inotify_event))));
    struct inotify_event *event;
    std::string full_name;

    m_dir_fd = inotify_init1(IN_NONBLOCK);

    errno = 0;

    stt = inotify_add_watch(m_dir_fd, dir_path.c_str(), IN_CREATE);
    if (-1 == stt)
    {
        perror("");
        // LOG
    }

    epoll.Add(m_dir_fd, Epoll::READ_FD);

    while (1)
    {
        stt = epoll.WaitTimeOut(-1);

        if (!stt) // Epoll timed out
        {
            printf("out!\n");
            break ;
            // CheckMainThreadStatus();
        }
        else
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
}

typedef void (*fp_t)(void);

// DL_Loader ------------------------------------------------------------------
DL_Loader::DL_Loader(Dispatcher<std::string>* dispatcher) :
        m_callback(dispatcher, *this, &DL_Loader::LoadLib, nullptr)
{}



void DL_Loader::LoadLib(const std::string& path)
{
    void *handle = 0;
    fp_t func = 0;

    printf("LoadLib()\n%s\n", path.c_str());

    // handle = dlopen("./plugins/libtest.so", RTLD_LAZY);
    handle = dlopen(path.c_str(), RTLD_LAZY);
    if (!handle)
    {
        perror("handel\n");
        exit(1);
    }
    // func = (fp_t)dlsym(handle, "Foo");
    // if (!func)
    // {
    //     perror("func\n");
    //     exit(1);
    // }
    //
    // func();
}



// Static Functions -----------------------------------------------------------

}   // end namespace hrd11
