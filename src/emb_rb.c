#include "emb_rb.h"

// Initialize the ring buffer
uint32_t emb_rb_init( emb_rb_t *rb, uint8_t *bP, uint32_t size )
{
    // Null check
    if( !rb || !bP || !size ) {
        return 0;
    }
    rb->bP = bP;
    rb->size = size;
    rb->head = 0;
    rb->tail = 0;
    return -1;
}

// Get the total size of the ring buffer
uint32_t emb_rb_size( emb_rb_t *rb )
{
    // Null check
    if( !rb ) {
        return 0;
    }
    return rb->size;
}

// Queue len nubmer of bytes into the ring buffer, making a deep copy
uint32_t emb_rb_queue( emb_rb_t *rb, uint8_t *bytes, uint32_t len )
{
    // Null check
    if( !rb || !bytes || !len ) {
        return 0;
    }
    // Check if there is enough space
    if( len > emb_rb_free_space( rb ) ) {
        len = emb_rb_free_space( rb );
    }
    // Copy bytes
    for( uint32_t i = 0; i < len; i++ ) {
        rb->bP[rb->head % rb->size] = bytes[i];
        rb->head++;
    }
    return len;
}

// Dequeue len number of bytes from the ring buffer
uint32_t emb_rb_dequeue( emb_rb_t *rb, uint8_t *bytes, uint32_t len )
{
    // Null check
    if( !rb || !bytes || !len ) {
        return 0;
    }
    // Check if there is enough data
    if( len > emb_rb_used_space( rb ) ) {
        len = emb_rb_used_space( rb );
    }
    // Copy bytes
    for( uint32_t i = 0; i < len; i++ ) {
        bytes[i] = rb->bP[rb->tail % rb->size];
        rb->tail++;
    }
    return len;
}

// Peek len number of bytes from the ring buffer without dequeuing
uint32_t emb_rb_peek( emb_rb_t *rb, uint8_t *bytes, uint32_t len )
{
    // Null check
    if( !rb || !bytes || !len ) {
        return 0;
    }
    // Check if there is enough data
    if( len > emb_rb_used_space( rb ) ) {
        len = emb_rb_used_space( rb );
    }
    // Copy bytes
    uint32_t local_tail = rb->tail;
    for( uint32_t i = 0; i < len; i++ ) {
        bytes[i] = rb->bP[local_tail % rb->size];
        local_tail++;
    }
    return len;
}

// Effectively empty the buffer by setting the tail vaule to the head value
uint32_t emb_rb_flush( emb_rb_t *rb )
{
    // Null check
    if( !rb ) {
        return 0;
    }
    rb->tail = rb->head;
}

// Get the number of free bytes in the ring buffer
uint32_t emb_rb_free_space( emb_rb_t *rb )
{
    return rb->size - emb_rb_used_space( rb );
}

// Get the number of used bytes in the ring buffer
uint32_t emb_rb_used_space( emb_rb_t *rb )
{
    // Handle the integer wrap around
    if( rb->head < rb->tail ) {
        return rb->size - ( rb->tail - rb->head );
    }
    else {
        return rb->head - rb->tail;
    }
}
