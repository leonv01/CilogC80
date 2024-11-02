#include "emulator.h"

#include "cpu.h"
#include "memory.h"
#include "utils.h"
#include "stdbool.h"
#include "graphics.h"
#include "file_reader.h"

static CPU_t        cpu;
static Memory_t     memory;
static bool         emulationRunning = false;

static void emulationTimeout()
{
    // TODO
}

void emulatorInit(int argc, char** argv)
{
    cpuInit(&cpu);
    memoryInit(&memory);

    int status = loadFile(&memory, "../roms/ascii_chars.z80");

    if(status != 0)
    {
        printf("Error loading file\n");
        return;
    }
    else
    {
        printf("File loaded successfully\n");
    }

    graphicsInit(argc, argv);
}

void emulatorRun()
{
    while(emulationRunning == true)
    {
        //cpuStep(&cpu, &memory);
    }
}

void emulatorStop()
{
    emulationRunning = false;
}

void emulatorReset()
{
    cpuReset(&cpu);
    // memoryReset(&memory);
}

void emulatorStep()
{
    // cpuStep(&cpu, &memory);
}

void emulatorStepOver()
{
    //TODO
}

void emulatorStepOut()
{
    //TODO
}

void emulatorPause()
{
    //TODO
}

void emulatorResume()
{
    //TODO
}
