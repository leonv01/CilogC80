#include "unity.h"
#include "mem.h"

void setUp(void)
{
    // Initialize CPU or any other setup needed for the tests.
}

void tearDown(void)
{
    // Cleanup resources after each test.
}

void test_memory_init_function(void)
{
    Memory_t memory;
    memoryInit(&memory);

    TEST_ASSERT_NOT_EQUAL(memory.data, NULL);

    for(int i = 0; i < MEMORY_SIZE; i++)
    {
        TEST_ASSERT_EQUAL(memory.data[i], 0x00);
    }
}

void test_memory_free_function(void)
{
    Memory_t memory;
    memoryInit(&memory);

    memoryDestroy(&memory);

    TEST_ASSERT_EQUAL(memory.data, NULL);
}

void test_memory_fetch_byte_function(void)
{
    Memory_t memory;
    memoryInit(&memory);

    for(int i = 0; i < MEMORY_SIZE; i++)
    {
        memory.data[i] = (byte_t) i;
    }

    for(int i = 0; i < MEMORY_SIZE; i++)
    {
        TEST_ASSERT_EQUAL(fetchByte(&memory, i), (byte_t)i);
    }
}

void test_memory_fetch_word_function(void)
{
    Memory_t memory;
    memoryInit(&memory);

    for(int i = 0; i < MEMORY_SIZE; i++)
    {
        memory.data[i] = (byte_t) i;
    }

    for(int i = 0; i < MEMORY_SIZE; i++)
    {
        word_t word = TO_WORD(memory.data[i + 1], memory.data[i]);
        TEST_ASSERT_EQUAL(fetchWord(&memory, i), word);
    }
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_memory_init_function);
    RUN_TEST(test_memory_free_function);
    RUN_TEST(test_memory_fetch_byte_function);
    RUN_TEST(test_memory_fetch_word_function);
    return UNITY_END();
}