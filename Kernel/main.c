#include "../Include/System_Config.h"
#include "../Include/main.h"

int main()
{
    init_memory();
    Serial_out("Hello World\n");
    int temp = memory_alloc(1024, 1, 0);
    int *mempointer = memory_access(temp, 1);
    Serial_out("Memory Allocated at: %p \n", mempointer);
    *mempointer = 0x1234;
    Serial_out("Memory Value: %x \n", *mempointer); 
    while(1){}
    return 0;
}