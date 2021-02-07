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
    if (!lst) return;

    Node *new_elem = (Node *) malloc(sizeof(Node));
    new_elem->next = NULL;
    new_elem->value = (char *) malloc(strlen(value) + 1);
    strcpy(new_elem->value, value);

    if (*lst) {
        Node *cur = *lst;
        while (cur->next) cur = cur->next;
        cur->next = new_elem;
    } else *lst = new_elem;
}

void list_destroy(List *lst) {
    if (!lst) return;
    if (*lst) {
        Node *cur = *lst, *next;
        while (cur) {
            next = cur->next;
            free(cur->value);
            free(cur);
            cur = next;
        }
    }
    free(lst);
}

void list_print(List *lst) {
    if (!lst) printf("Null list!\n");
    if (*lst) {
        Node *cur = *lst;
        while (cur) {
            printf("%s", cur->value);
            cur = cur->next;
        }
    } else printf("Empty list!\n");
}

int main() {
    char buf[BUFSIZ];
    List *lst = list_create();

    while (fgets(buf, BUFSIZ, stdin)) {
        if (buf[0] == '.') break;
        list_add(lst, buf);
    }

    list_print(lst);
    list_destroy(lst);

    return 0;
}
