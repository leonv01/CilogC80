#ifndef CILOG_C80_CPU_H
#define CILOG_C80_CPU_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "utils/utils.h"
#include "memory/mem.h"

/**
 * @brief Flag struct containing all flags as bitfield
 */
typedef struct F_t
{
    /** @brief Carry flag (1 bit) */
    byte_t C : 1;
    /** @brief Add/Subtract flag (1 bit) */
    byte_t N : 1;
    /** @brief Parity flag (1 bit)*/
    byte_t P : 1;
    /** @brief Empty bits (2 bits) */
    byte_t _ : 2;
    /** @brief Half-Carry flag (1 bit) */
    byte_t H : 1;
    /** @brief Zero flag (1 bit) */
    byte_t Z : 1;
    /** @brief Sign flag (1 bit) */
    byte_t S : 1;
} F_t;

/**
 * @brief Enum struct for defining the interrupt status
 */
typedef enum InterruptStatus
{
    INTERRUPTS_ENABLED = 0,
    INTERRUPTS_DISABLED
} InterruptStatus;

/**
 * @brief Enum struct for defining all interrupt modes
 */
typedef enum InterruptMode
{
    INTERRUPT_MODE_0 = 0,
    INTERRUPT_MODE_1,
    INTERRUPT_MODE_2
} InterruptMode;

/**
 * @brief Zilog Z80 processor struct containing all registers and flags
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

    InterruptStatus interruptStatus;
    InterruptMode interruptMode;

    byte_t currentOpcode;

    int cyclesInFrame;
    float frequency;
    long frequencyFactor;

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

#endif // CILOG_C80_CPU_H

