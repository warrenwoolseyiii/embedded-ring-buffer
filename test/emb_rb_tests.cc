#include <gtest/gtest.h>
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
TEST_F( RBTesting, Test_Null_Init )
{
    emb_rb_t rb;
    uint8_t  buf[10];
    uint32_t size = 10;
    ASSERT_FALSE( emb_rb_init( 0, 0, 0 ) );
    ASSERT_FALSE( emb_rb_init( &rb, 0, 0 ) );
    ASSERT_FALSE( emb_rb_init( &rb, buf, 0 ) );
    ASSERT_FALSE( emb_rb_init( &rb, 0, size ) );
}

// Ensure that the ring buffer initializes properly
TEST_F( RBTesting, Test_Init )
{
    emb_rb_t rb;
    uint8_t  buf[10];
    uint32_t size = 10;
    ASSERT_TRUE( emb_rb_init( &rb, buf, size ) );
}

// Ensure that the ring buffer returns the correct size
TEST_F( RBTesting, Test_Size )
{
    emb_rb_t rb;
    uint8_t  buf[10];
    uint32_t size = 10;
    ASSERT_TRUE( emb_rb_init( &rb, buf, size ) );
    ASSERT_EQ( emb_rb_size( &rb ), size );
}

// Ensure that the ring buffer returns the correct free space
TEST_F( RBTesting, Test_Free_Space )
{
    emb_rb_t rb;
    uint8_t  buf[10];
    uint32_t size = 10;
    ASSERT_TRUE( emb_rb_init( &rb, buf, size ) );
    ASSERT_EQ( emb_rb_free_space( &rb ), size );
}

// Ensure that the ring buffer returns the correct used space
TEST_F( RBTesting, Test_Used_Space )
{
    emb_rb_t rb;
    uint8_t  buf[10];
    uint32_t size = 10;
    ASSERT_TRUE( emb_rb_init( &rb, buf, size ) );
    ASSERT_EQ( emb_rb_used_space( &rb ), 0 );
}