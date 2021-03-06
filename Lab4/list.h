#ifndef LAB4_LIST_H
#define LAB4_LIST_H

#define LIST_ERROR -1
#define LIST_SUCCESS 0

typedef struct Node {
    char *value;
    struct Node *next;
} Node;

typedef struct List {
    Node *head;
    Node *last;
} List;

List *list_create();
int list_add(List *lst, const char *value);
void list_destroy(List *lst);
void list_print(List *lst);

#endif

