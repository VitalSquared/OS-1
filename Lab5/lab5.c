#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define ERROR_OPEN_FILE -1
#define ERROR_CLOSE_FILE -1
#define ERROR_READ -1
#define ERROR_WRITE -1
#define ERROR_ADD_TO_TABLE -1
#define ERROR_GET_LINE_NUMBER -1
#define ERROR_PRINT -1
#define ERROR_LSEEK -1

#define SUCCESS_CLOSE_FILE 0
#define SUCCESS_READ 0
#define SUCCESS_WRITE 0

#define INVALID_LINE_NUMBER_INPUT 0
#define SUCCESS_GET_LINE_NUMBER 1

#define READ_EOF 0
#define TABLE_INIT_SIZE 100
#define CONSOLE_INPUT_SIZE 128
#define TRUE 1
#define STOP_INPUT 0
#define DECIMAL_SYSTEM 10

typedef struct line_info {
	size_t offset;
	size_t length;
} line_info;

int open_file(const char *fname) {
	int fildes = open(fname, O_RDONLY);
	if (fildes == ERROR_OPEN_FILE) {
		perror("Unable to open file");
		return ERROR_OPEN_FILE;
	}	
	return fildes;
}

int close_file(int fildes) {
	int close_check = close(fildes);
	if (close_check == ERROR_CLOSE_FILE) {
		perror("Unable to close file");
		return ERROR_CLOSE_FILE;
	}
	return SUCCESS_CLOSE_FILE;
}

line_info *create_table(int fildes, size_t *table_length) {
	if (table_length == NULL) {
		fprintf(stderr, "Can't create table: Invalid argument(s)\n");
		return NULL;
	}
	
	size_t size = TABLE_INIT_SIZE;
	*table_length = 0;
	line_info *table = (line_info *) malloc(size * sizeof(line_info));
	if (table == NULL) {
		perror("Can't create table");
		return NULL;
	}

	char c;
	ssize_t read_check;
	size_t line_length = 0, offset = lseek(fildes, 0L, SEEK_SET);
	do {
		read_check = read(fildes, &c, 1);
		if (read_check == ERROR_READ) {
			free(table);
			perror("Can't read from file");
			return NULL;
		}
		if (read_check == READ_EOF || c == '\n') {
			if (*table_length == size) {
				line_info *ptr = (line_info *)realloc(table, 2 * size * sizeof(line_info));
				if (ptr == NULL) {
					free(table);
					perror("Can't create table");
					return NULL;
				}
				table = ptr;
				size *= 2;
			}

			line_info elem = { .offset = offset, .length = line_length };		
			table[*table_length] = elem;
			(*table_length)++;

			line_length = 0;
			off_t lseek_check = lseek(fildes, 0L, SEEK_CUR);
			if (lseek_check == ERROR_LSEEK) {
				free(table);
				perror("Can't get position in file");
				return NULL;
			}
			offset = lseek_check;
			continue;
		} 
		line_length++;
	} while (read_check != READ_EOF);

	return table;
}

int get_line_number(long long *line_num) {
	char console_input[CONSOLE_INPUT_SIZE]; 
	
	printf("Enter line number: ");
	fflush(stdout);

        int read_check = read(STDIN_FILENO, console_input, CONSOLE_INPUT_SIZE);
	if (read_check == ERROR_READ) {
		perror("Can't get line number");
		return ERROR_GET_LINE_NUMBER;
	}				        
	int bytes_read = read_check;

        char* endptr = console_input;
	*line_num = strtoll(console_input, &endptr, DECIMAL_SYSTEM);	
	if (console_input <= endptr && endptr < console_input + bytes_read - 1) {
		fprintf(stderr, "Number contains invalid symbols\n");
		return INVALID_LINE_NUMBER_INPUT;
	}	

	return SUCCESS_GET_LINE_NUMBER;
}

int read_line(int fildes, size_t offset, size_t length, char *buf) {
	off_t lseek_check = lseek(fildes, offset, SEEK_SET);
	if (lseek_check == ERROR_LSEEK) {
		perror("Can't set position in file");
		return ERROR_READ;
	}
	int read_check = read(fildes, buf, length);
	if (read_check == ERROR_READ) {
		perror("Can't read from file");
		return ERROR_READ;
	}
	buf[length] = '\0';
	return SUCCESS_READ;
}

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Usage: %s <filename>\n", argv[0]);
		return 0;
	}

	int fildes = open_file(argv[1]);
	if (fildes == ERROR_OPEN_FILE) {
		return 0;
	}	

	size_t table_length = 0;
	line_info *table = create_table(fildes, &table_length);
	if (table == NULL) {
		close_file(fildes);
		return 0;
	}

	while(TRUE) {	
		long long line_num;
	        int get_line_num_check = get_line_number(&line_num);
		if (get_line_num_check == ERROR_GET_LINE_NUMBER) {
			break;   
		}
		if (get_line_num_check == INVALID_LINE_NUMBER_INPUT) {
			continue;
		}
		if (line_num < 0 || line_num > table_length) {
			fprintf(stderr, "Invalid line number. It has to be in range [0, %lu]\n", table_length);
			continue;
		}
		if (line_num == STOP_INPUT) {
			break;
		}

		size_t offset = table[line_num - 1].offset;
		size_t length = table[line_num - 1].length;
		
		char buf[length + 1];
		int read_check = read_line(fildes, offset, length, buf);
		if (read_check == ERROR_READ) {
			break;
		}
		printf("%s\n", buf);
	}

	free(table);
	close_file(fildes); 
	return 0;
}
