#include "mem.h"

#include <string.h>
#include <time.h>

#include "utils/error_handler.h"

void memoryInit(Memory_t* memory, size_t memoryStartAddress, size_t memorySize)
{
    memory->memoryStartAddress = memoryStartAddress;
    memory->memorySize = memorySize;

    memory->data = (byte_t*) malloc(sizeof(byte_t) * memorySize);

    if(memory->data == NULL)
    {
        setError(C80_ERROR_MEMORY_INIT_ERROR);
        return;
    }

    memset(memory->data, 0x00, memory->memorySize);
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

void memoryReset(Memory_t *memory)
{
    memset(memory->data, 0x00, memory->memorySize);
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
        memory->data[address] = value;
    }
}

void storeWord(Memory_t* memory, word_t address, word_t value)
{
    if(memory == NULL || memory->data == NULL)
    {
        setError(C80_ERROR_MEMORY_STORE_WORD_ERROR);
    }
    else if(address < memory->memorySize)
    {
        storeByte(memory, address, value & 0xFF);
        storeByte(memory, address + 1, value >> 8);
    }
    else
    {
        setError(C80_ERROR_MEMORY_STORE_WORD_ERROR);
    }
}

void loadProgramToRom(Memory_t *rom, byte_t *data, size_t programSize)
{
    if(rom == NULL || rom->data == NULL)
    {
        // TODO: Change error
        setError(C80_ERROR_MEMORY_STORE_BYTE_ERROR);
    }
    else
    {
        if(rom->memorySize <= programSize)
        {
            memcpy(rom->data, data, programSize);
        }
        else
        {
            // TODO: Change error
            setError(C80_ERROR_MEMORY_STORE_BYTE_ERROR);
        }
    }
}

void loadProgramToRam(Memory_t *ram, byte_t *data, size_t programSize)
{
    if(ram == NULL || ram->data == NULL)
    {
        // TODO: Change error
        setError(C80_ERROR_MEMORY_STORE_BYTE_ERROR);
    }
    else
    {
        if(ram->memorySize < programSize)
        {
            memcpy(ram->data, data, programSize);
        }
        else
        {
            // TODO: Change error
            setError(C80_ERROR_MEMORY_STORE_BYTE_ERROR);
        }
    }
}

byte_t fetchByteAddressSpace(Memory_t *ram, Memory_t *rom, word_t address)
{
    byte_t value = 0x00;
    if(ram == NULL || ram->data == NULL || rom == NULL || rom->data == NULL)
    {
        setError(C80_ERROR_MEMORY_FETCH_BYTE_ERROR);
    }
    else
    {
        // Check if address is in ROM bounds
        if(address >= rom->memoryStartAddress && address <= (rom->memoryStartAddress + rom->memorySize))
        {
            value = fetchByte(rom, address);
        }
        // Check if address is in RAM bounds
        else if(address >= ram->memoryStartAddress && address <= (ram->memoryStartAddress + ram->memorySize))
        {
            value = fetchByte(ram, address);
        }
        else
        {
            setError(C80_ERROR_MEMORY_FETCH_BYTE_ERROR);
        }
    }

    return value;
}

void storeByteAddressSpace(Memory_t *ram, Memory_t *rom, word_t address, byte_t value)
{
    if(ram == NULL || ram->data == NULL || rom == NULL || rom->data == NULL)
    {
        setError(C80_ERROR_MEMORY_STORE_BYTE_ERROR);
    }
    else
    {
        // Check if address is in RAM bounds
        if(address >= ram->memoryStartAddress && address <= (ram->memoryStartAddress + ram->memorySize))
        {
            storeByte(ram, address, value);
        }
        else
        {
            setError(C80_ERROR_MEMORY_FETCH_BYTE_ERROR);
        }
    }
}

word_t fetchWordAddressSpace(Memory_t *ram, Memory_t *rom, word_t address)
{
    word_t value;
    if(ram == NULL || ram->data == NULL || rom == NULL || rom->data == NULL)
    {
        setError(C80_ERROR_MEMORY_FETCH_WORD_ERROR);
    }
    else
    {
        // Check if address is in ROM bounds
        if(address >= rom->memoryStartAddress && address <= (rom->memoryStartAddress + rom->memorySize))
        {
            value = fetchWord(rom, address);
        }
        // Check if address is in RAM bounds
        else if(address >= ram->memoryStartAddress && address <= (ram->memoryStartAddress + ram->memorySize))
        {
            value = fetchWord(ram, address);
        }
        else
        {
            setError(C80_ERROR_MEMORY_FETCH_BYTE_ERROR);
        }
    }

    return value;
}

void storeWordAddressSpace(Memory_t *ram, Memory_t *rom, word_t address, word_t value)
{
    if(ram == NULL || ram->data == NULL || rom == NULL || rom->data == NULL)
    {
        setError(C80_ERROR_MEMORY_STORE_WORD_ERROR);
    }
    else
    {
        // Check if address is in RAM bounds
        if(address >= ram->memoryStartAddress && address <= (ram->memoryStartAddress + ram->memorySize))
        {
            storeWord(ram, address, value);
        }
        else
        {
            setError(C80_ERROR_MEMORY_FETCH_BYTE_ERROR);
        }
    }
}
