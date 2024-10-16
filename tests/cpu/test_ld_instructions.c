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
    
    storeByte(&memory, 0x0000, LD_SP_IMMEDIATE);
    storeByte(&memory, 0x0001, 0x12);
    storeByte(&memory, 0x0002, 0x34);

    cpuExecute(&cpu, &memory);
    // TODO: Check SP value
    check_registers(&cpu,
    0, 0, 0, 0, 0, 0, 0,
    0x1234, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}

void test_cpu_load_b_b(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0001;

    cpu.B = value;

    storeByte(&memory, 0x0000, LD_B_B);
    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, value, 0, 0, 0, 0, 0,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}
void test_cpu_load_b_c(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0001;

    cpu.C = value;

    storeByte(&memory, 0x0000, LD_B_C);
    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, value, value, 0, 0, 0, 0,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}
void test_cpu_load_b_d(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0001;

    cpu.D = value;

    storeByte(&memory, 0x0000, LD_B_D);
    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, value, 0, value, 0, 0, 0,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}
void test_cpu_load_b_e(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0001;

    cpu.E = value;

    storeByte(&memory, 0x0000, LD_B_E);
    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, value, 0, 0, value, 0, 0,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}
void test_cpu_load_b_h(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0001;

    cpu.H = value;

    storeByte(&memory, 0x0000, LD_B_H);
    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, value, 0, 0, 0, value, 0,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}
void test_cpu_load_b_l(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0001;

    cpu.L = value;

    storeByte(&memory, 0x0000, LD_B_L);
    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, value, 0, 0, 0, 0, value,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}
void test_cpu_load_b_hl(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t address = 0x1234;
    word_t expectedPC = 0x0001;

    cpu.H = UPPER_BYTE(address);
    cpu.L = LOWER_BYTE(address);

    storeByte(&memory, 0x0000, LD_B_HL);
    storeByte(&memory, address, value);

    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, value, 0, 0, 0, UPPER_BYTE(address), LOWER_BYTE(address),
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}
void test_cpu_load_b_a(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0001;

    cpu.A = value;

    storeByte(&memory, 0x0000, LD_B_A);
    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    value, value, 0, 0, 0, 0, 0,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}

void test_cpu_load_c_b(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0001;

    cpu.B = value;

    storeByte(&memory, 0x0000, LD_C_B);
    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, value, value, 0, 0, 0, 0,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}
void test_cpu_load_c_c(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0001;

    cpu.C = value;

    storeByte(&memory, 0x0000, LD_C_C);
    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, 0, value, 0, 0, 0, 0,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}
void test_cpu_load_c_d(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0001;

    cpu.D = value;

    storeByte(&memory, 0x0000, LD_C_D);
    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, 0, value, value, 0, 0, 0,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}
void test_cpu_load_c_e(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0001;

    cpu.E = value;

    storeByte(&memory, 0x0000, LD_C_E);
    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, 0, value, 0, value, 0, 0,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}
void test_cpu_load_c_h(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0001;

    cpu.H = value;

    storeByte(&memory, 0x0000, LD_C_H);
    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, 0, value, 0, 0, value, 0,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}
void test_cpu_load_c_l(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0001;

    cpu.L = value;

    storeByte(&memory, 0x0000, LD_C_L);
    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, 0, value, 0, 0, 0, value,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}
void test_cpu_load_c_hl(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t address = 0x1234;
    word_t expectedPC = 0x0001;

    cpu.H = UPPER_BYTE(address);
    cpu.L = LOWER_BYTE(address);

    storeByte(&memory, 0x0000, LD_C_HL);
    storeByte(&memory, address, value);

    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, 0, value, 0, 0, UPPER_BYTE(address), LOWER_BYTE(address),
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}
void test_cpu_load_c_a(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0001;

    cpu.A = value;

    storeByte(&memory, 0x0000, LD_C_A);
    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    value, 0, value, 0, 0, 0, 0,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}

void test_cpu_load_d_b(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0001;

    cpu.B = value;

    storeByte(&memory, 0x0000, LD_D_B);
    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, value, 0, value, 0, 0, 0,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}
void test_cpu_load_d_c(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0001;

    cpu.C = value;

    storeByte(&memory, 0x0000, LD_D_C);
    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, 0, value, value, 0, 0, 0,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}
void test_cpu_load_d_d(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0001;

    cpu.D = value;

    storeByte(&memory, 0x0000, LD_D_D);
    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, 0, 0, value, 0, 0, 0,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}
void test_cpu_load_d_e(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0001;

    cpu.E = value;

    storeByte(&memory, 0x0000, LD_D_E);
    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, 0, 0, value, value, 0, 0,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}
void test_cpu_load_d_h(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0001;

    cpu.H = value;

    storeByte(&memory, 0x0000, LD_D_H);
    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, 0, 0, value, 0, value, 0,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}
void test_cpu_load_d_l(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0001;

    cpu.L = value;

    storeByte(&memory, 0x0000, LD_D_L);
    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, 0, 0, value, 0, 0, value,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}
void test_cpu_load_d_hl(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t address = 0x1234;
    word_t expectedPC = 0x0001;

    cpu.H = UPPER_BYTE(address);
    cpu.L = LOWER_BYTE(address);

    storeByte(&memory, 0x0000, LD_D_HL);
    storeByte(&memory, address, value);

    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, 0, 0, value, 0, UPPER_BYTE(address), LOWER_BYTE(address),
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}
void test_cpu_load_d_a(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0001;

    cpu.A = value;

    storeByte(&memory, 0x0000, LD_D_A);
    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    value, 0, 0, value, 0, 0, 0,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}

void test_cpu_load_e_b(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0001;

    cpu.B = value;

    storeByte(&memory, 0x0000, LD_E_B);
    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, value, 0, 0, value, 0, 0,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}
void test_cpu_load_e_c(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0001;

    cpu.C = value;

    storeByte(&memory, 0x0000, LD_E_C);
    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, 0, value, 0, value, 0, 0,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}
void test_cpu_load_e_d(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0001;

    cpu.D = value;

    storeByte(&memory, 0x0000, LD_E_D);
    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, 0, 0, value, value, 0, 0,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}
void test_cpu_load_e_e(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0001;

    cpu.E = value;

    storeByte(&memory, 0x0000, LD_E_E);
    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, 0, 0, 0, value, 0, 0,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}
void test_cpu_load_e_h(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0001;

    cpu.H = value;

    storeByte(&memory, 0x0000, LD_E_H);
    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, 0, 0, 0, value, value, 0,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}
void test_cpu_load_e_l(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0001;

    cpu.L = value;

    storeByte(&memory, 0x0000, LD_E_L);
    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, 0, 0, 0, value, 0, value,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}
void test_cpu_load_e_hl(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t address = 0x1234;
    word_t expectedPC = 0x0001;

    cpu.H = UPPER_BYTE(address);
    cpu.L = LOWER_BYTE(address);

    storeByte(&memory, 0x0000, LD_E_HL);
    storeByte(&memory, address, value);

    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    0, 0, 0, 0, value, UPPER_BYTE(address), LOWER_BYTE(address),
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}
void test_cpu_load_e_a(void)
{
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0001;

    cpu.A = value;

    storeByte(&memory, 0x0000, LD_E_A);
    cpuExecute(&cpu, &memory);

    check_registers(&cpu,
    value, 0, 0, 0, value, 0, 0,
    0, expectedPC, 0, 0,
    0, 0, 
    0, 0, 0, 0, 0, 0, 0);
}

// TODO: Add missing tests


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

    RUN_TEST(test_cpu_load_b_b);
    RUN_TEST(test_cpu_load_b_c);
    RUN_TEST(test_cpu_load_b_d);
    RUN_TEST(test_cpu_load_b_e);
    RUN_TEST(test_cpu_load_b_h);
    RUN_TEST(test_cpu_load_b_l);
    RUN_TEST(test_cpu_load_b_hl);
    RUN_TEST(test_cpu_load_b_a);

    RUN_TEST(test_cpu_load_c_b);
    RUN_TEST(test_cpu_load_c_c);
    RUN_TEST(test_cpu_load_c_d);
    RUN_TEST(test_cpu_load_c_e);
    RUN_TEST(test_cpu_load_c_h);
    RUN_TEST(test_cpu_load_c_l);
    RUN_TEST(test_cpu_load_c_hl);
    RUN_TEST(test_cpu_load_c_a);

    RUN_TEST(test_cpu_load_d_b);
    RUN_TEST(test_cpu_load_d_c);
    RUN_TEST(test_cpu_load_d_d);
    RUN_TEST(test_cpu_load_d_e);
    RUN_TEST(test_cpu_load_d_h);
    RUN_TEST(test_cpu_load_d_l);
    RUN_TEST(test_cpu_load_d_hl);
    RUN_TEST(test_cpu_load_d_a);

    RUN_TEST(test_cpu_load_e_b);
    RUN_TEST(test_cpu_load_e_c);
    RUN_TEST(test_cpu_load_e_d);
    RUN_TEST(test_cpu_load_e_e);
    RUN_TEST(test_cpu_load_e_h);
    RUN_TEST(test_cpu_load_e_l);
    RUN_TEST(test_cpu_load_e_hl);
    RUN_TEST(test_cpu_load_e_a);
    return UNITY_END();
}