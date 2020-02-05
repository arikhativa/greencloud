
#ifndef __HRD11_TPTASK_HPP__
#define __HRD11_TPTASK_HPP__

#include <memory>       // std::unique_ptr


namespace hrd11
{
// interface only. User must inherit and define functions specification per TPTask-class
//move TPTask to separate hpp file
class TPTask
{
public:
    enum TaskPriority
    {
        LOW = 0,
        MEDIUM,
        HIGH,
        SYSTEM  // this priority is only for internl function. do not use it
    };

	TPTask(TaskPriority priority = MEDIUM); // priority is enum - not possible to implicit convert. No need for explicit.
	virtual ~TPTask() = default;

	TPTask(const TPTask& other) = default;
	TPTask& operator= (const TPTask& other) = default;
	TPTask(TPTask&& other) = default;
	TPTask& operator= (TPTask&& other) = default;

    bool operator< (const TPTask& other) const;

private:
    friend class ThreadPool;

	virtual void Execute() = 0;
	TaskPriority m_priority;
};


}	// end namespace hrd11

template<>
bool std::operator< (const std::unique_ptr<hrd11::TPTask>& task_a,
    const std::unique_ptr<hrd11::TPTask>& task_b);

#endif // __HRD11_TPTASK_HPP__
