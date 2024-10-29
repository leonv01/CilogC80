#ifndef INSTRUCTION_HANDLER_H
#define INSTRUCTION_HANDLER_H

#include "cpu.h"
#include "mem.h"
#include "instructions.h"
#include "utils.h"

int executeInstruction(CPU_t *cpu, Memory_t *memory);

#endif // INSTRUCTION_HANDLER_H