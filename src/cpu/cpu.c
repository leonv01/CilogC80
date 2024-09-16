#include "cpu.h"
#include "instructions.h"
#include "time.h"

#define CYCLES_PER_FRAME(x) ((int) ((x * 1000000 ) / 60)) 
#define MAX_ITERATIONS 1000000

void cpuInit(CPU_t* cpu)
{
    if(cpu == NULL)
    {
        return;
    }

    *cpu = (CPU_t) {
        .A = 0x00,
        .B = 0x00,
        .C = 0x00,
        .D = 0x00,
        .E = 0x00,
        .H = 0x00,
        .L = 0x00,
        .SP = 0x0000,
        .PC = 0x0000,
        .IX = 0x0000,
        .IY = 0x0000,
        .I = 0x00,
        .R = 0x00,
        .F = (F_t) {
            .C = 0,
            .N = 0,
            .P = 0,
            ._ = 0,
            .H = 0,
            .Z = 0,
            .S = 0
        }
    };

    cpu->cyclesInFrame = 0;
    cpu->frequency = 3.5f;
}

void cpuReset(CPU_t* cpu)
{
    cpuInit(cpu);
}

void cpuEmulate(CPU_t* cpu, Memory_t* memory)
{
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    int cycles = 0;
    int iterations = 0;

    while(cycles < CYCLES_PER_FRAME(cpu->frequency) && iterations < MAX_ITERATIONS)
    {
        cycles += cpuExecute(cpu, memory);
        iterations++;
    }

    // Handle iterations limit reached
    if(iterations >= MAX_ITERATIONS)
    {
        // Handle error
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    long elapsedNs = (end.tv_sec - start.tv_sec) * 1000000000L +
                      (end.tv_nsec - start.tv_nsec);

    const long targetNs = 1000000000L / 60;

    if(elapsedNs < targetNs)
    {
        struct timespec sleepTime;
        sleepTime.tv_sec = 0;
        sleepTime.tv_nsec = targetNs - elapsedNs;
        nanosleep(&sleepTime, NULL);
    }
}

static int calculateParity(byte_t value)
{
    int count = 0;
    while(value)
    {
        count += value & 1;
        value >>= 1;
    }

    return (count % 2) == 0;
}

static void setFlags(CPU_t* cpu, byte_t regA, byte_t operand, word_t result, bool_t isSubstraction)
{
    cpu->F.Z = (result & 0xFF) == 0;
    cpu->F.S = (result & 0x80) >> 7;
    cpu->F.H = isSubstraction ? ((regA & 0x0F) - (operand & 0x0F)) < 0 : ((regA & 0x0F) + (operand & 0x0F)) > 0x0F;    cpu->F.P = ((regA ^ result) & (operand ^ result) & 0x80) != 0;
    cpu->F.P = calculateParity(result & 0xFF);
    cpu->F.N = isSubstraction;
    cpu->F.C = result > 0xFF;
}

static int mainInstructions(CPU_t* cpu, Memory_t* memory, byte_t instruction);
static int bitInstructions(CPU_t* cpu, Memory_t* memory, byte_t instruction);
static int ixInstructions(CPU_t* cpu, Memory_t* memory, byte_t instruction);
static int iyInstructions(CPU_t* cpu, Memory_t* memory, byte_t instruction);
static int miscInstructions(CPU_t* cpu, Memory_t* memory, byte_t instruction);

int cpuExecute(CPU_t* cpu, Memory_t* memory)
{
    int cycles = 0;
    byte_t instruction = fetchByte(memory, cpu->PC);
    cpu->PC++;

    typedef int (*InstructionHandler)(CPU_t*, Memory_t*, byte_t);
    InstructionHandler handler = NULL;

    switch(instruction)
    {
        case BIT_INSTRUCTIONS:
            handler = &bitInstructions;
            break;
        case IX_INSTRUCTIONS:
            handler = &ixInstructions;
            break;
        case IY_INSTRUCTIONS:
            handler = &iyInstructions;
            break;
        case MISC_INSTRUCTIONS:
            handler = &miscInstructions;
            break;
        default:
            handler = &mainInstructions;
            break;
    }

    if(handler != NULL)
    {
        cycles = handler(cpu, memory, instruction);
    }

    return cycles;    
}

static int mainInstructions(CPU_t* cpu, Memory_t* memory, byte_t instruction)
{
    int cycles = 0;

    word_t result;
    word_t address;
    byte_t operand;

    switch(instruction)
    {
        // NOP
        case NOP:
            cycles = 4;
            break;

        // ADD
        case ADD_A_n:
            operand = fetchByte(memory, cpu->PC);
            result = (word_t)(cpu->A + operand);
            setFlags(cpu, cpu->A, operand, result, false);
            cpu->A = result & 0xFF;
            cpu->PC++;
            cycles = 7;
            break;
        case ADD_A_HL:
            address = TO_WORD(cpu->H, cpu->L);
            operand = fetchByte(memory, address);
            result = (word_t)(cpu->A + operand);
            setFlags(cpu, cpu->A, operand, result, false);
            cpu->A = result & 0xFF;
            cycles = 7;
            break;
        case ADD_A_B:
            result = cpu->A + cpu->B;
            setFlags(cpu, cpu->A, cpu->B, result, false);
            cpu->A = result & 0xFF;
            cycles = 4;
            break;
        case ADD_A_C:
            result = cpu->A + cpu->C;
            setFlags(cpu, cpu->A, cpu->C, result, false);
            cpu->A = result & 0xFF;
            cycles = 4;
            break;
        case ADD_A_D:
            result = cpu->A + cpu->D;
            setFlags(cpu, cpu->A, cpu->D, result, false);
            cpu->A = result & 0xFF;
            cycles = 4;
            break;
        case ADD_A_E:
            result = cpu->A + cpu->E;
            setFlags(cpu, cpu->A, cpu->E, result, false);
            cpu->A = result & 0xFF;
            cycles = 4;
            break;
        case ADD_A_H:
            result = cpu->A + cpu->H;
            setFlags(cpu, cpu->A, cpu->H, result, false);
            cpu->A = result & 0xFF;
            cycles = 4;
            break;
        case ADD_A_L:
            result = cpu->A + cpu->L;
            setFlags(cpu, cpu->A, cpu->L, result, false);
            cpu->A = result & 0xFF;
            cycles = 4;
            break;
        case ADD_A_A:
            result = cpu->A + cpu->A;
            setFlags(cpu, cpu->A, cpu->A, result, false);
            cpu->A = result & 0xFF;
            cycles = 4;
            break;

        // SUB
        case SUB_n:
            operand = fetchByte(memory, cpu->PC);
            result = cpu->A - operand;
            setFlags(cpu, cpu->A, operand, result, true);
            cpu->A = result & 0xFF;
            cpu->PC++;
            cycles = 7;
            break;
        case SUB_HL:
            address = TO_WORD(cpu->H, cpu->L);
            operand = fetchByte(memory, address);
            result = cpu->A - operand;
            setFlags(cpu, cpu->A, operand, result, true);
            cpu->A = result & 0xFF;
            cycles = 7;
            break;
        case SUB_B:
            result = cpu->A - cpu->B;
            setFlags(cpu, cpu->A, cpu->B, result, true);
            cpu->A = result & 0xFF;
            cycles = 4;
            break;
        case SUB_C:
            result = cpu->A - cpu->C;
            setFlags(cpu, cpu->A, cpu->C, result, true);
            cpu->A = result & 0xFF;
            cycles = 4;
            break;
        case SUB_D:
            result = cpu->A - cpu->D;
            setFlags(cpu, cpu->A, cpu->D, result, true);
            cpu->A = result & 0xFF;
            cycles = 4;
            break;
        case SUB_E:
            result = cpu->A - cpu->E;
            setFlags(cpu, cpu->A, cpu->E, result, true);
            cpu->A = result & 0xFF;
            cycles = 4;
            break;
        case SUB_H:
            result = cpu->A - cpu->H;
            setFlags(cpu, cpu->A, cpu->H, result, true);
            cpu->A = result & 0xFF;
            cycles = 4;
            break;
        case SUB_L:
            result = cpu->A - cpu->L;
            setFlags(cpu, cpu->A, cpu->L, result, true);
            cpu->A = result & 0xFF;
            cycles = 4;
            break;
        case SUB_A:
            result = cpu->A - cpu->A;
            setFlags(cpu, cpu->A, cpu->A, result, true);
            cpu->A = result & 0xFF;
            cycles = 4;
            break;

        case LD_A_n:
            cpu->A = fetchByte(memory, cpu->PC);
            cpu->PC++;
            cycles = 7;
            break;
    }
}

static int bitInstructions(CPU_t* cpu, Memory_t* memory, byte_t instruction)
{
    return 0;
}
static int ixInstructions(CPU_t* cpu, Memory_t* memory, byte_t instruction)
{
    return 0;
}

static int iyInstructions(CPU_t* cpu, Memory_t* memory, byte_t instruction)
{
    return 0;
}

static int miscInstructions(CPU_t* cpu, Memory_t* memory, byte_t instruction)
{
    return 0;
}