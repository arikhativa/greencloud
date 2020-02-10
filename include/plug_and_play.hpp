
/*
    Plug & play :
    Based on "observer / pub-sub" design pattern.
    It enable to add tasks to the DriverProxy and create it by factory.
    The task went directly into the thread-pool priority-queue and will execute.

    Requierment:    Implement task by shared object and insert to configuration directory.
                    Do not drag and drop into the diractory!
                    Use copy and paste

    Date: 5.2.20
    Ver 1.0
*/

#ifndef __HRD11_PLUG_AND_PLAY_HPP
#define __HRD11_PLUG_AND_PLAY_HPP

#include <string>       // std::string
#include <thread>       // std::thread
#include <vector>       // std::vector

#include "dispatcher_callback.hpp"

namespace hrd11
{

class DirMonitor
{
    enum ThreadStatus
    {
        RUNING = 0,
        INIT,
        ERROR,
        END_THREAD,
        SUCCESS
    };

public:
    explicit DirMonitor(const std::string& dir_path);
    ~DirMonitor();

    //uncopiable
    DirMonitor(const DirMonitor&) = delete;
    DirMonitor& operator=(const DirMonitor&) = delete;

    //unmoveable
    DirMonitor(DirMonitor&&) = delete;
    DirMonitor& operator=(DirMonitor&&) = delete;

    Dispatcher<std::string>* GetDispatcher();
    ThreadStatus GetThreadStatus();
    void SetThreadStatus(DirMonitor::ThreadStatus stt);

private:
    void Monitor(const std::string& dir_path);

    ThreadStatus m_thread_stt;
    Dispatcher<std::string> m_dispatcher;
    std::thread m_monitor;
};


class DL_Loader
{
public:
    explicit DL_Loader(Dispatcher<std::string>* dispatcher);
    ~DL_Loader();

    //uncopiable
    DL_Loader(const DL_Loader&) = delete;
    DL_Loader& operator=(const DL_Loader&) = delete;
    //unmoveable
    DL_Loader(DL_Loader&&) = delete;
    DL_Loader& operator=(DL_Loader&&) = delete;

    void LoadLib(const std::string& path);

private:
    std::vector<void*> m_fds;
    CallBack<std::string,DL_Loader> m_callback;
};

}   // end namespace hrd11

#endif // __HRD11_PLUG_AND_PLAY_HPP
