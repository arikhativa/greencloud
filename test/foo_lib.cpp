
#include <unistd.h>


#define LOG_PATH "/home/student/git/projects/greencloud/log/greencloud"

#include "handleton.hpp"
#include "base.hpp"

#include "logger.hpp"
#include "globals.hpp"

#define LOG(lvl, msg) s_log->Write(lvl, msg, __FILE__, __LINE__)

using namespace hrd11;


extern "C" Base* Foo()
    {
        Logger* s_log = Handleton<Logger>::GetInstance(LOG_PATH, LOG_LVL);
        LOG(LOG_DEBUG, "in Fopo");

        Base* ptr=  Handleton<Base>::GetInstance();

        return ptr;
    }
