#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>
#include <ulimit.h>
#include <stdlib.h>
#include <stdio.h>

extern char **environ;

int main(int argc, char **argv) {
	int curarg;
	char options[] = "ispuU:cC:dvV:";
	struct rlimit rlp;
	if (argc < 2) {
		fprintf(stderr, "Usage: %s [-i] [-s] [-p] [-u] [-Uvalue] [-c] [-Cvalue] [-d] [-v] [-Vvalue]\n", argv[0]);
		exit(0);
	}
	while (1) {
		curarg = getopt(argc, argv, options);
		if (curarg == EOF) break;
		switch(curarg) {
			case 'i':
				printf("user id = %ld\n", getuid());
				printf("effective user id = %ld\n", geteuid());
				printf("group id = %ld\n", getgid());
				printf("effective group id = %ld\n", getegid());
				break;
			case 's':
				(void) setpgid(0, 0);
				break;
			case 'p':
				printf("process id = %ld\n", getpid());
				printf("parent process id = %ld\n", getppid());
				printf("process group id = %ld\n", getpgid(0));
				break;
			case 'U':
				if (ulimit(UL_SETFSIZE, atol(optarg)) == -1) {
					perror("Must be super-user to increase ulimit\n");
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
				if (setrlimit(RLIMIT_CORE, &rlp) == -1)
					perror("Must be super-user to increase core\n");
				break;
			case 'd':
				printf("current working directiry is: %s\n", getcwd(NULL, 100));
				break;
			case 'v':
				printf("environment variables are:\n");
				for (char** env = environ; *env; env++)
					printf("%s\n", *env);
				break;
			case 'V':
				putenv(optarg);
				break;
		}
	}
	return 0;
}
