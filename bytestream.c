#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "OS_Config.h"
#include "bytestream.h"

#define get_buf_bytestream_buffered(stream) (void*)((bytestream_buffered*)stream)->buf_base
#define get_buf_bytestream_custom_buffered(stream) (void*)((bytestream_custom_buffered*)stream)->buf_base

typedef struct {
#ifdef BYTESTREAM_HOLD_TYPE
    unsigned char type;
#endif
    void (*destroy)(void* self); //destructor
    size_t (*read)(void* self, char* buf, size_t max); // Reads from stream with max characters, returns how many characters were read.
    size_t (*write)(void* self, const char* buf, size_t n); // Writes to stream n characters, returns how many actually got written.
    size_t (*read_custom)(char* buf, size_t max); // Reads from stream with max characters, returns how many characters were read.
    size_t (*write_custom)(const char* buf, size_t n); // Writes to stream n characters, returns how many actually got written.
    char *buf_base,*buf_begin,*buf_end; // Represents internal state of the buffer
} bytestream_buffered; // the void* self represets the pointer to the bytestream.

typedef struct {
#ifdef BYTESTREAM_HOLD_TYPE
    unsigned char type;
#endif
    void (*destroy)(void* self); //destructor
    size_t (*read)(void* self, char* buf, size_t max); // Reads from stream with max characters, returns how many characters were read.
    size_t (*write)(void* self, const char* buf, size_t n); // Writes to stream n characters, returns how many actually got written.
    size_t (*custom_read)(void* self_custom, char* buf, size_t max); // Reads from stream with max characters, returns how many characters were read.
    size_t (*custom_write)(void* self_custom, const char* buf, size_t n); // Writes to stream n characters, returns how many actually got written.
    void (*custom_destroy)(void* self); //destructor
    void* custom; // Represents a custom pointer by the user
} bytestream_custom; // the void* self represets the pointer to the bytestream.

typedef struct {
#ifdef BYTESTREAM_HOLD_TYPE
    unsigned char type;
#endif
    void (*destroy)(void* self); //destructor
    size_t (*read)(void* self, char* buf, size_t max); // Reads from stream with max characters, returns how many characters were read.
    size_t (*write)(void* self, const char* buf, size_t n); // Writes to stream n characters, returns how many actually got written.
    size_t (*custom_read)(void* self_custom, char* buf, size_t max); // Reads from stream with max characters, returns how many characters were read.
    size_t (*custom_write)(void* self_custom, const char* buf, size_t n); // Writes to stream n characters, returns how many actually got written.
    void (*destroy_custom)(void* custom); //destructor for the custom instance
    void* custom; // Represents a custom pointer by the user
    char *buf_base,*buf_begin,*buf_end; // Represents internal state of the buffer
} bytestream_custom_buffered; // the void* self represets the pointer to the bytestream.

typedef struct {
    char *buf_base,*buf_begin,*buf_end; // Represents internal state of the buffer
} bytearray_buffer;

typedef int(*flush_buffer_bytestream_func_t)(bytestream*);

void destroy_raw_bytestream(void* b) {} // empty

flush_buffer_bytestream_func_t get_flush_function_bytestream(bytestream*);

size_t buf_current_size(bytearray_buffer* buffer) {
    return buffer->buf_base-buffer->buf_begin;
}
size_t buf_max_size(bytearray_buffer* buffer) {
    return buffer->buf_end-buffer->buf_begin;
}
size_t buf_available_size(bytearray_buffer* buffer) {
    return buffer->buf_end-buffer->buf_base;
}
size_t buf_add_characters(bytearray_buffer* buffer, const char* bytes, size_t n) {
    if (n == 0) return 0;
    if (buf_available_size(buffer)>n) n = buf_available_size(buffer);
    memcpy(buffer->buf_base,bytes,n);
    buffer->buf_base += n;
    return n;
}
// returns how many characters got removed
size_t buf_remove_characters(bytearray_buffer* buffer, size_t n) {
    if (buf_current_size(buffer)<=n) {
        size_t last_size = buf_current_size(buffer);
        buffer->buf_base = buffer->buf_begin;
        return last_size;
    } else {
        memmove(
                buffer->buf_begin,
                buffer->buf_begin+n,
                buf_current_size(buffer)-n
            );
        buffer->buf_base-=n;
        return n;
    }
}

void destroy_buffered_bytestream(void* stream) {
    free(((bytestream_buffered*)stream)->buf_begin);
}

void destroy_custom_bytestream(void* stream) {
    ((bytestream_custom_buffered*)stream)->destroy_custom(
            ((bytestream_custom_buffered*)stream)->custom
        );
}

void destroy_custom_buffered_bytestream(void* stream) {
    free(((bytestream_custom_buffered*)stream)->buf_begin);
    ((bytestream_custom_buffered*)stream)->destroy_custom(((bytestream_custom_buffered*)stream)->custom);
}

int flush_buffered_bytestream(bytestream* stream) {
    size_t characters_to_write = buf_current_size(
            (void*)((bytestream_buffered*)stream)->buf_base
        );
    if (characters_to_write == 0) return 0;
    size_t consumed = ((bytestream_buffered*)stream)->write_custom(
            ((bytestream_buffered*)stream)->buf_begin,
            characters_to_write
        );
    if (consumed == 0) return 0;
    assert(consumed <= characters_to_write);
    buf_remove_characters((void*)((bytestream_buffered*)stream)->buf_base,consumed);
    return ((bytestream_buffered*)stream)->buf_base-((bytestream_buffered*)stream)->buf_begin;
}

int flush_buffered_custom_bytestream(bytestream* stream) {
    size_t characters_to_write = buf_current_size(
            (void*)((bytestream_custom_buffered*)stream)->buf_base
        );
    if (characters_to_write == 0) return 0;
    size_t consumed = ((bytestream_custom_buffered*)stream)->custom_write(
            ((bytestream_custom_buffered*)stream)->custom,
            ((bytestream_custom_buffered*)stream)->buf_begin,
            characters_to_write
        );
    if (consumed == 0) return 0;
    assert(consumed <= characters_to_write);
    buf_remove_characters(
            (void*)((bytestream_custom_buffered*)stream)->buf_base,consumed
        );
    return ((bytestream_custom_buffered*)stream)->buf_base-((bytestream_custom_buffered*)stream)->buf_begin;
}

flush_buffer_bytestream_func_t get_flush_function_bytestream(bytestream* stream) {
#ifdef BYTESTREAM_HOLD_TYPE
    switch (stream->type) {
        1:
            return flush_buffered_bytestream;
        3:
            return flush_buffered_custom_bytestream;
        default:
            return NULL;
    }
#else
    if (stream->destroy == destroy_buffered_bytestream) return flush_buffered_bytestream;
    else if (stream->destroy == destroy_custom_buffered_bytestream) return flush_buffered_custom_bytestream;
    else return NULL;
#endif
}



bytestream* create_raw_bytestream(
        size_t (*read_f)(bytestream*, char*, size_t),
        size_t (*write_f)(bytestream*, const char*, size_t)
    )
{
    bytestream* stream = (bytestream*)malloc(sizeof(bytestream));
#ifdef BYTESTREAM_HOLD_TYPE
    stream->type = 0;
#endif
    stream->destroy = destroy_raw_bytestream;
    stream->read = (void*)read_f;
    stream->write = (void*)write_f;
    return stream;
}

size_t read_bytestream_buffered(bytestream_buffered* stream, char* buf, size_t max) {
    // no idea what i wrote here, might need refactoring
    size_t buf_size = buf_current_size(get_buf_bytestream_buffered(stream));
    char* orig_buf = buf;
    size_t amount_to_read = max;
    if (buf_size>=max) {
        memcpy(buf,stream->buf_begin,max);
        buf_remove_characters(get_buf_bytestream_buffered(stream), max);
        return max;
    } else if (buf_size>0) {
        char buffer[max-buf_size+buf_max_size(get_buf_bytestream_buffered(stream))];
        memcpy(buf,stream->buf_begin,buf_size);
        buf_remove_characters(get_buf_bytestream_buffered(stream), buf_size);
        buf+=buf_size;
        max-=buf_size;
        buf_size = max+buf_max_size(get_buf_bytestream_buffered(stream));
        buf_size = stream->read_custom(buffer,buf_size);
        if (buf_size >= max) {
            memcpy(buf,buffer,max);
            buf_add_characters(get_buf_bytestream_buffered(stream), buffer, buf_size-max);
            return amount_to_read;
        } else {
            memcpy(buf,buffer,buf_size);
            return buf-orig_buf+buf_size;
        }
    } else {
        char buffer[buf_max_size(get_buf_bytestream_buffered(stream))+max];
        buf_size = stream->read_custom(
                buffer, 
                buf_max_size(get_buf_bytestream_buffered(stream))+max
            );
        if (buf_size > max) {
            memcpy(buf,buffer,max);
            buf_add_characters(
                    get_buf_bytestream_buffered(stream),
                    buffer+max,
                    buf_size-max
                );
            return max;
        } else if (buf_size == max) {
            memcpy(buf,buffer,max);
            return max;
        } else {
            memcpy(buf,buffer,buf_size);
            return buf_size;
        }
    }
}

size_t write_bytestream_buffered(bytestream_buffered* stream, const char* buf, size_t n) {
    bytearray_buffer* buffer = get_buf_bytestream_buffered(stream);
    size_t buffer_size = buf_current_size(buffer);
    char stack_buffer[buffer_size+n];
    char* stack_buffer_ptr = stack_buffer;
    if (buffer_size>0) {
        memcpy(stack_buffer_ptr,stream->buf_begin,buffer_size);
        stream->buf_base = stream->buf_begin;
        stack_buffer_ptr+=buffer_size;
    }
    memcpy(stack_buffer_ptr,buf,n);
    buffer_size+=n; //buffer_size now represents the size of stack_buffer
    size_t consumed = stream->write_custom(stack_buffer, buffer_size);
    assert(consumed <= buffer_size);
    if (consumed == buffer_size) return n; // all characters have been gobbled up
    else if (buffer_size - consumed <= buf_max_size(buffer)) {
        buf_add_characters(buffer, stack_buffer+consumed, buffer_size - consumed);
        return n; // some characters have been put in the cache
    } else {
        //not all characters gobbled up
        buf_add_characters(buffer, stack_buffer+consumed, buf_max_size(buffer));
        return consumed-buf_max_size(buffer);
    }
}

size_t read_bytestream_custom(bytestream_custom* stream, char* buf, size_t max) {
    return stream->custom_read(stream->custom, buf, max);
}

size_t write_bytestream_custom(bytestream_custom* stream, const char* buf, size_t n) {
    return stream->custom_write(stream->custom, buf, n);
}

size_t read_bytestream_custom_buffered(bytestream_custom_buffered* stream, char* buf, size_t max) {
    // no idea what i wrote here, might need refactoring
    size_t buf_size = buf_current_size(get_buf_bytestream_custom_buffered(stream));
    char* orig_buf = buf;
    size_t amount_to_read = max;
    if (buf_size>=max) {
        memcpy(buf,stream->buf_begin,max);
        buf_remove_characters(get_buf_bytestream_custom_buffered(stream), max);
        return max;
    } else if (buf_size>0) {
        char buffer[max-buf_size+buf_max_size(get_buf_bytestream_custom_buffered(stream))];
        memcpy(buf,stream->buf_begin,buf_size);
        buf_remove_characters(get_buf_bytestream_custom_buffered(stream), buf_size);
        buf+=buf_size;
        max-=buf_size;
        buf_size = max+buf_max_size(get_buf_bytestream_custom_buffered(stream));
        buf_size = stream->custom_read(stream->custom,buffer,buf_size);
        if (buf_size >= max) {
            memcpy(buf,buffer,max);
            buf_add_characters(get_buf_bytestream_custom_buffered(stream), buffer, buf_size-max);
            return amount_to_read;
        } else {
            memcpy(buf,buffer,buf_size);
            return buf-orig_buf+buf_size;
        }
    } else {
        char buffer[buf_max_size(get_buf_bytestream_custom_buffered(stream))+max];
        buf_size = stream->custom_read(
                stream->custom,
                buffer, 
                buf_max_size(get_buf_bytestream_custom_buffered(stream))+max
            );
        if (buf_size > max) {
            memcpy(buf,buffer,max);
            buf_add_characters(
                    get_buf_bytestream_custom_buffered(stream),
                    buffer+max,
                    buf_size-max
                );
            return max;
        } else if (buf_size == max) {
            memcpy(buf,buffer,max);
            return max;
        } else {
            memcpy(buf,buffer,buf_size);
            return buf_size;
        }
    }
}

size_t write_bytestream_custom_buffered(bytestream_custom_buffered* stream, const char* buf, size_t n) {
    bytearray_buffer* buffer = get_buf_bytestream_custom_buffered(stream);
    size_t buffer_size = buf_current_size(buffer);
    char stack_buffer[buffer_size+n];
    char* stack_buffer_ptr = stack_buffer;
    if (buffer_size>0) {
        memcpy(stack_buffer_ptr,stream->buf_begin,buffer_size);
        stream->buf_base = stream->buf_begin;
        stack_buffer_ptr+=buffer_size;
    }
    memcpy(stack_buffer_ptr,buf,n);
    buffer_size+=n; //buffer_size now represents the size of stack_buffer
    size_t consumed = stream->custom_write(stream->custom, stack_buffer, buffer_size);
    assert(consumed <= buffer_size);
    if (consumed == buffer_size) return n; // all characters have been gobbled up
    else if (buffer_size - consumed <= buf_max_size(buffer)) {
        buf_add_characters(buffer, stack_buffer+consumed, buffer_size - consumed);
        return n; // some characters have been put in the cache
    } else {
        //not all characters gobbled up
        buf_add_characters(buffer, stack_buffer+consumed, buf_max_size(buffer));
        return consumed-buf_max_size(buffer);
    }
}

bytestream* create_buffered_bytestream(
        size_t (*read_f)(char*, size_t),
        size_t (*write_f)(const char*, size_t)
    )
{
    bytestream_buffered* stream = (bytestream_buffered*)malloc(sizeof(bytestream_buffered));
#ifdef BYTESTREAM_HOLD_TYPE
    stream->type = 1;
#endif
    stream->destroy = destroy_buffered_bytestream;
    stream->read = (void*)read_bytestream_buffered;
    stream->write = (void*)write_bytestream_buffered;
    stream->read_custom = (void*)read_f;
    stream->write_custom = (void*)write_f;

    //initialize buffer
    stream->buf_begin = (char*)malloc(sizeof(char)*BYTESTREAM_BUF_SIZE_DEFAULT);
    stream->buf_base = stream->buf_begin;
    stream->buf_end = stream->buf_begin+BYTESTREAM_BUF_SIZE_DEFAULT;

    return (void*)stream;
}

bytestream* create_buffered_bytestream_buf_size(
        size_t (*read_f)(char*, size_t),
        size_t (*write_f)(const char*, size_t),
        size_t buf_size
    )
{
    bytestream_buffered* stream = (bytestream_buffered*)malloc(sizeof(bytestream_buffered));
#ifdef BYTESTREAM_HOLD_TYPE
    stream->type = 1;
#endif
    stream->destroy = destroy_buffered_bytestream;
    stream->read = (void*)read_bytestream_buffered;
    stream->write = (void*)write_bytestream_buffered;
    stream->read_custom = (void*)read_f;
    stream->write_custom = (void*)write_f;

    //initialize buffer
    stream->buf_begin = (char*)malloc(sizeof(char)*buf_size);
    stream->buf_base = stream->buf_begin;
    stream->buf_end = stream->buf_begin+buf_size;

    return (void*)stream;
}

bytestream* create_custom_bytestream(
        size_t (*read_f)(void*, char*, size_t), // in here, void* is the pointer to the custom object
        size_t (*write_f)(void*, const char*, size_t),
        void (*destroy)(void*), // same over here.
        void* custom
    )
{
    bytestream_custom* stream = (bytestream_custom*)malloc(sizeof(bytestream_custom));
#ifdef BYTESTREAM_HOLD_TYPE
    stream->type = 2;
#endif
    stream->destroy = destroy_custom_bytestream;
    stream->read = (void*)read_bytestream_custom;
    stream->write = (void*)write_bytestream_custom;
    stream->custom_read = (void*)read_f;
    stream->custom_write = (void*)write_f;
    stream->custom_destroy = destroy;
    stream->custom = custom;

    return (void*)stream;
}

bytestream* create_custom_buffered_bytestream(
        size_t (*read_f)(void*, char*, size_t), // in here, void* is the pointer to the custom object
        size_t (*write_f)(void*, const char*, size_t),
        void (*destroy)(void*), // same over here.
        void* custom
    )
{
    bytestream_custom_buffered* stream = (bytestream_custom_buffered*)malloc(sizeof(bytestream_custom_buffered));
#ifdef BYTESTREAM_HOLD_TYPE
    stream->type = 3;
#endif
    stream->destroy = destroy_custom_buffered_bytestream;
    stream->read = (void*)read_bytestream_custom_buffered;
    stream->write = (void*)write_bytestream_custom_buffered;
    stream->custom_read = (void*)read_f;
    stream->custom_write = (void*)write_f;
    stream->destroy_custom = destroy;

    //initialize buffer
    stream->buf_begin = (char*)malloc(sizeof(char)*BYTESTREAM_BUF_SIZE_DEFAULT);
    stream->buf_base = stream->buf_begin;
    stream->buf_end = stream->buf_begin+BYTESTREAM_BUF_SIZE_DEFAULT;

    return (void*)stream;
}

bytestream* create_custom_buffered_bytestream_buf_size(
        size_t (*read_f)(void*, char*, size_t), // in here, void* is the pointer to the custom object
        size_t (*write_f)(void*, const char*, size_t),
        void (*destroy)(void*), // same over here.
        void* custom,
        size_t buf_size
    )
{
    bytestream_custom_buffered* stream = (bytestream_custom_buffered*)malloc(sizeof(bytestream_custom_buffered));
#ifdef BYTESTREAM_HOLD_TYPE
    stream->type = 3;
#endif
    stream->destroy = destroy_custom_buffered_bytestream;
    stream->read = (void*)read_bytestream_custom_buffered;
    stream->write = (void*)write_bytestream_custom_buffered;
    stream->custom_read = (void*)read_f;
    stream->custom_write = (void*)write_f;
    stream->destroy_custom = destroy;

    //initialize buffer
    stream->buf_begin = (char*)malloc(sizeof(char)*buf_size);
    stream->buf_base = stream->buf_begin;
    stream->buf_end = stream->buf_begin+buf_size;

    return (void*)stream;
}

size_t read_from_bytestream(bytestream* stream, char* buffer, size_t max) {
    return stream->read(stream, buffer, max);
}

size_t write_to_bytestream(bytestream *stream, const char *buffer, size_t n) {
    return stream->write(stream, buffer, n);
}

int flush_bytestream(bytestream *stream) {
    flush_buffer_bytestream_func_t func = get_flush_function_bytestream(stream);
    if (func == NULL) return 0;
    return func(stream);
}

void destroy_bytestream(bytestream* stream) {
    stream->destroy(stream);
    free(stream);
}