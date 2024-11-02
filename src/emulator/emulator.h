#ifndef EMULATOR_H
#define EMULATOR_H

void emulatorInit(int argc, char** argv);
void emulatorRun();
void emulatorStop();
void emulatorReset();
void emulatorStep();
void emulatorStepOver();
void emulatorStepOut();
void emulatorPause();
void emulatorResume();

#endif // EMULATOR_H