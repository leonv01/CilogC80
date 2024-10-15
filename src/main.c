#include <stdio.h>

#include "cpu.h"
#include "mem.h"
#include "instructions.h"

int main() {
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    storeByte(&memory, 0x0000, LD_A_nn);
    storeByte(&memory, 0x0001, 0x00);
    storeByte(&memory, 0x0002, 0x01);
    storeByte(&memory, 0x0100, 0x42);

    cpuExecute(&cpu, &memory);

    printf("%d\n", cpu.A);

    storeByte(&memory, 0x0002, ADD_A_n);
    storeByte(&memory, 0x0003, 0xFF);
    
    cpu.A = 0x0A;
    cpuExecute(&cpu, &memory);
    return 0;
}
