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

void test_cpu_load_a_immediate(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    storeByte(&memory, 0x0000, 0x3E);
    storeByte(&memory, 0x0001, 0x42);

    cpuExecute(&cpu, &memory);

    TEST_ASSERT_EQUAL(cpu.A, 0x42);
    TEST_ASSERT_EQUAL(cpu.PC, 0x0002);
}

void test_cpu_add_a_immediate(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    storeByte(&memory, 0x0000, ADD_A_n);
    storeByte(&memory, 0x0001, 0x42);

    cpuExecute(&cpu, &memory);

    TEST_ASSERT_EQUAL(cpu.A, 0x42);
    TEST_ASSERT_EQUAL(cpu.PC, 0x0002);

    TEST_ASSERT_EQUAL(cpu.F.C, 0);
    TEST_ASSERT_EQUAL(cpu.F.N, 0);
    TEST_ASSERT_EQUAL(cpu.F.P, 1);
    TEST_ASSERT_EQUAL(cpu.F.H, 0);
    TEST_ASSERT_EQUAL(cpu.F.Z, 0);
    TEST_ASSERT_EQUAL(cpu.F.S, 0);

    storeByte(&memory, 0x0002, ADD_A_n);
    storeByte(&memory, 0x0003, 0xFF);
    
    cpu.A = 0x01;
    cpuExecute(&cpu, &memory);

    TEST_ASSERT_EQUAL(cpu.A, 0x00);
    TEST_ASSERT_EQUAL(cpu.PC, 0x0004);

    TEST_ASSERT_EQUAL(cpu.F.C, 1);
    TEST_ASSERT_EQUAL(cpu.F.N, 0);
    TEST_ASSERT_EQUAL(cpu.F.P, 1);
    TEST_ASSERT_EQUAL(cpu.F.H, 1);
    TEST_ASSERT_EQUAL(cpu.F.Z, 1);
    TEST_ASSERT_EQUAL(cpu.F.S, 0);
}
void test_cpu_add_a_hl(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    storeByte(&memory, 0x0000, ADD_A_HL);
    storeByte(&memory, 0x0001, ADD_A_HL);

    cpu.H = 0x00;
    cpu.L = 0x42;

    storeByte(&memory, 0x0042, 0x42);

    cpuExecute(&cpu, &memory);

    TEST_ASSERT_EQUAL(cpu.A, 0x42);
    TEST_ASSERT_EQUAL(cpu.PC, 0x0001);

    TEST_ASSERT_EQUAL(cpu.F.C, 0);
    TEST_ASSERT_EQUAL(cpu.F.N, 0);
    TEST_ASSERT_EQUAL(cpu.F.P, 1);
    TEST_ASSERT_EQUAL(cpu.F.H, 0);
    TEST_ASSERT_EQUAL(cpu.F.Z, 0);
    TEST_ASSERT_EQUAL(cpu.F.S, 0);

    storeByte(&memory, 0x0042, 0xFF);

    cpu.A = 0x01;

    cpuExecute(&cpu, &memory);

    TEST_ASSERT_EQUAL(cpu.A, 0x00);
    TEST_ASSERT_EQUAL(cpu.PC, 0x0002);

    TEST_ASSERT_EQUAL(cpu.F.C, 1);
    TEST_ASSERT_EQUAL(cpu.F.N, 0);
    TEST_ASSERT_EQUAL(cpu.F.P, 1);
    TEST_ASSERT_EQUAL(cpu.F.H, 1);
    TEST_ASSERT_EQUAL(cpu.F.Z, 1);
    TEST_ASSERT_EQUAL(cpu.F.S, 0);
}
void test_cpu_add_a_b(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    storeByte(&memory, 0x0000, ADD_A_B);

    cpu.A = 0x00;
    cpu.B = 0x42;

    cpuExecute(&cpu, &memory);

    TEST_ASSERT_EQUAL(cpu.A, 0x42);
    TEST_ASSERT_EQUAL(cpu.PC, 0x0001);
}
void test_cpu_add_a_c(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    storeByte(&memory, 0x0000, ADD_A_C);

    cpu.A = 0x00;
    cpu.C = 0x42;

    cpuExecute(&cpu, &memory);

    TEST_ASSERT_EQUAL(cpu.A, 0x42);
    TEST_ASSERT_EQUAL(cpu.PC, 0x0001);
}
void test_cpu_add_a_d(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    storeByte(&memory, 0x0000, ADD_A_D);

    cpu.A = 0x00;
    cpu.D = 0x42;

    cpuExecute(&cpu, &memory);

    TEST_ASSERT_EQUAL(cpu.A, 0x42);
    TEST_ASSERT_EQUAL(cpu.PC, 0x0001);
}
void test_cpu_add_a_e(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    storeByte(&memory, 0x0000, ADD_A_E);

    cpu.A = 0x00;
    cpu.E = 0x42;

    cpuExecute(&cpu, &memory);

    TEST_ASSERT_EQUAL(cpu.A, 0x42);
    TEST_ASSERT_EQUAL(cpu.PC, 0x0001);
}
void test_cpu_add_a_h(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    storeByte(&memory, 0x0000, ADD_A_H);

    cpu.A = 0x00;
    cpu.H = 0x42;

    cpuExecute(&cpu, &memory);

    TEST_ASSERT_EQUAL(cpu.A, 0x42);
    TEST_ASSERT_EQUAL(cpu.PC, 0x0001);
}
void test_cpu_add_a_l(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    storeByte(&memory, 0x0000, ADD_A_L);

    cpu.A = 0x00;
    cpu.L = 0x42;

    cpuExecute(&cpu, &memory);

    TEST_ASSERT_EQUAL(cpu.A, 0x42);
    TEST_ASSERT_EQUAL(cpu.PC, 0x0001);
}
void test_cpu_add_a_a(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    storeByte(&memory, 0x0000, ADD_A_A);

    cpu.A = 0x42;

    cpuExecute(&cpu, &memory);

    TEST_ASSERT_EQUAL(cpu.A, 0x84);
    TEST_ASSERT_EQUAL(cpu.PC, 0x0001);
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

    TEST_ASSERT_EQUAL(cpu.A, 0x00);
    TEST_ASSERT_EQUAL(cpu.PC, 0x0002);

    TEST_ASSERT_EQUAL(cpu.F.C, 0);
    TEST_ASSERT_EQUAL(cpu.F.N, 1);
    TEST_ASSERT_EQUAL(cpu.F.P, 1);
    TEST_ASSERT_EQUAL(cpu.F.H, 0);
    TEST_ASSERT_EQUAL(cpu.F.Z, 1);
    TEST_ASSERT_EQUAL(cpu.F.S, 0);

    storeByte(&memory, 0x0002, SUB_n);
    storeByte(&memory, 0x0003, 0x43);
    cpu.A = 0x42;

    cpuExecute(&cpu, &memory);

    TEST_ASSERT_EQUAL(cpu.A, 0xFF);
    TEST_ASSERT_EQUAL(cpu.PC, 0x0004);

    TEST_ASSERT_EQUAL(cpu.F.C, 1);
    TEST_ASSERT_EQUAL(cpu.F.N, 1);
    TEST_ASSERT_EQUAL(cpu.F.P, 1);
    TEST_ASSERT_EQUAL(cpu.F.H, 1);
    TEST_ASSERT_EQUAL(cpu.F.Z, 0);
    TEST_ASSERT_EQUAL(cpu.F.S, 1);
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

    TEST_ASSERT_EQUAL(cpu.A, 0x00);
    TEST_ASSERT_EQUAL(cpu.PC, 0x0001);

    TEST_ASSERT_EQUAL(cpu.F.C, 0);
    TEST_ASSERT_EQUAL(cpu.F.N, 1);
    TEST_ASSERT_EQUAL(cpu.F.P, 1);
    TEST_ASSERT_EQUAL(cpu.F.H, 0);
    TEST_ASSERT_EQUAL(cpu.F.Z, 1);
    TEST_ASSERT_EQUAL(cpu.F.S, 0);

    storeByte(&memory, 0x0001, SUB_B);
    cpu.A = 0x42;
    cpu.B = 0x43;

    cpuExecute(&cpu, &memory);

    TEST_ASSERT_EQUAL(cpu.A, 0xFF);
    TEST_ASSERT_EQUAL(cpu.PC, 0x0002);

    TEST_ASSERT_EQUAL(cpu.F.C, 1);
    TEST_ASSERT_EQUAL(cpu.F.N, 1);
    TEST_ASSERT_EQUAL(cpu.F.P, 1);
    TEST_ASSERT_EQUAL(cpu.F.H, 1);
    TEST_ASSERT_EQUAL(cpu.F.Z, 0);
    TEST_ASSERT_EQUAL(cpu.F.S, 1);
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

    TEST_ASSERT_EQUAL(cpu.A, 0x00);
    TEST_ASSERT_EQUAL(cpu.PC, 0x0001);

    TEST_ASSERT_EQUAL(cpu.F.C, 0);
    TEST_ASSERT_EQUAL(cpu.F.N, 1);
    TEST_ASSERT_EQUAL(cpu.F.P, 1);
    TEST_ASSERT_EQUAL(cpu.F.H, 0);
    TEST_ASSERT_EQUAL(cpu.F.Z, 1);
    TEST_ASSERT_EQUAL(cpu.F.S, 0);

    storeByte(&memory, 0x0001, SUB_C);
    cpu.A = 0x42;
    cpu.C = 0x43;

    cpuExecute(&cpu, &memory);

    TEST_ASSERT_EQUAL(cpu.A, 0xFF);
    TEST_ASSERT_EQUAL(cpu.PC, 0x0002);

    TEST_ASSERT_EQUAL(cpu.F.C, 1);
    TEST_ASSERT_EQUAL(cpu.F.N, 1);
    TEST_ASSERT_EQUAL(cpu.F.P, 1);
    TEST_ASSERT_EQUAL(cpu.F.H, 1);
    TEST_ASSERT_EQUAL(cpu.F.Z, 0);
    TEST_ASSERT_EQUAL(cpu.F.S, 1);
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

    TEST_ASSERT_EQUAL(cpu.A, 0x00);
    TEST_ASSERT_EQUAL(cpu.PC, 0x0001);

    TEST_ASSERT_EQUAL(cpu.F.C, 0);
    TEST_ASSERT_EQUAL(cpu.F.N, 1);
    TEST_ASSERT_EQUAL(cpu.F.P, 1);
    TEST_ASSERT_EQUAL(cpu.F.H, 0);
    TEST_ASSERT_EQUAL(cpu.F.Z, 1);
    TEST_ASSERT_EQUAL(cpu.F.S, 0);

    storeByte(&memory, 0x0001, SUB_D);
    cpu.A = 0x42;
    cpu.D = 0x43;

    cpuExecute(&cpu, &memory);

    TEST_ASSERT_EQUAL(cpu.A, 0xFF);
    TEST_ASSERT_EQUAL(cpu.PC, 0x0002);

    TEST_ASSERT_EQUAL(cpu.F.C, 1);
    TEST_ASSERT_EQUAL(cpu.F.N, 1);
    TEST_ASSERT_EQUAL(cpu.F.P, 1);
    TEST_ASSERT_EQUAL(cpu.F.H, 1);
    TEST_ASSERT_EQUAL(cpu.F.Z, 0);
    TEST_ASSERT_EQUAL(cpu.F.S, 1);
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

    TEST_ASSERT_EQUAL(cpu.A, 0x00);
    TEST_ASSERT_EQUAL(cpu.PC, 0x0001);

    TEST_ASSERT_EQUAL(cpu.F.C, 0);
    TEST_ASSERT_EQUAL(cpu.F.N, 1);
    TEST_ASSERT_EQUAL(cpu.F.P, 1);
    TEST_ASSERT_EQUAL(cpu.F.H, 0);
    TEST_ASSERT_EQUAL(cpu.F.Z, 1);
    TEST_ASSERT_EQUAL(cpu.F.S, 0);

    storeByte(&memory, 0x0001, SUB_E);
    cpu.A = 0x42;
    cpu.E = 0x43;

    cpuExecute(&cpu, &memory);

    TEST_ASSERT_EQUAL(cpu.A, 0xFF);
    TEST_ASSERT_EQUAL(cpu.PC, 0x0002);

    TEST_ASSERT_EQUAL(cpu.F.C, 1);
    TEST_ASSERT_EQUAL(cpu.F.N, 1);
    TEST_ASSERT_EQUAL(cpu.F.P, 1);
    TEST_ASSERT_EQUAL(cpu.F.H, 1);
    TEST_ASSERT_EQUAL(cpu.F.Z, 0);
    TEST_ASSERT_EQUAL(cpu.F.S, 1);
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

    TEST_ASSERT_EQUAL(cpu.A, 0x00);
    TEST_ASSERT_EQUAL(cpu.PC, 0x0001);

    TEST_ASSERT_EQUAL(cpu.F.C, 0);
    TEST_ASSERT_EQUAL(cpu.F.N, 1);
    TEST_ASSERT_EQUAL(cpu.F.P, 1);
    TEST_ASSERT_EQUAL(cpu.F.H, 0);
    TEST_ASSERT_EQUAL(cpu.F.Z, 1);
    TEST_ASSERT_EQUAL(cpu.F.S, 0);

    storeByte(&memory, 0x0001, SUB_H);
    cpu.A = 0x42;
    cpu.H = 0x43;

    cpuExecute(&cpu, &memory);

    TEST_ASSERT_EQUAL(cpu.A, 0xFF);
    TEST_ASSERT_EQUAL(cpu.PC, 0x0002);

    TEST_ASSERT_EQUAL(cpu.F.C, 1);
    TEST_ASSERT_EQUAL(cpu.F.N, 1);
    TEST_ASSERT_EQUAL(cpu.F.P, 1);
    TEST_ASSERT_EQUAL(cpu.F.H, 1);
    TEST_ASSERT_EQUAL(cpu.F.Z, 0);
    TEST_ASSERT_EQUAL(cpu.F.S, 1);
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

    TEST_ASSERT_EQUAL(cpu.A, 0x00);
    TEST_ASSERT_EQUAL(cpu.PC, 0x0001);

    TEST_ASSERT_EQUAL(cpu.F.C, 0);
    TEST_ASSERT_EQUAL(cpu.F.N, 1);
    TEST_ASSERT_EQUAL(cpu.F.P, 1);
    TEST_ASSERT_EQUAL(cpu.F.H, 0);
    TEST_ASSERT_EQUAL(cpu.F.Z, 1);
    TEST_ASSERT_EQUAL(cpu.F.S, 0);

    storeByte(&memory, 0x0001, SUB_L);
    cpu.A = 0x42;
    cpu.L = 0x43;

    cpuExecute(&cpu, &memory);

    TEST_ASSERT_EQUAL(cpu.A, 0xFF);
    TEST_ASSERT_EQUAL(cpu.PC, 0x0002);

    TEST_ASSERT_EQUAL(cpu.F.C, 1);
    TEST_ASSERT_EQUAL(cpu.F.N, 1);
    TEST_ASSERT_EQUAL(cpu.F.P, 1);
    TEST_ASSERT_EQUAL(cpu.F.H, 1);
    TEST_ASSERT_EQUAL(cpu.F.Z, 0);
    TEST_ASSERT_EQUAL(cpu.F.S, 1);
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

    TEST_ASSERT_EQUAL(cpu.A, 0x00);
    TEST_ASSERT_EQUAL(cpu.PC, 0x0001);

    TEST_ASSERT_EQUAL(cpu.F.C, 0);
    TEST_ASSERT_EQUAL(cpu.F.N, 1);
    TEST_ASSERT_EQUAL(cpu.F.P, 1);
    TEST_ASSERT_EQUAL(cpu.F.H, 0);
    TEST_ASSERT_EQUAL(cpu.F.Z, 1);
    TEST_ASSERT_EQUAL(cpu.F.S, 0);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_cpu_init_function);

    // ADD tests    
    RUN_TEST(test_cpu_add_a_immediate);
    RUN_TEST(test_cpu_add_a_hl);
    RUN_TEST(test_cpu_add_a_b);
    RUN_TEST(test_cpu_add_a_c);
    RUN_TEST(test_cpu_add_a_d);
    RUN_TEST(test_cpu_add_a_e);
    RUN_TEST(test_cpu_add_a_h);
    RUN_TEST(test_cpu_add_a_l);
    RUN_TEST(test_cpu_add_a_a);

    // SUB tests
    RUN_TEST(test_cpu_sub_n);
    RUN_TEST(test_cpu_sub_b);
    RUN_TEST(test_cpu_sub_c);
    RUN_TEST(test_cpu_sub_d);
    RUN_TEST(test_cpu_sub_e);
    RUN_TEST(test_cpu_sub_h);
    RUN_TEST(test_cpu_sub_l);
    RUN_TEST(test_cpu_sub_a);
    RUN_TEST(test_cpu_load_a_immediate);

    return UNITY_END();
}