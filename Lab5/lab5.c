#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ERROR_FILE_NOT_OPEN -1
#define ERROR_READ -1
#define ERROR_WRITE -1
#define ERROR_ADD_TO_TABLE -1
#define ERROR_GET_LINE_NUMBER -1
#define ERROR_PRINT -1
#define SUCCESS_ADD_TO_TABLE 0
#define SUCCESS_PRINT 0
#define READ_EOF 0
#define TABLE_INIT_SIZE 100
#define CONSOLE_INPUT_SIZE 100
#define TRUE 1
#define STOP_INPUT 0

typedef struct line_info {
	size_t offset;
	size_t length;
} line_info;

int table_add(line_info **table, size_t *size, size_t *length, line_info elem) {
	if (table == NULL || size == NULL || length == NULL) {
		fprintf(stderr, "Can't add element to table: Invalid argument(s)\n");
		return ERROR_ADD_TO_TABLE;
	}
	
	if (*table == NULL) {
		*table = (line_info *)malloc(TABLE_INIT_SIZE * sizeof(line_info));
		if (*table == NULL) {
			perror("Can't add element to table");
			return ERROR_ADD_TO_TABLE;
		}
		*size = TABLE_INIT_SIZE;
		*length = 0;
	}

	if (*length == *size) {
		line_info *ptr = (line_info *)realloc(*table, 2 * (*size) * sizeof(line_info));
		if (ptr == NULL) {
			perror("Can't add element to table");
			return ERROR_ADD_TO_TABLE;
		}
		(*size) *= 2;
		*table = ptr;
	}
	
	(*length)++;
	size_t last_index = (*length) - 1;
	(*table)[last_index] = elem;

	return SUCCESS_ADD_TO_TABLE;
}

line_info *table_create(int fildes, size_t *size, size_t *length) {
	if (length == NULL || size == NULL) {
		fprintf(stderr, "Can't create table: Invalid argument(s)\n");
		return NULL;
	}

	line_info *table = NULL;
	size_t size = 0;
	*length = 0;

	char c;
	ssize_t read_res;
	size_t length = 0, offset;
	do {
		read_res = read(fildes, &c, 1);
		if (read_res == ERROR_READ) {
			if (table != NULL) free(table);
			perror("Can't read from file");
			return NULL;
		}
		if (read_res == READ_EOF || c == '\n') {
			offset = lseek(fildes, 0L, SEEK_CUR);
			line_info elem = { .offset = offset, .length = *length };
			int add_res = table_add(&table, &size, length, elem);
			if (add_res == ERROR_ADD_TO_TABLE) {
				if (table != NULL) free(table);
				return NULL;
			}
			*length = 0;
			continue;
		}
		(*length)++;
	} while (read_res != READ_EOF);

	return table;
}

long long get_line_number() {
        long long line_number;
	char console_input[CONSOLE_INPUT_SIZE]; 
	printf("Enter line number: "); 

        int read_check = read(STDIN_FILENO, console_input, CONSOLE_INPUT_SIZE);
	if (read_check == ERROR_READ) {
		perror("Can't get line number");
		return ERROR_GET_LINE_NUMBER;
	}				        }

        char* endptr = NULL;
	line_number = strtoll(console_input, &endptr, DECIMAL_SYSTEM);	        

	return line_number;
}

void print_line(int fildes, size_t offset, size_t length) {
	int write_check = write(STDOUT_FILENO, line, line_length);
    	if (write_check == ERROR_WRITE)
    	{
        	perror("Can't print line");
        	return ERROR_PRINT;
    	}
    	return SUCCESS_PRINT;
}

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Usage: %s <filename>\n", argv[0]);
		return 0;
	}

	int fildes = open(argv[1], O_RDONLY);
	if (fildes == ERROR_FILE_NOT_OPEN) {
		perror("Unable to open file");
		return 0;
	}	

	size_t table_length = 0;
	line_info *table = table_create(fildes, &table_length);
	if (table == NULL) {
		close(fildes);
		return 0;
	}

	while(TRUE) {	
		long long line_num = get_line_number();
	
		if (line_num < 0 || line_num > table_length) {
			fprintf(stderr, "Invalid line number. It has to be in range [0, %lld]\n", table_length);
			continue;
		}
		if (line_num == STOP_INPUT) break;

		size_t offset = table[line_num - 1].offset;
		size_t length = table[line_num - 1].length;
		print_line(fildes, offset, length);
	}

	free(table);
	close(fildes);

	return 0;
}
