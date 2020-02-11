
#ifndef __HRD11_RETASK_HPP__
#define __HRD11_RETASK_HPP__

#include "tptask.hpp"

namespace hrd11
{

class RETask : public TPTask
{
public:
    enum class TaskPriority
    {
        LOW = 0,
        MEDIUM,
        HIGH
    };

	explicit RETask(TaskPriority priority = TaskPriority::MEDIUM) :
                                    TPTask((TPTask::TaskPriority)priority)
    {}

	virtual ~RETask() = default;

	RETask(const RETask& other) = default;
	RETask& operator= (const RETask& other) = default;
	RETask(RETask&& other) = default;
	RETask& operator= (RETask&& other) = default;

private:

	virtual void Execute() = 0;
};

}	// end namespace hrd11

#endif // __HRD11_RETASK_HPP__
