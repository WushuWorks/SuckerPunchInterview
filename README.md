# Sucker Punch Interview Programming Problem

This is my solution to the pre-screening problem for Sucker Punch Studios for a Gameplay Programming position.

### Problem
Below is the problem as stated to me.  

The problem is to write a set of functions to manage a variable number of byte queues, each with
variable length, in a small, fixed amount of memory.  
You should provide implementations of the following four functions:  
```c++
 // Creates a FIFO byte queue, returning a handle to it.  
 Q * create_queue();  

 // Destroy an earlier created byte queue.  
 void destroy_queue(Q * q);  

 // Adds a new byte to a queue.  
 void enqueue_byte(Q * q, unsigned char b);  

 // Pops the next byte off the FIFO queue  
 unsigned char dequeue_byte(Q * q);   
```

So, the output from the following set of calls:  
```c++
 Q * q0 = create_queue();
 enqueue_byte(q0, 0);
 enqueue_byte(q0, 1);
 Q * q1 = create_queue();
 enqueue_byte(q1, 3);
 enqueue_byte(q0, 2);
 enqueue_byte(q1, 4);
 printf("%d", dequeue_byte(q0));
 printf("%d\n", dequeue_byte(q0));
 enqueue_byte(q0, 5);
 enqueue_byte(q1, 6);
 printf("%d", dequeue_byte(q0));
 printf("%d\n", dequeue_byte(q0));
 destroy_queue(q0);
 printf("%d", dequeue_byte(q1));
 printf("%d", dequeue_byte(q1));
 printf("%d\n", dequeue_byte(q1));
 destroy_queue(q1); 
```
should be:  
 0 1  
 2 5  
 3 4 6   
 
You can define the type Q to be whatever you want. Your code is not allowed to call malloc() or other heap management routines.  
Instead, all storage (other than local variables in your functions) must be within a provided array:  

` unsigned char data[2048]; `  

Memory efficiency is important. On average while your system is running, there will be about 15 queues with an average of 80 or so bytes in each queue.  
Your functions may be asked to create a larger number of queues with less bytes in each.   
Your functions may be asked to create a smaller number of queues with more bytes in each.  
Execution speed is important. Worst-case performance when adding and removing bytes is more important than average-case performance.  
If you are unable to satisfy a request due to lack of memory, your code should call a provided failure function, which will not return:  

` void on_out_of_memory(); `  

If the caller makes an illegal request, like attempting to dequeue a byte from an empty queue, your code should call a provided failure function, which will not return:  

` void on_illegal_operation(); `  

There may be spikes in the number of queues allocated, or in the size of an individual queue.  
Your code should not assume a maximum number of bytes in a queue (other than that imposed by the total amount of memory available, of course!)   
You can assume that no more than 64 queues will be created at once.


# Memory Structure

Since our problem description specifies we are using an array we structure our memory hierarchy using array coordinates.  

``
unsigned char data[2048];
``

Our memory coordinate structure is laid out like this

```
> index				purpose  
> -----------------------------------------------------------  
> 0-67				Freelist (index+1 == block #)  
> 68				Q Queue Count  
> 69 - 324			Q Queue Location  
> 325 - 349			Q Block 1  
> 350 - 374			Q Block 2  
>			...  
> 2000 - 2024		        Q Block 68  
> 2025 - 2047		        Unused  

Total Memory --------- 2048 Bytes  
Max Queue ------------ 64  
Block Size ----------- 25 Bytes  
Max Number of Blocks - 68  
```

Our Q Queue is laid out like this

```c++
//Data Types
typedef unsigned int Bit;
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

```

While our Q Block Memory is structured like this

```c++
typedef unsigned char QBlockHandle; // Index to next storage block
// Storage block of type Q
// Will always be sizeof(Q_BLOCK_SIZE)
struct QBlock {
    unsigned char bytes[Q_BLOCK_SIZE - sizeof(QBlockHandle)];
    QBlockHandle next;
};
```

We use constants and calculate our structure boundaries so we can easily change the size and shape of our
memory structure with a few simple changes instead of hard coding a structure into our solution.  

## Solution Function Description

My solution uses a hybrid dynamic array solution that is essentially a dequeue with pointers being replaced with
next block location indices as they are 1 byte rather than the 4 bytes occupied by a pointer. In our freelist we store
the position of the memory and next block inside that memory to write to as a number.

So for example if the index 2 stores the value 12 then Q Block #3 has 12 elements and index 12 has the next empty space.

We add to our dequeue using the tail index stored in `tail` and track which element is next to be removed from the dequeue
using the index of the Q Block in `head` and the position of the next element in `head_offset`.

To preserve constant time queueing and dequeueing we do not overwrite elements and simply track the element to be dequeued next.

## Solution Rationale

>Worst case performance is more important than average case performance for deletion and insertion

To accommodate O(1) time insertion and deletion we structure our solution around a hybrid array approach that is essentially
a dequeue. We track the the beginning and end of the queue through a few indexes that can be used to calculate the exact place
in memory that we will both dequeue from and insert to next. To link the various storage blocks we set aside a byte to store the index
of the block it is linked to. In this way we can guarantee that insertion, deletion, and access are all constant time operations.

To better accommodate our highly limited space I also built the solution around calculating positions in memory based off indexes,
 which for this application is a good way to compact memory since a pointer will be four or eight bytes but an index will only be one. 
 Since this solution does no unnecessary writing to memory and frees and allocates memory dynamically based on the situation we can accommodate 
 requests for queues and stored bytes up to the queue and memory limits described in the problem description which answers the
 question neatly.
 
 ## Solution Tests
 
I include tests to show that the solution performs as expected under mixed conditions, such as many bytes in few queues, 
few bytes in many queues, and mixed insertion and deletion. Running this repo as written will yield an output like the below
along with the finished state of memory after execution.

```
Hello SuckerPunch!
0 1
2 5
3 4 6
0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 ... 68 69 70 71 72 73 74 75 76 77 78 79
0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 ... 68 69 70 71 72 73 74 75 76 77 78 79
1 2
3 4
5 6
7 8
```
 
## License

This solution is licensed under the GNU GPL as described in the provided license file.
