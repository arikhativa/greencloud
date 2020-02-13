

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

std::unique_ptr<NBDRead> CCC(std::unique_ptr<TaskInfo> info)
{
    printf("here\n");

    return std::move(NBDRead::Create(std::move(info)));
}

void InitSharedObjet() __attribute__((constructor));

void InitSharedObjet()
{
    write(1, "L\n", 2);

    Logger* s_log = Handleton<Logger>::GetInstance(LOG_PATH, LOG_LVL);

    LOG(LOG_DEBUG, "in .so");

    RequestEngine<TEMPLATE>* req_eng = Handleton<RequestEngine<TEMPLATE>>
                                                ::GetInstance(PLUGINS_PATH);

    req_eng->AddTask(FactoryKey::READ, &CCC);

}

} // end namespace hrd11
