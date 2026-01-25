#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include "linked_list.h"

 
Node * add_newNode(Node* head, pid_t new_pid, char * new_path){
	    Node *new_node = malloc(sizeof(Node));
    if (new_node == NULL) {
        perror("malloc");
        return head;
    }

    new_node->pid = new_pid;
    new_node->path = strdup(new_path);
    new_node->next = NULL;

    if (head == NULL) {
        return new_node;
    }

    Node *curr = head;
    while (curr->next != NULL) {
        curr = curr->next;
    }
    curr->next = new_node;
    return head;
}

Node * deleteNode(Node* head, pid_t pid){
	    Node *curr = head;
    Node *prev = NULL;

    while (curr != NULL) {
        if (curr->pid == pid) {
            if (prev == NULL) {
                head = curr->next;
            } else {
                prev->next = curr->next;
            }
            free(curr->path);
            free(curr);
            return head;
        }
        prev = curr;
        curr = curr->next;
    }
    return head;
}

void printList(Node *node){
    int count = 0;
    while (node != NULL) {
        printf("%d: %s\n", node->pid, node->path);
        node = node->next;
        count++;
    }
    printf("Total background jobs: %d\n", count);
}


int PifExist(Node *node, pid_t pid){
    while (node != NULL) {
        if (node->pid == pid) {
            return 1;
        }
        node = node->next;
    }
    return 0;
}
