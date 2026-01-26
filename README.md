# PMan – Process Manager (CSC 360)

PMan is a simplified Unix-style process manager written in C for CSC 360 – Operating Systems at the University of Victoria. The goal of this assignment is to become familiar with C systems programming, Unix process management, signal handling, and the Linux /proc pseudo-filesystem.

PMan allows users to start programs in the background and continue interacting with the shell while those programs execute. It also provides commands to list, stop, resume, terminate, and inspect background processes. This project was developed and tested according to the CSC 360 assignment specifications.

The following commands are supported by PMan:
- `bg <program> [args...]`: execute a program in the background
- `bglist`: list all currently running background processes
- `bgkill <pid>`: terminate a background process using SIGTERM
- `bgstop <pid>`: stop a background process using SIGSTOP
- `bgstart <pid>`: resume a stopped background process using SIGCONT
- `pstat <pid>`: display process statistics using information from /proc
- `q`: quit the program

The pstat command reports the executable name, process state, user mode CPU time, kernel mode CPU time, resident set size, and voluntary and involuntary context switches. If the specified process does not exist, an appropriate error message is displayed.

This repository also includes helper programs provided with the assignment. The program `inf.c` repeatedly prints a tag at a fixed time interval and is useful for observing background process behavior. The program `args.c` prints all command-line arguments it receives and is useful for verifying correct argument passing to background processes.

A Makefile is provided to compile the project. To build the program, run:
make
This will produce the executable `pman`.

To run the program:
./pman

The program will then display the prompt:
Pman: >

Example usage:
bg ./inf A 2
bglist
bgstop 12345
bgstart 12345
pstat 12345
bgkill 12345


This program is intended to be compiled and run on `linux.csc.uvic.ca`. Behavior may differ on macOS or other Unix-like systems due to differences in process handling, signal semantics, and the /proc filesystem. Development on non-Linux systems should be used only for preliminary testing.

Because this project uses fork(), it is recommended to limit the maximum number of user processes before testing to avoid accidental fork bombs:
ulimit -u 35

This project was completed individually for CSC 360 and is intended for educational purposes only.