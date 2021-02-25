#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct Node {
    char *value;
    struct Node *next;
} Node;

typedef Node * List;

List *list_create() {
    List *lst = (List *) malloc(sizeof(List));
    *lst = NULL;
    return lst;
}

void list_add(List *lst, const char *value) {
    if (lst == NULL) return;

    Node *new_elem = (Node *) malloc(sizeof(Node));
    new_elem->next = NULL;
    new_elem->value = (char *) malloc(strlen(value) + 1);
    strcpy(new_elem->value, value);

    if (*lst != NULL) {
        Node *cur = *lst;
        while (cur->next != NULL) {
		cur = cur->next;
	}
        cur->next = new_elem;
    } 
    else *lst = new_elem;
}

void list_destroy(List *lst) {
    if (lst == NULL) return;
    if (*lst != NULL) {
        Node *cur = *lst, *next;
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
    if (*lst != NULL) {
        Node *cur = *lst;
        while (cur != NULL) {
            printf("%s", cur->value);
            cur = cur->next;
        }
    } 
    else printf("Empty list!\n");
}

int main() {
    char buf[BUFSIZ];
    List *lst = list_create();

    char *fgets_res = fgets(buf, BUFSIZ, stdin);
    while (fgets_res != NULL) {
        if (buf[0] == '.') break;
        list_add(lst, buf);
	fgets_res = fgets(buf, BUFSIZ, stdin);
    }

    list_print(lst);
    list_destroy(lst);

    return 0;
}
