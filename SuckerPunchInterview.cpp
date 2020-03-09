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
#include <string.h> //memset
#include <assert.h> //assert

/*
Data Types, Constants, Structs, and Data
*/

//Data Types
typedef unsigned int Bit;
typedef unsigned char QBlockHandle; // Pointer to next storage block


//Byte queue type - Q
struct Q {
    Bit alloc       : 1;
    Bit empty       : 1; // has this Q been assigned to before? (used because sometimes 0 is valid)
    Bit head        : 8;
    Bit tail        : 8;
    Bit queueIndex  : 8; //Index of the q struct

    Bit unused : 6;
};

// Constants
const unsigned int MAX_QUEUES = 64;
const unsigned int TOTAL_MEM_SIZE = 2048;
const unsigned int Q_BLOCK_SIZE = 25; // Size in bytes
const unsigned int MAX_Q_BLOCKS = 68; // Max number of blocks
const unsigned int Q_QUEUE_SIZE = sizeof(Q); // Size of our Q type queue
const unsigned int MAX_SIZE = 255; // Maximum number that can be represented in a byte of memory

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
        unsigned int current_index = calculate_q_queue_index(i);
        Q* queue = reinterpret_cast<Q*>(&data[current_index]);
        if (!queue->alloc) {
            return_index = current_index;
            break;
        }
    }

    if (return_index == TOTAL_MEM_SIZE) {
        assert(false); // call provided function
    }

    return return_index;
}

// Finds the next available memory block handle 0-67, or call out of memory function
unsigned int find_memory() {
    unsigned int return_index = TOTAL_MEM_SIZE;

    for (int i = 0; i < MAX_Q_BLOCKS; i++) {   
        if (data[i] == 0) {
            return_index = i;
            break;
        }
    }

    if (return_index == TOTAL_MEM_SIZE) {
        assert(false); // call provided function
    }

    return return_index;
}

// Finds the size of elements in the q block with the handle number 0-67
unsigned int find_size(unsigned int index) {
    unsigned int count = 0;
    memcpy(&count, &data[index], sizeof(unsigned char)); // read in old count
    //printf("Freelist count: %d Index: %d\n", count, index);
    return count;
}

// Safely updates the queue counter at the index
void inc_count(unsigned int index) {
    unsigned int newCount = data[index] + 1;
    if (newCount > MAX_SIZE) { newCount = MAX_SIZE; }
    data[index] = newCount;
}


// Run first to setup memory
void init_memory() {
    //Init everything to null
    memset(data, 0, TOTAL_MEM_SIZE);
    //Init everything else to null
    //memset(data + MAX_Q_BLOCKS + 1, 0, TOTAL_MEM_SIZE - (MAX_Q_BLOCKS + 1));
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
    const unsigned int queue_ind = find_next_q_index(); // Find next queue index or call out of memory function
    Q tempQueue = Q();
    tempQueue.alloc = 1U;
    tempQueue.empty = 1U;
    tempQueue.head = 0U;
    tempQueue.tail = 0U;
    tempQueue.queueIndex = queue_ind;

    memcpy(&data[queue_ind], &tempQueue, sizeof(Q));
    newQueue = reinterpret_cast<Q*>(&data[queue_ind]);
    inc_count(MAX_Q_BLOCKS);

    return newQueue;
}

// Destroy an earlier created byte queue.
void destroy_queue(Q* q) {

}

// Adds a new byte to a queue.
void enqueue_byte(Q* q, unsigned char b) {

    if (q->empty) { // Empty queue
        const unsigned int mem_location = find_memory(); // find empty memory block or call out of memory
        const unsigned int mem_start_loc = calculate_q_block_index(mem_location);
        //Create QBlock, write to memory, update freelist, update q struct with latest index
        QBlock firstBlock = QBlock{};
        firstBlock.bytes[0] = b;
        firstBlock.next = NULL;
        memcpy(&data[mem_start_loc], &firstBlock, sizeof(QBlock));
        inc_count(mem_location);
        q->head = mem_location;
        q->tail = mem_location;
        q->empty = false; //no longer empty
        //std::cout << "q->tail: " << q->tail << " mem_start: " << mem_start_loc <<
            //" sizeInStorage: " << find_size(q->tail) << std::endl;
    }
    else if (!q->empty){ // One element in queue
        const unsigned int size_in_storage = find_size(q->tail);
        const unsigned int mem_start_loc = calculate_q_block_index(q->tail);
        //QBlock * curr_block = reinterpret_cast<QBlock*>(mem_start_loc);
        if (size_in_storage >= Q_BLOCK_SIZE) { // Storage block full
            //TODO: Grow list
            const unsigned int mem_location = find_memory(); // find new block or call out of memory
            const unsigned int new_mem_start_loc = calculate_q_block_index(mem_location);
            const unsigned int curr_block_index = q->tail;
            //Make new Q_block
            QBlock new_block = QBlock{};
            new_block.next = NULL;
            new_block.bytes[0] = b;
            //Write tail index to `next` of current block
            memcpy(&data[mem_start_loc + Q_BLOCK_SIZE], &curr_block_index, sizeof(unsigned char));
            //Write new Q_block to memory
            memcpy(&data[new_mem_start_loc], &new_block, sizeof(QBlock));
            //Update tail on Q queue struct
            q->tail = mem_location;
        }
        else { // Not full
            memcpy(&data[mem_start_loc+size_in_storage], &b, sizeof(unsigned char)); // add new byte
            inc_count(q->tail); // Update count
        }
    }
    else { 
        //catch all for debug
        assert(false); 
    }

}

// Pops the next byte off the FIFO queue
unsigned char dequeue_byte(Q* q) {
    return 'b';
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
    
    Q* q0 = create_queue();
    enqueue_byte(q0, 6);
    enqueue_byte(q0, 2);
    Q* q1 = create_queue();
    enqueue_byte(q1, 3);
    enqueue_byte(q1, 5);
    enqueue_byte(q1, 7);
    enqueue_byte(q1, 3);
    enqueue_byte(q1, 5);
    enqueue_byte(q1, 7);
    enqueue_byte(q1, 3);
    enqueue_byte(q1, 5);
    enqueue_byte(q1, 7);
    enqueue_byte(q1, 3);
    enqueue_byte(q1, 5);
    enqueue_byte(q1, 7);
    enqueue_byte(q1, 3);
    enqueue_byte(q1, 5);
    enqueue_byte(q1, 7);
    enqueue_byte(q1, 3);
    enqueue_byte(q1, 5);
    enqueue_byte(q1, 7);
    enqueue_byte(q1, 3);
    enqueue_byte(q1, 5);
    enqueue_byte(q1, 7);
    enqueue_byte(q1, 3);
    enqueue_byte(q1, 5);
    enqueue_byte(q1, 7);
    enqueue_byte(q1, 3);
    enqueue_byte(q1, 5);
    enqueue_byte(q1, 7); //30 total elements

    /*
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
    
    //Examine memory
    print_mem(data);
}