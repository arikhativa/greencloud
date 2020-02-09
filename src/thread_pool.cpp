
// C++ include ----------------------------------------------------------------
#include <chrono>       		// std::chrono
#include <thread>       		// std::thread
#include <memory>				// std::unique_ptr
#include <mutex>                // std::mutex
#include <condition_variable>	// std::condition_variable


// Local include --------------------------------------------------------------
#include "wpriorityqueue.hpp"	// wrapper priority queue
#include "thread_pool.hpp"
#include "tptask.hpp"
#include "tp_sys_task.hpp"

/*
	TODO:
	general -
		- del unistd
		- Stop() with time_out
		- SetSize()
*/

#include <unistd.h>


namespace hrd11
{

// Declarations ---------------------------------------------------------------

static size_t CheackNum(size_t num_of_threads);

enum ThreadStatus
{
	DONT_RUN = 0,
	RUN
};


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
	size_t thread_num = m_threads.size();

	for (size_t i = 0; i < thread_num; ++i)
	{
		AddTask(std::unique_ptr<TPTask>(new TPSuspend));
	}
	for (size_t i = 0; i < thread_num; ++i)
	{
		m_main_sem.Wait();
	}
}

void ThreadPool::Resume()
{
	size_t thread_num = m_threads.size();

	for (size_t i = 0; i < thread_num; ++i)
	{
		m_thread_sem.Post();
	}
	for (size_t i = 0; i < thread_num; ++i)
	{
		m_main_sem.Wait();
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
				{
					m_main_sem.Post();
					m_thread_sem.Wait();
					m_main_sem.Post();
					break ;
				}
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
