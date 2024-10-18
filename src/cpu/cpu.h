#ifndef CILOG_C80_CPU_H
#define CILOG_C80_CPU_H

#include <stdint.h>
#include <stdlib.h>

#include "utils.h"
#include "mem.h"

typedef struct F_t
{
    byte_t C : 1;
    byte_t N : 1;
    byte_t P : 1;
    byte_t _ : 2;
    byte_t H : 1;
    byte_t Z : 1;
    byte_t S : 1;
} F_t;

typedef struct CPU_t
{   byte_t A, B, C, D, E, H, L;
    F_t F;
    word_t SP, PC;

    word_t IX, IY;
    byte_t I, R;

    int cyclesInFrame;
    float frequency;
} CPU_t;

/**
 * @brief Initialize the CPU
 * 
 * @param cpu The CPU to initialize
 */
void cpuInit(CPU_t* cpu);

/**
 * @brief Resets the CPU
 * 
 * @param cpu The CPU to reset
 */
void cpuReset(CPU_t* cpu);

/**
 * @brief Start the CPU emulation
 * 
 * @param cpu 
 * @param memory 
 */
void cpuEmulate(CPU_t* cpu, Memory_t* memory);

/**
 * @brief Execute one step of the CPU
 * 
 * @param cpu 
 * @param memory 
 * @return int 
 */
int cpuExecute(CPU_t* cpu, Memory_t* memory);

#endif

