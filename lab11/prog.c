#include <stdio.h>
#include <stdlib.h>

#define MIN_NUM_OF_ARGS 2

extern char **environ;

int main(int argc, char **argv) {
    printf("My environment variables: \n");
    for (char **env_var = environ; *env_var != NULL; env_var++) {
        printf("%s\n", *env_var);
    }

    if (argc >= MIN_NUM_OF_ARGS) {
        printf("My arguments: \n");
        for (int arg_num = 1; arg_num < argc; arg_num++) {
            printf("%s\n", argv[arg_num]);
        }
    }

    printf("End of program\n");
    return EXIT_SUCCESS;
}
