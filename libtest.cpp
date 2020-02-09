

#include "globals.hpp"
#include "handleton.hpp"
#include "factory.hpp"
#include "tptask.hpp"
#include "driver_data.hpp"
#include "thread_pool.hpp"
#include "tp_sys_task.hpp"
#include "nbd_tasks.hpp"

namespace hrd11
{

class NBDRead1 : public TPTask
{
public:
    static std::unique_ptr<NBDRead1> Create(std::unique_ptr<ThreadInfo> info)
    {
        return std::unique_ptr<NBDRead1>(new NBDRead1(std::move(info)));
    }

private:
    NBDRead1(std::unique_ptr<ThreadInfo> info) : m_info(std::move(info))
    {}

    void Execute() override
    {
        printf("\nREAD1\n");
        m_info->m_data = m_info->m_storage->Read(std::move(m_info->m_data));
        m_info->m_driver->SendReply(std::move(m_info->m_data));
    }

    std::unique_ptr<ThreadInfo> m_info;
};


extern "C"
{

static void InitSharedObjet() __attribute__((constructor));

void InitSharedObjet()
{
    FACTORY* factory = Handleton<FACTORY>::GetInstance();

    factory->Add(READ, NBDRead1::Create);
}

}   // end extern C

} // end namespace hrd11
