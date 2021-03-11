#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "file_reader.h"

#define END_OF_INPUT '.'
#define TRUE 1

int main() {
	size_t size = 0;
	char* buf = NULL;
	List *lst = list_create();
	
	if (lst == NULL) {
		perror("Can't create list");
		exit(0);
	}
	
	do {
		ssize_t read_res = read_line(&buf, &size, stdin);
		if (read_res == READ_ERROR) {
			perror("Can't read next line");
			break;
		}
		if (read_res == 0 || buf[0] == END_OF_INPUT) {
			break;
		}

		int list_res = list_add(lst, buf);
		if (list_res == LIST_ERROR) {
			perror("Can't add element");
			break;
		}
	} while(TRUE);
	
	if (buf != NULL) free(buf);
	
	list_print(lst);
	list_destroy(lst);
	
	exit(0);
}

