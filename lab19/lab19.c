#define _FILE_OFFSET_BITS 64

#include <stdio.h>
#include <sys/types.h>
#include <limits.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>

#define SIZE (NAME_MAX + 1)
#define NO_ERROR 0

extern int errno;

int main(int argc, char **argv) {
    char pattern[SIZE];
    struct dirent *dp;
    DIR *dirp;
    int i = 0, length, count = 0, j, patternLength;
    int flag = 0;

    dirp = opendir(".");
    if (dirp == NULL) {
         perror("Can't open directory");
         exit(0);
    }

    printf("Enter pattern: ");
    fgets(pattern, SIZE, stdin);
    patternLength = strlen(pattern) - 1;
    pattern[patternLength] = 0;

    for (i = 0; i < patternLength; i++) {
        if (pattern[i] == '/') {
            printf("Symbol '/' is not supported pattern\n");
            exit(0);
        }
    }

    do {
	dp = readdir(dirp);
	if (dp == NULL) {
		if (errno != NO_ERROR) {
			perror("Can't read directory");
		}
		break;
	}

        length = strlen(dp->d_name);
        j = 0;
        flag = 0;

        for (i = 0; (length > i) && (patternLength > j); i++) {
            if (('?' != pattern[j]) && ('*' != pattern[j])) {
                if (pattern[j] != dp->d_name[i]) break;
                j++;
            } else if ('?' == pattern[j]) {
                j++;
            } else {
                while (patternLength > j) {
                    j++;
                    if ('*' != pattern[j]) break;
                }
                if (patternLength == j) {
                    flag = 1;
                    break;
                }
                if (pattern[j] == '?') {
                    j++;
                    continue;
                }
                while (length > i) {
                    if (pattern[j] == dp->d_name[i]) {
                        break;
                    }
                    i++;
                }
                j++;
            }
        }

        if (length == i) {
            while (patternLength > j) {
                if ('*' != pattern[j])
                    break;
                j++;
            }
            if (patternLength == j) flag = 1;
        }

        if ((1 == flag) || (i == length && j == patternLength)) {
            printf("%s\n", dp->d_name);
            count++;
        }

    } while (1);

    if (count == 0) {
        printf("Pattern was = %s\n", pattern);
    }
    closedir(dirp);
    return 0;
}

