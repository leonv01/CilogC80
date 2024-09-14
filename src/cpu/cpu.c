#include "cpu.h"

void cpuInit(CPU_t* cpu)
{
    if(cpu == NULL)
    {
        return;
    }

    *cpu = (CPU_t) {
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
        .F = (F_t) {
            .C = 0,
            .N = 0,
            .P = 0,
            ._ = 0,
            .H = 0,
            .Z = 0,
            .S = 0
        }
    };
}

void cpuReset(CPU_t* cpu)
{
    cpuInit(cpu);
}

void cpuStep(CPU_t* cpu)
{

}