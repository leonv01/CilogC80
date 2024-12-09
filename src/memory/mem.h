#ifndef CILOG_C80_MEMORY_H
#define CILOG_C80_MEMORY_H

#include <sal.h>
#include <stdint.h>
#include <stdlib.h>

#include "utils/utils.h"

typedef struct Memory_t
{
    byte_t* data;

    size_t memoryStartAddress;
    size_t memorySize;
} Memory_t;

/**
 * @brief Initializes the memory
 * 
 * @param memory 
 */
void memoryInit(Memory_t* memory, size_t memoryStartAddress, size_t memorySize);

/**
 * @brief Destroys the memory object
 * 
 * @param memory 
 */
void memoryDestroy(Memory_t* memory);

/**
 * @brief Clears the memory object
 * 
 * @param memory 
 */
void memoryReset(Memory_t *memory);

/**
 * @brief Fetches byte from memory, depending on the address | Sets error if the address is invalid
 * @param memory 
 * @param address 
 * @return byte_t Returns:
 * - 0: If the address is invalid
 * - Byte value in ROM: 0 <= address < ROM_SIZE
 * - Byte value in RAM: ROM_SIZE <= address < MEMORY_SIZE
 */
byte_t fetchByte(Memory_t* memory, word_t address);

/**
 * @brief Fetches word from memory, depending on the address | Sets error if the address is invalid | 
 * HB = address, LB = address + 1
 * @param memory 
 * @param address 
 * @return word_t 
 * - 0: If the address is invalid
 * - Word value in ROM: 0 <= address < ROM_SIZE
 * - Word value in RAM: ROM_SIZE <= address < MEMORY_SIZE
 */
word_t fetchWord(Memory_t* memory, word_t address);

/**
 * @brief Stores byte in memory, depending on the address | Sets error if the address is invalid
 * 
 * @param memory 
 * @param address 
 * @param value 
 */
void storeByte(Memory_t* memory, word_t address, byte_t value);

/**
 * @brief Stores word in memory, depending on the address | Sets error if the address is invalid |
 * HB = address, LB = address + 1
 * @param memory 
 * @param address 
 * @param value 
 */
void storeWord(Memory_t* memory, word_t address, word_t value);

void loadProgramToRom(Memory_t *rom, byte_t *program, size_t programSize);

void loadProgramToRam(Memory_t *ram, byte_t *program, size_t programSize);

byte_t fetchByteAddressSpace(Memory_t *ram, Memory_t *rom, word_t address);
void storeByteAddressSpace(Memory_t *ram, Memory_t *rom, word_t address, byte_t value);

word_t fetchWordAddressSpace(Memory_t *ram, Memory_t *rom, word_t address);
void storeWordAddressSpace(Memory_t *ram, Memory_t *rom, word_t address, word_t value);

#endif //CILOG_C80_MEMORY_H