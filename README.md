# Sucker Punch Interview Programming Problem

This is my solution to the pre-screening problem for Sucker Punch Studios for a Gameplay Programming position.

# Memory Structure

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
We use constants and calculate boundaries with them so we can easily change the size and shape of our
memory structure with a few simple changes instead of hard coding a structure into our solution.