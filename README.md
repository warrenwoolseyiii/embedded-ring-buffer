# embedded-ring-buffer
Templated embedded ring buffer that can be used for any C application, specifically targeted at embedded applications for size and speed.

# Features
1. Lightweight C library for the most basic implementation of a ring buffer you can get.
2. Allows queing and dequeing of `uint8_t` type data into and out of a buffer allocated by the caller.
3. Allows size, occupied, and free space calling.
4. Allows peeking into the entire buffer.
5. Allows flushing the buffer outright.
6. Full unit test coverage.

# How to use it
Here's a sample snippet of C code to instantiate and use an embedded ring buffer.
```
#include "emb_rb.h"
#define RB_SIZE 64

void test()
{
    uint8_t  buf[RB_SIZE];
    emb_rb_t rb;

    // Init the buffer
    if( emb_rb_init( &rb, buf, RB_SIZE ) ) printf( "buffer init success\n" );

    // Queue some bytes
    uint8_t  a_byte = 0x22;
    uint32_t more_bytes = 0xFFEE2200;
    char     astring[] = "hello let me show you all this stuff";

    // Queue a byte
    if( emb_rb_queue( &rb, &a_byte, sizeof( a_byte ) ) == sizeof( a_byte ) ) printf( "queued %x\n", a_byte );
    if( emb_rb_queue( &rb, (uint8_t *)&more_bytes, sizeof( more_bytes ) ) == sizeof( more_bytes ) ) printf( "queued %x\n", more_bytes );
    if( emb_rb_queue( &rb, (uint8_t *)astring, sizeof( astring ) ) == sizeof( astring ) ) printf( "queued %s\n", astring );

    // Check how many bytes are queued
    printf( "we have %d bytes queued\n", emb_rb_used_space( &rb ) );

    // Check how many bytes are left
    printf( "we have %d bytes free\n", emb_rb_free_space( &rb ) );

    // Dequeue a all the bytes
    uint8_t  readout[RB_SIZE];
    uint32_t stored = emb_rb_used_space( &rb );
    if( emb_rb_dequeue( &rb, readout, stored ) == stored ) printf( "read out %d bytes\n", stored );
}
```
  
