#include <gtest/gtest.h>
#include <emb_rb.h>

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

TEST_F(RBTesting, Test1)
{
  // test code here
  ASSERT_TRUE(true);
}