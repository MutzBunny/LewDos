#ifndef SYSTEM_CONFIG_H
#define SYSTEM_CONFIG_H

//define the Target system Hardware Constants
//Amount of RAM avaiable on Target host. (In addresses, aka. System words)
#define LEWDOS_SYSTEM_MEMORY 65536

//define the start of The Memory Block (Usually 0, some systems have weird memory maps)
#define LEWDOS_MEMORY_START_ADDRESS 0

//Size of individual memory allocations blocks. (recommended 512 or 1024 Bytes)
#define LEWDOS_MEMORY_SIZE 1024

//Defines, if the Target host has a phsyical mass storage device. If set to 0, the code will run in ROM mode. 
#define LEWDOS_HAS_SYSTEM_DRIVE 0

#endif

