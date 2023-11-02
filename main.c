/*
Visible Cases
./allocate -f cases/task1/simple.txt -s SJF -m infinite -q 1 | diff - cases/task1/simple-sjf.out
./allocate -f cases/task1/more-processes.txt -s SJF -m infinite -q 3 | diff - cases/task1/more-processes.out

./allocate -f cases/task2/simple.txt -s RR -m infinite -q 3 | diff - cases/task2/simple-rr.out
./allocate -f cases/task2/two-processes.txt -s RR -m infinite -q 1 | diff - cases/task2/two-processes-1.out
./allocate -f cases/task2/two-processes.txt -s RR -m infinite -q 3 | diff - cases/task2/two-processes-3.out

./allocate -f cases/task3/simple.txt -s SJF -m best-fit -q 3 | diff - cases/task3/simple-bestfit.out
./allocate -f cases/task3/non-fit.txt -s SJF -m best-fit -q 3 | diff - cases/task3/non-fit-sjf.out
./allocate -f cases/task3/non-fit.txt -s RR -m best-fit -q 3 | diff - cases/task3/non-fit-rr.out

./allocate -f cases/task4/spec.txt -s SJF -m infinite -q 3 | diff - cases/task4/spec.out
./allocate -f cases/task1/more-processes.txt -s SJF -m infinite -q 3 | diff - cases/task1/more-processes.out
./allocate -f cases/task2/simple.txt -s RR -m infinite -q 3 | diff - cases/task2/simple-rr.out

./allocate -f cases/task1/simple.txt -s SJF -m infinite -q 1 | diff - cases/task1/simple-sjf.out
./allocate -f cases/task2/two-processes.txt -s RR -m infinite -q 3 | diff - cases/task2/two-processes-3.out

    */


/*
simple.txt
0 P1 50 8
10 P2 30 8
40 P5 80 8
50 P4 10 8

    */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "schedule.c"


int main(int argc, char *argv[]) {

// Handle input args
    int quantum = 0;
    char* file_name = NULL;
    char* algorithm = NULL;
    char* memory_algo = NULL;
    int c;
    while ((c = getopt(argc, argv, "f:s:m:q:")) != -1) {
        switch (c) {
            case 'f':
                file_name = optarg;
                break;
            case 's':
                algorithm = optarg;
                break;
            case 'm':
                memory_algo = optarg;
                break;
            case 'q':
                quantum = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Invalid argument\n");
                exit(EXIT_FAILURE);
        }
    }
    // error handle
    if (!file_name || !algorithm || !memory_algo) {
        fprintf(stderr, "Missing argument\n");
        exit(EXIT_FAILURE);
    }
    // Create an array to hold the memory blocks
    
    int memory[TOTAL_MB];
    for (int i = 0; i < TOTAL_MB ; i++) {
        memory[i] = -1;
    }
    Process *process = schedule(file_name, algorithm, quantum, memory, memory_algo);
    free(process);
    return 0;

    
}
