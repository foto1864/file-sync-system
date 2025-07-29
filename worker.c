#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <error.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_BUFF_SIZE 1024

int copy_file(const char* src, const char* dest) {
    int fd_source = open(src, O_RDONLY);
    if (fd_source < 0) {
        perror("open failed: ");
        return errno;
    }
    int fd_dest = open(dest, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd_dest < 0) {
        perror("open failed: ");
        return errno;
    }

    char buffer[MAX_BUFF_SIZE];
    ssize_t bytes_read = read(fd_source, buffer, MAX_BUFF_SIZE);
    if (bytes_read < 0) {
        perror("read failed: ");
        return errno;
    }
    ssize_t bytes_written = write(fd_dest, buffer, bytes_read);
    if (bytes_written < 0) {
        perror("write failed: ");
        return errno;
    }
    return 0;
}

#define ERR_USAGE "Usage: ./worker <src> <dest>"
#define EWRUSAGE 1

int main(int argc, char* argv[]) {
    
    if (argc != 3) {
        printf("%s\n", ERR_USAGE);
        exit(EWRUSAGE);
    }
    const char* src = argv[1];
    const char* dest = argv[2];

    int copied = copy_file(src, dest);
    if (copied != 0) {
        strerror(errno);
        exit(errno);
    }
    printf("File copied from <%s> to <%s>\n", src, dest);
    return 0;
}