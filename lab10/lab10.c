#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define ERROR_FORK -1
#define ERROR_WAIT -1
#define CHILD_PROCESS 0
#define MIN_NUM_OF_ARGS 2
#define STATUS_INIT 0
#define FALSE 0

int main(int argc, char **argv) {
    if (argc < MIN_NUM_OF_ARGS) {
        printf("Usage: %s program_name [program_arg0 ...]\n", argv[0]);
        return EXIT_SUCCESS;
    }

    pid_t fork_check = fork();
    if (fork_check == ERROR_FORK) {
        perror("Error while creating process");
        return EXIT_FAILURE;
    }

    if (fork_check == CHILD_PROCESS) {
        execvp(argv[1], &argv[1]);
        perror("execvp error");
        return EXIT_FAILURE;
    }

    int status = STATUS_INIT;
    pid_t wait_check = wait(&status);
    if (wait_check == ERROR_WAIT) {
        perror("wait error");
        return EXIT_FAILURE;
    }

    if (WIFEXITED(status) != FALSE) {
        int exit_status = WEXITSTATUS(status);
        printf("Child process terminated successfully with exit status: %d\n", exit_status);
    }
    else if (WIFSIGNALED(status) != FALSE) {
        int signal = WTERMSIG(status);
        printf("Child process terminated by a signal: %d\n", signal);
    }
    return EXIT_SUCCESS;
}
