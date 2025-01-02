#ifndef GRAPHICS_UNIT_H
#define GRAPHICS_UNIT_H

#include "../utils/utils.h"
#include "../memory/mem.h"

#define TMS_REGISTER_COUNT 8

#define TMS_SPRITE_COUNT 32


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

    /** @brief VRAM memory */
    Memory_t vram;
    /** @brief VRAM address (auto-incremented) */
    word_t vramAddress;

    TMS9918_Mode mode;
} TMS9918;

void tms9918Init(TMS9918 *unit);
void tms9918Destroy(TMS9918 *unit);
void tms9918Reset(TMS9918 *unit);

void tms9918Step(TMS9918 *unit);

void tms9918WriteRegister(TMS9918 *unit, byte_t reg, byte_t value);
byte_t tms9918ReadRegister(TMS9918 *unit, byte_t reg);

void tms9918WriteVRAM(TMS9918 *unit, word_t address, byte_t value);
byte_t tms9918ReadVRAM(TMS9918 *unit, word_t address);

void tms9918SetMode(TMS9918 *unit, TMS9918_Mode mode);


#endif //GRAPHICS_UNIT_H