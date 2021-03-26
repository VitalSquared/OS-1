#include <sys/types.h>
#include <sys/select.h>
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
#define ERROR_LSEEK -1
#define ERROR_PRINT_FILE -1
#define ERROR_SELECT -1
#define SUCCESS_CLOSE_FILE 0
#define SUCCESS_READ 0
#define SUCCESS_WRITE 0
#define SUCCESS_GET_LINE_NUMBER 1
#define SUCCESS_PRINT_FILE 0
#define SUCCESS_SELECT 1
#define GET_LINE_NUMBER_TIMEOUT 2
#define INVALID_LINE_NUMBER_INPUT 0
#define READ_EOF 0
#define TABLE_INIT_SIZE 100
#define INPUT_SIZE 128
#define BUFFER_SIZE 256
#define TRUE 1
#define FALSE 0
#define STOP_INPUT 0
#define DECIMAL_SYSTEM 10
#define MAX_DP 1
#define SELECT_NO_REACTION 0
#define SELECT_WRONG_DESC 2
#define TIMEOUT_SEC 5
#define TIMEOUT_USEC 0

typedef struct line_info {
	off_t offset;
	size_t length;
} line_info;

line_info *create_table(int fildes, long long *table_length) {
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
	size_t line_length = 0;

        off_t offset = lseek(fildes, 0L, SEEK_SET);
	if (offset == ERROR_LSEEK) {
		perror("Can't get/set position in file");
		return NULL;
	}

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
			offset = lseek(fildes, 0L, SEEK_CUR);
			if (offset == ERROR_LSEEK) {
				free(table);
				perror("Can't get/set position in file");
				return NULL;
			}
			continue;
		} 
		line_length++;
	} while (read_check != READ_EOF);

	return table;
}

int write_to_file(int fildes, const void *buf, size_t nbytes) {
	int write_check = write(fildes, buf, nbytes);
	if (write_check == ERROR_WRITE) {
		perror("Can't write to file");
		return ERROR_WRITE;
	}
	return SUCCESS_WRITE;
}

int wait_for_input() {
   	fd_set read_descriptors;
        struct timeval timeout;
	int result;

	FD_ZERO(&read_descriptors);
	FD_SET(STDIN_FILENO, &read_descriptors);
	
	timeout.tv_sec = TIMEOUT_SEC;
	timeout.tv_usec = TIMEOUT_USEC;

	result = select(MAX_DP, &read_descriptors, NULL, NULL, &timeout);

    	if (result == ERROR_SELECT) {
        	perror("Select error");
        	return ERROR_SELECT;
    	}

    	if (result == SELECT_NO_REACTION) {
		int write_check = write_to_file(STDOUT_FILENO, "Time is out!\n", 13);
		if (write_check == ERROR_WRITE) {
			return ERROR_SELECT;
		}
		return SELECT_NO_REACTION;
    	}

    	if (FD_ISSET(STDIN_FILENO, &read_descriptors) == FALSE) {
		int write_check = write_to_file(STDERR_FILENO, "Input was taken outside of STDOUT\n", 34);
		if (write_check == ERROR_WRITE) {
			return ERROR_SELECT;
		}
        	return SELECT_WRONG_DESC;
    	}
	return SUCCESS_SELECT;
}

int get_line_number(long long *line_num) {
	char input[INPUT_SIZE + 1]; 
	
	int write_check = write_to_file(STDOUT_FILENO, "Five seconds to enter line number: ", 35); 
	if (write_check == ERROR_WRITE) {
		return ERROR_GET_LINE_NUMBER;
	}

	int wait_check = wait_for_input();
	if (wait_check == ERROR_SELECT || wait_check == SELECT_WRONG_DESC) {
		return INVALID_LINE_NUMBER_INPUT;
	}
	if (wait_check == SELECT_NO_REACTION) {
		return GET_LINE_NUMBER_TIMEOUT;
	}

        ssize_t bytes_read = read(STDIN_FILENO, input, INPUT_SIZE);
	if (bytes_read == ERROR_READ) {
		perror("Can't get line number");
		return ERROR_GET_LINE_NUMBER;
	}	
	if (bytes_read == 0) {
		return INVALID_LINE_NUMBER_INPUT;
	}	
   	input[bytes_read] = '\0';
	if (input[bytes_read - 1] == '\n') {
		input[bytes_read - 1] = '\0';
		bytes_read--;
	}
	if (bytes_read == 0) {
		return INVALID_LINE_NUMBER_INPUT;
	}

	char *ptr_first_char = input;
        char *ptr_last_char = input + bytes_read - 1;
	char *endptr = input;

	*line_num = strtoll(input, &endptr, DECIMAL_SYSTEM);	
	if (ptr_first_char <= endptr && endptr <= ptr_last_char) {
		int write_check = write_to_file(STDOUT_FILENO, "Number contains invalid symbols\n", 32); 
		if (write_check == ERROR_WRITE) {
			return ERROR_GET_LINE_NUMBER;
		}
		return INVALID_LINE_NUMBER_INPUT;
	}	

	return SUCCESS_GET_LINE_NUMBER;
}

int read_line(int fildes, off_t offset, size_t length, char *buf) {
	off_t lseek_check = lseek(fildes, offset, SEEK_SET);
	if (lseek_check == ERROR_LSEEK) {
		perror("Can't get/set position in file");
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

int print_file(int fildes) {
	off_t lseek_check = lseek(fildes, 0L, SEEK_SET);
	if (lseek_check == ERROR_LSEEK) {
		perror("Can't get/set file position");
		return ERROR_PRINT_FILE;
	}
	char buf[BUFFER_SIZE + 1];
	while (TRUE) {
		int bytes_read = read(fildes, buf, BUFFER_SIZE);
		if (bytes_read == ERROR_READ) {
			perror("Can't read from file");
			return ERROR_PRINT_FILE;
		}
		if (bytes_read == READ_EOF) {
			break;
		}
		buf[bytes_read] = '\0';
		printf("%s", buf);
	}
	printf("\n");
	return SUCCESS_PRINT_FILE;
}

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Usage: %s <filename>\n", argv[0]);
		return 0;
	}

	int fildes = open(argv[1], O_RDONLY);
	if (fildes == ERROR_OPEN_FILE) {
		perror("Can't open file");
		return 0;
	}	

	long long table_length = 0;
	line_info *table = create_table(fildes, &table_length);
	if (table != NULL) {
		while(TRUE) {	
			long long line_num;
			int get_line_num_check = get_line_number(&line_num);
			if (get_line_num_check == ERROR_GET_LINE_NUMBER) {
				break;   
			}
			if (get_line_num_check == INVALID_LINE_NUMBER_INPUT) {
				continue;
			}
			if (get_line_num_check == GET_LINE_NUMBER_TIMEOUT) {
				printf("Printing out your file: \n");
				print_file(fildes);
				break;
			}
			if (line_num < 0 || line_num > table_length) {
				fprintf(stderr, "Invalid line number. It has to be in range [0, %lld]\n", table_length);
				continue;
			}
			if (line_num == STOP_INPUT) {
				break;
			}

			off_t offset = table[line_num - 1].offset;
			size_t length = table[line_num - 1].length;
			
			char buf[length + 1];
			int read_check = read_line(fildes, offset, length, buf);
			if (read_check == ERROR_READ) {
				break;
			}
			printf("%s\n", buf);
		}
	}

	free(table);
	int close_check = close(fildes); 
	if (close_check == ERROR_CLOSE_FILE) {
		perror("Can't close file");
	}
	return 0;
}
