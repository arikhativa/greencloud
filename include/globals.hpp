
/*
    global.hpp -

    Discription:    This header is for all the global configorations.

    Usage:          The User can config the setting from this file or in
                    compile time with the -D flag.

        LOG_LVL:        The lvl for writing to the log file. see logger.hpp

        LOG_PATH:       Path and file name to write to.

        PLUGINS_PATH:   Path to the plugins directory. see plag_and_play.hpp

        TIMEOUT:        Time in miliseconds that will pass between every time
                        the RequestEngine will check if it needs to stop
                        it's Run() function. see request_engine.hpp

        FactoryKey:     KEY$ are empty keys for future RETasks.

    Date:           12.2.2020

    Ver 1.0
*/

#ifndef __HRD11_GLOBALS_HPP__
#define __HRD11_GLOBALS_HPP__

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

#ifndef TIMEOUT
    #define TIMEOUT 100
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
    Q_EXIT,
    KEY1,
    KEY2,
    KEY3,
    KEY4,
    KEY5,
    KEY6,
    KEY7,
    KEY8,
    KEY9,
    KEY10
};

class TaskInfo;

#define TEMPLATE FactoryKey , std::unique_ptr<TaskInfo>
#define REQUEST_ENGINE RequestEngine<TEMPLATE>


}   //end namespace hrd11

#endif // __HRD11_GLOBALS_HPP__
