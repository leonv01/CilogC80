#include "cpu/cpu.h"

#include <stdbool.h>

#include "cpu/instructions.h"
#include "cpu/instruction_handler.h"

void zilogZ80Init(ZilogZ80_t *cpu)
{
    if (cpu == NULL)
    {
        return;
    }

    *cpu = (ZilogZ80_t){
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

    cpu->isHaltered = false;
}

void zilogZ80Reset(ZilogZ80_t *cpu)
{
    zilogZ80Init(cpu);
}

void zilogZ80Step(ZilogZ80_t *cpu, Memory_t *memory)
{
    if (cpu == NULL || memory == NULL)
    {
    }
    else
    {
        if(cpu->isHaltered == false)
        {
            int cycles = executeInstruction(cpu, memory);
            cpu->cyclesInFrame -= cycles;
            cpu->totalCycles += cycles;
            int iterations = 0;
        }
    }
}

