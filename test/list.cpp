#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>

#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>

#include <unistd.h>
#include <sys/types.h>

#define FILE_LENGTH 10

int main()
{
    int fd;
    void* file_memory;

    fd = open("./test.txt", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    lseek (fd, FILE_LENGTH + 1, SEEK_SET);
    write (fd, "", 1);
    lseek (fd, 0, SEEK_SET);

    file_memory = mmap(0, FILE_LENGTH, PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    sprintf((char*)file_memory, "%d\n", 55);

    munmap(file_memory, FILE_LENGTH);

    return 0;
}
