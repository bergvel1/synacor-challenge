//
// Created by David Bergvelt on 9/4/18.
//

#include "gtest/gtest.h"

TEST(basic_check, test_eq) {
EXPECT_EQ(1, 1);
}

TEST(basic_check, test_neq) {
EXPECT_NE(1, 0);
}