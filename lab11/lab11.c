#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>

#define ERROR_EXECVPE -1
#define MIN_NUM_OF_ARGS 2

extern char **environ;

int execvpe(const char *file, char *const argv[], char *const envp[]) {
    char **saved_environ = environ;
    environ = envp;
    execvp(file, argv);
    perror("execvp error");
    environ = saved_environ;
    return ERROR_EXECVPE;
}

int main(int argc, char **argv) {
    if (argc < MIN_NUM_OF_ARGS) {
        printf("Usage: %s program_name [program_arg0 ...]\n", argv[0]);
        return EXIT_SUCCESS;
    }

    char *new_envp[2] = { "PATH=/home/students/19200/v.spirin/lab11/", NULL };
    execvpe(argv[1], &argv[1], new_envp);
    return EXIT_FAILURE;
}
