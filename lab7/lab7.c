#include <sys/types.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

extern int errno;

#define ERROR_OPEN_FILE -1
#define ERROR_CLOSE_FILE -1
#define ERROR_READ -1
#define ERROR_WRITE -1
#define ERROR_ADD_TO_TABLE -1
#define ERROR_GET_LINE_NUMBER -1
#define ERROR_LSEEK -1
#define ERROR_ADD_TO_TABLE -1
#define ERROR_PRINT_FILE -1
#define ERROR_PRINT_LINES -1
#define ERROR_PRINT_LINE -1
#define ERROR_SELECT -1
#define ERROR_FSTAT -1
#define ERROR_MUNMAP -1
#define ERROR_STRTOLL -1
#define ERROR_FILL_TABLE -1

#define NO_ERROR 0
#define SUCCESS_OPEN_FILE 0
#define SUCCESS_CLOSE_FILE 0
#define SUCCESS_READ 0
#define SUCCESS_WRITE 0
#define SUCCESS_GET_LINE_NUMBER 1
#define SUCCESS_ADD_TO_TABLE 0
#define SUCCESS_PRINT_FILE 0
#define SUCCESS_PRINT_LINES 0
#define SUCCESS_PRINT_LINE 0
#define SUCCESS_SELECT 1
#define SUCCESS_STRTOLL 0
#define SUCCESS_FILL_TABLE 0

#define GET_LINE_NUMBER_TIMEOUT 2
#define INVALID_LINE_NUMBER_INPUT 0
#define SELECT_NO_REACTION 0
#define READ_TIMEOUT 2
#define READ_NOTHING 1

#define NULL_WRITEFDS NULL
#define NULL_ERRORFDS NULL
#define STRING_EQUAL 0
#define READ_EOF 0
#define TABLE_INIT_SIZE 100
#define INPUT_SIZE 128
#define BUFFER_SIZE 256
#define NOT_STOP_INPUT 1
#define TRUE 1
#define FALSE 0
#define STOP_INPUT 0
#define DECIMAL_SYSTEM 10
#define SELECT_MAX_FILDES_PLUS_1 1
#define TIMEOUT_SEC 5
#define TIMEOUT_USEC 0
#define WITH_NEW_LINE 1
#define WITHOUT_NEW_LINE 0
#define ANY_ADDRESS 0
#define FILE_START_POS 0

typedef struct line_info {
    off_t offset;
    size_t length;
} line_info;

int add_to_table(line_info **table, long long *table_size, long long *table_length, off_t line_offset, size_t line_length) {
    if (table == NULL || *table == NULL || table_size == NULL || table_length == NULL) {
        fprintf(stderr, "Can't add element to table: Invalid argument(s)");
        return ERROR_ADD_TO_TABLE;
    }

    if (*table_length == *table_size) {
        line_info *ptr = (line_info *) realloc(*table, 2 * (*table_size) * sizeof(line_info));
        if (ptr == NULL) {
            perror("Can't create table");
            return ERROR_ADD_TO_TABLE;
        }
        *table = ptr;
        (*table_size) *= 2;
    }

    line_info elem;
    elem.offset = line_offset;
    elem.length = line_length;

    (*table)[*table_length] = elem;
    (*table_length)++;

    return SUCCESS_ADD_TO_TABLE;
}

int fill_table(char *file_addr, off_t file_size, line_info **table, long long *table_size, long long *table_length) {
    size_t line_length = 0;
    off_t line_offset = 0, file_offset = 0;

    while (file_offset <= file_size) {
        char *c = NULL;
        if (file_offset < file_size) {
            c = file_addr + file_offset;
        }

        if (file_offset < file_size && (c != NULL && *c != '\n')) {
            line_length++;
            file_offset++;
            continue;
        }

        int add_check = add_to_table(table, table_size, table_length, line_offset, line_length);
        if (add_check == ERROR_ADD_TO_TABLE) {
            return ERROR_FILL_TABLE;
        }

        line_length = 0;
        file_offset++;
        line_offset = file_offset;
    }

    return SUCCESS_FILL_TABLE;
}

line_info *create_table(char *file_addr, off_t file_size, long long *table_length) {
    if (table_length == NULL) {
        fprintf(stderr, "Can't create table: Invalid argument(s)\n");
        return NULL;
    }

    long long size = TABLE_INIT_SIZE;
    *table_length = 0;
    line_info *table = (line_info *) malloc(size * sizeof(line_info));
    if (table == NULL) {
        perror("Can't create table");
        return NULL;
    }

    int fill_check = fill_table(file_addr, file_size, &table, &size, table_length);
    if (fill_check == ERROR_FILL_TABLE) {
        free(table);
        return NULL;
    }

    return table;
}

int write_to_console(const char *buf, int length, int new_line) {
    ssize_t write_check = write(STDOUT_FILENO, buf, length);
    if (write_check == ERROR_WRITE) {
        perror("Can't write to console");
        return ERROR_WRITE;
    }
    if (new_line == WITH_NEW_LINE) {
        return write_to_console("\n", 1, WITHOUT_NEW_LINE);
    }
    return SUCCESS_WRITE;
}

int wait_for_input() {
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);

    struct timeval timeout;
    timeout.tv_sec = TIMEOUT_SEC;
    timeout.tv_usec = TIMEOUT_USEC;

    int select_check = select(SELECT_MAX_FILDES_PLUS_1, &readfds, NULL_WRITEFDS, NULL_ERRORFDS, &timeout);

    if (select_check == ERROR_SELECT) {
        perror("Select error");
        return ERROR_SELECT;
    }
    if (select_check == SELECT_NO_REACTION) {
        int write_check = write_to_console("Time is out!\n", 13, WITHOUT_NEW_LINE);
        if (write_check == ERROR_WRITE) {
            return ERROR_SELECT;
        }
        return SELECT_NO_REACTION;
    }
    if (FD_ISSET(STDIN_FILENO, &readfds) == FALSE) {
        return SELECT_NO_REACTION;
    }
    return SUCCESS_SELECT;
}

int validate_strtoll(char *endptr) {
    if (endptr == NULL) {
        return SUCCESS_STRTOLL;
    }

    if (errno != NO_ERROR) {
        perror("Can't convert given number");
        errno = 0;
        return ERROR_STRTOLL;
    }

    int compare1_result = strcmp(endptr, "\n");
    int compare2_result = strcmp(endptr, "");
    if (compare1_result != STRING_EQUAL && compare2_result != STRING_EQUAL) {
        fprintf(stderr, "Number contains invalid symbols\n");
        return ERROR_STRTOLL;
    }

    return SUCCESS_STRTOLL;
}

int read_from_console(char *input, size_t size) {
    int wait_check = wait_for_input();
    if (wait_check == ERROR_SELECT) {
        return ERROR_READ;
    }
    if (wait_check == SELECT_NO_REACTION) {
        return READ_TIMEOUT;
    }

    ssize_t bytes_read = read(STDIN_FILENO, input, size);
    if (bytes_read == ERROR_READ) {
        perror("Can't get line number");
        return ERROR_READ;
    }
    if (bytes_read == 0) {
        return READ_NOTHING;
    }
    input[bytes_read] = '\0';
    return SUCCESS_READ;
}

int get_line_number(long long *line_num) {
    char input[INPUT_SIZE + 1];

    int write_check = write_to_console("Five seconds to enter line number: ", 35, WITHOUT_NEW_LINE);
    if (write_check == ERROR_WRITE) {
        return ERROR_GET_LINE_NUMBER;
    }

    int read_check = read_from_console(input, INPUT_SIZE);
    switch (read_check) {
        case ERROR_READ:
            return ERROR_GET_LINE_NUMBER;
        case READ_TIMEOUT:
            return GET_LINE_NUMBER_TIMEOUT;
        case READ_NOTHING:
            return INVALID_LINE_NUMBER_INPUT;
    }

    char *endptr = input;
    *line_num = strtoll(input, &endptr, DECIMAL_SYSTEM);

    int strtoll_check = validate_strtoll(endptr);
    if (strtoll_check == ERROR_STRTOLL) {
        return INVALID_LINE_NUMBER_INPUT;
    }

    return SUCCESS_GET_LINE_NUMBER;
}

int print_file(char *file_addr, off_t file_size) {
    off_t file_offset = 0;
    while (file_offset < file_size) {
        int write_size = BUFFER_SIZE;
        if (file_size - file_offset < BUFFER_SIZE) {
            write_size = file_size - file_offset;
        }

        int write_check = write_to_console(file_addr + file_offset, write_size, WITHOUT_NEW_LINE);
        if (write_check == ERROR_WRITE) {
            return ERROR_PRINT_FILE;
        }
        file_offset += write_size;
    }

    int write_check = write_to_console("\n", 1, WITHOUT_NEW_LINE);
    if (write_check == ERROR_WRITE) {
        return ERROR_PRINT_FILE;
    }

    return SUCCESS_PRINT_FILE;
}

int open_file(int argc, char** argv, int *fildes, off_t *file_size) {
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return ERROR_OPEN_FILE;
    }
    *fildes = open(argv[1], O_RDONLY);
    if (*fildes == ERROR_OPEN_FILE) {
        perror("Can't open file");
        return ERROR_OPEN_FILE;
    }

    struct stat st;
    int fstat_check = fstat(*fildes, &st);
    if (fstat_check == ERROR_FSTAT) {
        perror("Can't get file stat");
        return ERROR_OPEN_FILE;
    }
    *file_size = st.st_size;

    return SUCCESS_OPEN_FILE;
}

int close_file(int fildes) {
    int close_check = close(fildes);
    if (close_check == ERROR_CLOSE_FILE) {
        perror("Can't close file");
        return ERROR_CLOSE_FILE;
    }
    return SUCCESS_CLOSE_FILE;
}

int print_line(char *file_addr, line_info *table, long long line_num) {
    off_t line_offset = table[line_num - 1].offset;
    size_t line_length = table[line_num - 1].length;

    int write_check = write_to_console(file_addr + line_offset, line_length, WITH_NEW_LINE);
    if (write_check == ERROR_WRITE) {
        return ERROR_PRINT_LINE;
    }

    return SUCCESS_PRINT_LINE;
}

int print_lines(char *file_addr, off_t file_size, line_info *table, long long table_length) {
    if (table == NULL) {
        return ERROR_PRINT_LINES;
    }

    long long line_num;
    while (NOT_STOP_INPUT) {
        int get_line_num_check = get_line_number(&line_num);

        if (get_line_num_check == ERROR_GET_LINE_NUMBER) {
            break;
        }
        if (get_line_num_check == INVALID_LINE_NUMBER_INPUT) {
            continue;
        }
        if (get_line_num_check == GET_LINE_NUMBER_TIMEOUT) {
            print_file(file_addr, file_size);
            break;
        }
        if (line_num < 0 || line_num > table_length) {
            fprintf(stderr, "Invalid line number. It has to be in range [0, %lld]\n", table_length);
            continue;
        }
        if (line_num == STOP_INPUT) {
            break;
        }

        int print_line_check = print_line(file_addr, table, line_num);
        if (print_line_check == ERROR_PRINT_LINE) {
            return ERROR_PRINT_LINES;
        }
    }
    return SUCCESS_PRINT_LINES;
}

int main(int argc, char** argv) {
    int fildes;
    off_t file_size;

    int open_check = open_file(argc, argv, &fildes, &file_size);
    if (open_check == ERROR_OPEN_FILE) {
        return 0;
    }

    char *file_addr = (char *) mmap(ANY_ADDRESS, file_size, PROT_READ, MAP_SHARED, fildes, FILE_START_POS);
    if (file_addr == MAP_FAILED) {
        perror("Can't map file");
        return 0;
    }

    long long table_length = 0, line_num = 0;
    line_info *table = create_table(file_addr, file_size, &table_length);
    print_lines(file_addr, file_size, table, table_length);

    if (table != NULL) {
        free(table);
    }

    int munmap_check = munmap(file_addr, file_size);
    if (munmap_check == ERROR_MUNMAP) {
        perror("Can't clean memory");
    }

    close_file(fildes);
    return 0;
}
