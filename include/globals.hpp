
#ifndef __HRD11_GLOBALS_HPP__
#define __HRD11_GLOBALS_HPP__

#ifndef LOG_LVL
    #define LOG_LVL LOG_DEBUG
#endif

#ifndef LOG_PATH
    #define LOG_PATH "./log/greencloud"
#endif

#define FACTORY Factory<TPTask, DataType,std::unique_ptr<ThreadInfo>>


#endif // __HRD11_GLOBALS_HPP__
