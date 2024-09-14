#include "unity.h"
#include "cpu.h"

void setUp(void)
{
    // Initialize CPU or any other setup needed for the tests.
}

void tearDown(void)
{
    // Cleanup resources after each test.
}

void test_cpu_init_function(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    TEST_ASSERT_EQUAL(cpu.A, 0x00);
    TEST_ASSERT_EQUAL(cpu.B, 0x00);
    TEST_ASSERT_EQUAL(cpu.C, 0x00);
    TEST_ASSERT_EQUAL(cpu.D, 0x00);
    TEST_ASSERT_EQUAL(cpu.E, 0x00);
    TEST_ASSERT_EQUAL(cpu.H, 0x00);
    TEST_ASSERT_EQUAL(cpu.L, 0x00);
    TEST_ASSERT_EQUAL(cpu.SP, 0x0000);
    TEST_ASSERT_EQUAL(cpu.PC, 0x0000);
    TEST_ASSERT_EQUAL(cpu.IX, 0x0000);
    TEST_ASSERT_EQUAL(cpu.IY, 0x0000);
    TEST_ASSERT_EQUAL(cpu.I, 0x00);
    TEST_ASSERT_EQUAL(cpu.R, 0x00);

    TEST_ASSERT_EQUAL(cpu.F.C, 0);
    TEST_ASSERT_EQUAL(cpu.F.N, 0);
    TEST_ASSERT_EQUAL(cpu.F.P, 0);
    TEST_ASSERT_EQUAL(cpu.F._, 0);
    TEST_ASSERT_EQUAL(cpu.F.H, 0);
    TEST_ASSERT_EQUAL(cpu.F.Z, 0);
    TEST_ASSERT_EQUAL(cpu.F.S, 0);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_cpu_init_function);
    return UNITY_END();
}