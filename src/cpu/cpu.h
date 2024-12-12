#ifndef CILOG_C80_CPU_H
#define CILOG_C80_CPU_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "utils/utils.h"
#include "memory/mem.h"

/**
 * @brief Flag struct
 * 
 */
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

/**
 * @brief CPU struct
 * 
 */
typedef struct ZilogZ80_t
{   
    byte_t A, B, C, D, E, H, L;
    byte_t A_, B_, C_, D_, E_, H_, L_;

    F_t F;
    F_t F_;
    word_t SP, PC;

    word_t IX, IY;
    byte_t I, R;

    void (*outputCallback[256])(byte_t value);

    int cyclesInFrame;
    float frequency;

    int currentCycles;
    int totalCycles;

    bool isHaltered;

    Memory_t memory;
    Memory_t ram;
    Memory_t rom;
} ZilogZ80_t;

/**
 * @brief Initialize the CPU
 * 
 * @param cpu The CPU to initialize
 */
void zilogZ80Init(ZilogZ80_t* cpu);

/**
 * @brief Resets the CPU
 * 
 * @param cpu The CPU to reset
 */
void zilogZ80Reset(ZilogZ80_t* cpu);

/**
 * @brief Fetches the next instruction and executes it
 * 
 * @param cpu 
 * @param memory 
 */
void zilogZ80Step(ZilogZ80_t* cpu);

#endif

