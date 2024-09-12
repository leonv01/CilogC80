//
// Created by leonv on 12.09.2024.
//

#include "unity.h"
#include "zilogZ80.h"

void setUp(void)
{

}

void tearDown(void)
{

}

void test_cpu_function(void)
{
    ZilogZ80_t* zilogZ80 = zilogInit();

    TEST_ASSERT_NOT_EQUAL(zilogInit, NULL);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_cpu_function);
    return UNITY_END();
}