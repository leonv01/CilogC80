#ifndef GRAPHICS_INTERFACE_H
#define GRAPHICS_INTERFACE_H

#include "cpu.h"
#include "mem.h"

/**
 * @brief Initializes the graphical interface
 * 
 * @param argc 
 * @param argv 
 * @param cpu 
 * @param memory 
 * @return int 
 */
int graphicsInit(int argc, char **argv, CPU_t *cpu, Memory_t *memory);

/**
 * @brief Destroys the graphical interface
 * 
 */
void graphicsDestroy();


#endif // GRAPHICS_INTERFACE_H