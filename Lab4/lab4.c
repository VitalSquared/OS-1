#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define END_OF_INPUT '.'
#define LIST_ADD_ERROR -1
#define LIST_ADD_SUCCESS 0

extern int errno;

typedef struct Node {
    char *value;
    struct Node *next;
} Node;

typedef struct List {
    Node *head;
    Node *last;
} List;

List *list_create() {
    List *lst = (List *) malloc(sizeof(List));
    if (lst != NULL) {
    	lst->head = NULL;
    	lst->last = NULL;
    }
    return lst;
}

int list_add(List *lst, const char *value) {
    if (lst == NULL || value == NULL) {
	    errno = EINVAL;
	    return LIST_ADD_ERROR;
    }

    Node *new_elem = (Node *) malloc(sizeof(Node));
    if (new_elem == NULL) return LIST_ADD_ERROR; //malloc will set errno=ENOMEM on error

    new_elem->next = NULL;
    new_elem->value = (char *) malloc(strlen(value) + 1);
    if (new_elem->value == NULL) return LIST_ADD_ERROR; //malloc will set errno=ENOMEM on error

    strcpy(new_elem->value, value);

    if (lst->head != NULL || lst->last != NULL) {
        lst->last->next = new_elem;
    } 
    else if (lst->head == NULL || lst->last == NULL) {
	lst->head = new_elem;
    }
    else {
	errno = EINVAL;
	return LIST_ADD_ERROR;
    }
    lst->last = new_elem;

    return LIST_ADD_SUCCESS;
}

void list_destroy(List *lst) {
    if (lst == NULL) return;

    if (lst->head != NULL) {
        Node *cur = lst->head, *next;
        while (cur != NULL) {
            next = cur->next;
            free(cur->value);
            free(cur);
            cur = next;
        }
    }

    free(lst);
}

void list_print(List *lst) {
    if (lst == NULL) printf("Null list!\n");
    if (lst->head != NULL) {
        Node *cur = lst->head;
        while (cur != NULL) {
            printf("%s", cur->value);
            cur = cur->next;
        }
    } 
    else printf("Empty list!\n");
}

int main() {
    int max_strlen = BUFSIZ;
    char buf[max_strlen];

    List *lst = list_create();
    if (lst == NULL) {
	perror("Can't create list");
	exit(0);
    }

    do {
	char *fgets_res = fgets(buf, max_strlen, stdin);
        if (fgets_res == NULL) {
	    if (errno != 0) perror("Can't read next line");
	    break;   
	}
	if (buf[0] == END_OF_INPUT) break;
    
    	int add_res = list_add(lst, buf);
	if (add_res == LIST_ADD_ERROR) {
		perror("Can't add element");
		break;
	}
    } while(1);

    list_print(lst);
    list_destroy(lst);

    exit(0);
}

