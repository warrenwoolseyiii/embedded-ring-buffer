#ifndef EMB_RB_H_
#define EMB_RB_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stddef.h>

typedef struct
{
   uint8_t *bP;
   uint32_t size;
   size_t   head, tail;
} emb_rb_t;

/**
 * @brief Initialize the ring buffer
 *
 * @param rb pointer to the ring buffer we want to initialize
 * @param bP pointer to the buffer we want to use
 * @param size size of the buffer we want to use
 * @return uint32_t returns 0 on failure, -1 on success
 */
int emb_rb_init(emb_rb_t *rb, uint8_t *bP, uint32_t size);

/**
 * @brief Get the total size of the ring buffer
 *
 * @param rb pointer to the ring buffer we want to get the size of
 * @return uint32_t the size of the ring buffer in bytes
 */
uint32_t emb_rb_size(emb_rb_t *rb);

/**
 * @brief Queue a single byte into the ring buffer, making a deep copy
 *
 * @param rb pointer to the ring buffer we want to queue a byte into
 * @param byte the byte we want to queue
 * @return uint8_t 1 if successful, 0 if not
 */
uint8_t emb_rb_queue_single(emb_rb_t *rb, uint8_t byte);

/**
 * @brief Queue len nubmer of bytes into the ring buffer, making a deep copy
 *
 * @param rb pointer to the ring buffer we want to queue bytes into
 * @param bytes pointer to the bytes we want to queue
 * @param len number of bytes we want to queue
 * @return uint32_t number of bytes queued
 */
uint32_t emb_rb_queue(emb_rb_t *rb, const uint8_t *bytes, uint32_t len);

/**
 * @brief Dequeue len number of bytes from the ring buffer
 *
 * @param rb pointer to the ring buffer we want to dequeue bytes from
 * @param bytes pointer to the bytes we want to dequeue
 * @param len number of bytes we want to dequeue
 * @return uint32_t number of bytes dequeued
 */
uint32_t emb_rb_dequeue(emb_rb_t *rb, uint8_t *bytes, uint32_t len);

/**
 * @brief Peek len number of bytes from the ring buffer without dequeuing
 *
 * @param rb pointer to the ring buffer we want to peek bytes from
 * @param position the position offset from the tail we want to peek bytes
 * @param bytes pointer to the bytes we want to peek
 * @param len number of bytes we want to peek
 * @return uint32_t number of bytes peeked
 */
uint32_t emb_rb_peek(emb_rb_t *rb, uint32_t position, uint8_t *bytes, uint32_t len);

/**
 * @brief Insert len number of bytes into the ring buffer at position
 *
 * @param rb pointer to the ring buffer we want to insert bytes into
 * @param position the position offset from the tail we want to insert bytes
 * @param bytes pointer to the bytes we want to insert
 * @param len the number of bytes we want to insert
 * @param all_or_nothing flag to indicate if we want to insert all or nothing, 1 for all or nothing, 0 for do as much as you can
 * @return uint32_t number of bytes inserted
 */
uint32_t emb_rb_insert(emb_rb_t *rb, uint32_t position, const uint8_t *bytes, uint32_t len, uint8_t all_or_nothing);

/**
 * @brief Remove len number of bytes from the ring buffer at position
 *
 * @param rb pointer to the ring buffer we want to remove bytes from
 * @param position the position offset from the tail we want to remove bytes
 * @param bytes pointer to the bytes we want to remove
 * @param len the number of bytes we want to remove
 * @param all_or_nothing flag to indicate if we want to remove all or nothing, 1 for all or nothing, 0 for do as much as you can
 * @return uint32_t number of bytes removed
 */
uint32_t emb_rb_remove(emb_rb_t *rb, uint32_t position, uint8_t *bytes, uint32_t len, uint8_t all_or_nothing);

/**
 * @brief Flush the ring buffer
 *
 * @param rb pointer to the ring buffer we want to flush
 * @return uint32_t returns 0 on failure, -1 on success
 */
int emb_rb_flush(emb_rb_t *rb);

/**
 * @brief Flush len number of bytes from the ring buffer
 *
 * @param rb pointer to the ring buffer we want to flush bytes from
 * @param len number of bytes we want to flush
 * @return uint32_t number of bytes flushed
 */
uint32_t emb_rb_flush_partial(emb_rb_t *rb, uint32_t len);

/**
 * @brief Get the free space in the ring buffer
 *
 * @param rb pointer to the ring buffer we want to get the free space of
 * @return uint32_t free space in the ring buffer in bytes
 */
uint32_t emb_rb_free_space(emb_rb_t *rb);

/**
 * @brief Get the used space in the ring buffer
 *
 * @param rb pointer to the ring buffer we want to get the used space of
 * @return uint32_t used space in the ring buffer in bytes
 */
uint32_t emb_rb_used_space(emb_rb_t *rb);

/**
 * @brief Get the version of the library
 *
 * @return const char* version string
 */
const char *emb_rb_get_ver();

#ifdef __cplusplus
}
#endif

#endif /* EMB_RB_H_ */
