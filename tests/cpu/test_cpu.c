#include "unity.h"
#include "cpu.h"
#include "instructions.h"

void setUp(void)
{
    // Initialize CPU or any other setup needed for the tests.
}

void tearDown(void)
{
    // Cleanup resources after each test.
}

void check_registers(
    ZilogZ80_t* cpu,
    byte_t A, byte_t B, byte_t C, byte_t D, byte_t E, byte_t H, byte_t L, 
    word_t SP, word_t PC, word_t IX, word_t IY, 
    byte_t I, byte_t R,
    byte_t flagC, byte_t flagN, byte_t flagP, byte_t flagH, byte_t flagZ, byte_t flagS, byte_t flag_
)
{
    TEST_ASSERT_EQUAL(A, cpu->A);
    TEST_ASSERT_EQUAL(B, cpu->B);
    TEST_ASSERT_EQUAL(C, cpu->C);
    TEST_ASSERT_EQUAL(D, cpu->D);
    TEST_ASSERT_EQUAL(E, cpu->E);
    TEST_ASSERT_EQUAL(H, cpu->H);
    TEST_ASSERT_EQUAL(L, cpu->L);
    TEST_ASSERT_EQUAL(SP, cpu->SP);
    TEST_ASSERT_EQUAL(PC, cpu->PC);
    TEST_ASSERT_EQUAL(IX, cpu->IX);
    TEST_ASSERT_EQUAL(IY, cpu->IY);
    TEST_ASSERT_EQUAL(I, cpu->I);
    TEST_ASSERT_EQUAL(R, cpu->R);

    TEST_ASSERT_EQUAL(flagC, cpu->F.C);
    TEST_ASSERT_EQUAL(flagN, cpu->F.N);
    TEST_ASSERT_EQUAL(flagP, cpu->F.P);
    TEST_ASSERT_EQUAL(flag_, cpu->F._);
    TEST_ASSERT_EQUAL(flagH, cpu->F.H);
    TEST_ASSERT_EQUAL(flagZ, cpu->F.Z);
    TEST_ASSERT_EQUAL(flagS, cpu->F.S);
}

void test_cpu_init_function(void)
{
    ZilogZ80_t cpu;
    zilogZ80Init(&cpu);

    check_registers(&cpu,
    0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_cpu_init_function);

    return UNITY_END();
}