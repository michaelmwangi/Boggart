#include <string>
#include <gtest/gtest.h>
#include "boggartqueue.h"


class BlockingQueueTest : public testing::Test {
 protected:
    virtual void SetUp(){
        queue_.Push(kTestString);
    }


    BoggartQueue<std::string> queue_;
    const std::string kTestString = "Test";
};

TEST_F(BlockingQueueTest, DefaultConstructor){
    EXPECT_EQ(0, queue_.Size());
}
