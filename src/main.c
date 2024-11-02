#include <stdio.h>

#include "emulator.h"

int main(int argc, char *argv[]) 
{
    emulatorInit(argc, argv);

    emulatorRun();
}
