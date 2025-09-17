#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

//define the Target system Hardware Constants
//Amount of RAM avaiable on Target host. (In addresses, aka. System words)
#define System_Memory 2097152

//define the start of The Memory Block (Usually 0, some systems have weird memory maps)
#define Memory_Start_Address 0 

//Size of individual memory allocations blocks. (recommended 512 or 1024 Bytes)
#define Memory_Block_Size 1024

//Defines, if the Target host has a phsyical mass storage device. If set to 0, the code will run in ROM mode. 
#define System_has_SystemDrive 0

#endif