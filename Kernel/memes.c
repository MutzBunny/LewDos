#include "../Include/System_Config.h"
#include "../Include/main.h"
#include <stdio.h>

//Calculate the amount of memory blocks available
#define Memory_Blocks (System_Memory / Memory_Block_Size) 

//Create a BitMap for the Memory Blocks (0xFF means unallocated.
//Bitmasked are Bits 0-5 for UserID, 6 for Read ony (0=full access, 1= read only block)
//Bit 7 is the free flag (1=free, 0=allocated) )
unsigned char Memory_BitMap[Memory_Blocks];

//Create a pointer to the start of the Memory Block
//(used as a pointer to return the memory access location, based on which block wants to be accessed)
unsigned char* start_address = (unsigned char*)Memory_Start_Address;

//Start routine to initialize The memory bitmap (set all blocks to free)
void init_memory()
{   //for loop to cycle through all the avaiable memory blocks
    for (int i = 0; i < Memory_Blocks; i++)
    {
        Memory_BitMap[i] = 0xFF; //set them to 0xFF (free)
    }
    //for loop to check if all memory blocks are actually free
    for (int i = 0; i < Memory_Blocks; i++)
    {
        if(Memory_BitMap[i] != 0xFF) //Check if the memory block is not free
        {
            Serial_out("Memory Initialization Failed\n");
            return;
        }
    }
    Serial_out("%dkB of Memory Initialized\n", Memory_Blocks * Memory_Block_Size / 1024);
    return; 
}

//Memory Allocation Function, Sets Flags, and UserID appropriately
int memory_alloc(int size, unsigned char UserID, unsigned char rw_flags){
    //Serial_out("test");
    int blocks_needed = (size +  Memory_Block_Size -1) / Memory_Block_Size; //Calculate the amount of blocks needed
    
    //for loop to cycle through all the avaiable memory blocks
    for(int i = 0; i < Memory_Blocks; i++)
    {
        
        if(Memory_BitMap[i] == 0xFF) //Check if the memory block is free (0xFF means free)
        {
            //Loops through the memory blocks to check if the amount of blocks needed are free
            int j = 0;
            for(j = 0; j < blocks_needed; j++)
            {
                //Serial_out("test");
                if(Memory_BitMap[i+j] != 0xFF) //Check if the memory block is not free
                {
                    //if found a allocated block within the needed blocks, break the loop and try again
                    break;
                }
            }
            if(j == blocks_needed) //Check if the memory block is free
            {
                //if found the amount of free blocks needed, set the flags and UserID
                for(j = 0; j < blocks_needed; j++)
                {
                    //Set the flags, and UserID (Batmasked 0x40 for the rw flag and 0x3F for UserID (bits 0-5) 0x80 is bit 7 set)
                    Memory_BitMap[i+j] = (rw_flags & 0x40) | (UserID & 0x3F) | 0x80;
                }
                Serial_out("found empty memory block: %d \n", i);
                return i; //Return the memory block location
            }
        }
    }
    return 0;
}

//Memory Free Function, Sets the block pointed to, to free
void free_memory(void *ptr, unsigned char UserID)
{
    //Calculate the block number based on the pointer
    int block = ((unsigned char *)ptr - start_address) / Memory_Block_Size;
    //Check if the block is within the memory bounds
    if(block < 0 || block >= Memory_Blocks)
    {
        //out of bounds error
        Serial_out("Invalid Memory Block (address %p @ %d)\n", ptr, UserID);
        return;
    }
    //Check if the UserID matches the block
    if((Memory_BitMap[block] & 0x3F) != UserID)
    {
        //Access violation error
        Serial_out("Access vioation; Invalid memory acess (address %p @ %d)\n", ptr, UserID);
        return;
    }
    //Set the block to free
    Memory_BitMap[block] = 0xFF;
    return;
}   

//Function that checks if the user is allowed to access the memory at the location.
void *memory_access(int block_index, unsigned char userID){
    //Check if the block is within the memory bounds
    if(block_index < 0 || block_index > Memory_Block_Size) return 0;

    //Return 1 if the user is allowed to access the memory
    unsigned char flags = Memory_BitMap[block_index];
    if((flags & 0x3F) != userID) return 0;
    if((flags & 0x80) == 0) return 0;
    return (start_address + block_index * Memory_Block_Size);
}