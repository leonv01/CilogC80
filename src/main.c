#include <stdio.h>

#include "cpu.h"
#include "mem.h"
#include "instructions.h"
#include "instruction_handler.h"
#include "file_loader.h"

int main() {
    Memory_t memory;
    memoryInit(&memory);

    int err = loadFile(&memory, "../roms/test.z80");

    if(err != 0) {
        printf("Error loading file\n");
        return 1;
    }

    for(int i = 0; i < 10; i++) {
        printf("%02X", memory.data[i]);
    }

    return 0;
}
