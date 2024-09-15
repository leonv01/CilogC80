#include <stdio.h>

#include "cpu.h"
#include "mem.h"
#include "instructions.h"

int main() {
    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    storeByte(&memory, 0x0000, ADD_A_n);
    storeByte(&memory, 0x0001, 0xFF);
    cpu.A = 0x01;
    cpuEmulate(&cpu, &memory);  

    return 0;
}
