#include "emb_rb.h"
#include "rb_version.h"
#include <stdint.h>
#include <string.h>

// Internal helper methods, that are mutex safe

// Get the number of used bytes in the ring buffer
static uint32_t _internal_emb_rb_used_space(emb_rb_t *rb)
{
   // Handle the integer wrap around
   if (rb->head < rb->tail)
   {
      return(rb->size - (rb->tail - rb->head));
   }
   else
   {
      return(rb->head - rb->tail);
   }
}

// Get the number of free bytes in the ring buffer
static uint32_t _internal_emb_rb_free_space(emb_rb_t *rb)
{
   return(rb->size - _internal_emb_rb_used_space(rb));
}

// Initialize the ring buffer
int emb_rb_init(emb_rb_t *rb, uint8_t *bP, uint32_t size)
{
   // Null check
   if (!rb || !bP || !size)
   {
      return(EMB_RB_ERR_ILLEGAL_ARGS);
   }
   rb->bP   = bP;
   rb->size = size;
   rb->head = 0;
   rb->tail = 0;
   if (pthread_mutex_init(&rb->lock, NULL) != 0)
   {
      return(EMB_RB_ERR_LOCK);
   }
   return(EMB_RB_ERR_OK);
}

// Get the total size of the ring buffer
uint32_t emb_rb_size(emb_rb_t *rb, int *err)
{
   // Null check
   if (!rb)
   {
      if (err)
      {
         *err = EMB_RB_ERR_ILLEGAL_ARGS;
      }
      return(0);
   }
   // Lock the buffer, get the size, and unlock
   if (pthread_mutex_trylock(&rb->lock) != 0)
   {
      if (err)
      {
         *err = EMB_RB_ERR_LOCK;
      }
      return(0);
   }
   uint32_t size = rb->size;
   pthread_mutex_unlock(&rb->lock);

   // Set the error code
   if (err)
   {
      *err = EMB_RB_ERR_OK;
   }

   return(size);
}

// Queue a single byte into the ring buffer, making a deep copy
uint8_t emb_rb_queue_single(emb_rb_t *rb, uint8_t byte, int *err)
{
   // Null check
   if (!rb)
   {
      if (err)
      {
         *err = EMB_RB_ERR_ILLEGAL_ARGS;
      }
      return(0);
   }
   // Lock the buffer
   if (pthread_mutex_trylock(&rb->lock) != 0)
   {
      if (err)
      {
         *err = EMB_RB_ERR_LOCK;
      }
      return(0);
   }
   uint8_t ret = 0;
   // Check if there is enough free space
   if (_internal_emb_rb_free_space(rb))
   {
      // Queue the byte
      rb->bP[rb->head % rb->size] = byte;
      rb->head++;
      ret = 1;

      // Set the error code
      if (err)
      {
         *err = EMB_RB_ERR_OK;
      }
   }
   else if (err)
   {
      *err = EMB_RB_ERR_BUFFER_FULL;
   }
   // Unlock the buffer
   pthread_mutex_unlock(&rb->lock);
   return(ret);
}

// Queue len nubmer of bytes into the ring buffer, making a deep copy
uint32_t emb_rb_queue(emb_rb_t *rb, const uint8_t *bytes, uint32_t len, int *err)
{
   // Null check
   if (!rb || !bytes || !len)
   {
      if (err)
      {
         *err = EMB_RB_ERR_ILLEGAL_ARGS;
      }
      return(0);
   }
   // Lock the buffer
   if (pthread_mutex_trylock(&rb->lock) != 0)
   {
      if (err)
      {
         *err = EMB_RB_ERR_LOCK;
      }
      return(0);
   }
   // Check if there is enough free space
   uint32_t space = _internal_emb_rb_free_space(rb);
   if (len > space)
   {
      len = space;
   }
   // 1. If len is 1, just copy the byte and index since we will have a modulo anyway
   // 2. if len is greater than 1, handle the wrap around
   // 3. otherwise len is 0 and don't do anything
   if (len == 1)
   {
      rb->bP[rb->head % rb->size] = *bytes;
      rb->head++;
   }
   else if (len > 1)
   {
      // Optimize for speed, handle an index wrap around
      uint32_t cur_index     = rb->head % rb->size;
      uint32_t len_till_wrap = rb->size - cur_index;
      uint32_t n             = len;
      if (n > len_till_wrap)
      {
         memcpy(rb->bP + cur_index, bytes, len_till_wrap);
         bytes    += len_till_wrap;
         n        -= len_till_wrap;
         cur_index = 0;
      }
      memcpy(rb->bP + cur_index, bytes, n);
      rb->head += len;
   }
   // Unlock the buffer
   pthread_mutex_unlock(&rb->lock);

   if (err)
   {
      if (len > 0)
      {
         *err = EMB_RB_ERR_OK;
      }
      else
      {
         *err = EMB_RB_ERR_BUFFER_FULL;
      }
   }
   return(len);
}

// Dequeue len number of bytes from the ring buffer
uint32_t emb_rb_dequeue(emb_rb_t *rb, uint8_t *bytes, uint32_t len, int *err)
{
   // Null check
   if (!rb || !bytes || !len)
   {
      if (err)
      {
         *err = EMB_RB_ERR_ILLEGAL_ARGS;
      }
      return(0);
   }
   // Lock the buffer
   if (pthread_mutex_trylock(&rb->lock) != 0)
   {
      if (err)
      {
         *err = EMB_RB_ERR_LOCK;
      }
      return(0);
   }
   // Check if there is enough used space
   uint32_t used = _internal_emb_rb_used_space(rb);
   if (len > used)
   {
      len = used;
   }
   // 1. If len is 1, just copy the byte and index since we will have a modulo anyway
   // 2. if len is greater than 1, handle the wrap around
   // 3. otherwise len is 0 and don't do anything
   if (len == 1)
   {
      *bytes = rb->bP[rb->tail % rb->size];
      rb->tail++;
   }
   else if (len > 1)
   {
      // Optimize for speed, handle an index wrap around
      uint32_t cur_index     = rb->tail % rb->size;
      uint32_t len_till_wrap = rb->size - cur_index;
      uint32_t n             = len;
      if (n > len_till_wrap)
      {
         memcpy(bytes, rb->bP + cur_index, len_till_wrap);
         bytes    += len_till_wrap;
         n        -= len_till_wrap;
         cur_index = 0;
      }
      memcpy(bytes, rb->bP + cur_index, n);
      rb->tail += len;
   }
   // Unlock the buffer
   pthread_mutex_unlock(&rb->lock);

   if (err)
   {
      if (len > 0)
      {
         *err = EMB_RB_ERR_OK;
      }
      else
      {
         *err = EMB_RB_ERR_BUFFER_EMPTY;
      }
   }
   return(len);
}

// Peek len number of bytes at position, from the ring buffer without dequeuing
uint32_t emb_rb_peek(emb_rb_t *rb, uint32_t position, uint8_t *bytes, uint32_t len)
{
   // Null check
   if (!rb || !bytes || !len)
   {
      return(0);
   }
   // Lock the buffer
   pthread_mutex_lock(&rb->lock);
   // Illegal position check
   if (position > rb->size || (position > _internal_emb_rb_used_space(rb)))
   {
      // Unlock the buffer
      pthread_mutex_unlock(&rb->lock);
      return(0);
   }
   // Illegal length + position check
   if (position + len > _internal_emb_rb_used_space(rb))
   {
      len = _internal_emb_rb_used_space(rb) - position;
   }
   // 1. If len is 1, just copy the byte and index since we will have a modulo anyway
   // 2. if len is greater than 1, handle the wrap around
   // 3. otherwise len is 0 and don't do anything
   uint32_t local_tail = rb->tail + position;
   if (len == 1)
   {
      *bytes = rb->bP[local_tail % rb->size];
   }
   else if (len > 1)
   {
      // Optimize for speed, handle an index wrap around
      uint32_t cur_index     = local_tail % rb->size;
      uint32_t len_till_wrap = rb->size - cur_index;
      uint32_t n             = len;
      if (n > len_till_wrap)
      {
         memcpy(bytes, rb->bP + cur_index, len_till_wrap);
         bytes    += len_till_wrap;
         n        -= len_till_wrap;
         cur_index = 0;
      }
      memcpy(bytes, rb->bP + cur_index, n);
   }
   // Unlock the buffer
   pthread_mutex_unlock(&rb->lock);
   return(len);
}

// Insert len number of bytes into the ring buffer at position
uint32_t emb_rb_insert(emb_rb_t *rb, uint32_t position, const uint8_t *bytes, uint32_t len, uint8_t all_or_nothing)
{
   // Null check
   if (!rb || !bytes || !len)
   {
      return(0);
   }
   // Lock the buffer
   pthread_mutex_lock(&rb->lock);
   // Illegal position check
   if (position > rb->size || (position > _internal_emb_rb_used_space(rb)))
   {
      // Unlock the buffer
      pthread_mutex_unlock(&rb->lock);
      return(0);
   }
   // Check if there is enough free space
   uint32_t space = _internal_emb_rb_free_space(rb);
   if (len > space)
   {
      if (all_or_nothing)
      {
         // Unlock the buffer
         pthread_mutex_unlock(&rb->lock);
         return(0);
      }
      else
      {
         len = space;
      }
   }
   // Calculate the real position in the buffer
   uint32_t pos_index = (rb->tail + position) % rb->size;
   uint32_t end_index = (rb->tail + _internal_emb_rb_used_space(rb)) % rb->size;

   // Calculate the displacement considering the circular buffer.
   uint32_t displacement = (pos_index > end_index) ? (rb->size - pos_index + end_index) : (end_index - pos_index);

   // Copy the data from position to head to the right by len bytes.
   memmove(rb->bP + (pos_index + len) % rb->size, rb->bP + pos_index, displacement);

   // Back fill the original data at position
   uint32_t len_till_wrap = rb->size - pos_index;
   uint32_t n             = len;
   if (n > len_till_wrap)
   {
      memcpy(rb->bP + pos_index, bytes, len_till_wrap);
      bytes    += len_till_wrap;
      n        -= len_till_wrap;
      pos_index = 0;
   }
   memcpy(rb->bP + pos_index, bytes, n);
   rb->head += len;
   // Unlock the buffer
   pthread_mutex_unlock(&rb->lock);
   return(len);
}

// Remove len number of bytes from the ring buffer at position
uint32_t emb_rb_remove(emb_rb_t *rb, uint32_t position, uint8_t *bytes, uint32_t len, uint8_t all_or_nothing)
{
   // Null check
   if (!rb || !len)
   {
      return(0);
   }
   // Lock the buffer
   pthread_mutex_lock(&rb->lock);
   // Illegal position check
   if (position > rb->size || position > _internal_emb_rb_used_space(rb))
   {
      // Unlock the buffer
      pthread_mutex_unlock(&rb->lock);
      return(0);
   }

   // Calculate the real position in the buffer
   uint32_t pos_index = (rb->tail + position) % rb->size;
   uint32_t end_index = rb->head % rb->size;

   // Calculate the displacement considering the circular buffer.
   uint32_t displacement = (end_index > pos_index) ? (end_index - pos_index) : (rb->size + end_index - pos_index);

   // Respect all or nothing
   if (_internal_emb_rb_used_space(rb) == 0)
   {
      // Unlock the buffer
      pthread_mutex_unlock(&rb->lock);
      return(0);
   }
   if (displacement < len)
   {
      if (all_or_nothing)
      {
         // Unlock the buffer
         pthread_mutex_unlock(&rb->lock);
         return(0);
      }
      else
      {
         len = displacement;
      }
   }

   // Copy the bytes to be removed, if requested.
   uint32_t len_till_wrap = rb->size - pos_index;
   uint32_t n             = len;
   if (bytes != NULL)
   {
      if (n > len_till_wrap)
      {
         memcpy(bytes, rb->bP + pos_index, len_till_wrap);
         bytes    += len_till_wrap;
         n        -= len_till_wrap;
         pos_index = 0;
      }
      memcpy(bytes, rb->bP + pos_index, n);
   }

   // Remove the data by shifting the rest of the data left.
   n         = len;
   pos_index = (rb->tail + position) % rb->size;
   if (n > len_till_wrap)
   {
      memmove(rb->bP + pos_index, rb->bP + pos_index + len_till_wrap, len_till_wrap);
      n        -= len_till_wrap;
      pos_index = 0;
   }
   memmove(rb->bP, rb->bP + n, n);

   // Adjust the head of the buffer.
   rb->head -= len;

   // Unlock the buffer
   pthread_mutex_unlock(&rb->lock);
   return(len);
}

// Effectively empty the buffer by setting the tail vaule to the head value
int emb_rb_flush(emb_rb_t *rb)
{
   // Null check
   if (!rb)
   {
      return(0);
   }
   // Lock the buffer
   pthread_mutex_lock(&rb->lock);
   rb->tail = rb->head;
   // Unlock the buffer
   pthread_mutex_unlock(&rb->lock);
   return(-1);
}

// Do a partial flush of len bytes from the tail
uint32_t emb_rb_flush_partial(emb_rb_t *rb, uint32_t len)
{
   // Null check
   if (!rb)
   {
      return(0);
   }
   // Lock the buffer
   pthread_mutex_lock(&rb->lock);
   // Check if there is enough used space
   uint32_t used = _internal_emb_rb_used_space(rb);
   if (len > used)
   {
      len = used;
   }
   rb->tail += len;
   // Unlock the buffer
   pthread_mutex_unlock(&rb->lock);
   return(len);
}

// Get the number of free bytes in the ring buffer
uint32_t emb_rb_free_space(emb_rb_t *rb)
{
   // Null check
   if (!rb)
   {
      return(0);
   }
   // Lock the buffer
   pthread_mutex_lock(&rb->lock);
   uint32_t ret = (rb->size - _internal_emb_rb_used_space(rb));
   // Unlock the buffer
   pthread_mutex_unlock(&rb->lock);
   return(ret);
}

// Get the number of used bytes in the ring buffer
uint32_t emb_rb_used_space(emb_rb_t *rb)
{
   // Null check
   if (!rb)
   {
      return(0);
   }
   // Lock the buffer
   pthread_mutex_lock(&rb->lock);
   // Handle the integer wrap around
   uint32_t ret = 0;
   if (rb->head < rb->tail)
   {
      ret = (rb->size - (rb->tail - rb->head));
   }
   else
   {
      ret = (rb->head - rb->tail);
   }
   // Unlock the buffer
   pthread_mutex_unlock(&rb->lock);
   return(ret);
}

// Get the version of the library
const char *emb_rb_get_ver()
{
   // Get the major, minor, and rev numbers
   int major = RB_VERSION_MAJOR;
   int minor = RB_VERSION_MINOR;
   int rev   = RB_VERSION_REV;

   // Build the string, without using sprintf
   static char ver[16];

   ver[0]  = 'v';
   ver[1]  = (major / 100) + '0';
   ver[2]  = ((major % 100) / 10) + '0';
   ver[3]  = (major % 10) + '0';
   ver[4]  = '.';
   ver[5]  = (minor / 100) + '0';
   ver[6]  = ((minor % 100) / 10) + '0';
   ver[7]  = (minor % 10) + '0';
   ver[8]  = '.';
   ver[9]  = (rev / 100) + '0';
   ver[10] = ((rev % 100) / 10) + '0';
   ver[11] = (rev % 10) + '0';
   ver[12] = '\0';

   // Return the string
   return(ver);
}

void emb_rb_destroy(emb_rb_t *rb)
{
   // Null check
   if (!rb)
   {
      return;
   }
   // Destroy the mutex
   pthread_mutex_destroy(&rb->lock);
}
