#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv) {
	FILE *file;

	printf("Real user id: %d,  effective user id: %d\n", getuid(), geteuid());
	file = fopen("file", "r");
	if (file == NULL) {
		perror("File not opened at first try");
		exit(0);
	}
	else {
		printf("File opened at first try\n");
		fclose(file);
	}

	seteuid(getuid());

	printf("New real user id: %d,  new effective user id: %d\n", getuid(), geteuid());
	file = fopen("file", "r");
	if (file == NULL) {
		perror("File not opened at second try");
		exit(0);
	}
	else {
		printf("File opened at second try\n");
		fclose(file);
	}
	return 0;
}
