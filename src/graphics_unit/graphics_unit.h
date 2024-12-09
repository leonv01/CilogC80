#ifndef GRAPHICS_UNIT_H
#define GRAPHICS_UNIT_H

#include "../utils/utils.h"
#include "../memory/mem.h"

#define TMS_REGISTER_COUNT 8

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
    byte_t registers[TMS_REGISTER_COUNT];
    /* -------------------------------------------------------------------------- */

    Memory_t vram;

    TMS9918_Mode mode;
} TMS9918;

void tms9918Init(TMS9918 *unit);


#endif //GRAPHICS_UNIT_H