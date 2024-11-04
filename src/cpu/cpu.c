#include "cpu.h"
#include "instructions.h"
#include "instruction_handler.h"

#include <stdio.h>

#include <stdbool.h>

#define CYCLES_PER_FRAME(x) ((int)((x * 1000000) / 60))
#define MAX_ITERATIONS 1000000

#define MAX_INSTRUCTION_COUNT 256

void cpuInit(CPU_t *cpu)
{
    if (cpu == NULL)
    {
        return;
    }

    *cpu = (CPU_t){
        .A = 0x00,
        .B = 0x00,
        .C = 0x00,
        .D = 0x00,
        .E = 0x00,
        .H = 0x00,
        .L = 0x00,
        .SP = 0x0000,
        .PC = 0x0000,
        .IX = 0x0000,
        .IY = 0x0000,
        .I = 0x00,
        .R = 0x00,
        .F = (F_t){
            .C = 0,
            .N = 0,
            .P = 0,
            ._ = 0,
            .H = 0,
            .Z = 0,
            .S = 0}};

    cpu->cyclesInFrame = 0;
    cpu->frequency = 3.5f;

}

void cpuReset(CPU_t *cpu)
{
    cpuInit(cpu);
}

void cpuStep(CPU_t *cpu, Memory_t *memory)
{
    if (cpu == NULL || memory == NULL)
    {
        return;
    }

    int iterations = 0;
    while (iterations < MAX_ITERATIONS)
    {
        if (cpu->cyclesInFrame <= 0)
        {
            cpu->cyclesInFrame = CYCLES_PER_FRAME(cpu->frequency);
        }

        int cycles = executeInstruction(cpu, memory);
        cpu->cyclesInFrame -= cycles;
        cpu->totalCycles += cycles;

        if (cpu->cyclesInFrame <= 0)
        {
            break;
        }

        iterations++;
    }
}
