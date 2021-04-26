#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <grp.h>
#include <pwd.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>

#define ERROR_PRINT_FILE_INFO -1
#define ERROR_STAT -1

#define SUCCESS_PRINT_FILE_INFO 0

#define NUMBER_OF_FILE_PERMISSIONS 9
#define NUMBER_OF_PERMISSIONS 3

#define TRUE 1
#define FALSE 0

#define NO_INFO "?"
#define CURRENT_DIRECTORY "."

int file_has_permission(struct stat *statbuf, mode_t permission) {
    if ((statbuf->st_mode & permission) != FALSE) {
        return TRUE;
    }
    return FALSE;
}

void get_file_permissions(struct stat *statbuf, char *permissions_string) {
    char permission_symbols[NUMBER_OF_PERMISSIONS] = {'r', 'w', 'x'};
    int file_permissions[NUMBER_OF_FILE_PERMISSIONS] = {S_IRUSR, S_IWUSR, S_IXUSR,
                                                        S_IRGRP, S_IWGRP, S_IXGRP,
                                                        S_IROTH, S_IWOTH, S_IXOTH};

    for (int perm_idx = 0; perm_idx < NUMBER_OF_FILE_PERMISSIONS; perm_idx++) {
        if (file_has_permission(statbuf, file_permissions[perm_idx]) == TRUE) {
            permissions_string[perm_idx] = permission_symbols[perm_idx % NUMBER_OF_PERMISSIONS];
        } else {
            permissions_string[perm_idx] = '-';
        }
    }
}

char *get_file_owner(struct stat *statbuf) {
    struct passwd *owner_info = getpwuid(statbuf->st_uid);
    if (owner_info == NULL) {
        perror("Can't get file owner info");
        return NO_INFO;
    }
    return owner_info->pw_name;
}

char *get_file_group(struct stat *statbuf) {
    struct group *group_info = getgrgid(statbuf->st_gid);
    if (group_info == NULL) {
        perror("Can't get file group info");
        return NO_INFO;
    }
    return group_info->gr_name;
}

char *get_file_modification_time(struct stat *statbuf) {
    char *time = ctime(&statbuf->st_mtime);
    if (time == NULL) {
        perror("Can't get file timestamp");
        exit(EXIT_FAILURE);
    }
    return time;
}

char get_file_type(struct stat *statbuf) {
    switch (statbuf->st_mode & S_IFMT) {
        case S_IFREG:
            return '-';
        case S_IFDIR:
            return 'd';
        default:
            return '?';
    }
}

void print_file_info(char *filename) {
    struct stat statbuf;
    int stat_check = stat(filename, &statbuf);
    if (stat_check == ERROR_STAT) {
        perror(filename);
        return;
    }

    char permissions_string[NUMBER_OF_FILE_PERMISSIONS + 1];
    get_file_permissions(&statbuf, permissions_string);
    permissions_string[NUMBER_OF_FILE_PERMISSIONS] = '\0';

    printf("%c%s   %lu %s %s    %ld %.19s %s\n",
           get_file_type(&statbuf),
           permissions_string,
           statbuf.st_nlink,            //number of links
           get_file_owner(&statbuf),
           get_file_group(&statbuf),
           statbuf.st_size,             //file size
           get_file_modification_time(&statbuf),
           basename(filename));         //get last component of path
}

int main(int argc, char **argv) {
    if (argc < 2) {
        print_file_info(CURRENT_DIRECTORY);
        return EXIT_SUCCESS;
    }

    for (int file_idx = 1; file_idx < argc; file_idx++) {
        print_file_info(argv[file_idx]);
    }

    return EXIT_SUCCESS;
}