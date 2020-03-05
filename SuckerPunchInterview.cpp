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
typedef unsigned char ByteBlock;

//Byte queue type - Q
struct Q {
    Q * head;
    Q * tail;
    unsigned int size;
    ByteBlock* byteArray;
};

// Constants
const unsigned int MAX_QUEUES = 64;
const unsigned int TOTAL_MEM_SIZE = 2048;

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
    //Init all memory to 0
    memset(GetStorage(), 0, TOTAL_MEM_SIZE);
}

// Creates a FIFO byte queue, returning a handle to it.
Q * create_queue() {
    Q* newQueue = NULL;

    // Allocate a queue and track how many are allocated
    if (GetStorage()[1] < MAX_QUEUES) {


        GetStorage()[1] += 1;
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



int main()
{
    using namespace std;

    cout << "Hello SuckerPunch!\n";

    init_memory();

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