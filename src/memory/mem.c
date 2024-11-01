#include "mem.h"
#include <string.h>

void memoryInit(Memory_t* memory)
{
    if(memory == NULL)
    {
        return;
    }

    memory->data = (byte_t*) malloc(sizeof(byte_t) * MEMORY_SIZE);

    if(memory->data == NULL)
    {
        return;
    }

    memset(memory->data, 0x00, MEMORY_SIZE);
}

void memoryDestroy(Memory_t* memory)
{
    if(memory == NULL || memory->data == NULL)
    {
        return;
    }

    free(memory->data);
    memory->data = NULL;
}

byte_t fetchByte(Memory_t* memory, word_t address)
{
    byte_t ret;
    if(memory == NULL || memory->data == NULL)
    {
        ret = 0x00;
    }
    else
    {
        ret = memory->data[address];
    }

    return ret;
}

word_t fetchWord(Memory_t* memory, word_t address)
{
    word_t ret;
    if(memory == NULL || memory->data == NULL)
    {
        ret = 0x0000;
    }
    else
    {
        ret = TO_WORD(memory->data[address + 1], memory->data[address]);
    }

    return ret;
}

void storeByte(Memory_t* memory, word_t address, byte_t value)
{
    if(memory == NULL || memory->data == NULL)
    {
        return;
    }

    memory->data[address] = value;
}

void storeWord(Memory_t* memory, word_t address, word_t value);