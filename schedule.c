


#include <stdlib.h>
#include <stdio.h>
#include <limits.h> 
#include <string.h>
#include <math.h>

const int TOTAL_MB = 2048;
const int ROBIN_QUEUE_SIZE = 10000;
typedef struct {
    int timeArrived;
    int name;
    int serviceTime;
    int serviceTime_cp;
    int memoryRequire;
    int memoryAddress;
} Process;

Process* schedule(char* file_name, char* algorithm, int quantum, int* memory, char* mem_algo);
void shortestJobFirst(Process* processes, int n, int quantum, int* memory, char* mem_algo);
int compare(const void* a, const void* b);
void slf_sort(Process** queue, int num_processes);
void roundRobin(Process* processes, int n, int quantum, int* memory, char* mem_algo);
int addToMemory(int* memory, Process* process, int current_time);
void delMemory(int* memory, Process* process, int current_time) ;
void calc_turnaround(int current_time, int start_times, int num_process);
void calc_overhead(float* timeoverhead, int num_process);
Process* schedule(char* file_name, char* algorithm, int quantum, int* memory, char* mem_algo) {
    // Open file and handle error
    FILE* fp = fopen(file_name, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error opening file\n");
        exit(EXIT_FAILURE);
    }

    // count for number of process
    int ch, num_process = 0;
    while(!feof(fp))
    {
        ch = fgetc(fp);
        if(ch == '\n')
        {
            num_process++;
        }
    }
    //allocated memory
    Process* process = malloc(sizeof(Process) * num_process);
    if (process == NULL){
        fprintf(stderr, "Malloc failure\n");
        exit(EXIT_FAILURE);
    }

    rewind(fp);
    // Read the contents of the file into the process array
    int i = 0;
    while (fscanf(fp, "%d P%d %d %d", &process[i].timeArrived, &process[i].name, &process[i].serviceTime, &process[i].memoryRequire) != EOF) {
        process[i].memoryAddress = -1;
        process[i].serviceTime_cp = process[i].serviceTime;
        i++;
    }
    

    // Close the file
    fclose(fp);

    
    // ready to run
    if (strcmp(algorithm,"SJF") == 0){
        shortestJobFirst(process, num_process, quantum, memory, mem_algo);
    }
    else{
        roundRobin(process, num_process, quantum, memory, mem_algo);
    }
    
    
    return process;
}







// Function to compare two processes by their service time
int compare(const void* a, const void* b) {
    Process* p1 = (Process*)a;
    Process* p2 = (Process*)b;
    return p1->serviceTime - p2->serviceTime;
}


void shortestJobFirst(Process* processes, int n, int quantum, int* memory, char* mem_algo) {
// Create a queue to hold the processes
    Process* queue[n];
    int front = 0;
    int rear = -1;
    int num_finished = 0;
    int current_time = 0;
    double start_times = 0;
    double end_times = 0;
    int runned = 0;
    float timeoverhead[n];
    
    // Loop through the processes
    while (num_finished < n) {
        // Add any processes that have arrived to the queue
        for (int i = 0; i < n; i++) {
            if (processes[i].timeArrived <= current_time) {
                int already_in_queue = 0;
                for (int j = 0; j <= rear; j++) { 
                    if (queue[j]->name == processes[i].name){
                        already_in_queue = 1;
                        break;
                    }
                }
                if (!already_in_queue ) {
                    start_times += processes[i].timeArrived;
                    if (strcmp(mem_algo,"best-fit") == 0){
                        if (addToMemory(memory, &processes[i], current_time)){
                            rear++;
                            queue[rear] = &processes[i];
                            }
                        }
                    else{
                        rear++;
                        queue[rear] = &processes[i];
                    }
                    //printf("%d,ADDED,process_name=P%d\n", current_time, processes[i].name);
                }
            }
        }
        
        // If the queue is empty, increment the current time
        if (rear < front) {
            current_time+= quantum;
            continue;
        }

        if (runned == 0)
        {
            slf_sort(&queue[front], rear - front + 1);
            
        }
        
        Process* current_process = queue[front];
        
        // Print the current status of the process
        if (runned == 0){
            
            printf("%d,RUNNING,process_name=P%d,remaining_time=%d\n", current_time, current_process->name, current_process->serviceTime);
            runned = 1;

        }
        // Update the remaining time for the current process
        current_process->serviceTime -= quantum;
        
        // If the process has finished, remove it from the queue and print the finished status
        if (current_process->serviceTime <= 0) {
            front++;
            num_finished++;
            
            current_time +=quantum;
            printf("%d,FINISHED,process_name=P%d,proc_remaining=%d\n", current_time, current_process->name, rear - front + 1);
            if (strcmp(mem_algo,"best-fit") == 0){
                delMemory(memory, current_process, current_time);
                }
            runned = 0;
            end_times += current_time;
            float turnaround = current_time - current_process->timeArrived;
            timeoverhead[num_finished - 1] = (turnaround / current_process->serviceTime_cp);

            continue;
            
        }

        // Increment the current time
        current_time+=quantum;
    }
    calc_turnaround(end_times, start_times, n);
    calc_overhead(timeoverhead, n);
    printf("Makespan %d\n", current_time);
}
void slf_sort(Process** queue, int num_processes) {
    // Sort the queue based on service time, arrival time, and name
    for (int i = 0; i < num_processes - 1; i++) {
        for (int j = i + 1; j < num_processes; j++) {
            if (queue[i]->serviceTime > queue[j]->serviceTime) {
                Process* temp = queue[i];
                queue[i] = queue[j];
                queue[j] = temp;
            } else if (queue[i]->serviceTime == queue[j]->serviceTime) {
                if (queue[i]->timeArrived > queue[j]->timeArrived) {
                    Process* temp = queue[i];
                    queue[i] = queue[j];
                    queue[j] = temp;
                } else if (queue[i]->timeArrived == queue[j]->timeArrived) {
                    if (queue[i]->name > queue[j]->name) {
                        Process* temp = queue[i];
                        queue[i] = queue[j];
                        queue[j] = temp;
                    }
                }
            }
        }
    }
}

void roundRobin(Process* processes, int n, int quantum, int* memory, char* mem_algo) {
    // Create a queue to hold the processes
    Process* queue[ROBIN_QUEUE_SIZE];
    Process* lastP;
    int front = 0;
    int rear = -1;
    int num_finished = 0;
    int current_time = 0;
    int start_times = 0;
    int end_times = 0;
    int runned = 0;
    float timeoverhead[n];

    // Loop through the processes
    while (num_finished < n) {
        // Add any processes that have arrived to the queue
        for (int i = 0; i < n; i++) {
            if (processes[i].timeArrived <= current_time) {
                int already_in_queue = 0;
                for (int j = 0; j <= rear; j++) {
                    if (queue[j]->name == processes[i].name) {
                        already_in_queue = 1;
                        break;
                    }
                }
                if (!already_in_queue ) {
                    start_times += processes[i].timeArrived;
                    if (strcmp(mem_algo,"best-fit") == 0){
                        if (addToMemory(memory, &processes[i], current_time)){
                            rear++;
                            queue[rear] = &processes[i];
                            }
                        }
                    else{
                        rear++;
                        queue[rear] = &processes[i];
                    }
                }
            }
        }

        // If the queue is empty, increment the current time
        if (rear < front) {
            current_time += quantum;
            continue;
        }

        Process* current_process = queue[front];

        if (runned == 0 || current_process != lastP){
            
            printf("%d,RUNNING,process_name=P%d,remaining_time=%d\n", current_time, current_process->name, current_process->serviceTime);
            runned = 1;
        }
        // Update the remaining time for the current process
        current_process->serviceTime -= quantum;

        // If the process has finished, remove it from the queue and print the finished status
        if (current_process->serviceTime <= 0) {
            int proc_remain_extra = 0;
            front++;
            num_finished++;
            current_time += quantum;
            if (strcmp(mem_algo,"best-fit") == 0){
                delMemory(memory, current_process, current_time);
                }
            for (int i = 0; i < n; i++) {
                if (processes[i].timeArrived <= current_time) {
                    int already_in_queue = 0;
                    for (int j = 0; j <= rear; j++) {
                        if (queue[j]->name == processes[i].name) {
                            already_in_queue = 1;
                            break;
                        }
                    }
                    if (!already_in_queue ) {
                        proc_remain_extra++;
                    }
                }
            }
            printf("%d,FINISHED,process_name=P%d,proc_remaining=%d\n", current_time, current_process->name, rear - front + 1 + proc_remain_extra);
            runned = 0;
            end_times += current_time;
            float turnaround = current_time - current_process->timeArrived;
            timeoverhead[num_finished - 1] = (turnaround / current_process->serviceTime_cp);

        } else {
            current_time += quantum;
            // Add any processes that have arrived to the queue
            for (int i = 0; i < n; i++) {
                if (processes[i].timeArrived <= current_time) {
                    int already_in_queue = 0;
                    for (int j = 0; j <= rear; j++) {
                        if (queue[j]->name == processes[i].name) {
                            already_in_queue = 1;
                            break;
                        }
                    }
                    if (!already_in_queue ) {
                        start_times += processes[i].timeArrived;
                        if (strcmp(mem_algo,"best-fit") == 0){
                            if (addToMemory(memory, &processes[i], current_time)){
                                rear++;
                                queue[rear] = &processes[i];
                                }
                            }
                        else{
                            rear++;
                            queue[rear] = &processes[i];
                        }
                    }
                }
            }
            // If the process is not finished, move it to the back of the queue

            front++;
            rear++;
            queue[rear] = current_process;
            lastP = current_process;
        }
    }
    calc_turnaround(end_times, start_times, n);
    calc_overhead(timeoverhead, n);
    printf("Makespan %d\n", current_time);

}
// Function to add a process to memory
int addToMemory(int* memory, Process* process, int current_time) {
    int process_size = process->memoryRequire;
    int best_fit_index = -1;
    int best_fit_size = TOTAL_MB;
    // Find the best fit block of memory for the process
    for (int i = 0; i < TOTAL_MB; i++) {
        if (memory[i] == -1) {
            int j = i;
            int block_size = 0;
            while (memory[j] == -1 && block_size < process_size) {
                block_size++;
                j++;
            }
            if (block_size == process_size && block_size < best_fit_size) {
                best_fit_index = i;
                best_fit_size = block_size;
            }
        }
    }
    // If a best fit block was found, allocate the memory to the process
    if (best_fit_index != -1) {
        for (int i = best_fit_index; i < best_fit_index + process_size; i++) {
            memory[i] = process->name;
        }
        process->memoryAddress = best_fit_index;
        printf("%d,READY,process_name=P%d,assigned_at=%d\n", current_time, process->name, best_fit_index);
        return 1;
    }
    else{
        return 0;
    }
}

// Function to delete a process from memory
void delMemory(int* memory, Process* process, int current_time) {
    int process_size = process->memoryRequire;
    int start_index = process->memoryAddress;
    // Free the memory occupied by the process
    for (int i = start_index; i < start_index + process_size; i++) {
        memory[i] = -1;
    }
}

void calc_turnaround(int current_time, int start_times, int num_process){

    printf("Turnaround time %d\n", (int)(ceil((float)(current_time - start_times) / num_process)));
}
void calc_overhead(float* arr, int size){
    float max = arr[0];
    float sum = 0;

    for (int i = 0; i < size; i++) {

        if (arr[i] > max) {
            max = arr[i];
        }
        sum += arr[i];
    }
    float average = (float)sum / size;
    printf("Time overhead %.2f %.2f\n", max, average);

}