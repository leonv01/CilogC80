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
    /** @brief Accumulator / Register A */
    byte_t A;
    /** @brief Register B */
    byte_t B;
    /** @brief Register C */
    byte_t C;
    /** @brief Register D */
    byte_t D;
    /** @brief Register E */
    byte_t E;
    /** @brief Register H */
    byte_t H;
    /** @brief Register L */
    byte_t L;

    /** @brief Shadow Accumulator / Register A */
    byte_t A_;
    /** @brief Shadow Register B */
    byte_t B_;
    /** @brief Shadow Register C */
    byte_t C_;
    /** @brief Shadow Register D */
    byte_t D_;
    /** @brief Shadow Register E */
    byte_t E_;
    /** @brief Shadow Register H */
    byte_t H_;
    /** @brief Shadow Register L */
    byte_t L_;  

    /** @brief Flags */
    F_t F;
    /** @brief Shadow Flags */
    F_t F_;

    /** @brief Stack Pointer */
    word_t SP;
    /** @brief Program Counter */           
    word_t PC;

    /** @brief Index Register X */
    word_t IX;
    /** @brief Index Register Y */
    word_t IY;

    byte_t I, R;

    /** @brief Input callback */
    void (*inputCallback[256])(byte_t* value);
    /** @brief Output callback */
    void (*outputCallback[256])(byte_t value);

    int cyclesInFrame;
    float frequency;

    int currentCycles;
    int totalCycles;

    bool isHaltered;

    /** @brief RAM memory */
    Memory_t ram;
    /** @brief ROM memory */
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

