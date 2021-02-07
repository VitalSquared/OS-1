#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>
#include <ulimit.h>
#include <stdlib.h>
#include <stdio.h>

extern char **environ;

int main(int argc, char **argv) {
	int currentArgument;
	char options[] = "ispuU:cC:dvV:";
	struct rlimit rlp;
	char **currentEnvVar;
	if (argc < 2) {
		fprintf(stderr, "Usage: %s options\n", argv[0]);
		exit(0);
	}
	while ((currentArgument = getopt(argc, argv, options)) != EOF) {
		switch(currentArgument) {
			case 'i':
				printf("uid = %ld\n", getuid());
				printf("euid = %ld\n", geteuid());
				printf("gid = %ld\n", getgid());
				printf("egid = %ld\n", getegid());
				break;
			case 's':
				(void) setpgid(0, 0);
				break;
			case 'p':
				break;
			case 'U':
				break;
			case 'u':
				break;
			case 'c':
				break;
			case 'C':
				break;
			case 'd':
				printf("current working directiry is: %s\n", getcwd(NULL, 100));
				break;
			case 'v':
				break;
			case 'V':
				break;
		}
	}
	return 0;
}
