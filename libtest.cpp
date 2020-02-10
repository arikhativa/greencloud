

#include <unistd.h>
#include <memory>

#include "globals.hpp"
#include "logger.hpp"
#include "handleton.hpp"
#include "factory.hpp"
#include "tptask.hpp"
#include "driver_data.hpp"
#include "thread_pool.hpp"
#include "tp_sys_task.hpp"
#include "nbd_tasks.hpp"

#define LOG(lvl, msg) s_log->Write(lvl, msg, __FILE__, __LINE__)

namespace hrd11
{


// class NBDRead1 : public TPTask
// {
// public:
//     static std::unique_ptr<NBDRead> Create(std::unique_ptr<ThreadInfo> info)
//     {
//         return NBDRead::Create(std::move(info));
//     }
//
// private:
//     NBDRead1(std::unique_ptr<ThreadInfo> info) : m_info(std::move(info))
//     {}
//
//     void Execute() override
//     {
//         m_info->m_data = m_info->m_storage->Read(std::move(m_info->m_data));
//         m_info->m_driver->SendReply(std::move(m_info->m_data));
//     }
//
//     std::unique_ptr<ThreadInfo> m_info;
// };

std::unique_ptr<NBDRead> CCC(std::unique_ptr<ThreadInfo> info)
{
    printf("here\n");

    // return std::move(NBDRead::Create(std::move(info)));
    return std::unique_ptr<NBDRead>(new NBDRead(std::move(info)));
}

void Foo()
{
        printf("here\n");
}

static void InitSharedObjet() __attribute__((constructor));

void InitSharedObjet()
{
    write(1, "L\n", 2);

    Logger* s_log = Handleton<Logger>::GetInstance(LOG_PATH, LOG_LVL);

    LOG(LOG_DEBUG, "in .so");

    FACTORY* factory = Handleton<FACTORY>::GetInstance();
    printf("-- fac ret %d\n", factory->Add(READ, &CCC));
}

} // end namespace hrd11
