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
    CPU_t* cpu,
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

void test_cpu_sub_n(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    storeByte(&memory, 0x0000, SUB_n);
    storeByte(&memory, 0x0001, 0x42);

    cpu.A = 0x42;

    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0x00, 0, 0, 0, 0, 0, 0,
    0, 2, 0, 0,
    0, 0, 
    0, 1, 1, 0, 1, 0, 0);

    storeByte(&memory, 0x0002, SUB_n);
    storeByte(&memory, 0x0003, 0x43);
    cpu.A = 0x42;

    cpuExecute(&cpu, &memory);
    check_registers(&cpu,
    0xFF, 0, 0, 0, 0, 0, 0,
    0, 4, 0, 0,
    0, 0, 
    1, 1, 1, 1, 0, 1, 0);
}
void test_cpu_sub_b(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    storeByte(&memory, 0x0000, SUB_B);

    cpu.A = 0x42;
    cpu.B = 0x42;

    cpuExecute(&cpu, &memory);
    check_registers(&cpu,
    0x00, 0x42, 0, 0, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 
    0, 1, 1, 0, 1, 0, 0);

    storeByte(&memory, 0x0001, SUB_B);
    cpu.A = 0x42;
    cpu.B = 0x43;

    cpuExecute(&cpu, &memory);
    check_registers(&cpu,
    0xFF, 0x43, 0, 0, 0, 0, 0,
    0, 2, 0, 0,
    0, 0, 
    1, 1, 1, 1, 0, 1, 0);
}
void test_cpu_sub_c(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    storeByte(&memory, 0x0000, SUB_C);

    cpu.A = 0x42;
    cpu.C = 0x42;

    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0x00, 0, 0x42, 0, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 
    0, 1, 1, 0, 1, 0, 0);

    storeByte(&memory, 0x0001, SUB_C);
    cpu.A = 0x42;
    cpu.C = 0x43;

    cpuExecute(&cpu, &memory);
    check_registers(&cpu,
    0xFF, 0, 0x43, 0, 0, 0, 0,
    0, 2, 0, 0,
    0, 0, 
    1, 1, 1, 1, 0, 1, 0);
}
void test_cpu_sub_d(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    storeByte(&memory, 0x0000, SUB_D);

    cpu.A = 0x42;
    cpu.D = 0x42;

    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0x00, 0, 0, 0x42, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 
    0, 1, 1, 0, 1, 0, 0);

    storeByte(&memory, 0x0001, SUB_D);
    cpu.A = 0x42;
    cpu.D = 0x43;

    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0xFF, 0, 0, 0x43, 0, 0, 0,
    0, 2, 0, 0,
    0, 0, 
    1, 1, 1, 1, 0, 1, 0);
}
void test_cpu_sub_e(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    storeByte(&memory, 0x0000, SUB_E);

    cpu.A = 0x42;
    cpu.E = 0x42;

    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0x00, 0, 0, 0, 0x42, 0, 0,
    0, 1, 0, 0,
    0, 0, 
    0, 1, 1, 0, 1, 0, 0);

    storeByte(&memory, 0x0001, SUB_E);
    cpu.A = 0x42;
    cpu.E = 0x43;

    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0xFF, 0, 0, 0, 0x43, 0, 0,
    0, 2, 0, 0,
    0, 0, 
    1, 1, 1, 1, 0, 1, 0);
}
void test_cpu_sub_h(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    storeByte(&memory, 0x0000, SUB_H);

    cpu.A = 0x42;
    cpu.H = 0x42;

    cpuExecute(&cpu, &memory);
    
    check_registers(&cpu,
    0x00, 0, 0, 0, 0, 0x42, 0,
    0, 1, 0, 0,
    0, 0, 
    0, 1, 1, 0, 1, 0, 0);

    storeByte(&memory, 0x0001, SUB_H);
    cpu.A = 0x42;
    cpu.H = 0x43;

    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0xFF, 0, 0, 0, 0, 0x43, 0,
    0, 2, 0, 0,
    0, 0, 
    1, 1, 1, 1, 0, 1, 0);
}
void test_cpu_sub_l(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    storeByte(&memory, 0x0000, SUB_L);

    cpu.A = 0x42;
    cpu.L = 0x42;

    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0x00, 0, 0, 0, 0, 0, 0x42,
    0, 1, 0, 0,
    0, 0, 
    0, 1, 1, 0, 1, 0, 0);

    storeByte(&memory, 0x0001, SUB_L);
    cpu.A = 0x42;
    cpu.L = 0x43;

    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0xFF, 0, 0, 0, 0, 0, 0x43,
    0, 2, 0, 0,
    0, 0, 
    1, 1, 1, 1, 0, 1, 0);
}
void test_cpu_sub_a(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    storeByte(&memory, 0x0000, SUB_A);

    cpu.A = 0x42;

    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0x00, 0, 0, 0, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 
    0, 1, 1, 0, 1, 0, 0);

    // TODO: Adjust test for overflow
}

int main(void)
{
    UNITY_BEGIN();

    // SUB tests
    RUN_TEST(test_cpu_sub_n);
    RUN_TEST(test_cpu_sub_b);
    RUN_TEST(test_cpu_sub_c);
    RUN_TEST(test_cpu_sub_d);
    RUN_TEST(test_cpu_sub_e);
    RUN_TEST(test_cpu_sub_h);
    RUN_TEST(test_cpu_sub_l);
    RUN_TEST(test_cpu_sub_a);

    return UNITY_END();
}