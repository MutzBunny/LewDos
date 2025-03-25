#ifndef MAIN_H
#define MAIN_H

#define Version "V0.1"

void init_memory(void);
void *memory_alloc(int size, unsigned char UserID, unsigned char rw_flags);
void free_memory(void *ptr, unsigned char UserID);
char memory_access(void *ptr, unsigned char userID);

#endif