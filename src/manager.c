#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define ERR_USAGE "Usage: ./manager -o <operation> <src> <dest> -l <logfile>"
#define EWRUSAGE 1

#define MAX_BUFFER_SIZE 65536

ssize_t readline(int _fd, char* _buf, ssize_t _nbytes);

int main(int argc, char* argv[]) {

    if (argc != 7) {
        printf("%s\n", ERR_USAGE);
        exit(EWRUSAGE);
    }
    if ((strncmp(argv[1], "-o", 2) != 0) || (strncmp(argv[5], "-l", 2)) != 0) {
        printf("%s\n", ERR_USAGE);
        exit(EWRUSAGE);
    }
    const char* operation = argv[2];
    const char* src = argv[3];
    const char* dest = argv[4];
    const char* logfile = argv[6];

    // Make a pipe for receiving input from the worker
    // This happens so that the worker can do its thing, then
    // send info back to the manager, and then the manager can
    // write into logfiles or whatever
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

        unlink("worker-manager");

        int status;
        waitpid(process, &status, 0);
        printf("success\n");
    }
    else {
        execl("worker", "worker", operation, src, dest, NULL);
    }

    return 0;
}

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