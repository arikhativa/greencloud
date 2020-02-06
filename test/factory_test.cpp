

#include <memory>
#include "factory.hpp"

using namespace hrd11;

struct Base
{
    virtual ~Base() = default;
    virtual void Print() = 0;

};

struct D1 : public Base
{
    D1(int i) : m_d (1)
    {
        (void)i;
    }

    static std::unique_ptr<Base> Create(int i)
    {
        return std::unique_ptr<Base>(new D1(i));
    }

    void Print() override
    {
        printf("-- this is D%d\n", m_d);
    }

    int m_d;
};

struct D2 : public Base
{
    D2(int i) : m_d (2)
    {
        (void)i;
    }

    static std::unique_ptr<Base> Create(int i)
    {
        return std::unique_ptr<Base>(new D2(i));
    }
    void Print() override
    {
        printf("-- this is D%d\n", m_d);
    }

    int m_d;
};

int main()
{
    std::unique_ptr<Base> tmp;
    FactoryAddStatus ret;

    Factory<Base, int, int> factory;

    ret = factory.Add(1, D1::Create);
    printf("-- tmp is %d\n", ret);
    ret = factory.Add(1, D1::Create);
    printf("-- tmp is %d\n", ret);
    factory.Add(2, D2::Create);

    tmp = factory.Create(1, 1);
    tmp->Print();
    tmp = factory.Create(2, 1);
    tmp->Print();
    tmp = factory.Create(1, 1);
    tmp->Print();

    return 0;
}
