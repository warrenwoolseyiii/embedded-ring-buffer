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

// Test getting the version
TEST_F(RBTesting, Test_Version)
{
   const char *s = emb_rb_get_ver();

   ASSERT_TRUE(s != NULL);
   printf("emb_rb_version: %s\n", s);
}
