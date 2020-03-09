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
#include "provided.h" // provided functions
#include <string.h> //memset

/*
Data Types, Constants, Structs, and Data
*/

//Data Types
typedef unsigned int Bit;
typedef unsigned char QBlockHandle; // Index to next storage block


//Byte queue type - Q
struct Q {
    Bit alloc        : 1;
    Bit empty        : 1;
    Bit head         : 8;
    Bit tail         : 8;
    Bit queue_index  : 8; // Index of the q struct
    Bit head_offset  : 5; // Offset of dequeueing

    Bit unused : 1;
};

// Constants
const unsigned int MAX_QUEUES = 64;
const unsigned int TOTAL_MEM_SIZE = 2048;
const unsigned int Q_BLOCK_SIZE = 25; // Size in bytes
const unsigned int MAX_Q_BLOCKS = 68; // Max number of blocks
const unsigned int Q_QUEUE_SIZE = sizeof(Q); // Size of our Q type queue
const unsigned int MAX_SIZE = 255; // Maximum number that can be represented in a byte of memory
const unsigned int NULL_VALUE = MAX_Q_BLOCKS+1; // value that can be used to represent a null index since 0 can be valid 

// Storage block of type Q
// Will always be sizeof(Q_BLOCK_SIZE)
struct QBlock {
    unsigned char bytes[Q_BLOCK_SIZE - sizeof(QBlockHandle)];
    QBlockHandle next;
};

// Resources - provided by problem description
unsigned char data[TOTAL_MEM_SIZE]; // Total available memory for storage from 0 - TOTAL_MEM_SIZE-1

/*
Helper functions
*/

// Given the Q Queues number 0-63, return the correct start location in memory
unsigned int calculate_q_queue_index(unsigned int queue_number) {
    return queue_number * Q_QUEUE_SIZE + MAX_Q_BLOCKS + 1;
}

// Given the Q_block number 0-67, reutrn the correct start location in memory
unsigned int calculate_q_block_index(unsigned int block_number) {
    return block_number * Q_BLOCK_SIZE + calculate_q_queue_index(MAX_QUEUES - 1) + Q_QUEUE_SIZE;
}

// Find the next free Q queue location in memory, or call out of memory function
unsigned int find_next_q_index() {
    unsigned int return_index = TOTAL_MEM_SIZE;

    for (int i = 0; i < MAX_QUEUES; i++) {
        unsigned int current_index = calculate_q_queue_index(i); // Find location of struct in memory
        Q* queue = reinterpret_cast<Q*>(&data[current_index]); //Read struct from memory
        if (!queue->alloc) { // Return index of first unallocated struct
            return_index = current_index;
            break;
        }
    }

    if (return_index == TOTAL_MEM_SIZE) {
        on_out_of_memory(); // call provided function
    }

    return return_index;
}

// Finds the next available memory block handle 0-67, or call out of memory function
unsigned int find_memory() {
    unsigned int return_index = TOTAL_MEM_SIZE;

    for (int i = 0; i < MAX_Q_BLOCKS; i++) {   
        if (data[i] == 0) { // return index of first empty element
            return_index = i;
            break;
        }
    }

    if (return_index == TOTAL_MEM_SIZE) {
        on_out_of_memory(); // call provided function
    }

    return return_index;
}

// Finds the size of elements in the q block with the handle number 0-67
unsigned int find_size(unsigned int index) {
    unsigned int count = data[index];
    return count;
}

// Safely increments the queue counter at the index
void inc_count(unsigned int index) {
    unsigned int newCount = data[index] + 1;
    if (newCount > MAX_SIZE) { newCount = MAX_SIZE; }
    data[index] = newCount;
}

// Safely decrements the queue counter at the index
void dec_count(unsigned int index) {
    int newCount = data[index] - 1;
    if (newCount < 0 || newCount > MAX_SIZE) { newCount = 0; } //
    data[index] = (unsigned int)newCount;
}


// Run first to setup memory
void init_memory() {
    //Init everything to null
    memset(data, 0, TOTAL_MEM_SIZE);
}

// Tests the correctnes of helper functions, return if 100% pass, crash otherwise
void test_helper() {
    //Note all tests are calculated for default settings
    assert(calculate_q_queue_index(0) == 69);
    assert(calculate_q_queue_index(1) == 73);
    assert(calculate_q_queue_index(63) == 321);

    assert(calculate_q_block_index(0) == 325);
    assert(calculate_q_block_index(1) == 350);
    assert(calculate_q_block_index(67) == 2000);
}

// Creates a FIFO byte queue, returning a handle to it.
Q* create_queue() {
    Q* newQueue = NULL;
    unsigned int newCount = 0;
    
    //Create Q struct, write to memory, assign address to return queue, update the queue count, return final queue
    const unsigned int QUEUE_IND = find_next_q_index(); // Find next queue index or call out of memory function
    Q tempQueue = Q();
    tempQueue.alloc = 1U;
    tempQueue.empty = 1U;
    tempQueue.head = 0U;
    tempQueue.tail = 0U;
    tempQueue.queue_index = QUEUE_IND;
    tempQueue.head_offset = 0U;

    memcpy(&data[QUEUE_IND], &tempQueue, sizeof(Q)); // Write struct to memory
    newQueue = reinterpret_cast<Q*>(&data[QUEUE_IND]); // Read location in memory
    inc_count(MAX_Q_BLOCKS); // Increment Q queue count

    return newQueue;
}

// Destroy an earlier created byte queue.
void destroy_queue(Q* q) {
    if (q->alloc && q->empty) { //Must be allocated and empty to destroy
        q->alloc = false;
        q->head = 0;
        q->head_offset = 0;
        q->queue_index = 0;
        q->tail = 0;
        dec_count(MAX_Q_BLOCKS); //Update Q count
    }
    else {
        on_illegal_operation(); // call provided function
    }
}

// Adds a new byte to a queue.
void enqueue_byte(Q* q, unsigned char b) {

    if (q->empty) { // Empty queue
        const unsigned int MEM_LOCATION  = find_memory(); // find empty memory block or call out of memory
        const unsigned int MEM_START_LOC = calculate_q_block_index(MEM_LOCATION);
        //Create QBlock, write to memory, update freelist, update q struct with latest index
        QBlock first_block = QBlock{}; // Create and initialize
        first_block.bytes[0] = b;
        first_block.next = NULL_VALUE; // Sentinel terminal value
        memcpy(&data[MEM_START_LOC], &first_block, sizeof(QBlock)); // Write to memory
        inc_count(MEM_LOCATION); // Update freelist
        q->head = MEM_LOCATION; // Set head and tail indexes
        q->tail = MEM_LOCATION;
        q->empty = false; //no longer empty
    }
    else if (!q->empty){ // One element in queue
        const unsigned int SIZE_OFFSET = find_size(q->tail);
        const unsigned int MEM_START_LOCATION = calculate_q_block_index(q->tail);
        QBlock * curr_block = reinterpret_cast<QBlock*>(&data[MEM_START_LOCATION]);
        if (SIZE_OFFSET >= Q_BLOCK_SIZE-1) { // Storage block full
            const unsigned int MEM_LOCATION = find_memory(); // find new block or call out of memory
            const unsigned int NEW_MEM_START_LOCATION = calculate_q_block_index(MEM_LOCATION);
            //Make new Q_block
            QBlock new_block = QBlock{};
            new_block.next = NULL_VALUE; // Sentinel terminal value
            new_block.bytes[0] = b;
            //Write tail index to `next` of current block
            curr_block->next = MEM_LOCATION;
            //Write new Q_block to memory
            memcpy(&data[NEW_MEM_START_LOCATION], &new_block, sizeof(QBlock));
            //Update tail on Q queue struct
            q->tail = MEM_LOCATION;
        }
        else { // Not full
            memcpy(&data[MEM_START_LOCATION + SIZE_OFFSET], &b, sizeof(unsigned char)); // add new byte
        }
        inc_count(q->tail); // Update count
    }
    else {
        on_illegal_operation(); //catch all
    }

}

// Pops the next byte off the FIFO queue
unsigned char dequeue_byte(Q* q) {
    unsigned char return_byte = 'n';

    if (!q->empty) {
        QBlock* head_block = reinterpret_cast<QBlock*>(&data[calculate_q_block_index(q->head)]); // Read block from memory
        const unsigned int BLOCK_INDEX = q->head;
        const unsigned int BLOCK_START_LOC = calculate_q_block_index(q->head);
        //Read byte from correct index in array
        return_byte = head_block->bytes[q->head_offset];

        //Update headOffset
        if (q->head_offset >= Q_BLOCK_SIZE-2 || q->head_offset+1 >= data[BLOCK_INDEX]) { //Recycle queue - do not directly write over memory
            data[BLOCK_INDEX] = 0; //reset freelist counter
            if (head_block->next == NULL_VALUE) { //if next is null then we are empty
                q->empty = true; // Set queue metadata correctly
                q->head = 0;
                q->head_offset = 0;
                q->tail = 0;
                q->queue_index = 0;
            }
            else {
                q->head = head_block->next; //set head to the next block
            }

            q->head_offset = 0; //reset offset
        }
        else {
            //increment headOffset to make the new head the next element
            q->head_offset += 1;
        }
    }
    else {
        on_illegal_operation(); // call provided function
    }

    return return_byte;
}

// Prints the current state of memory for debugging
void print_mem(unsigned char* memory) {
    // This is the threshold for memory block storage starts
    unsigned int q_block_mem_start = MAX_QUEUES * Q_QUEUE_SIZE + MAX_Q_BLOCKS + 1; // first index of storage - 325
    unsigned int end_of_storage = MAX_Q_BLOCKS * Q_BLOCK_SIZE + q_block_mem_start - 1; // last index of storage - 2024
    unsigned int items_per_row = 10;
    unsigned int row_count = 0;

    std::cout << "Memory Contents:\n";
    for (unsigned int i = 0; i < TOTAL_MEM_SIZE; i++) {

        if (i < MAX_Q_BLOCKS) { // Freelist scope
            if (!i) {
                printf("\nFreelist: Index - %d\n\n", i);
            }
            printf("[%d] ", memory[i]);
        }
        else if (i == MAX_Q_BLOCKS) { // Q Count
            printf("\n\nQ Count: Index - %d\n\n", i);
            printf("[%d]\n", memory[i]);
        }
        else if (i < q_block_mem_start) { // Q Queue location Scope
            if (i == MAX_Q_BLOCKS + 1) {
                printf("\nQ Queue Location: Index - %d\n\n", i);
                items_per_row = 8;
                row_count = 0;
            }
            printf("[%d] ", memory[i]);
        }
        else if (i < end_of_storage + 1) { //Memory block scope
            if (i == q_block_mem_start) {
                printf("\nQ Block Memory: Index - %d\n\n", i);
                items_per_row = Q_BLOCK_SIZE;
                row_count = 0;
            }
            printf("[%d] ", memory[i]);
        }
        else { // Unused memory location
            if (i == end_of_storage + 1) {
                printf("\nUnused Memory: Index - %d\n\n", i);
                items_per_row = 25;
                row_count = 0;
            }
            printf("[%d] ", memory[i]);
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
    std::cout << "Hello SuckerPunch!\n";

    //Run test suite on helper functions, crash on failure
    test_helper();
    init_memory();
    
    //Mixed test
    Q* q0 = create_queue();
    enqueue_byte(q0, 0);
    enqueue_byte(q0, 1);
    Q* q1 = create_queue();
    enqueue_byte(q1, 3);
    enqueue_byte(q0, 2);
    enqueue_byte(q1, 4);
    printf("%d ", dequeue_byte(q0));
    printf("%d\n", dequeue_byte(q0));
    enqueue_byte(q0, 5);
    enqueue_byte(q1, 6);
    printf("%d ", dequeue_byte(q0));
    printf("%d\n", dequeue_byte(q0));
    destroy_queue(q0);
    printf("%d ", dequeue_byte(q1));
    printf("%d ", dequeue_byte(q1));
    printf("%d\n", dequeue_byte(q1));
    destroy_queue(q1);

    //Large byte number, few queue test
    unsigned int test_size = 80;
    Q * q2 = create_queue();
    Q * q3 = create_queue();

    for (unsigned int i = 0; i < test_size; i++) {
        enqueue_byte(q2, i);
    }
    for (unsigned int i = 0; i < test_size; i++) {
        enqueue_byte(q3, i);
    }

    for (unsigned int i = 0; i < test_size-1; i++) {
        printf("%d ", dequeue_byte(q2));
    }
    printf("%d\n", dequeue_byte(q2));
    for (unsigned int i = 0; i < test_size-1; i++) {
        printf("%d ", dequeue_byte(q3));
    }
    printf("%d\n", dequeue_byte(q3));

    destroy_queue(q2);
    destroy_queue(q3);
    
    //Large queue number, few bytes test
    Q* q4 = create_queue();
    Q* q5 = create_queue();
    Q* q6 = create_queue();
    Q* q7 = create_queue();

    enqueue_byte(q4, 1);
    enqueue_byte(q4, 2);
    enqueue_byte(q5, 3);
    enqueue_byte(q5, 4);
    enqueue_byte(q6, 5);
    enqueue_byte(q6, 6);
    enqueue_byte(q7, 7);
    enqueue_byte(q7, 8);

    printf("%d ",  dequeue_byte(q4));
    printf("%d\n", dequeue_byte(q4));
    printf("%d ",  dequeue_byte(q5));
    printf("%d\n", dequeue_byte(q5));
    printf("%d ",  dequeue_byte(q6));
    printf("%d\n", dequeue_byte(q6));
    printf("%d ",  dequeue_byte(q7));
    printf("%d\n", dequeue_byte(q7));

    destroy_queue(q4);
    destroy_queue(q5);
    destroy_queue(q6);
    destroy_queue(q7);

    //Examine memory
    print_mem(data);
}