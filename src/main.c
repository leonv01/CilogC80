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
    storeByte(&memory, 0x0001, 0x42);

    cpuExecute(&cpu, &memory);

    storeByte(&memory, 0x0002, ADD_A_n);
    storeByte(&memory, 0x0003, 0xFF);
    
    cpu.A = 0x01;
    cpuExecute(&cpu, &memory);
    return 0;
}
