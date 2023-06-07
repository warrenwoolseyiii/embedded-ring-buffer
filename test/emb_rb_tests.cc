#include <gtest/gtest.h>
#include <string.h>
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

   ASSERT_FALSE(emb_rb_init(0, 0, 0));
   ASSERT_FALSE(emb_rb_init(&rb, 0, 0));
   ASSERT_FALSE(emb_rb_init(&rb, buf, 0));
   ASSERT_FALSE(emb_rb_init(&rb, 0, size));
}

// Ensure that the ring buffer initializes properly
TEST_F(RBTesting, Test_Init)
{
   emb_rb_t rb;
   uint8_t  buf[10];
   uint32_t size = 10;

   ASSERT_TRUE(emb_rb_init(&rb, buf, size));
}

// Ensure that the ring buffer returns the correct size
TEST_F(RBTesting, Test_Size)
{
   emb_rb_t rb;
   uint8_t  buf[10];
   uint32_t size = 10;

   ASSERT_TRUE(emb_rb_init(&rb, buf, size));
   ASSERT_EQ(emb_rb_size(&rb), size);
}

// Ensure that the ring buffer returns the correct free space
TEST_F(RBTesting, Test_Free_Space)
{
   emb_rb_t rb;
   uint8_t  buf[10];
   uint32_t size = 10;

   ASSERT_TRUE(emb_rb_init(&rb, buf, size));
   ASSERT_EQ(emb_rb_free_space(&rb), size);
}

// Ensure that the ring buffer returns the correct used space
TEST_F(RBTesting, Test_Used_Space)
{
   emb_rb_t rb;
   uint8_t  buf[10];
   uint32_t size = 10;

   ASSERT_TRUE(emb_rb_init(&rb, buf, size));
   ASSERT_EQ(emb_rb_used_space(&rb), 0);
}

// Ensure that we can queue and dequeue data
TEST_F(RBTesting, Test_Queue_Dequeue)
{
   emb_rb_t rb;
   uint8_t  buf[10];
   uint32_t size = 10;
   uint8_t  data = 0x55;

   ASSERT_TRUE(emb_rb_init(&rb, buf, size));
   ASSERT_EQ(emb_rb_queue(&rb, &data, 1), 1);
   ASSERT_EQ(emb_rb_used_space(&rb), 1);
   ASSERT_EQ(emb_rb_free_space(&rb), size - 1);
   uint8_t rd;
   ASSERT_EQ(emb_rb_dequeue(&rb, &rd, 1), 1);
   ASSERT_EQ(rd, data);
   ASSERT_EQ(emb_rb_used_space(&rb), 0);
   ASSERT_EQ(emb_rb_free_space(&rb), size);
}

// Ensure that wrap around works properly
TEST_F(RBTesting, Test_Queue_Overflow)
{
   emb_rb_t rb;
   uint8_t  buf[10];
   uint32_t size = 10;
   uint8_t  data[10];

   ASSERT_TRUE(emb_rb_init(&rb, buf, size));
   ASSERT_EQ(emb_rb_queue(&rb, data, 10), 10);
   ASSERT_EQ(emb_rb_used_space(&rb), 10);
   ASSERT_EQ(emb_rb_free_space(&rb), 0);
   ASSERT_EQ(emb_rb_queue(&rb, data, 1), 0);
   ASSERT_EQ(emb_rb_used_space(&rb), 10);
   ASSERT_EQ(emb_rb_free_space(&rb), 0);
   uint8_t rd[10];
   ASSERT_EQ(emb_rb_dequeue(&rb, rd, 10), 10);
   ASSERT_EQ(memcmp(data, rd, 10), 0);
   ASSERT_EQ(emb_rb_used_space(&rb), 0);
   ASSERT_EQ(emb_rb_free_space(&rb), size);
}

// Ensure that wrap around works properly at the end of the buffer
TEST_F(RBTesting, Test_Queue_Overflow_End)
{
   emb_rb_t rb;
   uint8_t  buf[10];
   uint32_t size = 10;
   uint8_t  data[10];

   ASSERT_TRUE(emb_rb_init(&rb, buf, size));
   ASSERT_EQ(emb_rb_queue(&rb, data, 9), 9);
   ASSERT_EQ(emb_rb_used_space(&rb), 9);
   ASSERT_EQ(emb_rb_free_space(&rb), 1);
   uint8_t rd[10];
   ASSERT_EQ(emb_rb_dequeue(&rb, rd, 9), 9);
   ASSERT_EQ(memcmp(data, rd, 9), 0);
   ASSERT_EQ(emb_rb_used_space(&rb), 0);
   ASSERT_EQ(emb_rb_free_space(&rb), size);
   ASSERT_EQ(emb_rb_queue(&rb, data, 10), 10);
   ASSERT_EQ(emb_rb_used_space(&rb), 10);
   ASSERT_EQ(emb_rb_free_space(&rb), 0);
   ASSERT_EQ(emb_rb_dequeue(&rb, rd, 10), 10);
   ASSERT_EQ(memcmp(data, rd, 10), 0);
   ASSERT_EQ(emb_rb_used_space(&rb), 0);
   ASSERT_EQ(emb_rb_free_space(&rb), size);
}

// Ensure that underflow for reading works properly
TEST_F(RBTesting, Test_Dequeue_Underflow)
{
   emb_rb_t rb;
   uint8_t  buf[10];
   uint32_t size = 10;
   uint8_t  data[10];

   ASSERT_TRUE(emb_rb_init(&rb, buf, size));
   ASSERT_EQ(emb_rb_queue(&rb, data, 10), 10);
   ASSERT_EQ(emb_rb_used_space(&rb), 10);
   ASSERT_EQ(emb_rb_free_space(&rb), 0);
   uint8_t rd[10];
   ASSERT_EQ(emb_rb_dequeue(&rb, rd, 10), 10);
   ASSERT_EQ(memcmp(data, rd, 10), 0);
   ASSERT_EQ(emb_rb_used_space(&rb), 0);
   ASSERT_EQ(emb_rb_free_space(&rb), size);
   ASSERT_EQ(emb_rb_dequeue(&rb, rd, 1), 0);
   ASSERT_EQ(emb_rb_used_space(&rb), 0);
   ASSERT_EQ(emb_rb_free_space(&rb), size);
}

// Ensure that flushing works properly
TEST_F(RBTesting, Test_Flush)
{
   emb_rb_t rb;
   uint8_t  buf[10];
   uint32_t size = 10;
   uint8_t  data[10];

   ASSERT_TRUE(emb_rb_init(&rb, buf, size));
   ASSERT_EQ(emb_rb_queue(&rb, data, 10), 10);
   ASSERT_EQ(emb_rb_used_space(&rb), 10);
   ASSERT_EQ(emb_rb_free_space(&rb), 0);
   emb_rb_flush(&rb);
   ASSERT_EQ(emb_rb_used_space(&rb), 0);
   ASSERT_EQ(emb_rb_free_space(&rb), size);
   emb_rb_flush(&rb);
   ASSERT_EQ(emb_rb_used_space(&rb), 0);
   ASSERT_EQ(emb_rb_free_space(&rb), size);
}

// Ensure that peak works properly
TEST_F(RBTesting, Test_Peek)
{
   emb_rb_t rb;
   uint8_t  buf[10];
   uint32_t size = 10;
   uint8_t  data[10];

   ASSERT_TRUE(emb_rb_init(&rb, buf, size));
   ASSERT_EQ(emb_rb_queue(&rb, data, 10), 10);
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
}

// Ensure that we can't underflow peak
TEST_F(RBTesting, Test_Peak_Underflow)
{
   emb_rb_t rb;
   uint8_t  buf[10];
   uint32_t size = 10;
   uint8_t  data[10];

   ASSERT_TRUE(emb_rb_init(&rb, buf, size));
   ASSERT_EQ(emb_rb_queue(&rb, data, 10), 10);
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
}

// Ensure that we can insert data in an empty buffer
TEST_F(RBTesting, Test_Insert_Empty)
{
   emb_rb_t rb;
   uint8_t  buf[10];
   uint32_t size = 10;
   uint8_t  data[10];

   ASSERT_TRUE(emb_rb_init(&rb, buf, size));
   ASSERT_EQ(emb_rb_insert(&rb, 0, data, 10, 1), 10);
   ASSERT_EQ(emb_rb_used_space(&rb), 10);
   ASSERT_EQ(emb_rb_free_space(&rb), 0);
   uint8_t rd[10];
   ASSERT_EQ(emb_rb_dequeue(&rb, rd, 10), 10);
   ASSERT_EQ(memcmp(data, rd, 10), 0);
   ASSERT_EQ(emb_rb_used_space(&rb), 0);
   ASSERT_EQ(emb_rb_free_space(&rb), size);
}

// Ensure that we can insert data inta a buffer that has data in it
TEST_F(RBTesting, Test_Insert)
{
   emb_rb_t rb;
   uint8_t  buf[20];
   uint32_t size = 20;
   uint8_t  data[10];

   ASSERT_TRUE(emb_rb_init(&rb, buf, size));
   ASSERT_EQ(emb_rb_queue(&rb, data, 10), 10);
   ASSERT_EQ(emb_rb_used_space(&rb), 10);
   ASSERT_EQ(emb_rb_free_space(&rb), 10);

   uint8_t insert[5] = { 0x01, 0x02, 0x03, 0x04, 0x05 };
   ASSERT_EQ(emb_rb_insert(&rb, emb_rb_used_space(&rb), insert, 5, 1), 5);
   ASSERT_EQ(emb_rb_used_space(&rb), 15);
   ASSERT_EQ(emb_rb_free_space(&rb), 5);

   uint8_t rd[15];
   ASSERT_EQ(emb_rb_dequeue(&rb, rd, 15), 15);
   ASSERT_EQ(memcmp(data, rd, 10), 0);
   ASSERT_EQ(memcmp(insert, rd + 10, 5), 0);
   ASSERT_EQ(emb_rb_used_space(&rb), 0);
   ASSERT_EQ(emb_rb_free_space(&rb), size);
}

// Ensure that we can insert data into a buffer and handle a wrap around
TEST_F(RBTesting, Test_Insert_Wrap_Around)
{
   emb_rb_t rb;
   uint8_t  buf[20];
   uint32_t size = 20;
   uint8_t  data[10];

   ASSERT_TRUE(emb_rb_init(&rb, buf, size));
   ASSERT_EQ(emb_rb_queue(&rb, data, 10), 10);
   ASSERT_EQ(emb_rb_used_space(&rb), 10);
   ASSERT_EQ(emb_rb_free_space(&rb), 10);
   ASSERT_EQ(emb_rb_dequeue(&rb, data, 10), 10);

   uint8_t insert[15] = { 0x01, 0x02, 0x03, 0x04, 0x05,
                          0x06, 0x07, 0x08, 0x09, 0x0A,
                          0x0B, 0x0C, 0x0D, 0x0E, 0x0F };
   ASSERT_EQ(emb_rb_insert(&rb, emb_rb_used_space(&rb), insert, 15, 1), 15);
   ASSERT_EQ(emb_rb_used_space(&rb), 15);
   ASSERT_EQ(emb_rb_free_space(&rb), 5);

   uint8_t rd[20];
   ASSERT_EQ(emb_rb_dequeue(&rb, rd, 20), 15);
   ASSERT_EQ(memcmp(insert, rd, 15), 0);
   ASSERT_EQ(emb_rb_used_space(&rb), 0);
   ASSERT_EQ(emb_rb_free_space(&rb), size);
}

// Ensure that insert will fail on an all or nothing, accept an all or nothing, and fail null checks
TEST_F(RBTesting, Test_Insert_Failure_Path)
{
   emb_rb_t rb;
   uint8_t  buf[20];
   uint32_t size = 20;
   uint8_t  data1[10];
   uint8_t  data2[15];

   ASSERT_TRUE(emb_rb_init(&rb, buf, size));
   ASSERT_EQ(emb_rb_insert(&rb, 0, 0, 10, 1), 0);      // Null data
   ASSERT_EQ(emb_rb_insert(&rb, 0, data1, 0, 1), 0);   // Zero length
   ASSERT_EQ(emb_rb_insert(&rb, 11, data1, 10, 1), 0); // Too big

   ASSERT_EQ(emb_rb_queue(&rb, data1, 10), 10);
   ASSERT_EQ(emb_rb_queue(&rb, data2, 5), 5);
   ASSERT_EQ(emb_rb_insert(&rb, 0, data2, 6, 1), 0);      // Too big
   ASSERT_EQ(emb_rb_insert(&rb, 16, data2, 1, 1), 0);     // Too big of an offset
   ASSERT_EQ(emb_rb_insert(&rb, 15, &data2[5], 6, 0), 5); // Allow it not to be all or nothing
   uint8_t rd[20];
   ASSERT_EQ(emb_rb_dequeue(&rb, rd, 20), 20);
   ASSERT_EQ(memcmp(data1, rd, 10), 0);
   ASSERT_EQ(memcmp(data2, &rd[10], 10), 0);
}

// Test single queue just for completeness
TEST_F(RBTesting, Test_Single_Queue)
{
   emb_rb_t rb;
   uint8_t  buf[20];
   uint32_t size = 20;

   // Fill the buffer
   ASSERT_TRUE(emb_rb_init(&rb, buf, size));
   for (int i = 0; i < size; i++)
   {
      ASSERT_EQ(emb_rb_queue_single(&rb, (uint8_t)i), 1);
   }
   ASSERT_EQ(emb_rb_used_space(&rb), size);
   ASSERT_EQ(emb_rb_free_space(&rb), 0);

   // Try to overflow
   ASSERT_EQ(emb_rb_queue_single(&rb, 0), 0);

   uint8_t rd[20];
   ASSERT_EQ(emb_rb_dequeue(&rb, rd, 20), 20);
   for (int i = 0; i < size; i++)
   {
      ASSERT_EQ(rd[i], (uint8_t)i);
   }
   ASSERT_EQ(emb_rb_used_space(&rb), 0);
   ASSERT_EQ(emb_rb_free_space(&rb), size);
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
   ASSERT_TRUE(emb_rb_init(&rb, buf, size));
   for (int i = 0; i < size; i += 8)
   {
      ASSERT_EQ(emb_rb_queue(&rb, pattern, 8), 8);
   }

   // Empty the buffer all the way out, validate the reads
   uint8_t rd[8];
   for (int i = 0; i < size; i += 8)
   {
      ASSERT_EQ(emb_rb_dequeue(&rb, rd, 8), 8);
      ASSERT_EQ(memcmp(pattern, rd, 8), 0);
   }

   // Fill the buffer half way up
   for (int i = 0; i < size / 2; i += 8)
   {
      ASSERT_EQ(emb_rb_queue(&rb, pattern, 8), 8);
   }

   // Insert at the base, fill it all the way
   for (int i = 0; i < size / 2; i += 8)
   {
      ASSERT_EQ(emb_rb_insert(&rb, i, pattern, 8, 1), 8);
   }

   // Read the entire buffer, ensure it's correct
   for (int i = 0; i < size; i += 8)
   {
      ASSERT_EQ(emb_rb_dequeue(&rb, rd, 8), 8);
      ASSERT_EQ(memcmp(pattern, rd, 8), 0);
   }

   // Now try to insert with an illegal offset
   ASSERT_EQ(emb_rb_insert(&rb, 1, pattern, 8, 1), 0);

   // Now fill half the buffer
   for (int i = 0; i < size / 2; i += 8)
   {
      ASSERT_EQ(emb_rb_queue(&rb, pattern, 8), 8);
   }

   // Now try and insert with a large offest of 128 bytes, fill the buffer
   for (int i = 0; i < size / 2; i += 8)
   {
      ASSERT_EQ(emb_rb_insert(&rb, i + 128, pattern, 8, 1), 8);
   }

   // Now read it all back and verify
   for (int i = 0; i < size; i += 8)
   {
      ASSERT_EQ(emb_rb_dequeue(&rb, rd, 8), 8);
      ASSERT_EQ(memcmp(pattern, rd, 8), 0);
   }
}

// Test peek with an offset
TEST_F(RBTesting, Test_Peek_Offset)
{
   emb_rb_t rb;
   uint8_t  buf[20];
   uint32_t size = 20;
   uint8_t  data1[10];
   uint8_t  data2[15];

   ASSERT_TRUE(emb_rb_init(&rb, buf, size));
   ASSERT_EQ(emb_rb_queue(&rb, data1, 10), 10);
   ASSERT_EQ(emb_rb_queue(&rb, data2, 5), 5);

   uint8_t rd[20];
   ASSERT_EQ(emb_rb_peek(&rb, 0, rd, 20), 15);
   ASSERT_EQ(memcmp(data1, rd, 10), 0);
   ASSERT_EQ(memcmp(data2, &rd[10], 5), 0);
   ASSERT_EQ(emb_rb_peek(&rb, 10, rd, 20), 5);
   ASSERT_EQ(memcmp(data2, rd, 5), 0);
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
   ASSERT_TRUE(emb_rb_init(&rb, buf, size));
   ASSERT_EQ(emb_rb_queue(&rb, pattern, 10), 10);
   ASSERT_EQ(emb_rb_queue(&rb, pattern, 10), 10);

   // Partial flush the first 15 bytes
   ASSERT_EQ(emb_rb_flush_partial(&rb, 15), 15);
   ASSERT_EQ(emb_rb_used_space(&rb), 5);
   ASSERT_EQ(emb_rb_free_space(&rb), 15);
   uint8_t rd[5];
   ASSERT_EQ(emb_rb_dequeue(&rb, rd, 5), 5);
   ASSERT_EQ(memcmp(&pattern[5], rd, 5), 0);
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
   ASSERT_TRUE(emb_rb_init(&rb, buf, size));
   for (int i = 0; i < size; i += 8)
   {
      ASSERT_EQ(emb_rb_queue(&rb, pattern, 8), 8);
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
   ASSERT_EQ(emb_rb_dequeue(&rb, rd, 8), 8);
   ASSERT_EQ(memcmp(pattern, rd, 8), 0);

   // Verify we have 0 bytes left
   ASSERT_EQ(emb_rb_used_space(&rb), 0);

   // Try to underflow it
   ASSERT_EQ(emb_rb_remove(&rb, 0, rd, 8, 1), 0);
   ASSERT_EQ(emb_rb_remove(&rb, 0, rd, 8, 0), 0);

   // Fill the buffer all the way up
   for (int i = 0; i < size; i += 8)
   {
      ASSERT_EQ(emb_rb_queue(&rb, pattern, 8), 8);
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
}

// Test getting the version
TEST_F(RBTesting, Test_Version)
{
   const char *s = emb_rb_get_ver();

   ASSERT_TRUE(s != NULL);
   printf("emb_rb_version: %s\n", s);
}
