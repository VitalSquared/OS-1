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
    List *lst = (List *) calloc(1, sizeof(List));
    return lst;
}

void list_add(List *lst, char *string) {
    if (!lst) return;
    //if there is at least one element
    if (*lst) {
        Node *cur = *lst;
        while (cur->next) {
            cur = cur->next;
        }
        cur->next = (Node *) calloc(1, sizeof(Node));
        cur->next->next = NULL;
        cur->next->value = string;
    }
        //if there is no elements (we add first one)
    else {
        Node *head = (Node *) calloc(1, sizeof(Node));
        head->next = NULL;
        head->value = string;
        *lst = head;
    }
}

void list_destroy(List *lst) {
    if (!lst) return;
    //if there is at least one element
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
    char *buf = (char *) calloc(MAX_STRING_LEN + 1, sizeof(char));
    if (!buf) {
        printf("Memory error!\n");
        return 0;
    }
    if (!fgets(buf, MAX_STRING_LEN, stdin)) {
        printf("Couldn't read string!\n");
    }
    List *lst = list_create();
    while (buf[0] != '.') {
        char *string = (char *) calloc(strlen(buf) + 1, sizeof(char));
        strcpy(string, buf);
        list_add(lst, string);
        if (!fgets(buf, MAX_STRING_LEN, stdin)) {
            printf("Couldn't read string!\n");
        }
    }
    list_print(lst);
    list_destroy(lst);
    return 0;
}
