//
// Created by David Bergvelt on 9/4/18.
//

#include "gtest/gtest.h"

#include "../../src/stack.c"

#define TEST_VALUE_1 18
#define TEST_VALUE_2 96

class StackFixture : public ::testing::Test {
protected:
    virtual void TearDown() {
        Stack_destroy(stack);
    }

    virtual void SetUp() {
        stack = Stack_create();
    }

public:
    StackFixture() : Test() {
    }

    virtual ~StackFixture() {
    }

    Stack * stack;
};
TEST_F(StackFixture, begin_empty) {
    EXPECT_EQ(Stack_size(stack), 0);
}

TEST_F(StackFixture, push_size_increment) {
    Stack_push(stack,TEST_VALUE_1);

    EXPECT_EQ(Stack_size(stack), 1);
}

TEST_F(StackFixture, push_pop_empty) {
    Stack_push(stack,TEST_VALUE_1);
    Stack_pop(stack);

    EXPECT_EQ(Stack_size(stack), 0);
}

TEST_F(StackFixture, push_pop_data) {
    Stack_push(stack,TEST_VALUE_1);

    EXPECT_EQ(TEST_VALUE_1, Stack_pop(stack));
}

TEST_F(StackFixture, push_peek_data) {
    Stack_push(stack,TEST_VALUE_1);

    EXPECT_EQ(TEST_VALUE_1, Stack_peek(stack,0));
}

TEST_F(StackFixture, data_fifo) {
    Stack_push(stack,TEST_VALUE_1);
    Stack_push(stack,TEST_VALUE_2);

    EXPECT_EQ(TEST_VALUE_2, Stack_pop(stack));
    EXPECT_EQ(TEST_VALUE_1, Stack_pop(stack));
}