#include <gtest/gtest.h>
#include <string.h>
#include <thread>
#include "../src/emb_rb.h"

class RBTesting : public ::testing::Test
{
public:
   RBTesting()
   {
      // initialization code here
   }

   void SetUp()
   {
   }

   void TearDown()
   {
   }

   ~RBTesting()
   {
      // cleanup any pending stuff, but no exceptions allowed
   }

   // put in any custom data members that you need
};

// Ensure that the ring buffer null checks all pass properly on initialization
TEST_F(RBTesting, Test_Null_Init)
{
   emb_rb_t rb;
   uint8_t  buf[10];
   uint32_t size = 10;

   ASSERT_EQ(emb_rb_init(0, 0, 0), EMB_RB_ERR_ILLEGAL_ARGS);
   ASSERT_EQ(emb_rb_init(&rb, 0, 0), EMB_RB_ERR_ILLEGAL_ARGS);
   ASSERT_EQ(emb_rb_init(&rb, buf, 0), EMB_RB_ERR_ILLEGAL_ARGS);
   ASSERT_EQ(emb_rb_init(&rb, 0, size), EMB_RB_ERR_ILLEGAL_ARGS);
   //emb_rb_destroy(&rb);
}

// Ensure that the ring buffer initializes properly
TEST_F(RBTesting, Test_Init)
{
   emb_rb_t rb;
   uint8_t  buf[10];
   uint32_t size = 10;

   ASSERT_EQ(emb_rb_init(&rb, buf, size), EMB_RB_ERR_OK);
   emb_rb_destroy(&rb);
}

// Ensure that the ring buffer returns the correct size
TEST_F(RBTesting, Test_Size)
{
   emb_rb_t rb;
   uint8_t  buf[10];
   uint32_t size = 10;

   ASSERT_EQ(emb_rb_init(&rb, buf, size), EMB_RB_ERR_OK);
   ASSERT_EQ(emb_rb_size(&rb, NULL), size);
   emb_rb_destroy(&rb);
}

// Ensure that the ring buffer returns the correct free space
TEST_F(RBTesting, Test_Free_Space)
{
   emb_rb_t rb;
   uint8_t  buf[10];
   uint32_t size = 10;

   ASSERT_EQ(emb_rb_init(&rb, buf, size), EMB_RB_ERR_OK);
   ASSERT_EQ(emb_rb_free_space(&rb), size);
   emb_rb_destroy(&rb);
}

// Ensure that the ring buffer returns the correct used space
TEST_F(RBTesting, Test_Used_Space)
{
   emb_rb_t rb;
   uint8_t  buf[10];
   uint32_t size = 10;

   ASSERT_EQ(emb_rb_init(&rb, buf, size), EMB_RB_ERR_OK);
   ASSERT_EQ(emb_rb_used_space(&rb), 0);
   emb_rb_destroy(&rb);
}

// Ensure that we can queue and dequeue data
TEST_F(RBTesting, Test_Queue_Dequeue)
{
   emb_rb_t rb;
   uint8_t  buf[10];
   uint32_t size = 10;
   uint8_t  data = 0x55;
   int      err;

   ASSERT_EQ(emb_rb_init(&rb, buf, size), EMB_RB_ERR_OK);
   ASSERT_EQ(emb_rb_queue(&rb, &data, 1, &err), 1);
   ASSERT_EQ(err, EMB_RB_ERR_OK);
   ASSERT_EQ(emb_rb_used_space(&rb), 1);
   ASSERT_EQ(emb_rb_free_space(&rb), size - 1);
   uint8_t rd;
   ASSERT_EQ(emb_rb_dequeue(&rb, &rd, 1, &err), 1);
   ASSERT_EQ(err, EMB_RB_ERR_OK);
   ASSERT_EQ(rd, data);
   ASSERT_EQ(emb_rb_used_space(&rb), 0);
   ASSERT_EQ(emb_rb_free_space(&rb), size);
   emb_rb_destroy(&rb);
}

// Ensure that wrap around works properly
TEST_F(RBTesting, Test_Queue_Overflow)
{
   emb_rb_t rb;
   uint8_t  buf[10];
   uint32_t size = 10;
   uint8_t  data[10];
   int      err;

   ASSERT_EQ(emb_rb_init(&rb, buf, size), EMB_RB_ERR_OK);
   ASSERT_EQ(emb_rb_queue(&rb, data, 10, &err), 10);
   ASSERT_EQ(err, EMB_RB_ERR_OK);
   ASSERT_EQ(emb_rb_used_space(&rb), 10);
   ASSERT_EQ(emb_rb_free_space(&rb), 0);
   ASSERT_EQ(emb_rb_queue(&rb, data, 1, &err), 0);
   ASSERT_EQ(err, EMB_RB_ERR_BUFFER_FULL);
   ASSERT_EQ(emb_rb_used_space(&rb), 10);
   ASSERT_EQ(emb_rb_free_space(&rb), 0);
   uint8_t rd[10];
   ASSERT_EQ(emb_rb_dequeue(&rb, rd, 10, &err), 10);
   ASSERT_EQ(err, EMB_RB_ERR_OK);
   ASSERT_EQ(memcmp(data, rd, 10), 0);
   ASSERT_EQ(emb_rb_used_space(&rb), 0);
   ASSERT_EQ(emb_rb_free_space(&rb), size);
   emb_rb_destroy(&rb);
}

// Ensure that wrap around works properly at the end of the buffer
TEST_F(RBTesting, Test_Queue_Overflow_End)
{
   emb_rb_t rb;
   uint8_t  buf[10];
   uint32_t size = 10;
   uint8_t  data[10];
   int      err;

   ASSERT_EQ(emb_rb_init(&rb, buf, size), EMB_RB_ERR_OK);
   ASSERT_EQ(emb_rb_queue(&rb, data, 9, &err), 9);
   ASSERT_EQ(err, EMB_RB_ERR_OK);
   ASSERT_EQ(emb_rb_used_space(&rb), 9);
   ASSERT_EQ(emb_rb_free_space(&rb), 1);
   uint8_t rd[10];
   ASSERT_EQ(emb_rb_dequeue(&rb, rd, 9, &err), 9);
   ASSERT_EQ(err, EMB_RB_ERR_OK);
   ASSERT_EQ(memcmp(data, rd, 9), 0);
   ASSERT_EQ(emb_rb_used_space(&rb), 0);
   ASSERT_EQ(emb_rb_free_space(&rb), size);
   ASSERT_EQ(emb_rb_queue(&rb, data, 10, &err), 10);
   ASSERT_EQ(err, EMB_RB_ERR_OK);
   ASSERT_EQ(emb_rb_used_space(&rb), 10);
   ASSERT_EQ(emb_rb_free_space(&rb), 0);
   ASSERT_EQ(emb_rb_dequeue(&rb, rd, 10, &err), 10);
   ASSERT_EQ(err, EMB_RB_ERR_OK);
   ASSERT_EQ(memcmp(data, rd, 10), 0);
   ASSERT_EQ(emb_rb_used_space(&rb), 0);
   ASSERT_EQ(emb_rb_free_space(&rb), size);
   emb_rb_destroy(&rb);
}

// Ensure that underflow for reading works properly
TEST_F(RBTesting, Test_Dequeue_Underflow)
{
   emb_rb_t rb;
   uint8_t  buf[10];
   uint32_t size = 10;
   uint8_t  data[10];
   int      err;

   ASSERT_EQ(emb_rb_init(&rb, buf, size), EMB_RB_ERR_OK);
   ASSERT_EQ(emb_rb_queue(&rb, data, 10, &err), 10);
   ASSERT_EQ(err, EMB_RB_ERR_OK);
   ASSERT_EQ(emb_rb_used_space(&rb), 10);
   ASSERT_EQ(emb_rb_free_space(&rb), 0);
   uint8_t rd[10];
   ASSERT_EQ(emb_rb_dequeue(&rb, rd, 10, &err), 10);
   ASSERT_EQ(err, EMB_RB_ERR_OK);
   ASSERT_EQ(memcmp(data, rd, 10), 0);
   ASSERT_EQ(emb_rb_used_space(&rb), 0);
   ASSERT_EQ(emb_rb_free_space(&rb), size);
   ASSERT_EQ(emb_rb_dequeue(&rb, rd, 1, &err), 0);
   ASSERT_EQ(err, EMB_RB_ERR_BUFFER_EMPTY);
   ASSERT_EQ(emb_rb_used_space(&rb), 0);
   ASSERT_EQ(emb_rb_free_space(&rb), size);
   emb_rb_destroy(&rb);
}

// Ensure that flushing works properly
TEST_F(RBTesting, Test_Flush)
{
   emb_rb_t rb;
   uint8_t  buf[10];
   uint32_t size = 10;
   uint8_t  data[10];

   ASSERT_EQ(emb_rb_init(&rb, buf, size), EMB_RB_ERR_OK);
   ASSERT_EQ(emb_rb_queue(&rb, data, 10, NULL), 10);
   ASSERT_EQ(emb_rb_used_space(&rb), 10);
   ASSERT_EQ(emb_rb_free_space(&rb), 0);
   emb_rb_flush(&rb);
   ASSERT_EQ(emb_rb_used_space(&rb), 0);
   ASSERT_EQ(emb_rb_free_space(&rb), size);
   emb_rb_flush(&rb);
   ASSERT_EQ(emb_rb_used_space(&rb), 0);
   ASSERT_EQ(emb_rb_free_space(&rb), size);
   emb_rb_destroy(&rb);
}

// Ensure that peak works properly
TEST_F(RBTesting, Test_Peek)
{
   emb_rb_t rb;
   uint8_t  buf[10];
   uint32_t size = 10;
   uint8_t  data[10];

   ASSERT_EQ(emb_rb_init(&rb, buf, size), EMB_RB_ERR_OK);
   ASSERT_EQ(emb_rb_queue(&rb, data, 10, NULL), 10);
   ASSERT_EQ(emb_rb_used_space(&rb), 10);
   ASSERT_EQ(emb_rb_free_space(&rb), 0);
   uint8_t rd[10];
   ASSERT_EQ(emb_rb_peek(&rb, 0, rd, 10), 10);
   ASSERT_EQ(memcmp(data, rd, 10), 0);
   ASSERT_EQ(emb_rb_used_space(&rb), 10);
   ASSERT_EQ(emb_rb_free_space(&rb), 0);
   ASSERT_EQ(emb_rb_peek(&rb, 0, rd, 1), 1);
   ASSERT_EQ(rd[0], data[0]);
   ASSERT_EQ(emb_rb_used_space(&rb), 10);
   ASSERT_EQ(emb_rb_free_space(&rb), 0);
   emb_rb_destroy(&rb);
}

// Ensure that we can't underflow peak
TEST_F(RBTesting, Test_Peak_Underflow)
{
   emb_rb_t rb;
   uint8_t  buf[10];
   uint32_t size = 10;
   uint8_t  data[10];

   ASSERT_EQ(emb_rb_init(&rb, buf, size), EMB_RB_ERR_OK);
   ASSERT_EQ(emb_rb_queue(&rb, data, 10, NULL), 10);
   ASSERT_EQ(emb_rb_used_space(&rb), 10);
   ASSERT_EQ(emb_rb_free_space(&rb), 0);
   uint8_t rd[10];
   ASSERT_EQ(emb_rb_peek(&rb, 0, rd, 10), 10);
   ASSERT_EQ(memcmp(data, rd, 10), 0);
   ASSERT_EQ(emb_rb_used_space(&rb), 10);
   ASSERT_EQ(emb_rb_free_space(&rb), 0);
   ASSERT_EQ(emb_rb_peek(&rb, 0, rd, 11), 10);
   ASSERT_EQ(memcmp(data, rd, 10), 0);
   ASSERT_EQ(emb_rb_used_space(&rb), 10);
   ASSERT_EQ(emb_rb_free_space(&rb), 0);
   emb_rb_destroy(&rb);
}

// Ensure that we can insert data in an empty buffer
TEST_F(RBTesting, Test_Insert_Empty)
{
   emb_rb_t rb;
   uint8_t  buf[10];
   uint32_t size = 10;
   uint8_t  data[10];

   ASSERT_EQ(emb_rb_init(&rb, buf, size), EMB_RB_ERR_OK);
   ASSERT_EQ(emb_rb_insert(&rb, 0, data, 10, 1), 10);
   ASSERT_EQ(emb_rb_used_space(&rb), 10);
   ASSERT_EQ(emb_rb_free_space(&rb), 0);
   uint8_t rd[10];
   ASSERT_EQ(emb_rb_dequeue(&rb, rd, 10, NULL), 10);
   ASSERT_EQ(memcmp(data, rd, 10), 0);
   ASSERT_EQ(emb_rb_used_space(&rb), 0);
   ASSERT_EQ(emb_rb_free_space(&rb), size);
   emb_rb_destroy(&rb);
}

// Ensure that we can insert data inta a buffer that has data in it
TEST_F(RBTesting, Test_Insert)
{
   emb_rb_t rb;
   uint8_t  buf[20];
   uint32_t size = 20;
   uint8_t  data[10];

   ASSERT_EQ(emb_rb_init(&rb, buf, size), EMB_RB_ERR_OK);
   ASSERT_EQ(emb_rb_queue(&rb, data, 10, NULL), 10);
   ASSERT_EQ(emb_rb_used_space(&rb), 10);
   ASSERT_EQ(emb_rb_free_space(&rb), 10);

   uint8_t insert[5] = { 0x01, 0x02, 0x03, 0x04, 0x05 };
   ASSERT_EQ(emb_rb_insert(&rb, emb_rb_used_space(&rb), insert, 5, 1), 5);
   ASSERT_EQ(emb_rb_used_space(&rb), 15);
   ASSERT_EQ(emb_rb_free_space(&rb), 5);

   uint8_t rd[15];
   ASSERT_EQ(emb_rb_dequeue(&rb, rd, 15, NULL), 15);
   ASSERT_EQ(memcmp(data, rd, 10), 0);
   ASSERT_EQ(memcmp(insert, rd + 10, 5), 0);
   ASSERT_EQ(emb_rb_used_space(&rb), 0);
   ASSERT_EQ(emb_rb_free_space(&rb), size);
   emb_rb_destroy(&rb);
}

// Ensure that we can insert data into a buffer and handle a wrap around
TEST_F(RBTesting, Test_Insert_Wrap_Around)
{
   emb_rb_t rb;
   uint8_t  buf[20];
   uint32_t size = 20;
   uint8_t  data[10];

   ASSERT_EQ(emb_rb_init(&rb, buf, size), EMB_RB_ERR_OK);
   ASSERT_EQ(emb_rb_queue(&rb, data, 10, NULL), 10);
   ASSERT_EQ(emb_rb_used_space(&rb), 10);
   ASSERT_EQ(emb_rb_free_space(&rb), 10);
   ASSERT_EQ(emb_rb_dequeue(&rb, data, 10, NULL), 10);

   uint8_t insert[15] = { 0x01, 0x02, 0x03, 0x04, 0x05,
                          0x06, 0x07, 0x08, 0x09, 0x0A,
                          0x0B, 0x0C, 0x0D, 0x0E, 0x0F };
   ASSERT_EQ(emb_rb_insert(&rb, emb_rb_used_space(&rb), insert, 15, 1), 15);
   ASSERT_EQ(emb_rb_used_space(&rb), 15);
   ASSERT_EQ(emb_rb_free_space(&rb), 5);

   uint8_t rd[20];
   ASSERT_EQ(emb_rb_dequeue(&rb, rd, 20, NULL), 15);
   ASSERT_EQ(memcmp(insert, rd, 15), 0);
   ASSERT_EQ(emb_rb_used_space(&rb), 0);
   ASSERT_EQ(emb_rb_free_space(&rb), size);
   emb_rb_destroy(&rb);
}

// Ensure that insert will fail on an all or nothing, accept an all or nothing, and fail null checks
TEST_F(RBTesting, Test_Insert_Failure_Path)
{
   emb_rb_t rb;
   uint8_t  buf[20];
   uint32_t size = 20;
   uint8_t  data1[10];
   uint8_t  data2[15];

   ASSERT_EQ(emb_rb_init(&rb, buf, size), EMB_RB_ERR_OK);
   ASSERT_EQ(emb_rb_insert(&rb, 0, 0, 10, 1), 0);      // Null data
   ASSERT_EQ(emb_rb_insert(&rb, 0, data1, 0, 1), 0);   // Zero length
   ASSERT_EQ(emb_rb_insert(&rb, 11, data1, 10, 1), 0); // Too big

   ASSERT_EQ(emb_rb_queue(&rb, data1, 10, NULL), 10);
   ASSERT_EQ(emb_rb_queue(&rb, data2, 5, NULL), 5);
   ASSERT_EQ(emb_rb_insert(&rb, 0, data2, 6, 1), 0);      // Too big
   ASSERT_EQ(emb_rb_insert(&rb, 16, data2, 1, 1), 0);     // Too big of an offset
   ASSERT_EQ(emb_rb_insert(&rb, 15, &data2[5], 6, 0), 5); // Allow it not to be all or nothing
   uint8_t rd[20];
   ASSERT_EQ(emb_rb_dequeue(&rb, rd, 20, NULL), 20);
   ASSERT_EQ(memcmp(data1, rd, 10), 0);
   ASSERT_EQ(memcmp(data2, &rd[10], 10), 0);
   emb_rb_destroy(&rb);
}

// Test single queue just for completeness
TEST_F(RBTesting, Test_Single_Queue)
{
   emb_rb_t rb;
   uint8_t  buf[20];
   uint32_t size = 20;
   int      err;

   // Fill the buffer
   ASSERT_EQ(emb_rb_init(&rb, buf, size), EMB_RB_ERR_OK);
   for (int i = 0; i < size; i++)
   {
      ASSERT_EQ(emb_rb_queue_single(&rb, (uint8_t)i, &err), 1);
      ASSERT_EQ(err, EMB_RB_ERR_OK);
   }
   ASSERT_EQ(emb_rb_used_space(&rb), size);
   ASSERT_EQ(emb_rb_free_space(&rb), 0);

   // Try to overflow
   ASSERT_EQ(emb_rb_queue_single(&rb, 0, &err), 0);
   ASSERT_EQ(err, EMB_RB_ERR_BUFFER_FULL);

   uint8_t rd[20];
   ASSERT_EQ(emb_rb_dequeue(&rb, rd, 20, NULL), 20);
   for (int i = 0; i < size; i++)
   {
      ASSERT_EQ(rd[i], (uint8_t)i);
   }
   ASSERT_EQ(emb_rb_used_space(&rb), 0);
   ASSERT_EQ(emb_rb_free_space(&rb), size);
   emb_rb_destroy(&rb);
}

// Comprehensive test for insert
TEST_F(RBTesting, Test_Insert_Comprehensive)
{
   emb_rb_t rb;
   uint8_t  buf[512];
   uint32_t size       = 512;
   uint8_t  pattern[8] = { 0x01, 0x02, 0x03, 0x04,
                           0x05,  0x06, 0x07, 0x08 };

   // Fill the buffer all the way up
   ASSERT_EQ(emb_rb_init(&rb, buf, size), EMB_RB_ERR_OK);
   for (int i = 0; i < size; i += 8)
   {
      ASSERT_EQ(emb_rb_queue(&rb, pattern, 8, NULL), 8);
   }

   // Empty the buffer all the way out, validate the reads
   uint8_t rd[8];
   for (int i = 0; i < size; i += 8)
   {
      ASSERT_EQ(emb_rb_dequeue(&rb, rd, 8, NULL), 8);
      ASSERT_EQ(memcmp(pattern, rd, 8), 0);
   }

   // Fill the buffer half way up
   for (int i = 0; i < size / 2; i += 8)
   {
      ASSERT_EQ(emb_rb_queue(&rb, pattern, 8, NULL), 8);
   }

   // Insert at the base, fill it all the way
   for (int i = 0; i < size / 2; i += 8)
   {
      ASSERT_EQ(emb_rb_insert(&rb, i, pattern, 8, 1), 8);
   }

   // Read the entire buffer, ensure it's correct
   for (int i = 0; i < size; i += 8)
   {
      ASSERT_EQ(emb_rb_dequeue(&rb, rd, 8, NULL), 8);
      ASSERT_EQ(memcmp(pattern, rd, 8), 0);
   }

   // Now try to insert with an illegal offset
   ASSERT_EQ(emb_rb_insert(&rb, 1, pattern, 8, 1), 0);

   // Now fill half the buffer
   for (int i = 0; i < size / 2; i += 8)
   {
      ASSERT_EQ(emb_rb_queue(&rb, pattern, 8, NULL), 8);
   }

   // Now try and insert with a large offest of 128 bytes, fill the buffer
   for (int i = 0; i < size / 2; i += 8)
   {
      ASSERT_EQ(emb_rb_insert(&rb, i + 128, pattern, 8, 1), 8);
   }

   // Now read it all back and verify
   for (int i = 0; i < size; i += 8)
   {
      ASSERT_EQ(emb_rb_dequeue(&rb, rd, 8, NULL), 8);
      ASSERT_EQ(memcmp(pattern, rd, 8), 0);
   }
   emb_rb_destroy(&rb);
}

// Test peek with an offset
TEST_F(RBTesting, Test_Peek_Offset)
{
   emb_rb_t rb;
   uint8_t  buf[20];
   uint32_t size = 20;
   uint8_t  data1[10];
   uint8_t  data2[15];

   ASSERT_EQ(emb_rb_init(&rb, buf, size), EMB_RB_ERR_OK);
   ASSERT_EQ(emb_rb_queue(&rb, data1, 10, NULL), 10);
   ASSERT_EQ(emb_rb_queue(&rb, data2, 5, NULL), 5);

   uint8_t rd[20];
   ASSERT_EQ(emb_rb_peek(&rb, 0, rd, 20), 15);
   ASSERT_EQ(memcmp(data1, rd, 10), 0);
   ASSERT_EQ(memcmp(data2, &rd[10], 5), 0);
   ASSERT_EQ(emb_rb_peek(&rb, 10, rd, 20), 5);
   ASSERT_EQ(memcmp(data2, rd, 5), 0);
   emb_rb_destroy(&rb);
}

// Test partial flush
TEST_F(RBTesting, Test_Partial_Flush)
{
   emb_rb_t rb;
   uint8_t  buf[20];
   uint32_t size        = 20;
   uint8_t  pattern[10] = { 0x01, 0x02, 0x03, 0x04,
                            0x05,  0x06, 0x07, 0x08,0x69, 0x69 };

   // Fill the buffer
   ASSERT_EQ(emb_rb_init(&rb, buf, size), EMB_RB_ERR_OK);
   ASSERT_EQ(emb_rb_queue(&rb, pattern, 10, NULL), 10);
   ASSERT_EQ(emb_rb_queue(&rb, pattern, 10, NULL), 10);

   // Partial flush the first 15 bytes
   ASSERT_EQ(emb_rb_flush_partial(&rb, 15), 15);
   ASSERT_EQ(emb_rb_used_space(&rb), 5);
   ASSERT_EQ(emb_rb_free_space(&rb), 15);
   uint8_t rd[5];
   ASSERT_EQ(emb_rb_dequeue(&rb, rd, 5, NULL), 5);
   ASSERT_EQ(memcmp(&pattern[5], rd, 5), 0);
   emb_rb_destroy(&rb);
}

// Test remove comprehensive
TEST_F(RBTesting, Test_Remove_Comprehensive)
{
   emb_rb_t rb;
   uint8_t  buf[512];
   uint32_t size       = 512;
   uint8_t  pattern[8] = { 0x01, 0x02, 0x03, 0x04,
                           0x05,  0x06, 0x07, 0x08 };

   // Fill the buffer all the way up
   ASSERT_EQ(emb_rb_init(&rb, buf, size), EMB_RB_ERR_OK);
   for (int i = 0; i < size; i += 8)
   {
      ASSERT_EQ(emb_rb_queue(&rb, pattern, 8, NULL), 8);
   }

   // Remove the first 8 bytes
   uint8_t rd[8];
   ASSERT_EQ(emb_rb_remove(&rb, 0, rd, 8, 1), 8);
   ASSERT_EQ(emb_rb_used_space(&rb), 504);
   ASSERT_EQ(emb_rb_free_space(&rb), 8);
   ASSERT_EQ(memcmp(pattern, rd, 8), 0);

   // Remove everything after index 128 till the end. Validate the reads
   for (int i = 128; i < size - 8; i += 8)
   {
      ASSERT_EQ(emb_rb_remove(&rb, 128, rd, 8, 1), 8);
      ASSERT_EQ(memcmp(pattern, rd, 8), 0);
   }

   // Verify we have 128 bytes left
   ASSERT_EQ(emb_rb_used_space(&rb), 128);
   ASSERT_EQ(emb_rb_free_space(&rb), 384);

   // Remove the last 120 bytes using an offset of 8, no read
   ASSERT_EQ(emb_rb_remove(&rb, 8, NULL, 120, 1), 120);
   ASSERT_EQ(emb_rb_used_space(&rb), 8);
   ASSERT_EQ(emb_rb_free_space(&rb), 504);

   // Read out the last 8 bytes using dequeue
   ASSERT_EQ(emb_rb_dequeue(&rb, rd, 8, NULL), 8);
   ASSERT_EQ(memcmp(pattern, rd, 8), 0);

   // Verify we have 0 bytes left
   ASSERT_EQ(emb_rb_used_space(&rb), 0);

   // Try to underflow it
   ASSERT_EQ(emb_rb_remove(&rb, 0, rd, 8, 1), 0);
   ASSERT_EQ(emb_rb_remove(&rb, 0, rd, 8, 0), 0);

   // Fill the buffer all the way up
   for (int i = 0; i < size; i += 8)
   {
      ASSERT_EQ(emb_rb_queue(&rb, pattern, 8, NULL), 8);
   }

   // Try to remove more than we have, all or nothing
   uint8_t big_rd[512];
   ASSERT_EQ(emb_rb_remove(&rb, 0, big_rd, size + 1, 1), 0);

   // Try to remove more than we have, no all or nothing
   ASSERT_EQ(emb_rb_remove(&rb, 0, big_rd, size + 1, 0), 512);

   // Verify we have 0 bytes left
   ASSERT_EQ(emb_rb_used_space(&rb), 0);

   // Ensure the readout is correct
   for (int i = 0; i < size; i += 8)
   {
      ASSERT_EQ(memcmp(pattern, &big_rd[i], 8), 0);
   }
   emb_rb_destroy(&rb);
}

// Test getting the version
TEST_F(RBTesting, Test_Version)
{
   const char *s = emb_rb_get_ver();

   ASSERT_TRUE(s != NULL);
   printf("emb_rb_version: %s\n", s);
}

// Test concurrency
TEST_F(RBTesting, Test_Concurrency)
{
   emb_rb_t rb;
   uint8_t  buf[1000];
   uint32_t size = 1000;
   uint8_t  data = 0x55;
   int      err;

   ASSERT_EQ(emb_rb_init(&rb, buf, size), EMB_RB_ERR_OK);

   // Create a thread to queue data
   std::thread t1([&rb]() {
                  uint8_t t1_data = 0x01;
                  int t1_err;
                  for (int i = 0; i < 500; )
                  {
                     int rtn = emb_rb_queue(&rb, &t1_data, 1, &t1_err);
                     if (rtn == 1)
                     {
                        i++;
                        ASSERT_EQ(t1_err, EMB_RB_ERR_OK);
                     }
                     else
                     {
                        ASSERT_EQ(t1_err, EMB_RB_ERR_LOCK);
                     }
                     std::this_thread::sleep_for(std::chrono::milliseconds(10));
                  }
      });

   // Create a thread to queue data
   std::thread t2([&rb]() {
                  uint8_t t2_data = 0x02;
                  int t2_err;
                  for (int i = 0; i < 500; )
                  {
                     int rtn = emb_rb_queue(&rb, &t2_data, 1, &t2_err);
                     if (rtn == 1)
                     {
                        i++;
                        ASSERT_EQ(t2_err, EMB_RB_ERR_OK);
                     }
                     else
                     {
                        ASSERT_EQ(t2_err, EMB_RB_ERR_LOCK);
                     }
                     std::this_thread::sleep_for(std::chrono::milliseconds(10));
                  }
      });

   // Start the threads
   t1.join();
   t2.join();

   // Keep checking the used space until it's 1000
   while (emb_rb_used_space(&rb) != 1000)
   {
      // Do nothing
   }

   ASSERT_EQ(emb_rb_used_space(&rb), 1000);
   emb_rb_destroy(&rb);
}

// Test concurrency operations
TEST_F(RBTesting, Test_ConcurrencyComprehensive)
{
   emb_rb_t rb;
   uint8_t  buf[1000];
   uint32_t size = 1000;
   uint8_t  data = 0x55;
   int      err;

   ASSERT_EQ(emb_rb_init(&rb, buf, size), EMB_RB_ERR_OK);

   // Create a thread to queue data
   std::thread t1([&rb]() {
                  int err;
                  uint8_t pattern[] = { 0x01, 0x02, 0x03, 0x04, 0x05 };
                  while (emb_rb_free_space(&rb) > 0)
                  {
                     int rtn = emb_rb_queue(&rb, pattern, 5, &err);
                     if (rtn == 5)
                     {
                        ASSERT_EQ(err, EMB_RB_ERR_OK);
                     }
                     else
                     {
                        ASSERT_EQ(err, EMB_RB_ERR_LOCK);
                     }
                     std::this_thread::sleep_for(std::chrono::milliseconds(10));
                  }
      });

   // Create a thread to peak the data
   std::thread t2([&rb]() {
                  int err;
                  uint8_t pattern[5];
                  for (int i = 0; i < 200; i++)
                  {
                     int rtn = emb_rb_peek(&rb, 0, pattern, 5);
                     if (rtn == 5)
                     {
                        ASSERT_EQ(pattern[0], 0x01);
                        ASSERT_EQ(pattern[1], 0x02);
                        ASSERT_EQ(pattern[2], 0x03);
                        ASSERT_EQ(pattern[3], 0x04);
                        ASSERT_EQ(pattern[4], 0x05);
                     }
                     std::this_thread::sleep_for(std::chrono::milliseconds(10));
                  }
      });

   // Create a thread to dequeue the data
   std::thread t3([&rb]() {
                  int err;
                  uint8_t pattern[5];
                  for (int i = 0; i < 200; i++)
                  {
                     int rtn = emb_rb_dequeue(&rb, pattern, 5, &err);
                     if (rtn == 5)
                     {
                        ASSERT_EQ(pattern[0], 0x01);
                        ASSERT_EQ(pattern[1], 0x02);
                        ASSERT_EQ(pattern[2], 0x03);
                        ASSERT_EQ(pattern[3], 0x04);
                        ASSERT_EQ(pattern[4], 0x05);
                     }
                     std::this_thread::sleep_for(std::chrono::milliseconds(10));
                  }
      });

   // Start the threads
   t2.join();
   t3.join();
   t1.join();

   // Keep checking the free space until it's 0
   while (emb_rb_free_space(&rb) != 0)
   {
      // Do nothing
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
   }

   ASSERT_EQ(emb_rb_free_space(&rb), 0);
}
