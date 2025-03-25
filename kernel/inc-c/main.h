#ifndef LEWDOS_MAIN_H
#define LEWDOS_MAIN_H

#define LEWDOS_VERSION "v0.1.0-indev"

void init_memory(void);
void *memory_alloc(int size, unsigned char UserID, unsigned char rw_flags);
void free_memory(void *ptr, unsigned char UserID);
char memory_access(void *ptr, unsigned char userID);

#endif // LEWDOS_MAIN_H

