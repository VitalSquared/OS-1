#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

#define ERROR_FORK -1
#define ERROR_WAIT -1
#define CHILD_PROCESS 0
#define NUM_OF_ARGS_REQUIRED 2
#define END_OF_ARGS NULL
#define STATUS_IGNORE NULL

int main(int argc, char* argv[]) {
    if (argc != NUM_OF_ARGS_REQUIRED) {
        printf("Usage: %s file\n", argv[0]);
        return EXIT_SUCCESS;
    }

    pid_t fork_check = fork();
    if (fork_check == ERROR_FORK) {
        perror("Error while creating new process");
        return EXIT_FAILURE;
    }

    if (fork_check == CHILD_PROCESS) {
        execl("/bin/cat", "cat", argv[1], END_OF_ARGS);
        perror("execl error");
        return EXIT_FAILURE;
    }

    pid_t wait_check = wait(STATUS_IGNORE);
    if (wait_check == ERROR_WAIT) {
        perror("Error while waiting");
        return EXIT_FAILURE;
    }

    printf("Text written by parent\n");
    return EXIT_SUCCESS;
}
