
#include <errno.h>      // errno

#include "handleton.hpp"
#include "logger.hpp"
#include "factory.hpp"
#include "tptask.hpp"
#include "nbd_tasks.hpp"
#include "globals.hpp"


namespace hrd11
{
    INIT_HANDLETON(Logger)
    INIT_HANDLETON(FACTORY)

    // template<>
    // std::mutex Handleton<FFF>::s_mutex{};
    // template<>
    // std::atomic<FFF*> Handleton<FFF>::s_instance{nullptr};
    // template<>
    // std::unique_ptr<FFF> Handleton<FFF>::s_ptr{nullptr};



}   // end namespace hrd11
