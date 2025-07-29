#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#define ERR_USAGE "Usage: ./main <src> <dest>"
#define EWRUSAGE 1

int main(int argc, char* argv[]) {

    if (argc != 3) {
        printf("%s\n", ERR_USAGE);
        exit(EWRUSAGE);
    }
    const char* src = argv[1];
    const char* dest = argv[2];

    pid_t process = fork();

    if (process < 0) {
        perror("fork failed");
        exit(errno);
    }

    if (process == 0) {
        int status;
        waitpid(process, &status, 0);
        printf("Both processes exited tbf\n");
    }
    else {
        execl("worker", "worker", src, dest,NULL);
    }

    return 0;
}