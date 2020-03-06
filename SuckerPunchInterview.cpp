// SuckerPunchInterview.cpp : This file contains the 'main' function. Program execution begins and ends there.

/*
Requirements:
- Type Q can be anything
- Cannot call malloc() or heap management routines
- All storage must be in provided array `unsigned char data[2048];`
- 64 queues at max
- if we cannot satisfy a request due to lack of memory call provided function
- if an illegal request is made use provided function

Provided Functions:
 - void on_out_of_memory();
 - void on_illegal_operation();

Notes:
    - Worst case performance is more important than average case performance for deletion and insertion
    - Spikes in queue size or number may occur
    - Do not assume max number of bytes in a queue (except that imposed by memory)
    - 15 queues with 80~ bytes average
*/

#include <iostream>

/*
Data Types, Structs, Constants, and Data
*/

//Data Types
typedef unsigned int Bit;

//Byte queue type - Q
struct Q {
    Bit alloc       : 1;
    Bit head        : 7;
    Bit headOffset  : 4;
    Bit tail        : 7;
    Bit tailOffset  : 4;
    Bit budBlock    : 7; 

    Bit empty       : 2;
};

// Constants
const unsigned int MAX_QUEUES = 64;
const unsigned int TOTAL_MEM_SIZE = 2048;
const unsigned int Q_BLOCK_SIZE = 25; // Size in bytes
const unsigned int MAX_Q_BLOCKS = 68; // Max number of blocks
const unsigned int Q_QUEUE_SIZE = sizeof(Q);

// Resources - provided by problem description
unsigned char data[TOTAL_MEM_SIZE]; // Total available memory for storage from 0 - TOTAL_MEM_SIZE-1
/*
Memory layout

index       purpose
------------------------------------------
0           Freelist
1           Number of allocated queues
*/

/*
Helper functions
*/

// Handler to avoid shadowing member of std namespace
unsigned char * GetStorage(){
    return data;
}

// Run first to setup memory
void init_memory() {
    
    //Mark freelist as completely free
    memset(GetStorage(), '1', MAX_Q_BLOCKS);
    //Mark Q_Queue count to 0
    GetStorage()[MAX_Q_BLOCKS] = 0;
    //Init everything else to null
    memset(GetStorage() + MAX_Q_BLOCKS + 1, 0, TOTAL_MEM_SIZE - (MAX_Q_BLOCKS + 1));
}

// Creates a FIFO byte queue, returning a handle to it.
Q * create_queue() {
    Q* newQueue = NULL;

    // Allocate a queue and track how many are allocated
    if (GetStorage()[MAX_Q_BLOCKS] < MAX_QUEUES) {


        GetStorage()[MAX_Q_BLOCKS] += 1;
    }

    return newQueue;
}

// Destroy an earlier created byte queue.
void destroy_queue(Q * q) {

}

// Adds a new byte to a queue.
void enqueue_byte(Q * q, unsigned char b) {

}

// Pops the next byte off the FIFO queue
unsigned char dequeue_byte(Q * q) {

}

// Prints the current state of memory for debugging
void print_mem(unsigned char* memory) {
    // This is the threshold for memory block storage starts
    unsigned int q_block_mem_start = MAX_QUEUES * Q_QUEUE_SIZE + MAX_Q_BLOCKS + 1; // first index of storage - 325
    unsigned int end_of_storage = MAX_Q_BLOCKS * Q_BLOCK_SIZE + q_block_mem_start - 1; // last index of storage - 2024
    unsigned int items_per_row = 5;
    unsigned int row_count = 0;

    std::cout << "Memory Contents:\n";
    for (unsigned int i = 0; i < TOTAL_MEM_SIZE; i++) {

        if (i < MAX_Q_BLOCKS) { // Freelist scope
            if (!i) {
                printf("\nFreelist: Index - %d\n\n", i);
            }
            printf("[%c] ", memory[i]);
        }
        else if (i == MAX_Q_BLOCKS) { // Q Count
            printf("\n\nQ Count: Index - %d\n\n", i);
            printf("[%d]\n", memory[i]);
        }
        else if (i < q_block_mem_start) { // Q Queue location Scope
            if (i == MAX_Q_BLOCKS+1) {
                printf("\nQ Queue Location: Index - %d\n\n", i);
                items_per_row = 8;
                row_count = 0;
            }
            printf("[%c] ", memory[i]);
        }
        else if (i < end_of_storage + 1) { //Memory block scope
            if (i == q_block_mem_start) {
                printf("\nQ Block Memory: Index - %d\n\n", i);
                items_per_row = Q_BLOCK_SIZE;
                row_count = 0;
            }
            printf("[%c] ", memory[i]);
        }
        else { // Unused memory location
            if (i == end_of_storage + 1) {
                printf("\nUnused Memory: Index - %d\n\n", i);
                items_per_row = 10;
                row_count = 0;
            }
            printf("[%c] ", memory[i]);
        }
        row_count++;

        if (!(row_count % items_per_row)) {
            std::cout << std::endl;
            row_count = 0;
        }
    }

    std::cout << std::endl;
}

int main()
{
    using namespace std;

    cout << "Hello SuckerPunch!\n";

    init_memory();
    print_mem(GetStorage());

    /*
    Q* q0 = create_queue();
    enqueue_byte(q0, 0);
    enqueue_byte(q0, 1);
    Q* q1 = create_queue();
    enqueue_byte(q1, 3);
    enqueue_byte(q0, 2);
    enqueue_byte(q1, 4);
    printf("%d", dequeue_byte(q0));
    printf("%d\n", dequeue_byte(q0));
    enqueue_byte(q0, 5);
    enqueue_byte(q1, 6);
    printf("%d", dequeue_byte(q0));
    printf("%d", dequeue_byte(q0));
    destroy_queue(q0);

    printf("%d", dequeue_byte(q1));
    printf("%d", dequeue_byte(q1));
    printf("%d\n", dequeue_byte(q1));
    destroy_queue(q1);
    */
}