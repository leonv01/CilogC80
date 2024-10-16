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

void test_cpu_load_a_n(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0002;

    storeByte(&memory, 0x0000, LD_A_n);
    storeByte(&memory, 0x0001, value);

    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    value, 0, 0, 0, 0, 0, 0,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}
void test_cpu_load_b_n(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0002;
    
    storeByte(&memory, 0x0000, LD_B_n);
    storeByte(&memory, 0x0001, value);

    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, value, 0, 0, 0, 0, 0,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}
void test_cpu_load_c_n(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0002;
    
    storeByte(&memory, 0x0000, LD_C_n);
    storeByte(&memory, 0x0001, value);

    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, 0, value, 0, 0, 0, 0,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}
void test_cpu_load_d_n(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0002;
    
    storeByte(&memory, 0x0000, LD_D_n);
    storeByte(&memory, 0x0001, value);

    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, 0, 0, value, 0, 0, 0,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}
void test_cpu_load_e_n(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0002;
    
    storeByte(&memory, 0x0000, LD_E_n);
    storeByte(&memory, 0x0001, value);

    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, 0, 0, 0, value, 0, 0,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}
void test_cpu_load_h_n(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0002;
    
    storeByte(&memory, 0x0000, LD_H_n);
    storeByte(&memory, 0x0001, value);

    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, 0, 0, 0, 0, value, 0,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}
void test_cpu_load_l_n(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0002;
    
    storeByte(&memory, 0x0000, LD_L_n);
    storeByte(&memory, 0x0001, value);

    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, 0, 0, 0, 0, 0, value,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}

void test_cpu_load_a_nn(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0003;
    
    storeByte(&memory, 0x0000, LD_A_nn);
    storeByte(&memory, 0x0001, 0x00);
    storeByte(&memory, 0x0002, 0x01);
    storeByte(&memory, 0x0100, value);

    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    value, 0, 0, 0, 0, 0, 0,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}

void test_cpu_load_hl_nn(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0003;
    
    storeByte(&memory, 0x0000, LD_HL_nn);
    storeByte(&memory, 0x0001, 0x00);
    storeByte(&memory, 0x0002, 0x01);
    storeByte(&memory, 0x0100, value);
    storeByte(&memory, 0x0101, value + 1);

    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, 0, 0, 0, 0, value, value + 1,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}

void test_cpu_load_bc_immediate(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0003;
    
    storeByte(&memory, 0x0000, LD_BC_IMMEDIATE);
    storeByte(&memory, 0x0001, 0x12);
    storeByte(&memory, 0x0002, 0x34);

    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, 0x34, 0x12, 0, 0, 0, 0,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}

void test_cpu_load_de_immediate(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    word_t expectedPC = 0x0003;
    
    storeByte(&memory, 0x0000, LD_DE_IMMEDIATE);
    storeByte(&memory, 0x0001, 0x12);
    storeByte(&memory, 0x0002, 0x34);

    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, 0, 0, 0x34, 0x12, 0, 0,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}

void test_cpu_load_hl_immediate(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0003;
    
    storeByte(&memory, 0x0000, LD_HL_IMMEDIATE);
    storeByte(&memory, 0x0001, 0x12);
    storeByte(&memory, 0x0002, 0x34);

    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, 0, 0, 0, 0, 0x34, 0x12,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}

void test_cpu_load_sp_immediate(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0003;
    
    storeByte(&memory, 0x0000, LD_BC_IMMEDIATE);
    storeByte(&memory, 0x0001, 0x12);
    storeByte(&memory, 0x0002, 0x34);

    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, 0, 0, 0, 0, 0, 0,
    0x1234, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_cpu_load_a_n);
    RUN_TEST(test_cpu_load_b_n);
    RUN_TEST(test_cpu_load_c_n);
    RUN_TEST(test_cpu_load_d_n);
    RUN_TEST(test_cpu_load_e_n);
    RUN_TEST(test_cpu_load_h_n);
    RUN_TEST(test_cpu_load_l_n);

    RUN_TEST(test_cpu_load_a_nn);
    RUN_TEST(test_cpu_load_hl_nn);

    RUN_TEST(test_cpu_load_bc_immediate);
    RUN_TEST(test_cpu_load_de_immediate);
    RUN_TEST(test_cpu_load_hl_immediate);
    RUN_TEST(test_cpu_load_sp_immediate);
    return UNITY_END();
}