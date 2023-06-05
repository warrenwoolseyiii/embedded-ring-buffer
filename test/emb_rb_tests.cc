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
   ASSERT_EQ(emb_rb_peek(&rb, rd, 10), 10);
   ASSERT_EQ(memcmp(data, rd, 10), 0);
   ASSERT_EQ(emb_rb_used_space(&rb), 10);
   ASSERT_EQ(emb_rb_free_space(&rb), 0);
   ASSERT_EQ(emb_rb_peek(&rb, rd, 1), 1);
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
   ASSERT_EQ(emb_rb_peek(&rb, rd, 10), 10);
   ASSERT_EQ(memcmp(data, rd, 10), 0);
   ASSERT_EQ(emb_rb_used_space(&rb), 10);
   ASSERT_EQ(emb_rb_free_space(&rb), 0);
   ASSERT_EQ(emb_rb_peek(&rb, rd, 11), 10);
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

// Test getting the version
TEST_F(RBTesting, Test_Version)
{
   const char *s = emb_rb_get_ver();

   ASSERT_TRUE(s != NULL);
   printf("emb_rb_version: %s\n", s);
}
