
#ifndef __HRD11_GLOBALS_HPP__
#define __HRD11_GLOBALS_HPP__

#include "logger.hpp"

#ifndef LOG_LVL
    #define LOG_LVL LOG_DEBUG
#endif

#ifndef LOG_PATH
    #define LOG_PATH "./log/greencloud"
#endif

#ifndef PLUGINS_PATH
    #define PLUGINS_PATH "./plugins"
#endif

// extern "C"
// {
// extern hrd11::Logger* g_log;
// }

#define FACTORY Factory<TPTask, DataType, std::unique_ptr<ThreadInfo> >


#endif // __HRD11_GLOBALS_HPP__
