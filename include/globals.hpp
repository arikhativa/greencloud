
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

#define LOG_PATH    "./log/greencloud"
#define LOG_LVL     LOG_DEBUG
#define TIMEOUT 100


#define JSON_PATH "master_config.json"
#define SLAVE_JSON_PATH "slave_config.json"

#define MAGIC_STR "Yoav's Master - I surrender to you..."

enum class FactoryKey
{
    READ = 0,       // do not change
    WRITE = 1,      // do not change
    DISCONNECT = 2, // do not change
    FLUSH,
    TRIM,
    BAD_REQUEST,
    IGNORE,
    Q_EXIT,
    REPLAY,
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
