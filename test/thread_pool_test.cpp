

#include <unistd.h>

#include "thread_pool.hpp"
#include "tptask.hpp"
#include "tp_sys_task.hpp"


using namespace hrd11;


static int g_end = 1;

struct PrintA : public TPTask
{
    PrintA() : TPTask(TPTask::TaskPriority::MEDIUM)
    {}
    ~PrintA()
    {}

    void Execute() override
    {

        write(1, "O\n", 2);
    }
};

struct EndMain : public TPTask
{
    void Execute() override
    {
        write(1, "E\n", 2);
        g_end = 0;
    }
};


int main()
{
    ThreadPool tp(1);

    tp.Suspend();
    tp.AddTask(std::unique_ptr<TPTask>(new PrintA));
    tp.AddTask(std::unique_ptr<TPTask>(new PrintA));
    tp.AddTask(std::unique_ptr<TPTask>(new PrintA));
    tp.AddTask(std::unique_ptr<TPTask>(new PrintA));
    tp.AddTask(std::unique_ptr<TPTask>(new PrintA));
    tp.AddTask(std::unique_ptr<TPTask>(new PrintA));


    tp.AddTask(std::unique_ptr<TPTask>(new PrintA));
    tp.AddTask(std::unique_ptr<TPTask>(new PrintA));
    tp.AddTask(std::unique_ptr<TPTask>(new PrintA));
    tp.AddTask(std::unique_ptr<TPTask>(new PrintA));
    tp.AddTask(std::unique_ptr<TPTask>(new PrintA));



size_t i = 400000000;
    while (i)
    {
        --i;
    }
    write(1, "W\n", 2);


    tp.Resume();
    tp.AddTask(std::unique_ptr<TPTask>(new EndMain));

    while (g_end)
    {
    }


    return 0;
}
