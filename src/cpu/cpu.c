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

    memoryInit(&cpu->memory, 0x0000, 0x10000);
    memoryInit(&cpu->rom, 0x0000, 0x4000);
    memoryInit(&cpu->ram, 0x8000, 0x8000);
}

void zilogZ80Reset(ZilogZ80_t *cpu)
{
    cpu->A = 0x00;
    cpu->B = 0x00;
    cpu->C = 0x00;
    cpu->D = 0x00;
    cpu->E = 0x00;
    cpu->H = 0x00;
    cpu->L = 0x00;
    cpu->SP = 0x0000;
    cpu->PC = 0x0000;
    cpu->IX = 0x0000;
    cpu->IY = 0x0000;
    cpu->I = 0x00;
    cpu->R = 0x00;
    cpu->F = (F_t){
            .C = 0,
            .N = 0,
            .P = 0,
            ._ = 0,
            .H = 0,
            .Z = 0,
            .S = 0};

    cpu->cyclesInFrame = 0;
    cpu->frequency = 3.5f;

    cpu->isHaltered = false;
}

void zilogZ80Step(ZilogZ80_t *cpu)
{
    if(cpu->isHaltered == false)
    {
        int cycles = executeInstruction(cpu);
        cpu->cyclesInFrame -= cycles;
        cpu->totalCycles += cycles;
        int iterations = 0;
    }
}

