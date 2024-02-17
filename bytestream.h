#ifndef BYTESTREAM_H
#define BYTESTREAM_H

#include <stdlib.h>

#include "OS_Config.h"

// note: you don't have to actually use this structure, all you really need to use is the API
// aka the functions below.
typedef struct {
#ifdef BYTESTREAM_HOLD_TYPE
    unsigned char type;
#endif
    void (*destroy)(void* self); //destructor
    size_t (*read)(void* self, char* buf, size_t max); // Reads from stream with max characters, returns how many characters were read.
    size_t (*write)(void* self, const char* buf, size_t n); // Writes to stream n characters, returns how many actually got written.
} bytestream; // the void* self represets the pointer to the bytestream.

//Creates a raw bytestream. This may be used to, for example, read/write to objects that dont need instances
extern bytestream* create_raw_bytestream(
        size_t (*read_f)(bytestream*, char*, size_t),
        size_t (*write_f)(bytestream*, const char*, size_t)
    );

//Creates a raw bytestream with buffers.
// Note, read will be modified internally and will first read from the buffer, and then attempt to fill the byte array AND the buffer.
extern bytestream* create_buffered_bytestream(
        size_t (*read_f)(char*, size_t),
        size_t (*write_f)(const char*, size_t)
        //size_t buf_size = 16 (default)
    );
extern bytestream* create_buffered_bytestream_buf_size(
        size_t (*read_f)(char*, size_t),
        size_t (*write_f)(const char*, size_t),
        size_t buf_size
    );

//Creates a custom bytestream with an instance attached to it.
extern bytestream* create_custom_bytestream(
        size_t (*read_f)(void*, char*, size_t), // in here, void* is the pointer to the custom object
        size_t (*write_f)(void*, const char*, size_t),
        void (*destroy)(void*), // same over here.
        void* custom
    );

extern bytestream* create_custom_buffered_bytestream(
        size_t (*read_f)(void*, char*, size_t), // in here, void* is the pointer to the custom object
        size_t (*write_f)(void*, const char*, size_t),
        void (*destroy)(void*), // same over here.
        void* custom
        //size_t buf_size = 16 (default)
    );
extern bytestream* create_custom_buffered_bytestream_buf_size(
        size_t (*read_f)(void*, char*, size_t), // in here, void* is the pointer to the custom object
        size_t (*write_f)(void*, const char*, size_t),
        void (*destroy)(void*), // same over here.
        void* custom,
        size_t buf_size
    );

// Reads from bytestream max bytes and returns how many bytes have been read.
extern size_t read_from_bytestream(bytestream* stream, char* buffer, size_t max);

// Writes to bytestream n bytes and returns how many bytes have actually been written.
extern size_t write_to_bytestream(bytestream* stream, const char* buffer, size_t n);

// Flush buffer to cached byte stream. Returns 0 if:
//  - bytestream is unflushable
//  - nothing left to flush
//  - bytestream rejected flush (didnt take any bytes when written to)
// otherwise returns 1
extern int flush_bytestream(bytestream* stream);

// Calls bytestream destructor then frees it from memory.
extern void destroy_bytestream(bytestream* stream);

#endif // BYTESTREAM_H