

#include <unistd.h>
#include <mutex>

#include "thread_pool.hpp"
#include "tptask.hpp"
#include "tp_sys_task.hpp"


using namespace hrd11;


std::mutex g_mutex;

static int g_end = 1;

struct PrintA : public TPTask
{
    PrintA() : TPTask(TPTask::TaskPriority::MEDIUM)
    {}
    ~PrintA()
    {}

    void Execute() override
    {
        std::lock_guard<std::mutex> lock(g_mutex);

        write(1, "a\n", 2);
    }
};
struct PrintB : public TPTask
{
    PrintB() : TPTask(TPTask::TaskPriority::HIGH)
    {}
    ~PrintB()
    {}

    void Execute() override
    {
        std::lock_guard<std::mutex> lock(g_mutex);

        write(1, "b\n", 2);
    }
};

struct EndMain : public TPTask
{
    EndMain() : TPTask(TPTask::TaskPriority::LOW)
    {}
    ~EndMain()
    {}

    void Execute() override
    {
        std::lock_guard<std::mutex> lock(g_mutex);

        write(1, "E\n", 2);
        g_end = 0;
    }
};


int main()
{
    ThreadPool tp(4);

    tp.Suspend();

    tp.AddTask(std::unique_ptr<TPTask>(new PrintA));
    tp.AddTask(std::unique_ptr<TPTask>(new PrintA));
    tp.AddTask(std::unique_ptr<TPTask>(new PrintA));
    tp.AddTask(std::unique_ptr<TPTask>(new PrintA));

    tp.Resume();
    // write(1, "h\n", 2);
    tp.Suspend();

    tp.AddTask(std::unique_ptr<TPTask>(new PrintB));
    tp.AddTask(std::unique_ptr<TPTask>(new PrintB));
    tp.AddTask(std::unique_ptr<TPTask>(new PrintB));
    tp.AddTask(std::unique_ptr<TPTask>(new PrintB));

    tp.Resume();
    tp.Suspend();

    tp.AddTask(std::unique_ptr<TPTask>(new EndMain));

    tp.Resume();

    while (g_end)
    {
    }


    return 0;
}
