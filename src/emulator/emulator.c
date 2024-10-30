#include "emulator.h"

#include "cpu.h"
#include "memory.h"
#include "utils.h"
#include "stdbool.h"

static CPU_t        cpu;
static Memory_t     memory;
static bool         emulationRunning = false;

void emulatorInit()
{
    cpuInit(&cpu);
    memoryInit(&memory);
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
