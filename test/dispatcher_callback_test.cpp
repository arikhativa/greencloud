
#include <stdio.h>
#include <unistd.h>

#include "dispatcher_callback.hpp"
#include "plug_and_play.hpp"

using namespace hrd11;

struct Threm
{
    Threm()
    {}
    ~Threm()
    {
        printf("threm dtor\n");
    }

    void SetTemp(int temp)
    {
        m_dispatcher.NotifyAll(temp);
    }

    Dispatcher<int> m_dispatcher;
};

struct Ac
{
    Ac() : m_callback(nullptr)
    {}

    ~Ac()
    {
        printf("ac dtor\n");

        if (m_callback)
        {
            delete m_callback;
            m_callback = nullptr;
        }
    }
    void Disconnect()
    {
        if (m_callback)
        {
            delete m_callback;
            m_callback = nullptr;
        }
    }

    void Connect(Dispatcher<int>* dispatcher)
    {
        m_callback = new CallBack<int, Ac>(dispatcher,
                                            *this,
                                            &Ac::ChangeFan,
                                            &Ac::NoDetector);
    }

    void ChangeFan(const int& temp)
    {
        printf("-- Ac notify that temp is %d\n", temp);
    }
    void NoDetector()
    {

        printf("-- Ac notify there is no detector\n");
    }

    CallBack<int, Ac>* m_callback;
};


void Test1()
{
    Ac ac;
    Ac ac1;
    Ac ac2;
    Ac ac3;

    Threm threm;

    ac.Connect(&threm.m_dispatcher);
    ac1.Connect(&threm.m_dispatcher);
    ac2.Connect(&threm.m_dispatcher);
    ac3.Connect(&threm.m_dispatcher);

    threm.SetTemp(5);

    // ac.Disconnect();
    // threm.SetTemp(2);
    // ac1.Disconnect();
    // ac2.Disconnect();
    // threm.SetTemp(3);
    // ac3.Disconnect();
    // threm.SetTemp(10);
}

void Test2()
{
    DirMonitor(".");

    for (size_t i =0; i < 10000000; ++i)
    {
    }
}

int main()
{

    Test2();

    return 0;
}
