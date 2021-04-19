#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#define ERROR_FORK -1
#define ERROR_WAIT -1
#define CHILD_PROCESS 0

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s program_name [program_arg0 ...]\n", argv[0]);
        return 0;
    }

    pid_t fork_check = fork();
    if (fork_check == ERROR_FORK) {
        perror("Error while creating process");
        return 0;
    }

    if (fork_check == CHILD_PROCESS) {
        execvp(argv[1], &argv[1]);
        perror("execvp error");
        return 0;
    }

    int status = 0;
    pid_t wait_check = wait(&status);
    if (wait_check == ERROR_WAIT) {
        perror("wait error");
        return 0;
    }

    int exit_status = WEXITSTATUS(status);
    printf("Child process exit status: %d\n", exit_status);
    return 0;
}
