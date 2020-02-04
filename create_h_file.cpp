
#include <fstream>
#include <iostream>
#include <string>

#include <string.h>

#define FILE_LEN sizeof(NAME_ENDING) + sizeof(NAME_BEGIN)

using namespace std;

static const char* IFNDEF1 = "#ifndef __HRD11_";
static const char* IFNDEF2 = "_HPP__\n#define __HRD11_";
static const char* IFNDEF3 = "_HPP__\n\n";
static const char* NAMESPACE1 = "namespace hrd11\n{\n\n";
static const char* CLASS1 = "class ";
static const char* CLASS2 = "{\npublic:\nprivate:\n\n};\n\n";
static const char* NAMESPACE2 = "}	// end namespace hrd11\n\n";
static const char* ENDIF1 = "#endif // __HRD11_";
static const char* ENDIF2 = "_HPP__";

static const char* NAME_BEGIN = "./";
static const char* NAME_ENDING = ".hpp";
static char* g_file_name;
static char* g_file_name_upper;

static void FillFile(ostream& file, char* name)
{
    name[0] = toupper(name[0]);

    file << "\n"
    << IFNDEF1
    << g_file_name_upper
    << IFNDEF2
    << g_file_name_upper
    << IFNDEF3
    << NAMESPACE1
    << CLASS1
    << name << "\n"
    << CLASS2
    << NAMESPACE2
    << ENDIF1
    << g_file_name_upper
    << ENDIF2;
}

int main(int ac, char* av[])
{
    size_t i = 0;

    if (2 != ac)
    {
        cerr << "-- Usage: 1 argument pls\n";
        return 1;
    }

    size_t len = strlen(av[1]);

    g_file_name = new char[len + FILE_LEN];
    g_file_name_upper = new char[len + FILE_LEN];
    for (size_t h = 0; h < len + 1; ++h)
    {
        g_file_name_upper[h] = toupper(av[1][h]);
    }

    while(i < 2)
    {
        g_file_name[i] = NAME_BEGIN[i];
        ++i;
    }
    for (size_t a = 0; av[1][a]; ++i, ++a)
    {
        g_file_name[i] = av[1][a];
    }
    for (size_t a = 0; i < len + FILE_LEN; ++i, ++a)
    {
        g_file_name[i] = NAME_ENDING[a];
    }

    std::filebuf fb;
    fb.open(g_file_name, std::ios::out);
    ostream m_file(&fb);

    FillFile(m_file, av[1]);
    fb.close();

    delete[] g_file_name;
    delete[] g_file_name_upper;

    return 0;
}
