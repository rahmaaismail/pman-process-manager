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

 
/*
 * add_newNode
 * ------------
 * Adds a new background process node to the end of the linked list.
 * 
 * Parameters:
 *   head     - the head of the current linked list
 *   new_pid  - the PID of the newly created background process
 *   new_path - the command/path string used to start the process
 * 
 * Returns:
 *   The head of the linked list (unchanged if not empty, otherwise the new node)
 */
 
Node * add_newNode(Node* head, pid_t new_pid, char * new_path){
	    Node *new_node = malloc(sizeof(Node)); // allocate memory for new node
    if (new_node == NULL) {
        perror("malloc"); // handle memory allocation failure
        return head;
    }

    new_node->pid = new_pid; // store the PID 
    new_node->path = strdup(new_path); // duplicate the command string
    new_node->next = NULL; // new node is currently the last node

    if (head == NULL) {
        return new_node; // if the list is empty, the new node becomes the head
    }

    // traverse to the end of the list
    Node *curr = head;
    while (curr->next != NULL) {
        curr = curr->next;
    }
    curr->next = new_node; // append new node at the end
    return head;
}

/*
 * deleteNode
 * ----------
 * Deletes a node with the given PID from the linked list.
 * Frees the memory allocated for both the node and its path string.
 * 
 * Parameters:
 *   head - the head of the current linked list
 *   pid  - the PID of the process to delete
 * 
 * Returns:
 *   The (possibly updated) head of the linked list
 */

Node * deleteNode(Node* head, pid_t pid){
	    Node *curr = head;
    Node *prev = NULL;

    while (curr != NULL) {
        if (curr->pid == pid) { // found the node to delete 
            if (prev == NULL) {
                head = curr->next; // delete the head node
            } else {
                prev->next = curr->next; // bypass the node
            }
            free(curr->path); // free duplicated path string
            free(curr); // free node memory
            return head;
        }
        prev = curr;
        curr = curr->next;
    }
    return head; // PID not found, list unchanged
}

/*
 * printList
 * ---------
 * Prints all nodes in the linked list with their PID and command/path.
 * Also prints the total number of background jobs in the list.
 * 
 * Parameters:
 *   node - the head of the linked list
 */

void printList(Node *node){
    int count = 0;
    while (node != NULL) {
        printf("%d: %s\n", node->pid, node->path);
        node = node->next;
        count++;
    }
    printf("Total background jobs: %d\n", count);
}

/*
 * PifExist
 * --------
 * Checks if a process with a given PID exists in the linked list.
 * 
 * Parameters:
 *   node - the head of the linked list
 *   pid  - the PID to check
 * 
 * Returns:
 *   1 if the PID exists in the list, 0 otherwise
 */

int PifExist(Node *node, pid_t pid){
    while (node != NULL) {
        if (node->pid == pid) {
            return 1; // PID found
        }
        node = node->next;
    }
    return 0; // PID not found
}
