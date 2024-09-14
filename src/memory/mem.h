#ifndef CILOG_C80_MEMORY_H
#define CILOG_C80_MEMORY_H

#include <stdint.h>
#include <stdlib.h>

#include "utils.h"

#define MEMORY_SIZE 0x10000

typedef struct Memory_t
{
    byte_t* data;
} Memory_t;

void memoryInit(Memory_t* memory);
void memoryDestroy(Memory_t* memory);

byte_t fetchByte(Memory_t* memory, word_t address);
word_t fetchWord(Memory_t* memory, word_t address);

void storeByte(Memory_t* memory, word_t address, byte_t value);
void storeWord(Memory_t* memory, word_t address, word_t value);

#endif //CILOG_C80_MEMORY_H