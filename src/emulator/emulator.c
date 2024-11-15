#include "emulator.h"

#include <stdbool.h>

#include "cpu/cpu.h"
#include "memory/mem.h"
#include "utils/utils.h"
#include "utils/error_handler.h"
#include "emulator/graphics_interface.h"

// Enums
// -----------------------------------------------------------
typedef enum
{
    EMULATION_STATE_STOPPED = 0,
    EMULATION_STATE_RUNNING,
    EMULATION_STATE_PAUSED
} C80_EmulationState_t;


// Global variables
// -----------------------------------------------------------
static CPU_t                cpu;
static Memory_t             memory;
static C80_EmulationState_t emulationState = EMULATION_STATE_STOPPED;
// -----------------------------------------------------------

// Emulator functions
// -----------------------------------------------------------
void emulatorInit(int argc, char** argv);
// -----------------------------------------------------------

// Emulator function definitions
// -----------------------------------------------------------
void emulatorInit(int argc, char** argv)
{
    cpuInit(&cpu);
    memoryInit(&memory);
    errorStackInit();
    
    graphicsInit(argc, argv, &cpu, &memory);
}
// -----------------------------------------------------------