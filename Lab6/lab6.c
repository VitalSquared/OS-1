#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Usage: <programname> <filename>\n");
		return 0;
	}

	int fd = open(argv[1], O_RDONLY);
	if (fd == -1) {
		perror("Unable to open file\n");
		return 0;
	}	

	long offsets[101] = { -1 };
	int lineLength[101] = { -1 };
	int bufSize = 256, i = 1, j = 0, lineNumber = 0;
	char* buf = (char*) malloc(sizeof(char) * (bufSize + 1));
	char c = 0;

	while (read(fd, &c, 1)) {
		j++;
		if (c == '\n') {
			lineLength[i++] = j;
			offsets[i] = lseek(fd, 0L, SEEK_CUR);
			j = 0;	
		}
	}

	while(1) {
		printf("Enter line number: ");
		if (!scanf("%d", &lineNumber)) break;
		if (lineNumber == 0) break;
		if (lineNumber < 0 || lineNumber > 100 || lineLength[lineNumber] == -1) {
			fprintf(stderr, "Wrong line number\n");
			continue;
		}
		lseek(fd, offsets[lineNumber], SEEK_SET);
		if (lineLength[lineNumber] > bufSize) {
			bufSize = lineLength[lineNumber];
			realloc(buf, (bufSize + 1) * sizeof(char));
		}
		if (read(fd, buf, lineLength[lineNumber]))
			write(1, buf, lineLength[lineNumber]);
		else
			fprintf(stderr, "Wrong line number\n");
	}

	free(buf);
	close(fd);

	return 0;
}
