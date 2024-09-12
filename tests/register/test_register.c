//
// Created by leonv on 12.09.2024.
//

#include "unity.h"
#include "register.h"

void setUp(void)
{

}

void tearDown(void)
{

}

void test_register_init_function(void)
{
    Register_t* reg = registerInit();
    TEST_ASSERT_NOT_EQUAL(reg, NULL);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_register_init_function);
    return UNITY_END();
}