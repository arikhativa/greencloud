
#include <errno.h>      // errno

#include "handleton.hpp"
#include "logger.hpp"
#include "request_engine.hpp"
#include "globals.hpp"

namespace hrd11
{

    INIT_HANDLETON(Logger)
    INIT_HANDLETON(REQUEST_ENGINE)

}   // end namespace hrd11
