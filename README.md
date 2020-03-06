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

We use constants and calculate our structure boundaries so we can easily change the size and shape of our
memory structure with a few simple changes instead of hard coding a structure into our solution.

```angular2
// Constants
const unsigned int MAX_QUEUES = 64;
const unsigned int TOTAL_MEM_SIZE = 2048;
const unsigned int Q_BLOCK_SIZE = 25; // Size in bytes
const unsigned int MAX_Q_BLOCKS = 68; // Max number of blocks
const unsigned int Q_QUEUE_SIZE = sizeof(Q);
```

