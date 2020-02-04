// C include ------------------------------------------------------------------
#include <sys/types.h>  // stat()
#include <sys/stat.h>   // stat()
#include <unistd.h>     // stat()
#include <string.h>     // strlen()
#include <thread>	    // std::thread::id
#include <sstream>      // std::stringstream

// C++ include ----------------------------------------------------------------
#include <mutex>        // std::mutex
#include <iostream>     // std::fstream
#include <fstream>      // std::fstream
#include <chrono>       // std::chrono::system_clock::now()
#include <ctime>        // std::ctime()

// local include --------------------------------------------------------------
#include "logger.hpp"

namespace hrd11
{

// Declarations ---------------------------------------------------------------

static const char* GetFileNameFromFullName(const char* full_name);
static char* AddStr(char* dest, const char* src, size_t max);
static char* FillSpace(char* run, size_t i);
static bool IsFileExists(const std::string& name);
static const char* EditLine(char* ret, const char* file_name,
                            int line, const char* lvl_str);


// Global ---------------------------------------------------------------------

static const char* TAB_LEN = "      ";
static const size_t DATE_LEN = 27;
static const size_t ID_LEN = 8;
static const size_t FILENAME_LEN = 29;
static const size_t LINE_LEN = 6;
static const size_t TYPE_LEN = 7;
static const size_t BASIC_LINE_LEN = DATE_LEN +
                                ID_LEN + FILENAME_LEN + LINE_LEN + TYPE_LEN + 1;

// Special Members ------------------------------------------------------------

Logger::Logger(const std::string& path_and_filename, Loglvl lvl, int flag) :
                m_lvl(lvl)
{
    size_t num = 1;
    std::string file_name = path_and_filename + std::to_string(num);

    if (flag == DONT_OVERRIDE_FILE)
    {
        while (IsFileExists(file_name))
        {
            ++num;
            file_name = path_and_filename + std::to_string(num);
        }
    }

    m_file.open(file_name);

    if (!m_file.is_open())
    {
        throw LoggerBadOpen("fail to open fail");
    }

    m_file << "Data & Time                "
    << "ID" << TAB_LEN
    << "Source File" << TAB_LEN << TAB_LEN << TAB_LEN
    << "Line" << "  "
    << "Type" << "   "
    << "ThreadId" << TAB_LEN << "   "
    << "Massage" << std::endl;
}

Logger::~Logger()
{
    m_file.close();
}


void Logger::Write( Loglvl lvl,
                    const std::string& msg,
                    const std::string& src_file,
                    int line)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    if (lvl <= m_lvl)
    {
        char* full_line = new char[BASIC_LINE_LEN];
        full_line[BASIC_LINE_LEN -1] = '\0';

        const char* lvl_str;
        switch (lvl)
        {
            case LOG_ERROR:
            lvl_str = "Error";
            break ;
            case LOG_INFO:
            lvl_str = "Info";
            break ;
            case LOG_DEBUG:
            lvl_str = "Debug";
            break ;
        }

        const char* file = GetFileNameFromFullName(src_file.c_str());


        auto myid = std::this_thread::get_id();
        std::stringstream ss;
        ss << myid;
        std::string th_str(ss.str());

        m_file << EditLine(full_line, file, line, lvl_str) << th_str << "  "
                << msg << "\n";
        delete[] full_line;
    }
}

// Static Functions -----------------------------------------------------------

static const char* EditLine(char* ret, const char* file_name,
                            int line, const char* lvl_str)
{
    std::string id_str(std::to_string(getpid()));
    std::string line_str(std::to_string(line));
    char* run = ret;
    auto now = std::chrono::system_clock::now();
    std::time_t full_data = std::chrono::system_clock::to_time_t(now);
    char *time_str = std::ctime(&full_data);

    for (size_t i = 0; time_str[i]; ++i)
    {
        if ('\n' == time_str[i])
        {
            time_str[i] = '\0';
            break ;
        }
    }
    run = AddStr(run, time_str, DATE_LEN);
    run = AddStr(run, id_str.c_str(), ID_LEN);
    run = AddStr(run, file_name, FILENAME_LEN);
    run = AddStr(run, line_str.c_str(), LINE_LEN);
    run = AddStr(run, lvl_str, TYPE_LEN);

    return ret;
}

static const char* GetFileNameFromFullName(const char* full_name)
{
    const char* file = full_name;

    while (*file && *file != '/')
    {
        ++file;
    }
    if (*file)
    {
        ++file;
    }
    else
    {
        file = full_name;
    }

    return file;
}

static char* FillSpace(char* run, size_t i)
{
    while (i)
    {
        *run = ' ';
        ++run;
        --i;
    }

    return run;
}

static char* AddStr(char* dest, const char* src, size_t max)
{
    size_t i = 0;

    while (*src)
    {
        *dest = *src;
        ++dest;
        ++src;
        ++i;
    }
    for (;i < max; ++i)
    {
        *dest = ' ';
        ++dest;
    }

    return FillSpace(dest, max - i);
}

static bool IsFileExists(const std::string& name)
{
    struct stat buffer;

    return (stat(name.c_str(), &buffer) == 0);
}

}   // end namespace hrd11
