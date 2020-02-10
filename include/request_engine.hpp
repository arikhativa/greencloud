
/*
    RequestEngine:  A generic framework for listning on fds, resiving requests
                    from them, and executeing them asynchronic.

    Requirements:   AddTask() tasks must have a static function Create()
                    that will be used for creating tasks.
                    see CreatorFunc() in class RequestEngine.

    Ver 1.0
*/

#ifndef __HRD11_RequestEngine_HPP__
#define __HRD11_RequestEngine_HPP__

#include <functional>         // std:function
#include <string>             // std:sting

#include "tptask.hpp"

namespace hrd11
{

template <typename Key, typename Args>
class GateWay;

class RETask;

template <typename Key, typename Args>
class RequestEngine final
{

public:
    // hardware_concurrncy comment
    RequestEngine(size_t num_of_threads = std::thread::hardware_concurrncy, const std::string& plugings_dir_path);
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
    // added members

    std::vector<std::unique_ptr<GateWay<Key, Args>>> m_gateways;

};

// diff header
template <typename Key, typename Args>
class GateWay
{

public:
    explicit GateWay(int fd);
    virtual ~GateWay();

    GateWay(const GateWay& other) = default;
    GateWay& operator=(const GateWay& other)=  default;
    GateWay(GateWay&& other) = default;
    GateWay& operator=(GateWay&& other)=  default;

    virtual std::pair<Key, Args> Read() = 0;

private:

    int m_fd;
};


// diffrent header
class RETask : public TPTask
{
public:
    enum class TaskPriority
    {
        LOW = 0,
        MEDIUM,
        HIGH
    };

	explicit RETask(TaskPriority priority = MEDIUM);
	virtual ~RETask() = default;

	RETask(const RETask& other) = default;
	RETask& operator= (const RETask& other) = default;
	RETask(RETask&& other) = default;
	RETask& operator= (RETask&& other) = default;

private:

	virtual void Execute() = 0;
};


}	// end namespace hrd11

#endif // __HRD11_RequestEngine_HPP__
