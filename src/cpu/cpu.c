#include "cpu.h"
#include "instructions.h"
#include "time.h"

#define CYCLES_PER_FRAME(x) ((int)((x * 1000000) / 60))
#define MAX_ITERATIONS 1000000

#define MAX_INSTRUCTION_COUNT 0xFF

typedef int (*InstructionHandler)(CPU_t *, Memory_t *, byte_t);
static InstructionHandler mainInstructionTable[MAX_INSTRUCTION_COUNT] = {NULL};

static void initInstructionTable();

static int instructionNop(CPU_t *cpu, Memory_t *memory, byte_t instruction);

static int instructionAdd(CPU_t *cpu, Memory_t *memory, byte_t instruction);
static int instructionAdc(CPU_t *cpu, Memory_t *memory, byte_t instruction);
static int instructionInc(CPU_t *cpu, Memory_t *memory, byte_t instruction);

static int instructionSub(CPU_t *cpu, Memory_t *memory, byte_t instruction);
static int instructionSbc(CPU_t *cpu, Memory_t *memory, byte_t instruction);
static int instructionDec(CPU_t *cpu, Memory_t *memory, byte_t instruction);

static int instructionAnd(CPU_t *cpu, Memory_t *memory, byte_t instruction);
static int instructionOr(CPU_t *cpu, Memory_t *memory, byte_t instruction);
static int instructionXor(CPU_t *cpu, Memory_t *memory, byte_t instruction);

static int instructionLd(CPU_t *cpu, Memory_t *memory, byte_t instruction);

static int addToRegister(CPU_t *cpu, byte_t value);
static int addToRegisterWithCarry(CPU_t *cpu, byte_t value);
static int incrementRegister(CPU_t *cpu, byte_t *reg);

static int subtractFromRegister(CPU_t *cpu, byte_t value);
static int subtractFromRegisterWithCarry(CPU_t *cpu, byte_t value);
static int decrementRegister(CPU_t *cpu, byte_t *reg);

static int andWithRegister(CPU_t *cpu, byte_t value);
static int orWithRegister(CPU_t *cpu, byte_t value);
static int xorWithRegister(CPU_t *cpu, byte_t value);

void cpuInit(CPU_t *cpu)
{
    if (cpu == NULL)
    {
        return;
    }

    *cpu = (CPU_t){
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
        .F = (F_t){
            .C = 0,
            .N = 0,
            .P = 0,
            ._ = 0,
            .H = 0,
            .Z = 0,
            .S = 0}};

    cpu->cyclesInFrame = 0;
    cpu->frequency = 3.5f;

    initInstructionTable();
}

void cpuReset(CPU_t *cpu)
{
    cpuInit(cpu);
}

void cpuEmulate(CPU_t *cpu, Memory_t *memory)
{
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    int cycles = 0;
    int iterations = 0;

    while (cycles < CYCLES_PER_FRAME(cpu->frequency) && iterations < MAX_ITERATIONS)
    {
        cycles += cpuExecute(cpu, memory);
        iterations++;
    }

    // Handle iterations limit reached
    if (iterations >= MAX_ITERATIONS)
    {
        // Handle error
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    long elapsedNs = (end.tv_sec - start.tv_sec) * 1000000000L +
                     (end.tv_nsec - start.tv_nsec);

    const long targetNs = 1000000000L / 60;

    if (elapsedNs < targetNs)
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
    while (value)
    {
        count += value & 1;
        value >>= 1;
    }

    return (count % 2) == 0;
}

static void setFlags(CPU_t *cpu, byte_t regA, byte_t operand, word_t result, bool_t isSubstraction)
{
    cpu->F.Z = (result & 0xFF) == 0;
    cpu->F.S = (result & 0x80) >> 7;
    cpu->F.H = isSubstraction ? ((regA & 0x0F) - (operand & 0x0F)) < 0 : ((regA & 0x0F) + (operand & 0x0F)) > 0x0F;
    cpu->F.P = calculateParity(result & 0xFF);
    cpu->F.N = isSubstraction;
    cpu->F.C = result > 0xFF;
}

static int mainInstructions(CPU_t *cpu, Memory_t *memory, byte_t instruction);
static int bitInstructions(CPU_t *cpu, Memory_t *memory, byte_t instruction);
static int ixInstructions(CPU_t *cpu, Memory_t *memory, byte_t instruction);
static int iyInstructions(CPU_t *cpu, Memory_t *memory, byte_t instruction);
static int miscInstructions(CPU_t *cpu, Memory_t *memory, byte_t instruction);

int cpuExecute(CPU_t *cpu, Memory_t *memory)
{
    int cycles = 0;
    byte_t instruction = fetchByte(memory, cpu->PC);
    cpu->PC++;

    typedef int (*InstructionHandler)(CPU_t *, Memory_t *, byte_t);
    InstructionHandler handler = NULL;

    switch (instruction)
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

    if (handler != NULL)
    {
        cycles = handler(cpu, memory, instruction);
    }

    return cycles;
}

static void initInstructionTable()
{
    for(int entry = 0; entry < MAX_INSTRUCTION_COUNT; entry++)
    {
        mainInstructionTable[entry] = NULL;
    }

    // NOP
    mainInstructionTable[NOP] = &instructionNop;

    // Add instructions
    mainInstructionTable[ADD_A_A] = &instructionAdd;
    mainInstructionTable[ADD_A_B] = &instructionAdd;
    mainInstructionTable[ADD_A_C] = &instructionAdd;
    mainInstructionTable[ADD_A_D] = &instructionAdd;
    mainInstructionTable[ADD_A_E] = &instructionAdd;
    mainInstructionTable[ADD_A_H] = &instructionAdd;
    mainInstructionTable[ADD_A_L] = &instructionAdd;
    mainInstructionTable[ADD_A_HL] = &instructionAdd;
    mainInstructionTable[ADD_A_n] = &instructionAdd;

    // ADC instructions
    mainInstructionTable[ADC_A_A] = &instructionAdc;
    mainInstructionTable[ADC_A_B] = &instructionAdc;
    mainInstructionTable[ADC_A_C] = &instructionAdc;
    mainInstructionTable[ADC_A_D] = &instructionAdc;
    mainInstructionTable[ADC_A_E] = &instructionAdc;
    mainInstructionTable[ADC_A_H] = &instructionAdc;
    mainInstructionTable[ADC_A_L] = &instructionAdc;
    mainInstructionTable[ADC_A_HL] = &instructionAdc;
    mainInstructionTable[ADC_A_n] = &instructionAdc;

    // INC instructions
    mainInstructionTable[INC_A] = &instructionInc;
    mainInstructionTable[INC_B] = &instructionInc;
    mainInstructionTable[INC_C] = &instructionInc;
    mainInstructionTable[INC_D] = &instructionInc;
    mainInstructionTable[INC_E] = &instructionInc;
    mainInstructionTable[INC_H] = &instructionInc;
    mainInstructionTable[INC_L] = &instructionInc;
    mainInstructionTable[INC_HL] = &instructionInc;

    // Sub instructions
    mainInstructionTable[SUB_A_A] = &instructionSub;
    mainInstructionTable[SUB_A_B] = &instructionSub;
    mainInstructionTable[SUB_A_C] = &instructionSub;
    mainInstructionTable[SUB_A_D] = &instructionSub;
    mainInstructionTable[SUB_A_E] = &instructionSub;
    mainInstructionTable[SUB_A_H] = &instructionSub;
    mainInstructionTable[SUB_A_L] = &instructionSub;
    mainInstructionTable[SUB_A_HL] = &instructionSub;
    mainInstructionTable[SUB_A_n] = &instructionSub;

    // SBC instructions
    mainInstructionTable[SBC_A_A] = &instructionSbc;
    mainInstructionTable[SBC_A_B] = &instructionSbc;
    mainInstructionTable[SBC_A_C] = &instructionSbc;
    mainInstructionTable[SBC_A_D] = &instructionSbc;
    mainInstructionTable[SBC_A_E] = &instructionSbc;
    mainInstructionTable[SBC_A_H] = &instructionSbc;
    mainInstructionTable[SBC_A_L] = &instructionSbc;
    mainInstructionTable[SBC_A_HL] = &instructionSbc;
    mainInstructionTable[SBC_A_n] = &instructionSbc;

    // DEC instructions
    mainInstructionTable[DEC_A] = &instructionDec;
    mainInstructionTable[DEC_B] = &instructionDec;
    mainInstructionTable[DEC_C] = &instructionDec;
    mainInstructionTable[DEC_D] = &instructionDec;
    mainInstructionTable[DEC_E] = &instructionDec;
    mainInstructionTable[DEC_H] = &instructionDec;
    mainInstructionTable[DEC_L] = &instructionDec;
    mainInstructionTable[DEC_HL] = &instructionDec;

    // AND instructions
    mainInstructionTable[AND_A] = &instructionAnd;
    mainInstructionTable[AND_B] = &instructionAnd;
    mainInstructionTable[AND_C] = &instructionAnd;
    mainInstructionTable[AND_D] = &instructionAnd;
    mainInstructionTable[AND_E] = &instructionAnd;
    mainInstructionTable[AND_H] = &instructionAnd;
    mainInstructionTable[AND_L] = &instructionAnd;
    mainInstructionTable[AND_HL] = &instructionAnd;
    mainInstructionTable[AND_n] = &instructionAnd;

    // OR instructions
    mainInstructionTable[OR_A] = &instructionOr;
    mainInstructionTable[OR_B] = &instructionOr;
    mainInstructionTable[OR_C] = &instructionOr;
    mainInstructionTable[OR_D] = &instructionOr;
    mainInstructionTable[OR_E] = &instructionOr;
    mainInstructionTable[OR_H] = &instructionOr;
    mainInstructionTable[OR_L] = &instructionOr;
    mainInstructionTable[OR_HL] = &instructionOr;
    mainInstructionTable[OR_n] = &instructionOr;

    // XOR instructions
    mainInstructionTable[XOR_A] = &instructionXor;
    mainInstructionTable[XOR_B] = &instructionXor;
    mainInstructionTable[XOR_C] = &instructionXor;
    mainInstructionTable[XOR_D] = &instructionXor;
    mainInstructionTable[XOR_E] = &instructionXor;
    mainInstructionTable[XOR_H] = &instructionXor;
    mainInstructionTable[XOR_L] = &instructionXor;
    mainInstructionTable[XOR_HL] = &instructionXor;
    mainInstructionTable[XOR_n] = &instructionXor;


}

static int mainInstructions(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    byte_t cycles = mainInstructionTable[instruction](cpu, memory, instruction);
}

static int bitInstructions(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    return 0;
}
static int ixInstructions(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    return 0;
}

static int iyInstructions(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    return 0;
}

static int miscInstructions(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    int cycles = 0;

    word_t result;
    word_t address;
    byte_t operand;

    switch (instruction)
    {
    case MISC_LD_DE_IMMEDIATE:
        address = TO_WORD(cpu->D, cpu->E);
        operand = fetchByte(memory, cpu->PC);
        storeByte(memory, address, operand);
        cpu->PC++;
        cycles = 10;
        break;
    case MISC_LD_BC_IMMEDIATE:
        address = TO_WORD(cpu->B, cpu->C);
        operand = fetchByte(memory, cpu->PC);
        storeByte(memory, address, operand);
        cpu->PC++;
        cycles = 10;
        break;
    case MISC_LD_SP_IMMEDIATE:
        address = fetchWord(memory, cpu->PC);
        operand = fetchByte(memory, cpu->PC);
        storeByte(memory, address, operand);
        cpu->PC += 2;
        cycles = 10;
        break;
    }
    return 0;
}

static int addToRegister(CPU_t *cpu, byte_t value)
{
    word_t result = (word_t)(cpu->A + value);
    setFlags(cpu, cpu->A, value, result, false);
    cpu->A = result & 0xFF;
    return result;
}
static int addToRegisterWithCarry(CPU_t *cpu, byte_t value)
{
    word_t result = (word_t)(cpu->A + value + cpu->F.C);
    setFlags(cpu, cpu->A, value, result, false);
    cpu->A = result & 0xFF;
    return result;
}
static int incrementRegister(CPU_t *cpu, byte_t *reg)
{
    word_t result = (word_t)(*reg + 1);
    setFlags(cpu, *reg, 1, result, false);
    *reg = result & 0xFF;
    return result;
}

static int subtractFromRegister(CPU_t *cpu, byte_t value)
{
    word_t result = (word_t)(cpu->A - value);
    setFlags(cpu, cpu->A, value, result, true);
    cpu->A = result & 0xFF;
    return result;
}
static int subtractFromRegisterWithCarry(CPU_t *cpu, byte_t value)
{
    word_t result = (word_t)(cpu->A - value - cpu->F.C);
    setFlags(cpu, cpu->A, value, result, true);
    cpu->A = result & 0xFF;
    return result;
}
static int decrementRegister(CPU_t *cpu, byte_t *reg)
{
    word_t result = (word_t)(*reg - 1);
    setFlags(cpu, *reg, 1, result, true);
    *reg = result & 0xFF;
    return result;
}

static int andWithRegister(CPU_t *cpu, byte_t value)
{
    word_t result = (word_t)(cpu->A & value);
    setFlags(cpu, cpu->A, value, result, false);
    cpu->A = result & 0xFF;
    return result;
}
static int orWithRegister(CPU_t *cpu, byte_t value)
{
    word_t result = (word_t)(cpu->A | value);
    setFlags(cpu, cpu->A, value, result, false);
    cpu->A = result & 0xFF;
    return result;
}
static int xorWithRegister(CPU_t *cpu, byte_t value)
{
    word_t result = (word_t)(cpu->A ^ value);
    setFlags(cpu, cpu->A, value, result, false);
    cpu->A = result & 0xFF;
    return result;
}

static int instructionNop(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    return 0;
}

static int instructionAdd(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    int cycles = 0;
    byte_t operand;

    switch (instruction)
    {
    case ADD_A_n:
        operand = fetchByte(memory, cpu->PC);
        addToRegister(cpu, operand);
        cycles = 7;
        cpu->PC++;
        break;
    case ADD_A_HL:
        operand = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
        addToRegister(cpu, operand);
        cycles = 7;
        cpu->PC++;
        break;
    case ADD_A_B: addToRegister(cpu, cpu->B); cycles = 4; break;
    case ADD_A_C: addToRegister(cpu, cpu->C); cycles = 4; break;
    case ADD_A_D: addToRegister(cpu, cpu->D); cycles = 4; break;
    case ADD_A_E: addToRegister(cpu, cpu->E); cycles = 4; break;
    case ADD_A_H: addToRegister(cpu, cpu->H); cycles = 4; break;
    case ADD_A_L: addToRegister(cpu, cpu->L); cycles = 4; break;
    case ADD_A_A: addToRegister(cpu, cpu->A); cycles = 4; break;
    }
    return cycles;
}
static int instructionAdc(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    int cycles = 0;
    byte_t operand;

    switch (instruction)
    {
    case ADC_A_n:
        operand = fetchByte(memory, cpu->PC);
        addToRegisterWithCarry(cpu, operand);
        cycles = 7;
        cpu->PC++;
        break;
    case ADC_A_HL:
        operand = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
        addToRegisterWithCarry(cpu, operand);
        cycles = 7;
        cpu->PC++;
        break;
    case ADC_A_B: addToRegisterWithCarry(cpu, cpu->B); cycles = 4; break;
    case ADC_A_C: addToRegisterWithCarry(cpu, cpu->C); cycles = 4; break;
    case ADC_A_D: addToRegisterWithCarry(cpu, cpu->D); cycles = 4; break;
    case ADC_A_E: addToRegisterWithCarry(cpu, cpu->E); cycles = 4; break;
    case ADC_A_H: addToRegisterWithCarry(cpu, cpu->H); cycles = 4; break;
    case ADC_A_L: addToRegisterWithCarry(cpu, cpu->L); cycles = 4; break;
    case ADC_A_A: addToRegisterWithCarry(cpu, cpu->A); cycles = 4; break;
    }
    return cycles;
}
static int instructionInc(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    int cycles = 0;
    byte_t operand;

    switch (instruction)
    {
    case INC_A: incrementRegister(cpu, &cpu->A); cycles = 4; break;
    case INC_B: incrementRegister(cpu, &cpu->B); cycles = 4; break;
    case INC_C: incrementRegister(cpu, &cpu->C); cycles = 4; break;
    case INC_D: incrementRegister(cpu, &cpu->D); cycles = 4; break;
    case INC_E: incrementRegister(cpu, &cpu->E); cycles = 4; break;
    case INC_H: incrementRegister(cpu, &cpu->H); cycles = 4; break;
    case INC_L: incrementRegister(cpu, &cpu->L); cycles = 4; break;
    case INC_HL:
        operand = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
        incrementRegister(cpu, &operand);
        storeByte(memory, TO_WORD(cpu->H, cpu->L), operand);
        cycles = 10;
        break;
    }
    return cycles;
}

static int instructionSub(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    int cycles = 0;

    byte_t operand;

    switch (instruction)
    {
        case SUB_A_n:
            operand = fetchByte(memory, cpu->PC);
            subtractFromRegister(cpu, operand);
            cycles = 7;
            cpu->PC++;
            break;
        case SUB_A_HL:
            operand = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
            subtractFromRegister(cpu, operand);
            cycles = 7;
            cpu->PC++;
            break;
        case SUB_A_B: subtractFromRegister(cpu, cpu->B); cycles = 4; break;
        case SUB_A_C: subtractFromRegister(cpu, cpu->C); cycles = 4; break;
        case SUB_A_D: subtractFromRegister(cpu, cpu->D); cycles = 4; break;
        case SUB_A_E: subtractFromRegister(cpu, cpu->E); cycles = 4; break;
        case SUB_A_H: subtractFromRegister(cpu, cpu->H); cycles = 4; break;
        case SUB_A_L: subtractFromRegister(cpu, cpu->L); cycles = 4; break;
        case SUB_A_A: subtractFromRegister(cpu, cpu->A); cycles = 4; break;
    }
    return cycles;
}
static int instructionSbc(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    int cycles = 0;

    byte_t operand;

    switch(instruction)
    {
        case SBC_A_n:
            operand = fetchByte(memory, cpu->PC);
            subtractFromRegisterWithCarry(cpu, operand);
            cycles = 7;
            cpu->PC++;
            break;
        case SBC_A_HL:
            operand = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
            subtractFromRegisterWithCarry(cpu, operand);
            cycles = 7;
            cpu->PC++;
            break;
        case SBC_A_B: subtractFromRegisterWithCarry(cpu, cpu->B); cycles = 4; break;
        case SBC_A_C: subtractFromRegisterWithCarry(cpu, cpu->C); cycles = 4; break;
        case SBC_A_D: subtractFromRegisterWithCarry(cpu, cpu->D); cycles = 4; break;
        case SBC_A_E: subtractFromRegisterWithCarry(cpu, cpu->E); cycles = 4; break;
        case SBC_A_H: subtractFromRegisterWithCarry(cpu, cpu->H); cycles = 4; break;
        case SBC_A_L: subtractFromRegisterWithCarry(cpu, cpu->L); cycles = 4; break;
        case SBC_A_A: subtractFromRegisterWithCarry(cpu, cpu->A); cycles = 4; break;
    }
    return cycles;
}
static int instructionDec(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    int cycles = 0;

    byte_t operand;

    switch(instruction)
    {
        case DEC_A: decrementRegister(cpu, &cpu->A); cycles = 4; break;
        case DEC_B: decrementRegister(cpu, &cpu->B); cycles = 4; break;
        case DEC_C: decrementRegister(cpu, &cpu->C); cycles = 4; break;
        case DEC_D: decrementRegister(cpu, &cpu->D); cycles = 4; break;
        case DEC_E: decrementRegister(cpu, &cpu->E); cycles = 4; break;
        case DEC_H: decrementRegister(cpu, &cpu->H); cycles = 4; break;
        case DEC_L: decrementRegister(cpu, &cpu->L); cycles = 4; break;
        case DEC_HL:
            operand = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
            decrementRegister(cpu, &operand);
            storeByte(memory, TO_WORD(cpu->H, cpu->L), operand);
            cycles = 10;
            break;
    }
    return cycles;
}

static int instructionAnd(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    int cycles = 0;

    byte_t operand;

    switch(instruction)
    {
        case AND_A: andWithRegister(cpu, cpu->A); cycles = 4; break;
        case AND_B: andWithRegister(cpu, cpu->B); cycles = 4; break;
        case AND_C: andWithRegister(cpu, cpu->C); cycles = 4; break;
        case AND_D: andWithRegister(cpu, cpu->D); cycles = 4; break;
        case AND_E: andWithRegister(cpu, cpu->E); cycles = 4; break;
        case AND_H: andWithRegister(cpu, cpu->H); cycles = 4; break;
        case AND_L: andWithRegister(cpu, cpu->L); cycles = 4; break;
        case AND_HL:
            operand = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
            andWithRegister(cpu, operand);
            cycles = 7;
            break;
        case AND_n:
            operand = fetchByte(memory, cpu->PC);
            andWithRegister(cpu, operand);
            cycles = 7;
            cpu->PC++;
            break;
    }
    return cycles;
}
static int instructionOr(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    int cycles = 0;

    byte_t operand;

    switch (instruction)
    {
        case OR_A: orWithRegister(cpu, cpu->A); cycles = 4; break;
        case OR_B: orWithRegister(cpu, cpu->B); cycles = 4; break;
        case OR_C: orWithRegister(cpu, cpu->C); cycles = 4; break;
        case OR_D: orWithRegister(cpu, cpu->D); cycles = 4; break;
        case OR_E: orWithRegister(cpu, cpu->E); cycles = 4; break;
        case OR_H: orWithRegister(cpu, cpu->H); cycles = 4; break;
        case OR_L: orWithRegister(cpu, cpu->L); cycles = 4; break;
        case OR_HL:
            operand = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
            orWithRegister(cpu, operand);
            cycles = 7;
            break;
        case OR_n:
            operand = fetchByte(memory, cpu->PC);
            orWithRegister(cpu, operand);
            cycles = 7;
            cpu->PC++;
            break;
    }
    return cycles;
}
static int instructionXor(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    int cycles = 0;

    byte_t operand;

    switch(instruction)
    {
        case XOR_A: xorWithRegister(cpu, cpu->A); cycles = 4; break;
        case XOR_B: xorWithRegister(cpu, cpu->B); cycles = 4; break;
        case XOR_C: xorWithRegister(cpu, cpu->C); cycles = 4; break;
        case XOR_D: xorWithRegister(cpu, cpu->D); cycles = 4; break;
        case XOR_E: xorWithRegister(cpu, cpu->E); cycles = 4; break;
        case XOR_H: xorWithRegister(cpu, cpu->H); cycles = 4; break;
        case XOR_L: xorWithRegister(cpu, cpu->L); cycles = 4; break;
        case XOR_HL:
            operand = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
            xorWithRegister(cpu, operand);
            cycles = 7;
            break;
        case XOR_n:
            operand = fetchByte(memory, cpu->PC);
            xorWithRegister(cpu, operand);
            cycles = 7;
            cpu->PC++;
            break;
    }
    return cycles;
}

static int instructionLd(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    int cycles = 0;

    word_t address;
    byte_t operand;

    switch (instruction)
    {
    case LD_A_n:
        cpu->A = fetchByte(memory, cpu->PC);
        cpu->PC++;
        cycles = 7;
        break;
    case LD_B_n:
        cpu->B = fetchByte(memory, cpu->PC);
        cpu->PC++;
        cycles = 7;
        break;
    case LD_C_n:
        cpu->C = fetchByte(memory, cpu->PC);
        cpu->PC++;
        cycles = 7;
        break;
    case LD_D_n:
        cpu->D = fetchByte(memory, cpu->PC);
        cpu->PC++;
        cycles = 7;
        break;
    case LD_E_n:
        cpu->E = fetchByte(memory, cpu->PC);
        cpu->PC++;
        cycles = 7;
        break;
    case LD_H_n:
        cpu->H = fetchByte(memory, cpu->PC);
        cpu->PC++;
        cycles = 7;
        break;
    case LD_L_n:
        cpu->L = fetchByte(memory, cpu->PC);
        cpu->PC++;
        cycles = 7;
        break;

    case LD_B_B:
        cpu->B = cpu->B;
        cycles = 4;
        break;
    case LD_B_C:
        cpu->B = cpu->C;
        cycles = 4;
        break;
    case LD_B_D:
        cpu->B = cpu->D;
        cycles = 4;
        break;
    case LD_B_E:
        cpu->B = cpu->E;
        cycles = 4;
        break;
    case LD_B_H:
        cpu->B = cpu->H;
        cycles = 4;
        break;
    case LD_B_L:
        cpu->B = cpu->L;
        cycles = 4;
        break;
    case LD_B_HL:
        address = TO_WORD(cpu->H, cpu->L);
        cpu->B = fetchByte(memory, address);
        cycles = 7;
        break;
    case LD_B_A:
        cpu->B = cpu->A;
        cycles = 4;
        break;

    case LD_C_B:
        cpu->C = cpu->B;
        cycles = 4;
        break;
    case LD_C_C:
        cpu->C = cpu->C;
        cycles = 4;
        break;
    case LD_C_D:
        cpu->C = cpu->D;
        cycles = 4;
        break;
    case LD_C_E:
        cpu->C = cpu->E;
        cycles = 4;
        break;
    case LD_C_H:
        cpu->C = cpu->H;
        cycles = 4;
        break;
    case LD_C_L:
        cpu->C = cpu->L;
        cycles = 4;
        break;
    case LD_C_HL:
        address = TO_WORD(cpu->H, cpu->L);
        cpu->C = fetchByte(memory, address);
        cycles = 7;
        break;
    case LD_C_A:
        cpu->C = cpu->A;
        cycles = 4;
        break;

    case LD_D_B:
        cpu->D = cpu->B;
        cycles = 4;
        break;
    case LD_D_C:
        cpu->D = cpu->C;
        cycles = 4;
        break;
    case LD_D_D:
        cpu->D = cpu->D;
        cycles = 4;
        break;
    case LD_D_E:
        cpu->D = cpu->E;
        cycles = 4;
        break;
    case LD_D_H:
        cpu->D = cpu->H;
        cycles = 4;
        break;
    case LD_D_L:
        cpu->D = cpu->L;
        cycles = 4;
        break;
    case LD_D_HL:
        address = TO_WORD(cpu->H, cpu->L);
        cpu->D = fetchByte(memory, address);
        cycles = 7;
        break;
    case LD_D_A:
        cpu->D = cpu->A;
        cycles = 4;
        break;

    case LD_E_B:
        cpu->E = cpu->B;
        cycles = 4;
        break;
    case LD_E_C:
        cpu->E = cpu->C;
        cycles = 4;
        break;
    case LD_E_D:
        cpu->E = cpu->D;
        cycles = 4;
        break;
    case LD_E_E:
        cpu->E = cpu->E;
        cycles = 4;
        break;
    case LD_E_H:
        cpu->E = cpu->H;
        cycles = 4;
        break;
    case LD_E_L:
        cpu->E = cpu->L;
        cycles = 4;
        break;
    case LD_E_HL:
        address = TO_WORD(cpu->H, cpu->L);
        cpu->E = fetchByte(memory, address);
        cycles = 7;
        break;
    case LD_E_A:
        cpu->E = cpu->A;
        cycles = 4;
        break;

    case LD_H_B:
        cpu->H = cpu->B;
        cycles = 4;
        break;
    case LD_H_C:
        cpu->H = cpu->C;
        cycles = 4;
        break;
    case LD_H_D:
        cpu->H = cpu->D;
        cycles = 4;
        break;
    case LD_H_E:
        cpu->H = cpu->E;
        cycles = 4;
        break;
    case LD_H_H:
        cpu->H = cpu->H;
        cycles = 4;
        break;
    case LD_H_L:
        cpu->H = cpu->L;
        cycles = 4;
        break;
    case LD_H_HL:
        address = TO_WORD(cpu->H, cpu->L);
        cpu->H = fetchByte(memory, address);
        cycles = 7;
        break;
    case LD_H_A:
        cpu->H = cpu->A;
        cycles = 4;
        break;

    case LD_L_B:
        cpu->L = cpu->B;
        cycles = 4;
        break;
    case LD_L_C:
        cpu->L = cpu->C;
        cycles = 4;
        break;
    case LD_L_D:
        cpu->L = cpu->D;
        cycles = 4;
        break;
    case LD_L_E:
        cpu->L = cpu->E;
        cycles = 4;
        break;
    case LD_L_H:
        cpu->L = cpu->H;
        cycles = 4;
        break;
    case LD_L_L:
        cpu->L = cpu->L;
        cycles = 4;
        break;
    case LD_L_HL:
        address = TO_WORD(cpu->H, cpu->L);
        cpu->L = fetchByte(memory, address);
        cycles = 7;
        break;
    case LD_L_A:
        cpu->L = cpu->A;
        cycles = 4;
        break;

    // LD INDIRECT
    case LD_A_nn:
        address = fetchWord(memory, cpu->PC);
        cpu->A = fetchByte(memory, address);
        cpu->PC += 2;
        cycles = 13;
        break;
    case LD_HL_nn:
        address = fetchWord(memory, cpu->PC);
        cpu->H = fetchByte(memory, address);
        cpu->L = fetchByte(memory, address + 1);
        cpu->PC += 2;
        cycles = 16;
        break;

    // LD IMMEDIATE
    case LD_BC_IMMEDIATE:
        cpu->B = fetchByte(memory, cpu->PC + 1);
        cpu->C = fetchByte(memory, cpu->PC);
        cpu->PC += 2;
        cycles = 10;
        break;
    case LD_DE_IMMEDIATE:
        cpu->D = fetchByte(memory, cpu->PC + 1);
        cpu->E = fetchByte(memory, cpu->PC);
        cpu->PC += 2;
        cycles = 10;
        break;
    case LD_HL_IMMEDIATE:
        cpu->H = fetchByte(memory, cpu->PC + 1);
        cpu->L = fetchByte(memory, cpu->PC);
        cpu->PC += 2;
        cycles = 10;
        break;
    case LD_SP_IMMEDIATE:
        cpu->SP = fetchWord(memory, cpu->PC);
        cpu->PC += 2;
        cycles = 10;
        break;
    }
    return cycles;
}