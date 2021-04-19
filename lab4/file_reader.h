#ifndef LAB4_FILE_READER_H
#define LAB4_FILE_READER_H

#include <stdio.h>

#define READ_ERROR -1

ssize_t read_line(char **lineptr, size_t *n, FILE *stream);

#endif

