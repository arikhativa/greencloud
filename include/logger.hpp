/*
    Logger -

    Discription:    Logger is a concept of handeling error or regular
                    ducmentation.
                    Looger will open a file and write to it a massge for
                    later to debug.
                    logger onlt works with Handleton<>

	Usage:			Create instace:
                    Logger* log = Handleton<Logger>::GetInstance(LOG_PATH, LOG_LVL);
                    log->Write(LOG_ERROR, "...", __FILE__, __LINE__)

	Exceptions:		std::bad_alloc
					LoggerBadOpen

    Date:           29.1.2020

    Ver 1.0
*/

#ifndef __HRD11_LOGGER_HPP__
#define __HRD11_LOGGER_HPP__

#include <mutex>        // std::mutex
#include <iostream>     // std::fstream
#include <fstream>      // std::fstream

#include "handleton.hpp"

namespace hrd11
{

enum Loglvl
{
    LOG_ERROR = 0,
    LOG_INFO = 1,
    LOG_DEBUG = 2
};

enum LogFlags
{
    OVERRIDE_FILE = 0,
    DONT_OVERRIDE_FILE
};

class Logger
{
public:
    ~Logger();
    void Write(Loglvl lvl, const std::string& msg,
                const std::string& src_file, int line);

private:
    template<typename T>
    friend class hrd11::Handleton;

    Logger(const std::string& path_and_filename, Loglvl lvl,
            bool overwrite_file = true);
    std::mutex m_mutex;
    std::ofstream m_file;
    Loglvl m_lvl;

};

struct LoggerBadOpen : public std::runtime_error
{
    LoggerBadOpen(const std::string& msg) : std::runtime_error(msg)
    {}
};

}   // end namespace hrd11


#endif // __HRD11_LOGGER_HPP__
