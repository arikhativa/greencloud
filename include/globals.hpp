
#ifndef __HRD11_GLOBALS_HPP__
#define __HRD11_GLOBALS_HPP__

#include "logger.hpp"


// #include "task_args.hpp"
// #include "request_engine.hpp"

namespace hrd11
{

#ifndef LOG_LVL
    #define LOG_LVL LOG_DEBUG
#endif

#ifndef LOG_PATH
    #define LOG_PATH "./log/greencloud"
#endif

#ifndef PLUGINS_PATH
    #define PLUGINS_PATH "./plugins"
#endif

enum class FactoryKey
{
    READ = 0,
    WRITE,
    DISCONNECT,
    FLUSH,
    TRIM,
    BAD_REQUEST,
    IGNORE,
    Q_EXIT
};

class TaskInfo;

#define TEMPLATE FactoryKey , std::unique_ptr<TaskInfo>
#define FACTORY Factory<TPTask, TEMPLATE >
#define REQUEST_ENGINE RequestEngine<TEMPLATE>
#define TIMEOUT 100


}   //end namespace hrd11

#endif // __HRD11_GLOBALS_HPP__
