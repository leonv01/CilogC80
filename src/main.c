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

    cpu.A = 0x05;

    storeByte(&memory, 0x0000, ADD_A_A);
    cpuExecute(&cpu, &memory);

    printf("%d\n", cpu.B);

    return 0;
}
