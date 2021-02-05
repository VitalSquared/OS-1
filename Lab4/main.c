#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define MAX_STRING_LEN 1024

typedef struct Node {
    char* value;
    struct Node* next;
} Node;

typedef Node * List;

List *list_create() {
    List *lst = (List *) malloc(sizeof(List));
    *lst = NULL;
    return lst;
}

void list_add(List *lst, char *string) {
    if (!lst) return;
    if (*lst) {
        Node *cur = *lst;
        while (cur->next) {
            cur = cur->next;
        }
        cur->next = (Node *) malloc(sizeof(Node));
        cur->next->next = NULL;
        cur->next->value = string;
    }
    else {
        Node *head = (Node *) malloc(sizeof(Node));
        head->next = NULL;
        head->value = string;
        *lst = head;
    }
}

void list_destroy(List *lst) {
    if (!lst) return;
    if (*lst) {
        Node *cur = *lst;
        while (cur) {
            Node *next = cur->next;
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
    char *buf = (char *) malloc(MAX_STRING_LEN + 1);
    if (!buf) {
        printf("Memory error!\n");
        return 0;
    }
    fgets(buf, MAX_STRING_LEN, stdin);
    List *lst = list_create();
    while (buf[0] != '.') {
        char *string = (char *) malloc(strlen(buf) + 1);
        strcpy(string, buf);
        list_add(lst, string);
        fgets(buf, MAX_STRING_LEN, stdin);
    }
    list_print(lst);
    list_destroy(lst);
    free(buf);
    return 0;
}
