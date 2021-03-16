#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "list.h"

extern int errno;

List *list_create() {
    List *lst = (List *) malloc(sizeof(List));
    if (lst != NULL) {
        lst->head = NULL;
        lst->last = NULL;
    }
    else {
	perror("Can't create list");
    }
    return lst;
}

int list_add(List *lst, const char *value) {
    if (lst == NULL || value == NULL) {
        errno = EINVAL;
	perror("Can't add element to list");
        return LIST_ERROR;
    }

    Node *new_elem = (Node *) malloc(sizeof(Node));
    if (new_elem == NULL) {
	perror("Can't allocate memory for element of list");
        return LIST_ERROR;
    }
    new_elem->value = (char *) malloc(strlen(value) + 1);
    if (new_elem->value == NULL) {
	perror("Can't allocate memory for value of list");
        free(new_elem);
        return LIST_ERROR;
    }

    new_elem->next = NULL;
    strcpy(new_elem->value, value);

    if (lst->head != NULL && lst->last != NULL) {
        lst->last->next = new_elem;
    }
    else if (lst->head == NULL && lst->last == NULL) {
        lst->head = new_elem;
    }
    else {
        errno = EINVAL;
	perror("Can't add element to list");
	free(new_elem->value);
	free(new_elem);
        return LIST_ERROR;
    }
    lst->last = new_elem;

    return LIST_SUCCESS;
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

