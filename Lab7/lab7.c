#include <sys/types.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/mman.h>
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
#define ERROR_STAT -1
#define ERROR_MUNMAP -1
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

line_info *create_table(void *file_addr, off_t file_size, long long *table_length) {
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

        off_t offset = 0, cur_offset = 0;

	do {
		c = *((char*) (file_addr + cur_offset));
		if (cur_offset == file_size || c == '\n') {
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
			cur_offset++;
			offset = cur_offset;
			continue;
		} 
		cur_offset++;
		line_length++;
	} while (cur_offset != file_size + 1);

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

int main(int argc, char** argv) {
	if (argc < 2) {
		printf("Usage: %s <filename>\n", argv[0]);
		return 0;
	}

	struct stat st;
	int stat_check = stat(argv[1], &st);
	if (stat_check == ERROR_STAT) {
		perror("Can't get file stat");
		return 0;
	}
	off_t file_size = st.st_size;
	
	int fildes = open(argv[1], O_RDONLY);
	if (fildes == ERROR_OPEN_FILE) {
		perror("Unable to open file");
		return 0;
	}		

	void *file_addr = mmap(0, file_size, PROT_READ, MAP_SHARED, fildes, 0);    
	if (file_addr == MAP_FAILED) {
		perror("Can't map file");
		return 0;
	}

	long long table_length = 0;
	line_info *table = create_table(file_addr, file_size, &table_length);
	
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
				int write_check = write_to_file(STDOUT_FILENO, file_addr, file_size);
				if (write_check == ERROR_WRITE) {
					perror("Can't write to STDOUT");
				}
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
			
			int write_check = write_to_file(STDOUT_FILENO, file_addr + offset, length);
			if (write_check == ERROR_WRITE) {
				break;
			}
			write_check = write_to_file(STDOUT_FILENO, "\n", 1);
			if (write_check == ERROR_WRITE) {
				break;
			}
		}
	}

	free(table);

	int munmap_check = munmap(file_addr, file_size);
    	if (munmap_check == ERROR_MUNMAP) {
        	perror("Can't clean memory");
    	}

	int close_check = close(fildes); 
	if (close_check == ERROR_CLOSE_FILE) {
		perror("Unable to close file");
	}
	return 0;
}
