#include "mem.h"

#include <string.h>

#include "utils/error_handler.h"

void memoryInit(Memory_t* memory)
{
    if(memory == NULL)
    {
        setError(C80_ERROR_MEMORY_INIT_ERROR);
        return;
    }

    memory->data = (byte_t*) malloc(sizeof(byte_t) * MEMORY_SIZE);

    if(memory->data == NULL)
    {
        setError(C80_ERROR_MEMORY_INIT_ERROR);
        return;
    }

    memset(memory->data, 0x00, MEMORY_SIZE);
    
    memory->ramSize = RAM_SIZE;
    memory->romSize = ROM_SIZE;
}

void memoryDestroy(Memory_t* memory)
{
    if(memory == NULL || memory->data == NULL)
    {
        setError(C80_ERROR_MEMORY_DESTROY_ERROR);
        return;
    }

    free(memory->data);
}

byte_t fetchByte(Memory_t* memory, word_t address)
{
    byte_t ret;
    if(memory == NULL || memory->data == NULL)
    {
        setError(C80_ERROR_MEMORY_FETCH_BYTE_ERROR);
        ret = 0x00;
    }
    else
    {
        return memory->data[address];
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
        ret = fetchByte(memory, address);
        ret |= fetchByte(memory, address + 1) << 8;
    }

    return ret;
}

void storeByte(Memory_t* memory, word_t address, byte_t value)
{
    if(memory == NULL || memory->data == NULL)
    {
        return;
    }
    else
    {
        if(address < ROM_SIZE)
        {
            setError(C80_ERROR_MEMORY_STORE_BYTE_ERROR);
        }
        memory->data[address] = value;
    }
}

void storeWord(Memory_t* memory, word_t address, word_t value)
{
    if(memory == NULL || memory->data == NULL)
    {
        return;
    }
    else
    {
        storeByte(memory, address, value & 0xFF);
        storeByte(memory, address + 1, value >> 8);
    }
}