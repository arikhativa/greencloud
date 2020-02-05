
#include <memory>       // std::unique_ptr

#include "tptask.hpp"

namespace hrd11
{

TPTask::TPTask(TaskPriority priority) : m_priority(priority)
{}


bool TPTask::operator< (const TPTask& other) const
{
    return m_priority < other.m_priority;
}

}	// end namespace hrd11

template<>
bool std::operator< (const std::unique_ptr<hrd11::TPTask>& task_a,
                        const std::unique_ptr<hrd11::TPTask>& task_b)
{
	return (*task_a) < (*task_b);
}
