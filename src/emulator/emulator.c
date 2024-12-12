#include "emulator.h"

#include <stdbool.h>
#include <stdio.h>

#include "cpu/cpu.h"
#include "memory/mem.h"
#include "utils/error_handler.h"

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
static ZilogZ80_t                cpu;
static Memory_t             memory;
static C80_EmulationState_t emulationState = EMULATION_STATE_STOPPED;
// -----------------------------------------------------------

// Emulator functions
// -----------------------------------------------------------
void emulatorInit(int argc, char** argv);
void outputPrint(byte_t value);
// -----------------------------------------------------------

// Emulator function definitions
// -----------------------------------------------------------
void emulatorInit(int argc, char** argv)
{
    zilogZ80Init(&cpu);
    cpu.outputCallback[0x01] = &outputPrint;
    errorStackInit();

    #if !defined(HEADLESS)
    graphicsInit(argc, argv, &cpu);
    #endif
}

void outputPrint(byte_t value)
{
    printf("Port: 0x01X -> %c | 0x%02X\n", (char)value, value);
}
// -----------------------------------------------------------