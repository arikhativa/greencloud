
#include <errno.h>      // errno

#include "handleton.hpp"
#include "logger.hpp"

#include "globals.hpp"


// #include "base.hpp"


namespace hrd11
{


    INIT_HANDLETON(Logger)

    // INIT_HANDLETON(FACTORY)

    // INIT_HANDLETON(Base)


    // static void InitSharedObjet() __attribute__((constructor));
    // void InitSharedObjet()
    // {
    //     Handleton<Logger>::GetInstance(LOG_PATH, LOG_LVL);
    // }

}   // end namespace hrd11
