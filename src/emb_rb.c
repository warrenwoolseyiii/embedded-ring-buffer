#include "emb_rb.h"
#include "rb_version.h"
#include <string.h>

// Initialize the ring buffer
int emb_rb_init(emb_rb_t *rb, uint8_t *bP, uint32_t size)
{
   // Null check
   if (!rb || !bP || !size)
   {
      return(0);
   }
   rb->bP   = bP;
   rb->size = size;
   rb->head = 0;
   rb->tail = 0;
   return(-1);
}

// Get the total size of the ring buffer
uint32_t emb_rb_size(emb_rb_t *rb)
{
   // Null check
   if (!rb)
   {
      return(0);
   }
   return(rb->size);
}

// Queue len nubmer of bytes into the ring buffer, making a deep copy
uint32_t emb_rb_queue(emb_rb_t *rb, const uint8_t *bytes, uint32_t len)
{
   // Null check
   if (!rb || !bytes || !len)
   {
      return(0);
   }
   // Check if there is enough free space
   uint32_t space = emb_rb_free_space(rb);
   if (len > space)
   {
      len = space;
   }
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
   return(len);
}

// Dequeue len number of bytes from the ring buffer
uint32_t emb_rb_dequeue(emb_rb_t *rb, uint8_t *bytes, uint32_t len)
{
   // Null check
   if (!rb || !bytes || !len)
   {
      return(0);
   }
   // Check if there is enough data
   if (len > emb_rb_used_space(rb))
   {
      len = emb_rb_used_space(rb);
   }
   // Copy bytes
   for (uint32_t i = 0; i < len; i++)
   {
      bytes[i] = rb->bP[rb->tail % rb->size];
      rb->tail++;
   }
   return(len);
}

// Peek len number of bytes from the ring buffer without dequeuing
uint32_t emb_rb_peek(emb_rb_t *rb, uint8_t *bytes, uint32_t len)
{
   // Null check
   if (!rb || !bytes || !len)
   {
      return(0);
   }
   // Check if there is enough data
   if (len > emb_rb_used_space(rb))
   {
      len = emb_rb_used_space(rb);
   }
   // Copy bytes
   uint32_t local_tail = rb->tail;
   for (uint32_t i = 0; i < len; i++)
   {
      bytes[i] = rb->bP[local_tail % rb->size];
      local_tail++;
   }
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
   rb->tail = rb->head;
   return(-1);
}

// Get the number of free bytes in the ring buffer
uint32_t emb_rb_free_space(emb_rb_t *rb)
{
   return(rb->size - emb_rb_used_space(rb));
}

// Get the number of used bytes in the ring buffer
uint32_t emb_rb_used_space(emb_rb_t *rb)
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
