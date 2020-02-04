
#include "tptask.hpp"

using namespace hrd11;

TPTask::TPTask(TaskPriority priority) : m_priority(priority)
{}

// TPTask::~TPTask()
// {}

bool TPTask::operator< (const TPTask& other) const
{
    return m_priority < other.m_priority;
}
