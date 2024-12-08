#ifndef INSTRUCTION_HANDLER_H
#define INSTRUCTION_HANDLER_H

#include "cpu/cpu.h"
#include "memory/mem.h"
#include "cpu/instructions.h"
#include "utils/utils.h"

int executeInstruction(ZilogZ80_t *cpu, Memory_t *memory);

#endif // INSTRUCTION_HANDLER_H