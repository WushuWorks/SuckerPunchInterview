// SuckerPunchInterview.cpp : This file contains the 'main' function. Program execution begins and ends there.

#include <iostream>

/*
Requirements:
- Type Q can be anything
- Cannot call malloc() or heap management routines
- All storage must be in provided array `unsigned char data[2048];`
- 64 queues at max
- if we cannot satisfy a request due to lack of memory call provided function
- if an illegal request is made use provided function
- Q -> int

Provided Functions:
 - void on_out_of_memory();
 - void on_illegal_operation();

Notes:
    - Worst case performance is more important than average case performance for deletion and insertion
    - Spikes in queue size or number may occur
    - Do not assume max number of bytes in a queue (except that imposed by memory)
    - 15 queues with 80~ bytes average
*/

// Creates a FIFO byte queue, returning a handle to it.
int* create_queue() {

}


// Destroy an earlier created byte queue.
void destroy_queue(int* q) {

}

// Adds a new byte to a queue.
void enqueue_byte(int* q, unsigned char b) {

}

// Pops the next byte off the FIFO queue
unsigned char dequeue_byte(int* q) {

}



int main()
{
    std::cout << "Hello SuckerPunch!\n";

    unsigned char data[2048];

    int* q0 = create_queue();
    enqueue_byte(q0, 0);
    enqueue_byte(q0, 1);
    int* q1 = create_queue();
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
}