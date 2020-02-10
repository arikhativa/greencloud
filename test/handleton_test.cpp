
#include <unistd.h>

#include <thread>			//std::thread

#include <dlfcn.h>			/* dlopen(), dlclose, dlsym */

#include "handleton.hpp"
#include "base.hpp"

using namespace hrd11;

static int pass = 0;

struct B2
{
    int m_x;
};

static void ThreadFunc()
{
    for (int i = 0; i < 1000000; ++i)
    {
        Base* ptr = Handleton<Base>::GetInstance();
        (void)ptr;
    }

}

static void ExplineTest()
{
    printf("-- Test For Handleton - multi-thread\n");
    printf("-- Description:\n");
    printf("-- \t%s\n", "class Tmp has a static int m_x");
    printf("-- \t%s\n", "Every time the ctor is called");
    printf("-- \t%s\n", "it dose ++m_x");
    printf("-- \t%s\n", "");
}

static void ExplineTest2()
{
    printf("-- Test For Handleton - works with expilict load\n");
    printf("-- Description:\n");
    printf("-- \t%s\n", "in main() we call with dlopen()/dlsym() Foo()");
    printf("-- \t%s\n", "Foo() calls Handelton and returns the addres");
    printf("-- \t%s\n", "main() calls Handelton and checks with");
    printf("-- \t%s\n", "Foo()'s addres");
    printf("-- \t%s\n", "");
}

static void ThreadTest()
{
    std::thread th1(ThreadFunc);
    std::thread th2(ThreadFunc);
    std::thread th3(ThreadFunc);
    std::thread th4(ThreadFunc);
    std::thread th5(ThreadFunc);

    th1.join();
    th2.join();
    th3.join();
    th4.join();
    th5.join();
}

typedef Base* (*fp_t)();

static void RunTimeLinkTest()
{
    Base* ptr = Handleton<Base>::GetInstance();

    void *handle = 0;
    fp_t func = 0;

    handle = dlopen("/usr/lib/libFoo.so", RTLD_LAZY);
    if (!handle)
    {
        printf("-- can't open lib\n");
        printf("%s\n", dlerror());
        exit(1);
    }
    func = (fp_t)dlsym(handle, "Foo");
    if (!func)
    {
        printf("-- can't find func\n");
        printf("%s\n", dlerror());
        exit(1);
    }

    if (ptr != func())
    {
        ExplineTest2();
        printf("-- Fail...\n");
        printf("-- The addresss are diffrent.\n");
        printf("-- There is more the none hundleton.\n");
        pass =1;

    }
}

int main()
{
    Base* ptr = Handleton<Base>::GetInstance();

    // ThreadTest();
    RunTimeLinkTest();

    // if ('1' != ptr->m_x)
    // {
    //     ExplineTest();
    //     printf("-- Fail...\n");
    //     printf("-- The Ctor was called %c times\n\n", ptr->m_x);
    //     pass =1;
    // }
    if (0 == pass)
    {
        printf("-- Success :)\n");
    }



    return 0;
}
