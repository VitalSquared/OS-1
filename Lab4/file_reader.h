#ifndef LAB4_FILE_READER_H
#define LAB4_FILE_READER_H

#include <stdio.h>

#define READ_ERROR -1
#define READ_SUCCESS 0
#define READ_SUCCESS_EOF 1

int read_line(char **buf, int *size, FILE *stream);

#endif

