# Process Management

## 1. Overview

In this project, you will implement a process manager capable of allocating memory to processes and scheduling them for execution. This project has two phases:

1. In the first phase, a scheduling algorithm will allocate the CPU to processes, making the assumption that memory requirements are always satisfied.
2. In the second phase, a memory allocation algorithm will be used to allocate memory to processes, before the scheduling takes place.

Both the memory allocation and the process scheduling are simulated. There is also a challenge task that requires controlling the execution of real processes during the process scheduling. Process scheduling decisions will be based on a scheduling algorithm with the assumption of a single CPU (i.e, only one process can be running at a time).

## 2. The Process Manager

The process manager runs in cycles. A cycle occurs after one quantum has elapsed. The process manager has its own notion of time, referred to from here on as the simulation time. 

### 2.1. Process Lifecycle

The lifecycle of a process is as follows:

1. The process is submitted to the manager via an input file.
2. The process is in a READY state after it has arrived, and memory has been successfully allocated to it.
3. Once a process is in the READY state, it can now be considered by a process scheduling algorithm as a candidate to enter the RUNNING state.
4. Once a process is in the RUNNING state, it runs on the CPU for at least one quantum.
5. The total time a process gets to run on the CPU must be equal to or greater than its service time.
6. After the service time of a process has elapsed, the process moves to the FINISHED state.

### 2.2. Program termination

The process manager exits when all the processes in the input file have finished execution.

## 3. Programming Tasks

### 3.1. Task 1: Process Scheduling (Non-preemptive)

In this task, your program selects the next process to run among the READY processes based on the Shortest Job First (SJF) scheduling algorithm.

### 3.2. Task 2: Process Scheduling (Preemptive)

In this task, your program selects the next process to run among the READY processes based on the Round Robin (RR) scheduling algorithm.

### 3.3. Task 3: Memory Allocation

In this task, you have to implement an additional memory allocation phase before your program starts the scheduling phase of the simulation.

### 3.4. Task 4 (Challenge Task): Creating and Controlling Real Processes

If you choose to complete this task, every process scheduled by the manager should trigger the execution of a `process.c` program, supplied as part of this project.

Include the following line in your code for this task to be marked:
\`\`\`
#define IMPLEMENTS_REAL_PROCESS
\`\`\`
