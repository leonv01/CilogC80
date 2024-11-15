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

    memory->ram = (byte_t*) malloc(sizeof(byte_t) * RAM_SIZE);
    memory->rom = (byte_t*) malloc(sizeof(byte_t) * ROM_SIZE);

    if(memory->ram == NULL || memory->rom == NULL)
    {
        setError(C80_ERROR_MEMORY_INIT_ERROR);
        return;
    }

    memset(memory->ram, 0x00, RAM_SIZE);
    memset(memory->rom, 0x00, ROM_SIZE);
    
    memory->ramSize = RAM_SIZE;
    memory->romSize = ROM_SIZE;
}

void memoryDestroy(Memory_t* memory)
{
    if(memory == NULL || memory->ram == NULL || memory->rom == NULL)
    {
        setError(C80_ERROR_MEMORY_DESTROY_ERROR);
        return;
    }

    free(memory->ram);
    memory->ram = NULL;

    free(memory->rom);
    memory->rom = NULL;
}

byte_t fetchByte(Memory_t* memory, word_t address)
{
    byte_t ret;
    if(memory == NULL || memory->ram == NULL || memory->rom == NULL)
    {
        setError(C80_ERROR_MEMORY_FETCH_BYTE_ERROR);
        ret = 0x00;
    }
    else
    {
        if(address < ROM_SIZE)
        {
            ret = memory->rom[address];
        }
        else
        {
            ret = memory->ram[address - ROM_SIZE];
        }
    }

    return ret;
}

word_t fetchWord(Memory_t* memory, word_t address)
{
    word_t ret;
    if(memory == NULL || memory->ram == NULL || memory->rom == NULL)
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
    if(memory == NULL || memory->ram == NULL || memory->rom == NULL)
    {
        return;
    }
    else
    {
        if(address < ROM_SIZE)
        {
            memory->rom[address] = value;
            setError(C80_ERROR_MEMORY_STORE_BYTE_ERROR);
        }
        else
        {
            memory->ram[address - ROM_SIZE] = value;
        }
    }
}

void storeWord(Memory_t* memory, word_t address, word_t value)
{
    if(memory == NULL || memory->ram == NULL || memory->rom == NULL)
    {
        return;
    }
    else
    {
        storeByte(memory, address, value & 0xFF);
        storeByte(memory, address + 1, value >> 8);
    }
}