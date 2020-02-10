
#ifndef YYY
#define YYY

#include <stdio.h>
#include "handleton.hpp"

namespace hrd11
{

struct Base
{
public:
    ~Base() = default;

    void Print()
    {
        printf("Base\n\n");
    }


    int m_x;
    static int m_y;
private:
    template<typename T>
    friend class hrd11::Handleton;

    Base()
    {
        if (m_y == '0')
        {
            m_x = m_y;
            m_y = 0;
        }
        ++m_x;
    }

};

int Base::m_y = '0';

}

#endif // YYY
