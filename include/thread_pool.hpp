#ifndef __HRD11_THREAD_POOL__
#define __HRD11_THREAD_POOL__

#include <cstddef>      // size_t
#include <functional>   // std::function
#include <thread>       // std::thread
#include <memory>       // std::unique_ptr

#include "wpriorityqueue.hpp"		// wrapper priority queue
#include "tptask.hpp"
#include "semaphore.hpp"

namespace hrd11
{

class ThreadPool
{
public:
  //add comment to user regarding default value
  ThreadPool(size_t num_of_threads = std::thread::hardware_concurrency());
	~ThreadPool();

    //uncopiable
	ThreadPool(const ThreadPool& other) = delete;
	ThreadPool& operator= (const ThreadPool& other) = delete;

    //unmovable
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

    void AddTask(std::unique_ptr<TPTask> task); // according to Factory::Create

	void Stop(); //stop, but finish current running tasks
	void Stop(std::chrono::nanoseconds time_out); // stop with time-out - may loose data of current running tasks

    void Suspend(); //will not loose data of current running tasks
    void Resume();

    void SetSize(size_t num_of_threads); //the new number of threads in the pool

private:
    void ThreadFunc(); // Pop and execute from WPQ.
    void PushSysTask(std::unique_ptr<TPTask> task);

    Semaphore m_sem;
	WPriorityQueue<std::unique_ptr<TPTask>> m_task_queue; //consider to add comparer function
    std::vector<std::thread> m_threads;
};

}   // end namespace hrd11

#endif // __HRD11_THREAD_POOL__
