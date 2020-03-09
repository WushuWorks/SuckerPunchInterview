# Sucker Punch Interview Programming Problem

This is my solution to the pre-screening problem for Sucker Punch Studios for a Gameplay Programming position.

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

To preserve constant time insertion and dequeueing we do not overwrite elements and simply track what is to be dequeued next.

## Solution Rationale

>Worst case performance is more important than average case performance for deletion and insertion

To accommodate O(1) time insertion and deletion we structure our solution around a hybrid array approach that is essentially
a dequeue. We track the the beginning and end of the queue through a few indexes that can be used to calculate the exact place
in memory that we will both dequeue from and insert to next. To link the various storage blocks we set aside a byte to store the index
of the block it is linked to. In this way we can guarantee that insertion, deletion, and access are all constant time operations.

To better accommodate our highly limited space I also built the solution around calculating positions in memory based off indexes,
 which for this application is a good way to compact memory since a pointer will be four or eight bytes but an index will only be one.
  As mentioned before I structured the code around use of constant

