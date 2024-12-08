#ifndef GRAPHICS_UNIT_H
#define GRAPHICS_UNIT_H

#include "../utils/utils.h"
#include "../memory/mem.h"

typedef enum TMS9918_Mode
{
    GRAPHICS_I_MODE = 0,
    GRAPHICS_II_MODE,
    MULTICOLOR_MODE,
    TEXT_MODE
} TMS9918_Mode;

typedef struct TMS9918
{
    /* --------------------------- Register definition -------------------------- */
    byte_t register1;
    byte_t register2;
    byte_t register3;
    byte_t register4;
    byte_t register5;
    byte_t register6;
    byte_t register7;
    /* -------------------------------------------------------------------------- */

    Memory_t vram;

    TMS9918_Mode mode;
} TMS9918;

void TMS9918Init(TMS9918 *unit);

#endif //GRAPHICS_UNIT_H