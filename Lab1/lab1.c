#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>
#include <ulimit.h>
#include <stdlib.h>
#include <stdio.h>

extern char **environ;

#define PUTENV_SUCCESS 0
#define ERR_SETRLIMIT -1
#define ERR_ULIMIT -1
#define INVALID_ARG -1

int main(int argc, char **argv) {
	int curarg, putenv_res, setrlimit_res, ulimit_res;
	char options[] = "ispuU:cC:dvV:";
	struct rlimit rlp;
	if (argc < 2) {
		printf("Usage: %s [-i] [-s] [-p] [-u] [-Uvalue] [-c] [-Cvalue] [-d] [-v] [-Vvalue]\n", argv[0]);
		exit(0);
	}
	while (1) {
		curarg = getopt(argc, argv, options);
		if (curarg == INVALID_ARG) break;
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
					perror("Must be super-user to increase ulimit");
				}
				break;
			case 'u':
				printf("ulimit = %ld\n", ulimit(UL_GETFSIZE));
				break;
			case 'c':
				getrlimit(RLIMIT_CORE, &rlp);
				printf("core size = %ld\n", rlp.rlim_cur);
				break;
			case 'C':
				getrlimit(RLIMIT_CORE, &rlp);
				rlp.rlim_cur = atol(optarg);
				setrlimit_res = setrlimit(RLIMIT_CORE, &rlp);
				if (setrlimit_res == ERR_SETRLIMIT)
					perror("Must be super-user to increase core");
				break;
			case 'd':
				printf("Current working directiry is: %s\n", getcwd(NULL, 100));
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
	return 0;
}
