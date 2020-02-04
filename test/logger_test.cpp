
#include <thread>

#include "logger.hpp"
#include "singleton.hpp"

#define TH_NUM 10

using namespace hrd11;

void ThreadFun()
{

}

int main()
{
    std::string path("/home/student/git/log/test");
    Loglvl lvl = LOG_DEBUG;

    Logger* log = Singleton<Logger>::GetInstance(path, lvl);

    std::thread th[TH_NUM];

    for (int i = 0; i < 10; ++i)
    {
        th[i] = std::thread(ThreadFun, path, lvl);
    }

    log->Write(LOG_ERROR, "msg", "logger_test.cpp", __LINE__);
    log->Write(LOG_DEBUG, "msg", "logger_test.cpp", __LINE__);
    log->Write(LOG_ERROR, "msg", "logger_test.cpp", __LINE__);
    log->Write(LOG_INFO, "msg", "logger_test.cpp", __LINE__);

    return 0;
}
