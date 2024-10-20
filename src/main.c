#include <stdio.h>

#include "cpu.h"
#include "mem.h"
#include "instructions.h"

int main() {
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    byte_t value = 0x42;
    word_t expectedPC = 0x0001;

    cpu.H = 0x12;
    cpu.L = 0x34;
    cpu.B = 0x56;
    cpu.C = 0x78;

    storeByte(&memory, 0x0000, MAIN_ADD_HL_BC_IMM);
    cpuExecute(&cpu, &memory);

    printf("%d\n", TO_WORD(cpu.H, cpu.L));

    return 0;
}
