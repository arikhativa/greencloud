
/*
    RequestEngine:  A generic framework for listning on fds, resiving requests
                    from them, and executeing them asynchronic.

    Requirements:   AddTask() tasks must have a static function Create()
                    that will be used for creating tasks.
                    see CreatorFunc() in class RequestEngine.

    Exceptions:     bad_alloc
                    EpollError

    Ver 1.0
*/

#ifndef __HRD11_REQUEST_ENGINE_HPP__
#define __HRD11_REQUEST_ENGINE_HPP__

// C include ------------------------------------------------------------------

// C++ include ----------------------------------------------------------------
#include <functional>         // std:function
#include <vector>             // std:vector
#include <thread>             // std:thread
#include <string>             // std:sting

// Local include --------------------------------------------------------------
#include "handleton.hpp"
#include "epoll.hpp"
#include "gateway.hpp"
#include "retask.hpp"
#include "tptask.hpp"
#include "thread_pool.hpp"
#include "factory.hpp"
#include "plug_and_play.hpp"
#include "globals.hpp"
#include "monitor.hpp"

namespace hrd11
{

// Globals --------------------------------------------------------------------
static const unsigned int MAX_EVENTS_TO_WAKE = 1;


template <typename Key, typename Args>
class RequestEngine final
{

public:
    enum class Status
    {
        INIT = 0,
        RUN,
        STOP
    };

    ~RequestEngine();

    // unmcopyable
    RequestEngine(const RequestEngine& other) = delete;
    RequestEngine& operator=(const RequestEngine& other)=  delete;
    // unmovable
    RequestEngine(RequestEngine&& other) = delete;
    RequestEngine& operator=(RequestEngine&& other)=  delete;

    using CreatorFunc = std::function<std::unique_ptr<RETask>(Args args)>;
    void AddTask(const Key& key, CreatorFunc func);

    void AddGateWay(std::unique_ptr<GateWay<Key, Args>> gateway);

    void Run();
    void Stop();

private:
    template<typename T>
    friend class hrd11::Handleton;

    // TODO: hardware_concurrncy comment
    RequestEngine(const std::string& plugings_dir_path,
        size_t num_of_threads = std::thread::hardware_concurrency());

    Status m_req_engine_stt;
    std::unique_ptr<Monitor> m_fd_monitor;
    ThreadPool m_thread_pool;
    DirMonitor m_dir_monitor;
    DL_Loader m_lib_loader;
    Factory<TPTask, Key, Args> m_factory;
    std::vector<std::unique_ptr<GateWay<Key, Args>>> m_gateways;

};

template <typename Key, typename Args>
RequestEngine<Key, Args>::RequestEngine(const std::string& plugings_dir_path,
            size_t num_of_threads) :
            m_req_engine_stt(Status::INIT),
            m_fd_monitor(new Epoll(MAX_EVENTS_TO_WAKE)),
            m_thread_pool(num_of_threads),
            m_dir_monitor(plugings_dir_path),
            m_lib_loader(m_dir_monitor.GetDispatcher())
{}

template <typename Key, typename Args>
RequestEngine<Key, Args>::~RequestEngine()
{}

template <typename Key, typename Args>
void RequestEngine<Key, Args>::AddTask(const Key& key, CreatorFunc func)
{
    m_factory.Add(key, func);
}

template <typename Key, typename Args>
void RequestEngine<Key, Args>::
AddGateWay(std::unique_ptr<GateWay<Key, Args>> gateway)
{
    m_fd_monitor->Add(gateway->GetFd(), Epoll::READ_FD);

    m_gateways.push_back(std::move(gateway));
}

template <typename Key, typename Args>
void RequestEngine<Key, Args>::Run()
{
    int stt = 0;
    std::pair<Key, Args> info;

    m_req_engine_stt = Status::RUN;

    while (Status::RUN == m_req_engine_stt)
    {
        stt = m_fd_monitor->WaitTimeOut(TIMEOUT);

        if (0 != stt) // NOT Timed out
        {
            for (size_t i = 0; i < m_gateways.size(); ++i)
            {
                if (m_gateways[i]->GetFd() == (*m_fd_monitor)[0])
                {
                    info = m_gateways[i]->Read(this);
                    m_thread_pool.AddTask(m_factory.Create(
                                        info.first, std::move(info.second)));
                }
            }
        }
    }
}

template <typename Key, typename Args>
void RequestEngine<Key, Args>::Stop()
{
    m_req_engine_stt = Status::STOP;
}

}	// end namespace hrd11

#endif // __HRD11_REQUEST_ENGINE_HPP__
