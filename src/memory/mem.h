#ifndef CILOG_C80_MEMORY_H
#define CILOG_C80_MEMORY_H

#include <stdint.h>
#include <stdlib.h>

#include "utils/utils.h"

#define MEMORY_SIZE 0x10000
#define ROM_SIZE 0x2000
#define RAM_SIZE MEMORY_SIZE - ROM_SIZE

typedef struct Memory_t
{
    byte_t* data;
    
    size_t ramSize;
    size_t romSize;

    byte_t* io;
    size_t ioSize;

} Memory_t;

/**
 * @brief Initializes the memory
 * 
 * @param memory 
 */
void memoryInit(Memory_t* memory);

/**
 * @brief Destroys the memory objects
 * 
 * @param memory 
 */
void memoryDestroy(Memory_t* memory);

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

#endif //CILOG_C80_MEMORY_H