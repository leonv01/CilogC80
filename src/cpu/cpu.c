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

static int instructionHalt(CPU_t *cpu, Memory_t *memory, byte_t instruction);

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
static int addToRegisterPair(CPU_t *cpu, byte_t value1, byte_t value2); // TODO: Maybe change to word_t and name it addToRegisterPair
static int addToRegisterWithCarry(CPU_t *cpu, byte_t value);
static int incrementRegister(CPU_t *cpu, byte_t *reg);
static int incrementRegisterPair(CPU_t *cpu, byte_t* upperByte, byte_t* lowerByte);

static int subtractFromRegister(CPU_t *cpu, byte_t value);
static int subtractFromRegisterWithCarry(CPU_t *cpu, byte_t value);
static int decrementRegister(CPU_t *cpu, byte_t *reg);
static int decrementregisterPair(CPU_t *cpu, byte_t* upperByte, byte_t* lowerByte);

static int andWithRegister(CPU_t *cpu, byte_t value);
static int orWithRegister(CPU_t *cpu, byte_t value);
static int xorWithRegister(CPU_t *cpu, byte_t value);
static int cpWithRegister(CPU_t *cpu, byte_t value);

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

static int calculateParity(word_t value)
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

static void setFlagsWord(CPU_t *cpu, word_t reg1, word_t reg2, dword_t result)
{
    cpu->F.Z = (result & 0xFFFF) == 0;
    cpu->F.S = (result & 0x8000) >> 15;

    cpu->F.H = ((reg1 & 0x0FFF) + (reg2 & 0x0FFF)) > 0x0FFF;

    cpu->F.C = result > 0xFFFF;

    cpu->F.N = 0;
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
    mainInstructionTable[MAIN_NOP] = &instructionNop;

    // Add instructions
    mainInstructionTable[MAIN_ADD_A_A] = &instructionAdd;
    mainInstructionTable[MAIN_ADD_A_B] = &instructionAdd;
    mainInstructionTable[MAIN_ADD_A_C] = &instructionAdd;
    mainInstructionTable[MAIN_ADD_A_D] = &instructionAdd;
    mainInstructionTable[MAIN_ADD_A_E] = &instructionAdd;
    mainInstructionTable[MAIN_ADD_A_H] = &instructionAdd;
    mainInstructionTable[MAIN_ADD_A_L] = &instructionAdd;
    mainInstructionTable[MAIN_ADD_A_HL] = &instructionAdd;
    mainInstructionTable[MAIN_ADD_A_n] = &instructionAdd;

    // Add instructions (HL)
    mainInstructionTable[MAIN_ADD_HL_BC] = &instructionAdd;
    mainInstructionTable[MAIN_ADD_HL_DE] = &instructionAdd;
    mainInstructionTable[MAIN_ADD_HL_HL] = &instructionAdd;
    mainInstructionTable[MAIN_ADD_HL_SP] = &instructionAdd;

    // ADC instructions
    mainInstructionTable[MAIN_ADC_A_A] = &instructionAdc;
    mainInstructionTable[MAIN_ADC_A_B] = &instructionAdc;
    mainInstructionTable[MAIN_ADC_A_C] = &instructionAdc;
    mainInstructionTable[MAIN_ADC_A_D] = &instructionAdc;
    mainInstructionTable[MAIN_ADC_A_E] = &instructionAdc;
    mainInstructionTable[MAIN_ADC_A_H] = &instructionAdc;
    mainInstructionTable[MAIN_ADC_A_L] = &instructionAdc;
    mainInstructionTable[MAIN_ADC_A_HL] = &instructionAdc;
    mainInstructionTable[MAIN_ADC_A_n] = &instructionAdc;

    // INC instructions
    mainInstructionTable[MAIN_INC_A] = &instructionInc;
    mainInstructionTable[MAIN_INC_B] = &instructionInc;
    mainInstructionTable[MAIN_INC_C] = &instructionInc;
    mainInstructionTable[MAIN_INC_D] = &instructionInc;
    mainInstructionTable[MAIN_INC_E] = &instructionInc;
    mainInstructionTable[MAIN_INC_H] = &instructionInc;
    mainInstructionTable[MAIN_INC_L] = &instructionInc;
    mainInstructionTable[MAIN_INC_HL] = &instructionInc;

    mainInstructionTable[MAIN_INC_BC] = &instructionInc;
    mainInstructionTable[MAIN_INC_DE] = &instructionInc;
    mainInstructionTable[MAIN_INC_HL] = &instructionInc;
    mainInstructionTable[MAIN_INC_SP] = &instructionInc;

    // Sub instructions
    mainInstructionTable[MAIN_SUB_A_A] = &instructionSub;
    mainInstructionTable[MAIN_SUB_A_B] = &instructionSub;
    mainInstructionTable[MAIN_SUB_A_C] = &instructionSub;
    mainInstructionTable[MAIN_SUB_A_D] = &instructionSub;
    mainInstructionTable[MAIN_SUB_A_E] = &instructionSub;
    mainInstructionTable[MAIN_SUB_A_H] = &instructionSub;
    mainInstructionTable[MAIN_SUB_A_L] = &instructionSub;
    mainInstructionTable[MAIN_SUB_A_HL] = &instructionSub;
    mainInstructionTable[MAIN_SUB_A_n] = &instructionSub;

    // SBC instructions
    mainInstructionTable[MAIN_SBC_A_A] = &instructionSbc;
    mainInstructionTable[MAIN_SBC_A_B] = &instructionSbc;
    mainInstructionTable[MAIN_SBC_A_C] = &instructionSbc;
    mainInstructionTable[MAIN_SBC_A_D] = &instructionSbc;
    mainInstructionTable[MAIN_SBC_A_E] = &instructionSbc;
    mainInstructionTable[MAIN_SBC_A_H] = &instructionSbc;
    mainInstructionTable[MAIN_SBC_A_L] = &instructionSbc;
    mainInstructionTable[MAIN_SBC_A_HL] = &instructionSbc;
    mainInstructionTable[MAIN_SBC_A_n] = &instructionSbc;

    // DEC instructions
    mainInstructionTable[MAIN_DEC_A] = &instructionDec;
    mainInstructionTable[MAIN_DEC_B] = &instructionDec;
    mainInstructionTable[MAIN_DEC_C] = &instructionDec;
    mainInstructionTable[MAIN_DEC_D] = &instructionDec;
    mainInstructionTable[MAIN_DEC_E] = &instructionDec;
    mainInstructionTable[MAIN_DEC_H] = &instructionDec;
    mainInstructionTable[MAIN_DEC_L] = &instructionDec;
    mainInstructionTable[MAIN_DEC_HL] = &instructionDec;

    // AND instructions
    mainInstructionTable[MAIN_AND_A] = &instructionAnd;
    mainInstructionTable[MAIN_AND_B] = &instructionAnd;
    mainInstructionTable[MAIN_AND_C] = &instructionAnd;
    mainInstructionTable[MAIN_AND_D] = &instructionAnd;
    mainInstructionTable[MAIN_AND_E] = &instructionAnd;
    mainInstructionTable[MAIN_AND_H] = &instructionAnd;
    mainInstructionTable[MAIN_AND_L] = &instructionAnd;
    mainInstructionTable[MAIN_AND_HL] = &instructionAnd;
    mainInstructionTable[MAIN_AND_n] = &instructionAnd;

    // OR instructions
    mainInstructionTable[MAIN_OR_A] = &instructionOr;
    mainInstructionTable[MAIN_OR_B] = &instructionOr;
    mainInstructionTable[MAIN_OR_C] = &instructionOr;
    mainInstructionTable[MAIN_OR_D] = &instructionOr;
    mainInstructionTable[MAIN_OR_E] = &instructionOr;
    mainInstructionTable[MAIN_OR_H] = &instructionOr;
    mainInstructionTable[MAIN_OR_L] = &instructionOr;
    mainInstructionTable[MAIN_OR_HL] = &instructionOr;
    mainInstructionTable[MAIN_OR_n] = &instructionOr;

    // XOR instructions
    mainInstructionTable[MAIN_XOR_A] = &instructionXor;
    mainInstructionTable[MAIN_XOR_B] = &instructionXor;
    mainInstructionTable[MAIN_XOR_C] = &instructionXor;
    mainInstructionTable[MAIN_XOR_D] = &instructionXor;
    mainInstructionTable[MAIN_XOR_E] = &instructionXor;
    mainInstructionTable[MAIN_XOR_H] = &instructionXor;
    mainInstructionTable[MAIN_XOR_L] = &instructionXor;
    mainInstructionTable[MAIN_XOR_HL] = &instructionXor;
    mainInstructionTable[MAIN_XOR_n] = &instructionXor;


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
static int addToRegisterPair(CPU_t *cpu, byte_t value1, byte_t value2)
{
    word_t value = TO_WORD(value1, value2);
    dword_t result = (dword_t)(TO_WORD(cpu->H, cpu->L) + value);
    setFlagsWord(cpu, TO_WORD(cpu->H, cpu->L), value, result);
    cpu->H = cpu->H + value1;
    cpu->L = cpu->L + value2;
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
static int incrementRegisterPair(CPU_t *cpu, byte_t* upperByte, byte_t* lowerByte)
{
    word_t result = (word_t)(TO_WORD(*upperByte, *lowerByte) + 1);
    *upperByte = UPPER_BYTE(result);
    *lowerByte = LOWER_BYTE(result);
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
static int decrementregisterPair(CPU_t *cpu, byte_t* upperByte, byte_t* lowerByte)
{
    word_t result = (word_t)(TO_WORD(*upperByte, *lowerByte) - 1);
    *upperByte = UPPER_BYTE(result);
    *lowerByte = LOWER_BYTE(result);
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
static int cpWithRegister(CPU_t *cpu, byte_t value)
{
    word_t result = (word_t)(cpu->A - value);
    setFlags(cpu, cpu->A, value, result, true);
    return result;
}

static int instructionNop(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    return 0;
}

static int instructionHalt(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    return 0;
}

static int instructionAdd(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    int cycles = 0;
    byte_t operand;

    switch (instruction)
    {
    case MAIN_ADD_A_n:
        operand = fetchByte(memory, cpu->PC);
        addToRegister(cpu, operand);
        cycles = 7;
        cpu->PC++;
        break;
    case MAIN_ADD_A_HL:
        operand = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
        addToRegister(cpu, operand);
        cycles = 7;
        cpu->PC++;
        break;
    case MAIN_ADD_A_B: addToRegister(cpu, cpu->B); cycles = 4; break;
    case MAIN_ADD_A_C: addToRegister(cpu, cpu->C); cycles = 4; break;
    case MAIN_ADD_A_D: addToRegister(cpu, cpu->D); cycles = 4; break;
    case MAIN_ADD_A_E: addToRegister(cpu, cpu->E); cycles = 4; break;
    case MAIN_ADD_A_H: addToRegister(cpu, cpu->H); cycles = 4; break;
    case MAIN_ADD_A_L: addToRegister(cpu, cpu->L); cycles = 4; break;
    case MAIN_ADD_A_A: addToRegister(cpu, cpu->A); cycles = 4; break;
    case MAIN_ADD_HL_BC: addToRegisterPair(cpu, cpu->B, cpu->C); cycles = 11; break;
    case MAIN_ADD_HL_DE: addToRegisterPair(cpu, cpu->D, cpu->E); cycles = 11; break;
    case MAIN_ADD_HL_HL: addToRegisterPair(cpu, cpu->H, cpu->L); cycles = 11; break;
    case MAIN_ADD_HL_SP: addToRegisterPair(cpu, UPPER_BYTE(cpu->SP), LOWER_BYTE(cpu->SP)); cycles = 11; break;
    }
    return cycles;
}

static int instructionAdc(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    int cycles = 0;
    byte_t operand;

    switch (instruction)
    {
    case MAIN_ADC_A_n:
        operand = fetchByte(memory, cpu->PC);
        addToRegisterWithCarry(cpu, operand);
        cycles = 7;
        cpu->PC++;
        break;
    case MAIN_ADC_A_HL:
        operand = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
        addToRegisterWithCarry(cpu, operand);
        cycles = 7;
        cpu->PC++;
        break;
    case MAIN_ADC_A_B: addToRegisterWithCarry(cpu, cpu->B); cycles = 4; break;
    case MAIN_ADC_A_C: addToRegisterWithCarry(cpu, cpu->C); cycles = 4; break;
    case MAIN_ADC_A_D: addToRegisterWithCarry(cpu, cpu->D); cycles = 4; break;
    case MAIN_ADC_A_E: addToRegisterWithCarry(cpu, cpu->E); cycles = 4; break;
    case MAIN_ADC_A_H: addToRegisterWithCarry(cpu, cpu->H); cycles = 4; break;
    case MAIN_ADC_A_L: addToRegisterWithCarry(cpu, cpu->L); cycles = 4; break;
    case MAIN_ADC_A_A: addToRegisterWithCarry(cpu, cpu->A); cycles = 4; break;
    }
    return cycles;
}
static int instructionInc(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    int cycles = 0;
    byte_t operand;

    switch (instruction)
    {
    case MAIN_INC_A: incrementRegister(cpu, &cpu->A); cycles = 4; break;
    case MAIN_INC_B: incrementRegister(cpu, &cpu->B); cycles = 4; break;
    case MAIN_INC_C: incrementRegister(cpu, &cpu->C); cycles = 4; break;
    case MAIN_INC_D: incrementRegister(cpu, &cpu->D); cycles = 4; break;
    case MAIN_INC_E: incrementRegister(cpu, &cpu->E); cycles = 4; break;
    case MAIN_INC_H: incrementRegister(cpu, &cpu->H); cycles = 4; break;
    case MAIN_INC_L: incrementRegister(cpu, &cpu->L); cycles = 4; break;
    case MAIN_INC_HL_ADDR:
        operand = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
        incrementRegister(cpu, &operand);
        storeByte(memory, TO_WORD(cpu->H, cpu->L), operand);
        cycles = 10;
        break;
    case MAIN_INC_BC: incrementRegisterPair(cpu, &cpu->B, &cpu->C); cycles = 6; break;
    case MAIN_INC_DE: incrementRegisterPair(cpu, &cpu->D, &cpu->E); cycles = 6; break;
    case MAIN_INC_HL: incrementRegisterPair(cpu, &cpu->H, &cpu->L); cycles = 6; break;
    case MAIN_INC_SP: 
        byte_t upperByte = UPPER_BYTE(cpu->SP);
        byte_t lowerByte = LOWER_BYTE(cpu->SP);
        incrementRegisterPair(cpu, &upperByte, &lowerByte); 
        cpu->SP = TO_WORD(upperByte, lowerByte);
        cycles = 6; 
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
        case MAIN_SUB_A_n:
            operand = fetchByte(memory, cpu->PC);
            subtractFromRegister(cpu, operand);
            cycles = 7;
            cpu->PC++;
            break;
        case MAIN_SUB_A_HL:
            operand = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
            subtractFromRegister(cpu, operand);
            cycles = 7;
            cpu->PC++;
            break;
        case MAIN_SUB_A_B: subtractFromRegister(cpu, cpu->B); cycles = 4; break;
        case MAIN_SUB_A_C: subtractFromRegister(cpu, cpu->C); cycles = 4; break;
        case MAIN_SUB_A_D: subtractFromRegister(cpu, cpu->D); cycles = 4; break;
        case MAIN_SUB_A_E: subtractFromRegister(cpu, cpu->E); cycles = 4; break;
        case MAIN_SUB_A_H: subtractFromRegister(cpu, cpu->H); cycles = 4; break;
        case MAIN_SUB_A_L: subtractFromRegister(cpu, cpu->L); cycles = 4; break;
        case MAIN_SUB_A_A: subtractFromRegister(cpu, cpu->A); cycles = 4; break;
    }
    return cycles;
}
static int instructionSbc(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    int cycles = 0;

    byte_t operand;

    switch(instruction)
    {
        case MAIN_SBC_A_n:
            operand = fetchByte(memory, cpu->PC);
            subtractFromRegisterWithCarry(cpu, operand);
            cycles = 7;
            cpu->PC++;
            break;
        case MAIN_SBC_A_HL:
            operand = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
            subtractFromRegisterWithCarry(cpu, operand);
            cycles = 7;
            cpu->PC++;
            break;
        case MAIN_SBC_A_B: subtractFromRegisterWithCarry(cpu, cpu->B); cycles = 4; break;
        case MAIN_SBC_A_C: subtractFromRegisterWithCarry(cpu, cpu->C); cycles = 4; break;
        case MAIN_SBC_A_D: subtractFromRegisterWithCarry(cpu, cpu->D); cycles = 4; break;
        case MAIN_SBC_A_E: subtractFromRegisterWithCarry(cpu, cpu->E); cycles = 4; break;
        case MAIN_SBC_A_H: subtractFromRegisterWithCarry(cpu, cpu->H); cycles = 4; break;
        case MAIN_SBC_A_L: subtractFromRegisterWithCarry(cpu, cpu->L); cycles = 4; break;
        case MAIN_SBC_A_A: subtractFromRegisterWithCarry(cpu, cpu->A); cycles = 4; break;
    }
    return cycles;
}
static int instructionDec(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    int cycles = 0;

    byte_t operand;

    switch(instruction)
    {
        case MAIN_DEC_A: decrementRegister(cpu, &cpu->A); cycles = 4; break;
        case MAIN_DEC_B: decrementRegister(cpu, &cpu->B); cycles = 4; break;
        case MAIN_DEC_C: decrementRegister(cpu, &cpu->C); cycles = 4; break;
        case MAIN_DEC_D: decrementRegister(cpu, &cpu->D); cycles = 4; break;
        case MAIN_DEC_E: decrementRegister(cpu, &cpu->E); cycles = 4; break;
        case MAIN_DEC_H: decrementRegister(cpu, &cpu->H); cycles = 4; break;
        case MAIN_DEC_L: decrementRegister(cpu, &cpu->L); cycles = 4; break;
        case MAIN_DEC_HL_ADDR:
            operand = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
            decrementRegister(cpu, &operand);
            storeByte(memory, TO_WORD(cpu->H, cpu->L), operand);
            cycles = 10;
            break;
        case MAIN_DEC_BC: decrementregisterPair(cpu, &cpu->B, &cpu->C); cycles = 6; break;
        case MAIN_DEC_DE: decrementregisterPair(cpu, &cpu->D, &cpu->E); cycles = 6; break;
        case MAIN_DEC_HL: decrementregisterPair(cpu, &cpu->H, &cpu->L); cycles = 6; break;
        case MAIN_DEC_SP: 
            byte_t upperByte = UPPER_BYTE(cpu->SP);
            byte_t lowerByte = LOWER_BYTE(cpu->SP);
            decrementregisterPair(cpu, &upperByte, &lowerByte); 
            cpu->SP = TO_WORD(upperByte, lowerByte);
            cycles = 6; 
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
        case MAIN_AND_A: andWithRegister(cpu, cpu->A); cycles = 4; break;
        case MAIN_AND_B: andWithRegister(cpu, cpu->B); cycles = 4; break;
        case MAIN_AND_C: andWithRegister(cpu, cpu->C); cycles = 4; break;
        case MAIN_AND_D: andWithRegister(cpu, cpu->D); cycles = 4; break;
        case MAIN_AND_E: andWithRegister(cpu, cpu->E); cycles = 4; break;
        case MAIN_AND_H: andWithRegister(cpu, cpu->H); cycles = 4; break;
        case MAIN_AND_L: andWithRegister(cpu, cpu->L); cycles = 4; break;
        case MAIN_AND_HL:
            operand = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
            andWithRegister(cpu, operand);
            cycles = 7;
            break;
        case MAIN_AND_n:
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
        case MAIN_OR_A: orWithRegister(cpu, cpu->A); cycles = 4; break;
        case MAIN_OR_B: orWithRegister(cpu, cpu->B); cycles = 4; break;
        case MAIN_OR_C: orWithRegister(cpu, cpu->C); cycles = 4; break;
        case MAIN_OR_D: orWithRegister(cpu, cpu->D); cycles = 4; break;
        case MAIN_OR_E: orWithRegister(cpu, cpu->E); cycles = 4; break;
        case MAIN_OR_H: orWithRegister(cpu, cpu->H); cycles = 4; break;
        case MAIN_OR_L: orWithRegister(cpu, cpu->L); cycles = 4; break;
        case MAIN_OR_HL:
            operand = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
            orWithRegister(cpu, operand);
            cycles = 7;
            break;
        case MAIN_OR_n:
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
        case MAIN_XOR_A: xorWithRegister(cpu, cpu->A); cycles = 4; break;
        case MAIN_XOR_B: xorWithRegister(cpu, cpu->B); cycles = 4; break;
        case MAIN_XOR_C: xorWithRegister(cpu, cpu->C); cycles = 4; break;
        case MAIN_XOR_D: xorWithRegister(cpu, cpu->D); cycles = 4; break;
        case MAIN_XOR_E: xorWithRegister(cpu, cpu->E); cycles = 4; break;
        case MAIN_XOR_H: xorWithRegister(cpu, cpu->H); cycles = 4; break;
        case MAIN_XOR_L: xorWithRegister(cpu, cpu->L); cycles = 4; break;
        case MAIN_XOR_HL:
            operand = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
            xorWithRegister(cpu, operand);
            cycles = 7;
            break;
        case MAIN_XOR_n:
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
    case MAIN_LD_A_n:
        cpu->A = fetchByte(memory, cpu->PC);
        cpu->PC++;
        cycles = 7;
        break;
    }
}