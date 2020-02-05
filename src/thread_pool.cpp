
// C++ include ----------------------------------------------------------------
#include <thread>       		// std::thread
#include <memory>				// std::unique_ptr
#include <mutex>                // std::mutex
#include <condition_variable>	// std::condition_variable


// Local include --------------------------------------------------------------
#include "wpriorityqueue.hpp"	// wrapper priority queue
#include "thread_pool.hpp"
#include "tptask.hpp"
#include "tp_sys_task.hpp"

namespace hrd11
{

// Declarations ---------------------------------------------------------------

static size_t CheackNum(size_t num_of_threads);

enum ThreadStatus
{
	DONT_RUN = 0,
	RUN
};

/* TODO:
general -
	- Only cotr, dtor, AddTask and Stop work.
	write the rest
*/


// Public Members -------------------------------------------------------------

ThreadPool::ThreadPool(size_t num_of_threads) :
			m_threads(CheackNum(num_of_threads))
{
	num_of_threads = m_threads.size();

	for (size_t i = 0; i < num_of_threads; ++i)
	{
		m_threads[i] = std::thread(&ThreadPool::ThreadFunc, this);
	}
}

ThreadPool::~ThreadPool()
{
	if (m_threads[0].joinable())
	{
		Stop();
	}
}

// according to Factory::Create
void ThreadPool::AddTask(std::unique_ptr<TPTask> new_task)
{
	m_task_queue.Push(std::move(new_task));
}

void ThreadPool::Stop()
{
	size_t thread_num = m_threads.size();

	for (size_t i = 0; i < thread_num; ++i)
	{
		AddTask(std::unique_ptr<TPTask>(new TPStop));
	}
	for (size_t i = 0; i < thread_num; ++i)
	{
		m_threads[i].join();
	}
}

// stop with time-out - may loose data of current running TPTasks
void ThreadPool::Stop(std::chrono::nanoseconds time_out)
{
	(void)time_out;
}

void ThreadPool::Suspend() //will not loose data of current running TPTasks
{
	for (size_t i = 0; i < m_threads.size(); ++i)
	{
		AddTask(std::unique_ptr<TPTask>(new TPSuspend));
	}
}

void ThreadPool::Resume()
{
	for (size_t i = 0; i < m_threads.size(); ++i)
	{
		m_sem.Post();
	}
}

//add or reduce num of threads
void ThreadPool::SetSize(size_t new_num_of_threads)
{
	(void)new_num_of_threads;
}

// Private Members ------------------------------------------------------------

void ThreadPool::ThreadFunc()
{
	ThreadStatus run = RUN;
	std::unique_ptr<TPTask> task;

	while(RUN == run)
	{
		m_task_queue.Pop(task);
		try
		{
			task->Execute();
		}
		catch (const ThreadMsg& e)
		{
			switch (e.m_msg)
			{
				case ThreadMsg::MsgType::STOP:
					run = DONT_RUN;
					break ;

				case ThreadMsg::MsgType::SUSPEND:
					m_sem.Wait();
					break ;
				default:
				break ;
			}
		}
	}
}


// Static funcs ---------------------------------------------------------------

static size_t CheackNum(size_t num_of_threads)
{
	if (!num_of_threads)
	{
		return 1;
	}

	return num_of_threads;
}

}	// end namespace hrd11
