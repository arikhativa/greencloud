
#include <fstream>      // std::fstream

int main()
{
    std::fstream m_file("test.txt",
        std::ios::out | std::ios::in | std::ios::trunc);


      // m_file.write ("This is an apple",16);
      m_file.seekp (30);
      m_file.write (" ",1);

      m_file.close();

    return 0;
}
