#include "file_reader.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define NO_ERROR 0

extern int errno;

int read_line(char **buf, int *size, FILE *stream) {
    if (buf == NULL || size == NULL || stream == NULL || (*buf != NULL && *size <= 0)) {
        errno = EINVAL;
        return READ_ERROR;
    }

    int buf_size;
    if (*buf == NULL) {
        buf_size = BUFSIZ;
        *buf = (char *)malloc(buf_size * sizeof(char));
    }
    else {
        buf_size = *size;
    }

    int result = READ_SUCCESS;

    do {
        char *fgets_res = fgets(*buf, buf_size, stream);
        if (fgets_res == NULL) {
           if (errno == NO_ERROR) {
               result = READ_SUCCESS_EOF;
               break;
           }
           else {
               return READ_ERROR;
           }
        }

        int buf_len = strlen(*buf);
        if ((*buf)[buf_len - 1] == '\n') {
            break;
        }

        buf_size *= 2;
        char* realloc_res = (char *)realloc(*buf, buf_size * sizeof(char));
        if (realloc_res == NULL) {
            return READ_ERROR;
        }
    } while (1);

    *size = buf_size;
    return result;
}

