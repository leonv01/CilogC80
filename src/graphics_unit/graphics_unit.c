#include "graphics_unit.h"

void tms9918Init(TMS9918 *unit)
{
    memoryInit(&(unit->vram), 0x0000, 0x4000);

    for(size_t idx = 0; idx < TMS_REGISTER_COUNT; idx++)
    {
        unit->registers[idx] = 0x00;
    }
}