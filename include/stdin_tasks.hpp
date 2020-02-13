
#ifndef __HRD11_STDIN_TASKS_HPP__
#define __HRD11_STDIN_TASKS_HPP__

#include <memory>		// std::unique_ptr

#include "retask.hpp"
#include "task_args.hpp"

namespace hrd11
{

class STDINExit : public RETask
{
public:
    static std::unique_ptr<STDINExit> Create(std::unique_ptr<TaskInfo> info)
    {
        return std::unique_ptr<STDINExit>(new STDINExit(std::move(info)));
    }

private:
    STDINExit(std::unique_ptr<TaskInfo> info) : m_info(std::move(info))
    {}

    void Execute() override
    {
        m_info->m_driver->Disconnect();
    }

    std::unique_ptr<TaskInfo> m_info;
};

class STDINIgnore : public RETask
{
public:
    static std::unique_ptr<STDINIgnore> Create(std::unique_ptr<TaskInfo> info)
    {
        return std::unique_ptr<STDINIgnore>(new STDINIgnore(std::move(info)));
    }

private:
    STDINIgnore(std::unique_ptr<TaskInfo> info) : m_info(std::move(info))
    {}

    void Execute() override
    {
	}

    std::unique_ptr<TaskInfo> m_info;
};

}	// end namespace hrd11

#endif // __HRD11_STDIN_TASKS_HPP__
