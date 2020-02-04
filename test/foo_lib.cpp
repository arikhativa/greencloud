
#include <unistd.h>

#include "handleton.hpp"

using namespace hrd11;

struct Base
{
public:
    ~Base() = default;
    static int m_x;
    Base()
    {
        ++m_x;
    }
private:
    template<typename T>
    friend class hrd11::Handleton;

};

extern "C"
{
    Base* Foo()
    {
        return Handleton<Base>::GetInstance();
    }
}
