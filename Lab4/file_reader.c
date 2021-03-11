#include "file_reader.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define NO_ERROR 0
#define LINESZ 128
#define TRUE 1

extern int errno;

ssize_t read_line(char **lineptr, size_t *n, FILE *stream) {
    if (lineptr == NULL || n == NULL || stream == NULL) {
        errno = EINVAL;
        return READ_ERROR;
    }

    char delimiter = '\n';
    char *ptr = *lineptr;
    size_t size;
    ssize_t cnt;

    if (*lineptr == NULL || *n < LINESZ) {
        ptr = (char *)realloc(*lineptr, LINESZ * sizeof(char));
    }
    if (ptr == NULL) {
	errno = ENOMEM;
	return READ_ERROR;
    }

    *lineptr = ptr;
    size = LINESZ;
    *n = size;
    cnt = 0;

    do {
        char *fgets_res = fgets(ptr, LINESZ, stream);
        if (fgets_res == NULL && errno != NO_ERROR) {
           return READ_ERROR;
        }

        int length = strlen(ptr);
	cnt += length;

	if (length < (LINESZ - 1) || ptr[length - 1] == delimiter) {
            break;
        }

        ptr = (char *)realloc(*lineptr, 2 * size * sizeof(char));
        if (ptr == NULL) {
            return READ_ERROR;
        }
	size *= 2;
	*n = size;
	*lineptr = ptr;
	ptr = *lineptr + cnt;
    } while (TRUE);

    return cnt;
}

