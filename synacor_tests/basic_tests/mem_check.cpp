//
// Created by David Bergvelt on 9/4/18.
//

#include "gtest/gtest.h"

#include "../../src/mem.c"

#define TEST_ADDR_1 20
#define TEST_VALUE_1 18
#define TEST_ADDR_2 19
#define TEST_VALUE_2 96

class MemoryFixture : public ::testing::Test {
protected:
    virtual void TearDown() {
        Memory_destroy(memory);
    }

    virtual void SetUp() {
        memory = Memory_create();
    }

public:
    MemoryFixture() : Test() {
    }

    virtual ~MemoryFixture() {
    }

    Memory * memory;
};
TEST_F(MemoryFixture, begin_empty) {
    EXPECT_EQ(Memory_size(memory), 0);
}

TEST_F(MemoryFixture, add_cell_size_increment) {
    cell c = {((uint16_t) TEST_ADDR_1),((uint16_t) TEST_VALUE_1)};
    Memory_append(memory,&c);

    EXPECT_EQ(Memory_size(memory), 1);
}

TEST_F(MemoryFixture, add_cell_read_data) {
    cell c = {((uint16_t) TEST_ADDR_1),((uint16_t) TEST_VALUE_1)};
    Memory_append(memory,&c);

    const cell * c_read = Memory_get(memory,0);
    EXPECT_EQ(TEST_ADDR_1, c_read->addr);
    EXPECT_EQ(TEST_VALUE_1, c_read->value);
}

TEST_F(MemoryFixture, add_delete_cell) {
    cell c = {((uint16_t) TEST_ADDR_1),((uint16_t) TEST_VALUE_1)};
    Memory_append(memory,&c);
    Memory_delete(memory,0);

    EXPECT_EQ(Memory_size(memory), 0);
}

TEST_F(MemoryFixture, set_NULL) {
    cell c = {((uint16_t) TEST_ADDR_1), ((uint16_t) TEST_VALUE_1)};
    Memory_append(memory, &c);
    Memory_set(memory,0,NULL);

    EXPECT_EQ(Memory_size(memory),1);
}

TEST_F(MemoryFixture, set_overwrite) {
    cell c1 = {((uint16_t) TEST_ADDR_1), ((uint16_t) TEST_VALUE_1)};
    cell c2 = {((uint16_t) TEST_ADDR_2), ((uint16_t) TEST_VALUE_2)};

    Memory_append(memory, &c1);
    Memory_set(memory,0,&c2);

    const cell * c_read = Memory_get(memory,0);

    EXPECT_EQ(Memory_size(memory),1);
    EXPECT_EQ(TEST_ADDR_2, c_read->addr);
    EXPECT_EQ(TEST_VALUE_2, c_read->value);
}

TEST_F(MemoryFixture, delete_shift) {
    cell c1 = {((uint16_t) TEST_ADDR_1), ((uint16_t) TEST_VALUE_1)};
    cell c2 = {((uint16_t) TEST_ADDR_2), ((uint16_t) TEST_VALUE_2)};

    Memory_append(memory, &c1);
    Memory_append(memory, &c2);
    EXPECT_EQ(Memory_size(memory),2);

    const cell * c_read = Memory_get(memory,0);
    EXPECT_EQ(TEST_ADDR_1, c_read->addr);
    EXPECT_EQ(TEST_VALUE_1, c_read->value);

    Memory_delete(memory,0);

    c_read = Memory_get(memory,0);
    EXPECT_EQ(TEST_ADDR_2, c_read->addr);
    EXPECT_EQ(TEST_VALUE_2, c_read->value);
}