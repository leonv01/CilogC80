#include "graphics_unit.h"

void tms9918Init(TMS9918 *unit)
{
    memoryInit(&(unit->vram), 0x0000, 0x4000);

    for(size_t idx = 0; idx < TMS_REGISTER_COUNT; idx++)
    {
        unit->registers[idx] = 0x00;
    }
}

void tms9918Destroy(TMS9918 *unit)
{
    if(unit != NULL)
    {
        memoryDestroy(&(unit->vram));
    }
}

void tms9918Reset(TMS9918 *unit)
{
    if(unit != NULL)
    {
        for(size_t idx = 0; idx < TMS_REGISTER_COUNT; idx++)
        {
            unit->registers[idx] = 0x00;
        }

        unit->vramAddress = 0x0000;
        unit->mode = GRAPHICS_I_MODE;
        
        memoryReset(&(unit->vram));
    }
}

void tms9918Step(TMS9918 *unit)
{
    if(unit != NULL)
    {
        
    }
}