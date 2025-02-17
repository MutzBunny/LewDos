#include "System_Config.h"
#include "main.h"
#include <stdio.h>

#define Memory_Blocks (System_Memory / Memory_Block_Size)

uint8_t Memory_BitMap[Memory_Blocks];
uint8_t *start_address = (uint8_t*) Memory_Start_Address;
