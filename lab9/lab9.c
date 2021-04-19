#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

#define ERROR_FORK -1
#define ERROR_EXECL -1
#define ERROR_WAIT -1
#define CHILD_PROCESS 0
#define PARENT_PROCESS 1
#define END_OF_ARGS NULL

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: %s file\n", argv[0]);
        return 0;
    }

    pid_t fork_check = fork();
    if (fork_check == ERROR_FORK) {
        perror("Error while creating new process");
        return 0;
    }

    if (fork_check == CHILD_PROCESS) {
        printf("\nI am a new process\nMy PID: %d\nMy PPID:%d\n", getpid(), getppid());
        int execl_check = execl("/bin/cat", "cat", argv[1], END_OF_ARGS);

        if (execl_check == ERROR_EXECL) {
            perror("Error while opening cat");
            return 0;
        }
    }

    if (fork_check >= PARENT_PROCESS) {
        printf("I am parent\nMy child's PID: %d\nMy PID: %d\n", fork_check, getpid());
    }

    int status = 0;
    pid_t wait_check = wait(&status);
    if (wait_check == ERROR_WAIT) {
        perror("wait error");
        return 0;
    }

    printf("Wait ended.\n");
    return 0;
}

