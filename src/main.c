#include <stdio.h>

#include "cpu.h"
#include "mem.h"
#include "instructions.h"
#include "instruction_handler.h"

int main() {

    CPU_t cpu;
    cpuInit(&cpu);

    Memory_t memory;
    memoryInit(&memory);

    storeByte(&memory, 0, 0x00);

    cpuEmulate(&cpu, &memory);

    return 0;
}
