// C include ------------------------------------------------------------------
#include <sys/inotify.h>    // inotify_init()
#include <unistd.h>         // read()

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
    int wd[2];

    size_t buffsize = sizeof(struct inotify_event) + dir_path.size() + 1;
    struct inotify_event* event = (inotify_event*)new char[buffsize];


    m_dir_fd = inotify_init1(IN_NONBLOCK);

    errno = 0;

    for (int i = 0; i < 2; i++)
    {
       wd[i] = inotify_add_watch(m_dir_fd, ".",
                                 IN_OPEN | IN_CLOSE);
       if (wd[i] == -1)
       {
           perror("");
       }
   }


    // stt = inotify_add_watch(m_dir_fd, dir_path.c_str(), IN_OPEN | IN_CLOSE);
    // if (-1 == stt)
    // {
    //     printf("errno is %d\n", errno);
    //     printf("ENOSPC is %d\n", ENOSPC);
    //
    //     perror("here2\n");
    //     // LOG
    // }

    epoll.Add(m_dir_fd, EPOLLIN);


    while (1)
    {
        stt = epoll.WaitTimeOut(3);

        if (!stt) // Epoll timed out
        {
            break ;
            // CheckMainThreadStatus();
        }
        else
        {
            // read(m_dir_fd, event, buffsize);
            printf("new file!\n");

        }

    }

    close(m_dir_fd);
    delete[] event;
}


// DL_Loader ------------------------------------------------------------------
DL_Loader::DL_Loader(Dispatcher<std::string>* dispatcher) :
        m_callback(dispatcher, *this, &DL_Loader::LoadLib, nullptr)
{}

void DL_Loader::LoadLib(const std::string& path)
{
    (void)path;
}



// Static Functions -----------------------------------------------------------

}   // end namespace hrd11
