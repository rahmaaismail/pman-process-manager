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

Node* head = NULL;


void func_BG(char **cmd){
    if(cmd[1] == NULL){
        printf("Usage: bg <program> [args...]\n");
        return;
    }

    pid_t pid = fork();

    // if fork failed
    if(pid < 0){
        perror("fork failed");
        return;
    }

    if(pid == 0){
        // Child process: execute the command
        if(execvp(cmd[1], &cmd[1]) < 0){
            perror("execvp failed");
            exit(1);
        }
    } else {
        // Parent process: record background job 
        printf("Started process %s with PID %d\n", cmd[1], pid);
        // Add to linked list
        head = add_newNode(head, pid, cmd[1]);
    }
}

/*
 * bglist
 * Prints all currently running background processes
 * along with their PIDs and total count.
 */

void func_BGlist(char **cmd){
	Node* current = head;
    int count = 0;

    while(current != NULL){
        printf("%d: %s\n", current->pid, current->path);
        current = current->next;
        count++;
    }
    printf("Total background jobs: %d\n", count);
}

/*
 * bgkill <pid>
 * Terminates a background process using SIGTERM
 * and removes it from the linked list.
 */

void func_BGkill(char * str_pid){
	    if(str_pid == NULL){
        printf("Usage: bgkill <pid>\n");
        return;
    }

    pid_t pid = atoi(str_pid);

    // Check if PID exists in background list 
    if(!PifExist(head, pid)){
        printf("Error: Process %d does not exist.\n", pid);
        return;
    }

    // Send termination signal
    if(kill(pid, SIGTERM) == -1){
        perror("bgkill failed");
        return;
    }

    // Wait for process to exit and clean up
    waitpid(pid, NULL, 0);

    head = deleteNode(head, pid);
    printf("Process %d terminated\n", pid);
}

/*
 * bgstop <pid>
 * Stops a background process using SIGSTOP.
 */

void func_BGstop(char * str_pid){
    if(str_pid == NULL){
        printf("Usage: bgstop <pid>\n");
        return;
    }

    pid_t pid = atoi(str_pid);

    if(!PifExist(head, pid)){
        printf("Error: Process %d does not exist.\n", pid);
        return;
    }

    if(kill(pid, SIGSTOP) == -1){
        perror("bgstop failed");
        return;
    }

    printf("Process %d stopped\n", pid);
}

/*
 * bgstart <pid>
 * Resumes a stopped background process using SIGCONT.
 */

void func_BGstart(char * str_pid){
	    if(str_pid == NULL){
        printf("Usage: bgstart <pid>\n");
        return;
    }

    pid_t pid = atoi(str_pid);

    if(!PifExist(head, pid)){
        printf("Error: Process %d does not exist.\n", pid);
        return;
    }

    if(kill(pid, SIGCONT) == -1){
        perror("bgstart failed");
        return;
    }

    printf("Process %d resumed\n", pid);
}

/*
 * pstat <pid>
 * Prints process statistics by reading from:
 *   /proc/<pid>/stat
 *   /proc/<pid>/status
 * Displays CPU usage, memory usage, state, and context switches.
 */

void func_pstat(char * str_pid){
    if(str_pid == NULL){
        printf("Usage: pstat <pid>\n");
        return;
    }

    pid_t pid = atoi(str_pid);

    char stat_path[64];
    snprintf(stat_path, sizeof(stat_path), "/proc/%d/stat", pid);

    FILE *fp = fopen(stat_path, "r");
    if(fp == NULL){
        printf("Error: Process %d does not exist.\n", pid);
        return;
    }

    char comm[256], state;
    unsigned long utime, stime;
    long rss;

      // Read required fields from /proc/<pid>/stat
    fscanf(fp,
        "%*d %s %c "
        "%*d %*d %*d %*d %*d "
        "%*u %*u %*u %*u %*u "
        "%lu %lu "
        "%*d %*d %*d %*d %*d %*d "
        "%*u %*u %ld",
        comm, &state, &utime, &stime, &rss);

    fclose(fp);

    long clk = sysconf(_SC_CLK_TCK);

    printf("comm: %s\n", comm);
    printf("state: %c\n", state);
    printf("utime: %.2f seconds\n", (double)utime / clk);
    printf("stime: %.2f seconds\n", (double)stime / clk);
    printf("rss: %ld\n", rss);

    // Read voluntary and nonvoluntary context switches
    char status_path[64];
    snprintf(status_path, sizeof(status_path), "/proc/%d/status", pid);

    fp = fopen(status_path, "r");
    if(fp == NULL) return;

    char line[256];
    while(fgets(line, sizeof(line), fp)){
        if(strncmp(line, "voluntary_ctxt_switches", 23) == 0 ||
           strncmp(line, "nonvoluntary_ctxt_switches", 26) == 0){
            printf("%s", line);
        }
    }

    fclose(fp);
}

 
int main(){
    char user_input_str[50];
    while (true) {
      int status;
        pid_t pid;
        // Reap any finished background processes (non-blocking)
        while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
            printf("Process %d terminated\n", pid);
            head = deleteNode(head, pid);
        }
      printf("Pman: > ");
      fgets(user_input_str, 50, stdin);
      printf("User input: %s \n", user_input_str);
      char * ptr = strtok(user_input_str, " \n");
      if(ptr == NULL){
        continue;
      }
      char * lst[50];
      int index = 0;
      lst[index] = ptr;
      index++;
      while(ptr != NULL){
        ptr = strtok(NULL, " \n");
        lst[index]=ptr;
        index++;
      }
      if (strcmp("bg",lst[0]) == 0){
        func_BG(lst);
      } else if (strcmp("bglist",lst[0]) == 0) {
        func_BGlist(lst);
      } else if (strcmp("bgkill",lst[0]) == 0) {
        func_BGkill(lst[1]);
      } else if (strcmp("bgstop",lst[0]) == 0) {
        func_BGstop(lst[1]);
      } else if (strcmp("bgstart",lst[0]) == 0) {
        func_BGstart(lst[1]);
      } else if (strcmp("pstat",lst[0]) == 0) {
        func_pstat(lst[1]);
      } else if (strcmp("q",lst[0]) == 0) {
        printf("Bye Bye \n");
        exit(0);
      } else {
        printf("Invalid input\n");
      }
    }

  return 0;
}

