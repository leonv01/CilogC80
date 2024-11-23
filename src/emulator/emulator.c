#include "emulator.h"

#include <stdbool.h>

#include "cpu/cpu.h"
#include "memory/mem.h"
#include "utils/utils.h"
#include "utils/error_handler.h"
#include "file_grabber.h"

#if !defined(HEADLESS)
#include "emulator/graphics_interface.h"
#endif

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

    if(loadFile("C:\\Users\\leonv\\Documents\\CilogC80\\asm\\sub_test.bin", memory.rom, memory.romSize) == false)
    {
        printf("Error loading file\n");
        exit(-1);
    }
    
    #if !defined(HEADLESS)
    graphicsInit(argc, argv, &cpu, &memory);
    #endif
}
// -----------------------------------------------------------