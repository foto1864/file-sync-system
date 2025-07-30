#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define ERR_USAGE "Usage: ./main <src> <dest> -l <logfile>"
#define EWRUSAGE 1

#define MAX_BUFFER_SIZE 1024

ssize_t readline(int _fd, char* _buf, ssize_t _nbytes) {
    char temp[MAX_BUFFER_SIZE];
    ssize_t bytes_read = read(_fd, temp, MAX_BUFFER_SIZE);
    if (bytes_read < 0) {
        perror("read failed");
        exit(errno);
    }
    int i = 0;
    while (i < bytes_read && temp[i] != '\n' && i < _nbytes - 1) {
        _buf[i] = temp[i];
        i++;
    }
    if (i < _nbytes - 1) {
        _buf[i++] = '\n';
    }
    _buf[i] = '\0';
    return i;
}


int main(int argc, char* argv[]) {

    if (argc != 5) {
        printf("%s\n", ERR_USAGE);
        exit(EWRUSAGE);
    }
    if (strncmp(argv[3], "-l", 2) != 0) {
        printf("%s\n", ERR_USAGE);
        exit(EWRUSAGE);
    }
    const char* src = argv[1];
    const char* dest = argv[2];
    const char* logfile = argv[4];

    // mkfifo("manager-worker", 0666);
    // int fd_out = open("manager", O_WRONLY);

    mkfifo("worker-manager", 0666);

    pid_t process = fork();

    if (process < 0) {
        perror("fork failed");
        exit(errno);
    }

    if (process == 0) {
        
        char buffer[MAX_BUFFER_SIZE];
        int fd_in = open("worker-manager", O_RDONLY);
        ssize_t bytes_read = readline(fd_in, buffer, MAX_BUFFER_SIZE);
        if (bytes_read < 0) {
            perror("readline failed");
            exit(errno);
        }
        
        int fd_logs = open(logfile, O_CREAT | O_APPEND | O_WRONLY, 0644); 
        ssize_t bytes_written = write(fd_logs, buffer, strlen(buffer));
        if (bytes_written < 0) {
            perror("write failed");
            exit(errno);
        }

        int status;
        waitpid(process, &status, 0);
        printf("success\n");
    }
    else {
        execl("worker", "worker", "copy", src, dest, NULL);
    }

    return 0;
}