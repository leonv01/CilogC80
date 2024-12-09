#ifndef GRAPHICS_INTERFACE_H
#define GRAPHICS_INTERFACE_H

#include "cpu/cpu.h"
#include "memory/mem.h"

/**
 * @brief Initializes the graphical interface
 * 
 * @param argc 
 * @param argv 
 * @param cpu 
 * @param memory 
 * @return int 
 */
int graphicsInit(int argc, char **argv, ZilogZ80_t *cpu);

/**
 * @brief Destroys the graphical interface
 * 
 */
void graphicsDestroy();


#endif // GRAPHICS_INTERFACE_H