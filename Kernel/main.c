#include <stdio.h>
#include "../Include/System_Config.h"
#include "../Include/main.h"

int main()
{
    init_memory();
    printf("Hello World\n");
    int *temp = memory_alloc(1024, 1, 0);
    int *mempointer = temp;
    printf("Memory Allocated at: %p\n", (void*)&mempointer);
    *mempointer = 0x1234;
    printf("Memory Value: %d\n", memory_access((void*)&mempointer, 1)); 
    return 0;
}