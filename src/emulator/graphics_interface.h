#ifndef GRAPHICS_INTERFACE_H
#define GRAPHICS_INTERFACE_H

#include "cpu.h"
#include "mem.h"

int graphicsInit(int argc, char **argv, CPU_t *cpu, Memory_t *memory);
void graphicsDestroy();


#endif // GRAPHICS_INTERFACE_H