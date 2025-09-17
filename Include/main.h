#ifndef MAIN_H
#define MAIN_H

#define Version "V0.1"

void init_memory(void);
int memory_alloc(int size, unsigned char UserID, unsigned char rw_flags);
void free_memory(void *ptr, unsigned char UserID);
void *memory_access(int block_index, unsigned char userID);
void Serial_out(const char* fmt, ...);

#endif