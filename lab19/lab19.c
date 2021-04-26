#include <stdio.h>
#include <sys/types.h>
#include <limits.h>
#include <dirent.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <fnmatch.h>
#include <unistd.h>
#include <string.h>

#define ERROR_OPEN_DIR NULL
#define ERROR_CLOSE_DIR -1
#define ERROR_FIND_MATCHES -1
#define ERROR_GET_PATTERN -1
#define ERROR_READ -1
#define ERROR_WRITE -1

#define SUCCESS_CLOSE_DIR 0
#define SUCCESS_GET_PATTERN 0

#define PATTERN_SIZE NAME_MAX
#define NO_ERROR 0
#define FNM_MATCH 0
#define NOT_ALL_ENTRIES_CHECKED 1
#define NO_ENTRY NULL

#define CURRENT_DIRECTORY "."

extern int errno;

int find_matching_entries(DIR *dirp, char *pattern) {
    int matches_count = 0;
    while (NOT_ALL_ENTRIES_CHECKED) {
        struct dirent *directory_entry = readdir(dirp);
        if (directory_entry == NO_ENTRY) {
            if (errno != NO_ERROR) {
                perror("Can't read directory");
                return ERROR_FIND_MATCHES;
            }
            break;
        }

        int fnmatch_check = fnmatch(pattern, directory_entry->d_name, FNM_PATHNAME);
        switch(fnmatch_check) {
            case FNM_MATCH:
                printf("%s\n", directory_entry->d_name);
                matches_count++;
                break;
            case FNM_NOMATCH:
                break;
            default:
                fprintf(stderr, "Error occurred in fnmatch\n");
                return ERROR_FIND_MATCHES;
        }
    }
    return matches_count;
}

ssize_t get_pattern(char *pattern, size_t size) {
    char msg[] = "Enter pattern: ";
    ssize_t write_check = write(STDOUT_FILENO, msg, strlen(msg));
    if (write_check == ERROR_WRITE) {
        perror("Error while writing message for user");
        return ERROR_GET_PATTERN;
    }

    ssize_t bytes_read = read(STDIN_FILENO, pattern, size);
    if (bytes_read == ERROR_READ) {
        perror("Error while reading from stdin");
        return ERROR_GET_PATTERN;
    }
    if (pattern[bytes_read - 1] == '\n') {
        pattern[bytes_read - 1] = '\0';
        bytes_read--;
    }

    for (int i = 0; i < bytes_read; i++) {
        if (pattern[i] == '/') {
            printf("Symbol \'/\' is not supported in the pattern\n");
            return ERROR_GET_PATTERN;
        }
    }

    return bytes_read;
}

DIR *open_directory(const char *dirname) {
    DIR *dirp = opendir(dirname);
    if (dirp == NULL) {
        perror("Can't open directory");
        return ERROR_OPEN_DIR;
    }
    return dirp;
}

int close_directory(DIR *dirp) {
    int close_check = closedir(dirp);
    if (close_check == ERROR_CLOSE_DIR) {
        perror("Can't close directory");
        return ERROR_CLOSE_DIR;
    }
    return SUCCESS_CLOSE_DIR;
}

int main(int argc, char **argv) {
    DIR *dirp = open_directory(CURRENT_DIRECTORY);
    if (dirp == ERROR_OPEN_DIR) {
        return EXIT_FAILURE;
    }

    char pattern[PATTERN_SIZE + 1];
    ssize_t pattern_length = get_pattern(pattern, PATTERN_SIZE);
    if (pattern_length == ERROR_GET_PATTERN) {
        close_directory(dirp);
        return EXIT_FAILURE;
    }
    pattern[pattern_length] = '\0';

    int matched_entries_count = find_matching_entries(dirp, pattern);
    if (matched_entries_count == ERROR_FIND_MATCHES) {
        close_directory(dirp);
        return EXIT_FAILURE;
    }
    if (matched_entries_count == 0) {
        printf("Pattern was = %s\n", pattern);
    }

    int close_check = close_directory(dirp);
    if (close_check == ERROR_CLOSE_DIR) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

