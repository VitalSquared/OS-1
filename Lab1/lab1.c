#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>
#include <ulimit.h>
#include <stdlib.h>
#include <stdio.h>

extern char **environ;
extern int optopt;

#define PUTENV_SUCCESS 0
#define ERR_RLIMIT -1
#define ERR_ULIMIT -1

#define END_OF_OPTIONS -1
#define INVALID_OPTION '?'
#define MISSING_ARG ':'

#define TRUE 1

int main(int argc, char **argv) {
	int curarg, putenv_res, rlimit_res;
        long ulimit_res;
	char options[] = ":ispuU:cC:dvV:";
	char *dir = NULL;
	struct rlimit rlp;
	if (argc < 2) {
		printf("Usage: %s [-i] [-s] [-p] [-u] [-Uvalue] [-c] [-Cvalue] [-d] [-v] [-Vvalue]\n", argv[0]);
		exit(0);
	}
	while (TRUE) {
		curarg = getopt(argc, argv, options);
		
		if (curarg == END_OF_OPTIONS) break;
	        if (curarg == INVALID_OPTION) {
			fprintf(stderr, "Unrecognized option: -%c\n", optopt);
			break;
		}
		if (curarg == MISSING_ARG) {
			fprintf(stderr, "Option -%c requires an argument\n", optopt);
			break;
		}
		
		switch(curarg) {
			case 'i':
				printf("user id = %u\n", getuid());
				printf("effective user id = %u\n", geteuid());
				printf("group id = %u\n", getgid());
				printf("effective group id = %u\n", getegid());
				break;
			case 's':
				(void) setpgid(0, 0);
				break;
			case 'p':
				printf("process id = %u\n", getpid());
				printf("parent process id = %u\n", getppid());
				printf("process group id = %u\n", getpgid(0));
				break;
			case 'U':
				ulimit_res = ulimit(UL_SETFSIZE, atol(optarg));
				if (ulimit_res == ERR_ULIMIT) {
					perror("Can't change ulimit value");
				}
				break;
			case 'u':
				ulimit_res = ulimit(UL_GETFSIZE);
				if (ulimit_res == ERR_ULIMIT) 
					perror("Can't get ulimit value");
				else 
					printf("ulimit = %ld\n", ulimit_res);
				break;
			case 'c':
				rlimit_res = getrlimit(RLIMIT_CORE, &rlp);
				if (rlimit_res == ERR_RLIMIT)
					perror("Can't get core-file size");
				else 
					printf("core size = %ld\n", rlp.rlim_cur);
				break;
			case 'C':
				getrlimit(RLIMIT_CORE, &rlp);
				rlp.rlim_cur = atol(optarg);
				rlimit_res = setrlimit(RLIMIT_CORE, &rlp);
				if (rlimit_res == ERR_RLIMIT)
					perror("Can't change core-file size");
				break;
			case 'd':
				dir = getcwd(NULL, 512);
				printf("Current working directiry is: %s\n", dir);
				free(dir);
				break;
			case 'v':
				printf("Environment variables are:\n");
				for (char** env = environ; *env; env++) {
					printf("%s\n", *env);
				}
				break;
			case 'V':
				putenv_res = putenv(optarg);
				if (putenv_res != PUTENV_SUCCESS) {
					perror("Unable to put environment variable");
				}
				break;
		}
	}
	exit(0);
}
