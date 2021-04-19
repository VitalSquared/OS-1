#define _FILE_OFFSET_BITS 64

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>

#define SIZE 256

int main(int argc, char **argv) {
    char pattern[SIZE];
    struct dirent *p;
    DIR *dir;
    int i = 0, length, count = 0, j, patternLength;
    int flag = 0;
    dir = opendir(".");
    if (!dir) {
         perror("opendir");
         exit(2);
    }
    printf("Enter pattern: ");
    fgets(pattern, SIZE, stdin);
    patternLength = strlen(pattern) - 1;
    pattern[patternLength] = 0;
    
    while (0 != (p = readdir(dir))) {
        length = strlen(p->d_name);
        j = 0;
        flag = 0;
        for (i = 0; (length > i) && (patternLength > j); i++) {
            if (('?' != pattern[j]) && ('*' != pattern[j])) {
                if (pattern[j] != p->d_name[i]) break;
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
                    if (pattern[j] == p->d_name[i]) {
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
            printf("%s\n", p->d_name);
            count++;
        }
    }
    if (!count) {
        printf("Pattern was = %s\n", pattern);
    }
    closedir(dir);
    return 0;
}

