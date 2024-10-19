#include "cpu.h"
#include "instructions.h"
#include "time.h"
#include <stdio.h>

#include <stdbool.h>

#define CYCLES_PER_FRAME(x) ((int)((x * 1000000) / 60))
#define MAX_ITERATIONS 1000000

#define MAX_INSTRUCTION_COUNT 0xFF

typedef int (*InstructionHandler)(CPU_t *, Memory_t *, byte_t);
static InstructionHandler mainInstructionTable[MAX_INSTRUCTION_COUNT] = {NULL};

static void initInstructionTable();

// CPU helper functions -------------------------------------------------------
static byte_t flagsToByte(F_t flags);
static void byteToFlags(F_t *flags, byte_t value);

static int calculateParity(word_t value);
static void setFlags(CPU_t *cpu, byte_t regA, byte_t operand, word_t result, bool isSubstraction);
static void setFlagsWord(CPU_t *cpu, word_t reg1, word_t reg2, dword_t result);
// -----------------------------------------------------------------------------

// Instructions ----------------------------------------------------------------
// Main instructions -----------------------------------------------------------
// NOP
static int instructionNop(CPU_t *cpu, Memory_t *memory, byte_t instruction);
// HALT
static int instructionHalt(CPU_t *cpu, Memory_t *memory, byte_t instruction);
// Add instructions
static int instructionAdd(CPU_t *cpu, Memory_t *memory, byte_t instruction);
static int instructionAdc(CPU_t *cpu, Memory_t *memory, byte_t instruction);
static int instructionInc(CPU_t *cpu, Memory_t *memory, byte_t instruction);
// Sub instructions
static int instructionSub(CPU_t *cpu, Memory_t *memory, byte_t instruction);
static int instructionSbc(CPU_t *cpu, Memory_t *memory, byte_t instruction);
static int instructionDec(CPU_t *cpu, Memory_t *memory, byte_t instruction);
// Logical instructions
static int instructionAnd(CPU_t *cpu, Memory_t *memory, byte_t instruction);
static int instructionOr(CPU_t *cpu, Memory_t *memory, byte_t instruction);
static int instructionXor(CPU_t *cpu, Memory_t *memory, byte_t instruction);
static int instructionCp(CPU_t *cpu, Memory_t *memory, byte_t instruction);
// Push instructions
static int instructionPush(CPU_t *cpu, Memory_t *memory, byte_t instruction);
// Pop instructions
static int instructionPop(CPU_t *cpu, Memory_t *memory, byte_t instruction);
// Return instructions
static int instructionRet(CPU_t *cpu, Memory_t *memory, byte_t instruction);
// Call instructions
static int instructionCall(CPU_t *cpu, Memory_t *memory, byte_t instruction);
// Jump instructions
static int instructionJp(CPU_t *cpu, Memory_t *memory, byte_t instruction);
static int instructionJr(CPU_t *cpu, Memory_t *memory, byte_t instruction);
// RST instructions
static int instructionRst(CPU_t *cpu, Memory_t *memory, byte_t instruction);
// Disable interrupt instructions
static int instructionDi(CPU_t *cpu, Memory_t *memory, byte_t instruction);
// Enable interrupt instructions
static int instructionEi(CPU_t *cpu, Memory_t *memory, byte_t instruction);
// EX instructions
static int instructionEx(CPU_t *cpu, Memory_t *memory, byte_t instruction);
// Load instructions
static int instructionLd(CPU_t *cpu, Memory_t *memory, byte_t instruction);
// -----------------------------------------------------------------------------

// Helper functions ------------------------------------------------------------
static void addToRegister(CPU_t *cpu, byte_t value);
static void addToRegisterPair(CPU_t *cpu, byte_t value1, byte_t value2); // TODO: Maybe change to word_t and name it addToRegisterPair
static void addToRegisterWithCarry(CPU_t *cpu, byte_t value);
static void incrementRegister(CPU_t *cpu, byte_t *reg);
static void incrementRegisterPair(CPU_t *cpu, byte_t* upperByte, byte_t* lowerByte);

static void subtractFromRegister(CPU_t *cpu, byte_t value);
static void subtractFromRegisterWithCarry(CPU_t *cpu, byte_t value);
static void decrementRegister(CPU_t *cpu, byte_t *reg);
static void decrementRegisterPair(CPU_t *cpu, byte_t* upperByte, byte_t* lowerByte);

static void andWithRegister(CPU_t *cpu, byte_t value);
static void orWithRegister(CPU_t *cpu, byte_t value);
static void xorWithRegister(CPU_t *cpu, byte_t value);
static void cpWithRegister(CPU_t *cpu, byte_t value);

static void pushWord(CPU_t *cpu, Memory_t *memory, word_t value);
static void popWord(CPU_t *cpu, Memory_t *memory, byte_t *upperByte, byte_t *lowerByte);

static void ret(CPU_t *cpu, Memory_t *memory, bool condition);
static void call(CPU_t *cpu, Memory_t *memory, bool condition);
static void jump(CPU_t *cpu, Memory_t *memory, bool condition);
static void jumpRelative(CPU_t *cpu, Memory_t *memory, bool condition);

static void rst(CPU_t *cpu, Memory_t *memory, byte_t address);

static void ex(CPU_t *cpu, byte_t *reg1, byte_t *reg2);
static void exWord(CPU_t *cpu, word_t *reg1, word_t *reg2);

static void ld(CPU_t *cpu, byte_t *reg, byte_t value);
static void ldPair(CPU_t *cpu, byte_t *upperByte, byte_t *lowerByte, word_t value);
// -----------------------------------------------------------------------------

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

static void setFlags(CPU_t *cpu, byte_t regA, byte_t operand, word_t result, bool isSubstraction)
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

static byte_t flagsToByte(F_t flags)
{
    return (byte_t)(
        (flags.S << 7) |
        (flags.Z << 6) |
        (flags._ << 5) |
        (flags.H << 4) |
        (flags._ << 3) |
        (flags.P << 2) |
        (flags.N << 1) |
        (flags.C)
    );
}
static void byteToFlags(F_t *flags, byte_t value)
{
    flags->S = (value & 0x80) >> 7;
    flags->Z = (value & 0x40) >> 6;
    flags->_ = (value & 0x20) >> 5;
    flags->H = (value & 0x10) >> 4;
    flags->_ = (value & 0x08) >> 3;
    flags->P = (value & 0x04) >> 2;
    flags->N = (value & 0x02) >> 1;
    flags->C = (value & 0x01);
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

    // CP instructions
    mainInstructionTable[MAIN_CP_A] = &instructionCp;
    mainInstructionTable[MAIN_CP_B] = &instructionCp;
    mainInstructionTable[MAIN_CP_C] = &instructionCp;
    mainInstructionTable[MAIN_CP_D] = &instructionCp;
    mainInstructionTable[MAIN_CP_E] = &instructionCp;
    mainInstructionTable[MAIN_CP_H] = &instructionCp;
    mainInstructionTable[MAIN_CP_L] = &instructionCp;
    mainInstructionTable[MAIN_CP_HL] = &instructionCp;
    mainInstructionTable[MAIN_CP_n] = &instructionCp;

    // Return instructions
    mainInstructionTable[MAIN_RET] = &instructionRet;
    mainInstructionTable[MAIN_RET_NZ] = &instructionRet;
    mainInstructionTable[MAIN_RET_NC] = &instructionRet;
    mainInstructionTable[MAIN_RET_PO] = &instructionRet;
    mainInstructionTable[MAIN_RET_P] = &instructionRet;
    mainInstructionTable[MAIN_RET_Z] = &instructionRet;
    mainInstructionTable[MAIN_RET_C] = &instructionRet;
    mainInstructionTable[MAIN_RET_PE] = &instructionRet;
    mainInstructionTable[MAIN_RET_M] = &instructionRet;

    // Call instructions
    mainInstructionTable[MAIN_CALL_nn] = &instructionCall;
    mainInstructionTable[MAIN_CALL_NZ_nn] = &instructionCall;
    mainInstructionTable[MAIN_CALL_NC_nn] = &instructionCall;
    mainInstructionTable[MAIN_CALL_PO_nn] = &instructionCall;
    mainInstructionTable[MAIN_CALL_P_nn] = &instructionCall;
    mainInstructionTable[MAIN_CALL_Z_nn] = &instructionCall;
    mainInstructionTable[MAIN_CALL_C_nn] = &instructionCall;
    mainInstructionTable[MAIN_CALL_PE_nn] = &instructionCall;
    mainInstructionTable[MAIN_CALL_M_nn] = &instructionCall;

    // Jump instructions
    mainInstructionTable[MAIN_JP_nn] = &instructionJp;
    mainInstructionTable[MAIN_JP_NZ_nn] = &instructionJp;
    mainInstructionTable[MAIN_JP_NC_nn] = &instructionJp;
    mainInstructionTable[MAIN_JP_PO_nn] = &instructionJp;
    mainInstructionTable[MAIN_JP_P_nn] = &instructionJp;
    mainInstructionTable[MAIN_JP_Z_nn] = &instructionJp;
    mainInstructionTable[MAIN_JP_C_nn] = &instructionJp;
    mainInstructionTable[MAIN_JP_PE_nn] = &instructionJp;
    mainInstructionTable[MAIN_JP_M_nn] = &instructionJp;

    // Jump relative instructions
    mainInstructionTable[MAIN_JR_d] = &instructionJr;
    mainInstructionTable[MAIN_JR_NZ_d] = &instructionJr;
    mainInstructionTable[MAIN_JR_Z_d] = &instructionJr;
    mainInstructionTable[MAIN_JR_NC_d] = &instructionJr;
    mainInstructionTable[MAIN_JR_C_d] = &instructionJr;
    mainInstructionTable[MAIN_DJNZ_d] = &instructionJr;

    // RST 
    mainInstructionTable[MAIN_RST_00H] = &instructionRst;
    mainInstructionTable[MAIN_RST_08H] = &instructionRst;
    mainInstructionTable[MAIN_RST_10H] = &instructionRst;
    mainInstructionTable[MAIN_RST_18H] = &instructionRst;
    mainInstructionTable[MAIN_RST_20H] = &instructionRst;
    mainInstructionTable[MAIN_RST_28H] = &instructionRst;
    mainInstructionTable[MAIN_RST_30H] = &instructionRst;
    mainInstructionTable[MAIN_RST_38H] = &instructionRst;

    // Push instructions
    mainInstructionTable[MAIN_PUSH_AF] = &instructionPush;
    mainInstructionTable[MAIN_PUSH_BC] = &instructionPush;
    mainInstructionTable[MAIN_PUSH_DE] = &instructionPush;
    mainInstructionTable[MAIN_PUSH_HL] = &instructionPush;

    // Pop instructions
    mainInstructionTable[MAIN_POP_AF] = &instructionPop;
    mainInstructionTable[MAIN_POP_BC] = &instructionPop;
    mainInstructionTable[MAIN_POP_DE] = &instructionPop;
    mainInstructionTable[MAIN_POP_HL] = &instructionPop;

    // Disable interrupt instructions
    mainInstructionTable[MAIN_DI] = &instructionDi;

    // Enable interrupt instructions
    mainInstructionTable[MAIN_EI] = &instructionEi;

    // EX instructions
    mainInstructionTable[MAIN_EX_DE_HL] = &instructionEx;
    mainInstructionTable[MAIN_EX_AF_AF_ALT] = &instructionEx;
    mainInstructionTable[MAIN_EX_SP_HL] = &instructionEx;
    mainInstructionTable[MAIN_EXX] = &instructionEx;

    // Load instructions
    mainInstructionTable[MAIN_LD_A_n] = &instructionLd;
    mainInstructionTable[MAIN_LD_A_A] = &instructionLd;
    mainInstructionTable[MAIN_LD_A_B] = &instructionLd;
    mainInstructionTable[MAIN_LD_A_C] = &instructionLd;
    mainInstructionTable[MAIN_LD_A_D] = &instructionLd;
    mainInstructionTable[MAIN_LD_A_E] = &instructionLd;
    mainInstructionTable[MAIN_LD_A_H] = &instructionLd;
    mainInstructionTable[MAIN_LD_A_L] = &instructionLd;

    mainInstructionTable[MAIN_LD_B_n] = &instructionLd;
    mainInstructionTable[MAIN_LD_B_A] = &instructionLd;
    mainInstructionTable[MAIN_LD_B_B] = &instructionLd;
    mainInstructionTable[MAIN_LD_B_C] = &instructionLd;
    mainInstructionTable[MAIN_LD_B_D] = &instructionLd;
    mainInstructionTable[MAIN_LD_B_E] = &instructionLd;
    mainInstructionTable[MAIN_LD_B_H] = &instructionLd;
    mainInstructionTable[MAIN_LD_B_L] = &instructionLd;

    mainInstructionTable[MAIN_LD_C_n] = &instructionLd;
    mainInstructionTable[MAIN_LD_C_A] = &instructionLd;
    mainInstructionTable[MAIN_LD_C_B] = &instructionLd;
    mainInstructionTable[MAIN_LD_C_C] = &instructionLd;
    mainInstructionTable[MAIN_LD_C_D] = &instructionLd;
    mainInstructionTable[MAIN_LD_C_E] = &instructionLd;
    mainInstructionTable[MAIN_LD_C_H] = &instructionLd;
    mainInstructionTable[MAIN_LD_C_L] = &instructionLd;

    mainInstructionTable[MAIN_LD_D_n] = &instructionLd;
    mainInstructionTable[MAIN_LD_D_A] = &instructionLd;
    mainInstructionTable[MAIN_LD_D_B] = &instructionLd;
    mainInstructionTable[MAIN_LD_D_C] = &instructionLd;
    mainInstructionTable[MAIN_LD_D_D] = &instructionLd;
    mainInstructionTable[MAIN_LD_D_E] = &instructionLd;
    mainInstructionTable[MAIN_LD_D_H] = &instructionLd;
    mainInstructionTable[MAIN_LD_D_L] = &instructionLd;

    mainInstructionTable[MAIN_LD_E_n] = &instructionLd;
    mainInstructionTable[MAIN_LD_E_A] = &instructionLd;
    mainInstructionTable[MAIN_LD_E_B] = &instructionLd;
    mainInstructionTable[MAIN_LD_E_C] = &instructionLd;
    mainInstructionTable[MAIN_LD_E_D] = &instructionLd;
    mainInstructionTable[MAIN_LD_E_E] = &instructionLd;
    mainInstructionTable[MAIN_LD_E_H] = &instructionLd;
    mainInstructionTable[MAIN_LD_E_L] = &instructionLd;

    mainInstructionTable[MAIN_LD_H_n] = &instructionLd;
    mainInstructionTable[MAIN_LD_H_A] = &instructionLd;
    mainInstructionTable[MAIN_LD_H_B] = &instructionLd;
    mainInstructionTable[MAIN_LD_H_C] = &instructionLd;
    mainInstructionTable[MAIN_LD_H_D] = &instructionLd;
    mainInstructionTable[MAIN_LD_H_E] = &instructionLd;
    mainInstructionTable[MAIN_LD_H_H] = &instructionLd;
    mainInstructionTable[MAIN_LD_H_L] = &instructionLd;

    mainInstructionTable[MAIN_LD_L_n] = &instructionLd;
    mainInstructionTable[MAIN_LD_L_A] = &instructionLd;
    mainInstructionTable[MAIN_LD_L_B] = &instructionLd;
    mainInstructionTable[MAIN_LD_L_C] = &instructionLd;
    mainInstructionTable[MAIN_LD_L_D] = &instructionLd;
    mainInstructionTable[MAIN_LD_L_E] = &instructionLd;
    mainInstructionTable[MAIN_LD_L_H] = &instructionLd;
    mainInstructionTable[MAIN_LD_L_L] = &instructionLd;

    mainInstructionTable[MAIN_LD_A_HL] = &instructionLd;
    mainInstructionTable[MAIN_LD_A_BC] = &instructionLd;
    mainInstructionTable[MAIN_LD_A_DE] = &instructionLd;
    mainInstructionTable[MAIN_LD_A_nn] = &instructionLd;

    mainInstructionTable[MAIN_LD_HL_A_ADDR] = &instructionLd;;
    mainInstructionTable[MAIN_LD_HL_B_ADDR] = &instructionLd;;
    mainInstructionTable[MAIN_LD_HL_C_ADDR] = &instructionLd;;
    mainInstructionTable[MAIN_LD_HL_D_ADDR] = &instructionLd;;
    mainInstructionTable[MAIN_LD_HL_E_ADDR] = &instructionLd;;
    mainInstructionTable[MAIN_LD_HL_H_ADDR] = &instructionLd;;
    mainInstructionTable[MAIN_LD_HL_L_ADDR] = &instructionLd;;
    mainInstructionTable[MAIN_LD_BC_A_ADDR] = &instructionLd;;
    mainInstructionTable[MAIN_LD_DE_A_ADDR] = &instructionLd;;
    mainInstructionTable[MAIN_LD_HL_A_ADDR] = &instructionLd;;

    mainInstructionTable[MAIN_LD_nn_A] = &instructionLd;
    mainInstructionTable[MAIN_LD_nn_HL] = &instructionLd;;
    mainInstructionTable[MAIN_LD_HL_nn] = &instructionLd;;
    mainInstructionTable[MAIN_LD_DE_nn] = &instructionLd;;
    mainInstructionTable[MAIN_LD_BC_nn] = &instructionLd;;
    mainInstructionTable[MAIN_LD_SP_nn] = &instructionLd;;
    mainInstructionTable[MAIN_LD_SP_HL] = &instructionLd;;
    mainInstructionTable[MAIN_LD_HL_nn_ADDR] = &instructionLd;;
    mainInstructionTable[MAIN_LD_HL_nn] = &instructionLd;;

    for (int i = 0; i < MAX_INSTRUCTION_COUNT; i++)
    {
        if (mainInstructionTable[i] == NULL)
        {
            printf("Instruction not implemented: %02x\n", i);
        }
    }
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


// Helper functions ------------------------------------------------------------
static void addToRegister(CPU_t *cpu, byte_t value)
{
    word_t result = (word_t)(cpu->A + value);
    setFlags(cpu, cpu->A, value, result, false);
    cpu->A = result & 0xFF;
}
static void addToRegisterPair(CPU_t *cpu, byte_t value1, byte_t value2)
{
    word_t value = TO_WORD(value1, value2);
    dword_t result = (dword_t)(TO_WORD(cpu->H, cpu->L) + value);
    setFlagsWord(cpu, TO_WORD(cpu->H, cpu->L), value, result);
    cpu->H = cpu->H + value1;
    cpu->L = cpu->L + value2;
}
static void addToRegisterWithCarry(CPU_t *cpu, byte_t value)
{
    word_t result = (word_t)(cpu->A + value + cpu->F.C);
    setFlags(cpu, cpu->A, value, result, false);
    cpu->A = result & 0xFF;
}
static void incrementRegister(CPU_t *cpu, byte_t *reg)
{
    word_t result = (word_t)(*reg + 1);
    setFlags(cpu, *reg, 1, result, false);
    *reg = result & 0xFF;
}
static void incrementRegisterPair(CPU_t *cpu, byte_t* upperByte, byte_t* lowerByte)
{
    word_t result = (word_t)(TO_WORD(*upperByte, *lowerByte) + 1);
    *upperByte = UPPER_BYTE(result);
    *lowerByte = LOWER_BYTE(result);
}

static void subtractFromRegister(CPU_t *cpu, byte_t value)
{
    word_t result = (word_t)(cpu->A - value);
    setFlags(cpu, cpu->A, value, result, true);
    cpu->A = result & 0xFF;
}
static void subtractFromRegisterWithCarry(CPU_t *cpu, byte_t value)
{
    word_t result = (word_t)(cpu->A - value - cpu->F.C);
    setFlags(cpu, cpu->A, value, result, true);
    cpu->A = result & 0xFF;
}
static void decrementRegister(CPU_t *cpu, byte_t *reg)
{
    word_t result = (word_t)(*reg - 1);
    setFlags(cpu, *reg, 1, result, true);
    *reg = result & 0xFF;
}
static void decrementRegisterPair(CPU_t *cpu, byte_t* upperByte, byte_t* lowerByte)
{
    word_t result = (word_t)(TO_WORD(*upperByte, *lowerByte) - 1);
    *upperByte = UPPER_BYTE(result);
    *lowerByte = LOWER_BYTE(result);
}

static void andWithRegister(CPU_t *cpu, byte_t value)
{
    word_t result = (word_t)(cpu->A & value);
    setFlags(cpu, cpu->A, value, result, false);
    cpu->A = result & 0xFF;
}
static void orWithRegister(CPU_t *cpu, byte_t value)
{
    word_t result = (word_t)(cpu->A | value);
    setFlags(cpu, cpu->A, value, result, false);
    cpu->A = result & 0xFF;
}
static void xorWithRegister(CPU_t *cpu, byte_t value)
{
    word_t result = (word_t)(cpu->A ^ value);
    setFlags(cpu, cpu->A, value, result, false);
    cpu->A = result & 0xFF;
}
static void cpWithRegister(CPU_t *cpu, byte_t value)
{
    word_t result = (word_t)(cpu->A - value);
    setFlags(cpu, cpu->A, value, result, true);
}

static void pushWord(CPU_t *cpu, Memory_t *memory, word_t value)
{
    storeByte(memory, cpu->SP - 1, UPPER_BYTE(value));
    storeByte(memory, cpu->SP - 2, LOWER_BYTE(value));
    cpu->SP -= 2;
}
static void popWord(CPU_t *cpu, Memory_t *memory, byte_t *upperByte, byte_t *lowerByte)
{
    *lowerByte = fetchByte(memory, cpu->SP);
    *upperByte = fetchByte(memory, cpu->SP + 1);
    cpu->SP += 2;
}

static void ret(CPU_t *cpu, Memory_t *memory, bool condition)
{
    if(condition)
    {
        byte_t lowerByte, upperByte;
        popWord(cpu, memory, &upperByte, &lowerByte);
        cpu->PC = TO_WORD(upperByte, lowerByte);
    }
}
static void call(CPU_t *cpu, Memory_t *memory, bool condition)
{
    word_t address = fetchWord(memory, cpu->PC);
    cpu->PC += 2;

    if(condition)
    {
        pushWord(cpu, memory, cpu->PC);
        cpu->PC = address;
    }
}

static void jump(CPU_t *cpu, Memory_t *memory, bool condition)
{
    if(condition)
    {
        word_t address = fetchWord(memory, cpu->PC);
        cpu->PC = address;
    }
}
static void jumpRelative(CPU_t *cpu, Memory_t *memory, bool condition)
{
    if(condition)
    {
        byte_t offset = fetchByte(memory, cpu->PC);
        cpu->PC += offset;
    }
}

static void rst(CPU_t *cpu, Memory_t *memory, byte_t address)
{
    pushWord(cpu, memory, cpu->PC);
    cpu->PC = address;
}

static void ex(CPU_t *cpu, byte_t *reg1, byte_t *reg2)
{
    byte_t temp = *reg1;
    *reg1 = *reg2;
    *reg2 = temp;
}

static void exWord(CPU_t *cpu, word_t *reg1, word_t *reg2)
{
    word_t temp = *reg1;
    *reg1 = *reg2;
    *reg2 = temp;
}

static void ld(CPU_t *cpu, byte_t *reg, byte_t value)
{
    *reg = value;
}
static void ldPair(CPU_t *cpu, byte_t *upperByte, byte_t *lowerByte, word_t value)
{
    *upperByte = UPPER_BYTE(value);
    *lowerByte = LOWER_BYTE(value);
}
// -----------------------------------------------------------------------------

// Main instructions -----------------------------------------------------------
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
        case MAIN_DEC_BC: decrementRegisterPair(cpu, &cpu->B, &cpu->C); cycles = 6; break;
        case MAIN_DEC_DE: decrementRegisterPair(cpu, &cpu->D, &cpu->E); cycles = 6; break;
        case MAIN_DEC_HL: decrementRegisterPair(cpu, &cpu->H, &cpu->L); cycles = 6; break;
        case MAIN_DEC_SP: 
            byte_t upperByte = UPPER_BYTE(cpu->SP);
            byte_t lowerByte = LOWER_BYTE(cpu->SP);
            decrementRegisterPair(cpu, &upperByte, &lowerByte); 
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
static int instructionCp(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    int cycles = 0;

    byte_t operand;

    switch(instruction)
    {
        case MAIN_CP_A: cpWithRegister(cpu, cpu->A); cycles = 4; break;
        case MAIN_CP_B: cpWithRegister(cpu, cpu->B); cycles = 4; break;
        case MAIN_CP_C: cpWithRegister(cpu, cpu->C); cycles = 4; break;
        case MAIN_CP_D: cpWithRegister(cpu, cpu->D); cycles = 4; break;
        case MAIN_CP_E: cpWithRegister(cpu, cpu->E); cycles = 4; break;
        case MAIN_CP_H: cpWithRegister(cpu, cpu->H); cycles = 4; break;
        case MAIN_CP_L: cpWithRegister(cpu, cpu->L); cycles = 4; break;
        case MAIN_CP_HL:
            operand = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
            cpWithRegister(cpu, operand);
            cycles = 7;
            break;
    }
}

static int instructionPush(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    int cycles = 0;

    switch(instruction)
    {
        case MAIN_PUSH_BC: pushWord(cpu, memory, TO_WORD(cpu->B, cpu->C)); cycles = 11; break;
        case MAIN_PUSH_DE: pushWord(cpu, memory, TO_WORD(cpu->D, cpu->E)); cycles = 11; break;
        case MAIN_PUSH_HL: pushWord(cpu, memory, TO_WORD(cpu->H, cpu->L)); cycles = 11; break;  
        case MAIN_PUSH_AF: pushWord(cpu, memory, TO_WORD(cpu->A, flagsToByte(cpu->F))); cycles = 11; break;
    }
}
static int instructionPop(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    int cycles = 0;

    switch(instruction)
    {
        case MAIN_POP_BC: popWord(cpu, memory, &cpu->B, &cpu->C); cycles = 10; break;
        case MAIN_POP_DE: popWord(cpu, memory, &cpu->D, &cpu->E); cycles = 10; break;
        case MAIN_POP_HL: popWord(cpu, memory, &cpu->H, &cpu->L); cycles = 10; break;
        case MAIN_POP_AF: 
            byte_t upperByte, lowerByte;
            popWord(cpu, memory, &upperByte, &lowerByte);
            byteToFlags(&cpu->F, lowerByte);
            cpu->A = upperByte;
            cycles = 10;
            break;
    }
}

static int instructionRet(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    int cycles = 0;

    bool condition;
    switch(instruction)
    {
        case MAIN_RET: ret(cpu, memory, true); cycles = 10; break;
        case MAIN_RET_NZ: condition = cpu->F.Z == 0; ret(cpu, memory, condition); cycles = condition ? 11 : 5; break;
        case MAIN_RET_Z: condition = cpu->F.Z == 1; ret(cpu, memory, condition); cycles = condition ? 11 : 5; break;
        case MAIN_RET_NC: condition = cpu->F.C == 0; ret(cpu, memory, condition); cycles = condition ? 11 : 5; break;
        case MAIN_RET_C: condition = cpu->F.C == 1; ret(cpu, memory, condition); cycles = condition ? 11 : 5; break;
        case MAIN_RET_PO: condition = cpu->F.P == 0; ret(cpu, memory, condition); cycles = condition ? 11 : 5; break;
        case MAIN_RET_PE: condition = cpu->F.P == 1; ret(cpu, memory, condition); cycles = condition ? 11 : 5; break;
        case MAIN_RET_P: condition = cpu->F.S == 0; ret(cpu, memory, condition); cycles = condition ? 11 : 5; break;
        case MAIN_RET_M: condition = cpu->F.S == 1; ret(cpu, memory, condition); cycles = condition ? 11 : 5; break;
    }
}

static int instructionCall(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    int cycles = 0;

    word_t address;
    bool condition;

    switch (instruction)
    {
    case MAIN_CALL_nn: call(cpu, memory, true); cycles = 17; break;
    case MAIN_CALL_NZ_nn: condition = cpu->F.Z == 0; call(cpu, memory, condition); cycles = condition ? 17 : 10; break;
    case MAIN_CALL_Z_nn: condition = cpu->F.Z == 1; call(cpu, memory, condition); cycles = condition ? 17 : 10; break;
    case MAIN_CALL_NC_nn: condition = cpu->F.C == 0; call(cpu, memory, condition); cycles = condition ? 17 : 10; break;
    case MAIN_CALL_C_nn: condition = cpu->F.C == 1; call(cpu, memory, condition); cycles = condition ? 17 : 10; break;
    case MAIN_CALL_PO_nn: condition = cpu->F.P == 0; call(cpu, memory, condition); cycles = condition ? 17 : 10; break;
    case MAIN_CALL_PE_nn: condition = cpu->F.P == 1; call(cpu, memory, condition); cycles = condition ? 17 : 10; break;
    case MAIN_CALL_P_nn: condition = cpu->F.S == 0; call(cpu, memory, condition); cycles = condition ? 17 : 10; break;
    case MAIN_CALL_M_nn: condition = cpu->F.S == 1; call(cpu, memory, condition); cycles = condition ? 17 : 10; break;
    }
}

static int instructionJp(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    int cycles = 0;

    bool condition;

    switch(instruction)
    {
        case MAIN_JP_nn: jump(cpu, memory, true); cycles = 10; break;
        case MAIN_JP_NZ_nn: condition = cpu->F.Z == 0; jump(cpu, memory, condition); cycles = 10; break;
        case MAIN_JP_Z_nn: condition = cpu->F.Z == 1; jump(cpu, memory, condition); cycles = 10; break;
        case MAIN_JP_NC_nn: condition = cpu->F.C == 0; jump(cpu, memory, condition); cycles = 10; break;
        case MAIN_JP_C_nn: condition = cpu->F.C == 1; jump(cpu, memory, condition); cycles = 10; break;
        case MAIN_JP_PO_nn: condition = cpu->F.P == 0; jump(cpu, memory, condition); cycles = 10; break;
        case MAIN_JP_PE_nn: condition = cpu->F.P == 1; jump(cpu, memory, condition); cycles = 10; break;
        case MAIN_JP_P_nn: condition = cpu->F.S == 0; jump(cpu, memory, condition); cycles = 10; break;
        case MAIN_JP_M_nn: condition = cpu->F.S == 1; jump(cpu, memory, condition); cycles = 10; break;
        case MAIN_JP_HL: cpu->PC = TO_WORD(cpu->H, cpu->L); cycles = 4; break;
    }
}
static int instructionJr(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    int cycles = 0;

    bool condition;

    switch(instruction)
    {
    case MAIN_JR_d: jumpRelative(cpu, memory, true); cycles = 12; break;
    case MAIN_JR_NZ_d: condition = cpu->F.Z == 0; jumpRelative(cpu, memory, condition); cycles = condition ? 12 : 7; break;
    case MAIN_JR_Z_d: condition = cpu->F.Z == 1; jumpRelative(cpu, memory, condition); cycles = condition ? 12 : 7; break;
    case MAIN_JR_NC_d: condition = cpu->F.C == 0; jumpRelative(cpu, memory, condition); cycles = condition ? 12 : 7; break;
    case MAIN_JR_C_d: condition = cpu->F.C == 1; jumpRelative(cpu, memory, condition); cycles = condition ? 12 : 7; break;
    case MAIN_DJNZ_d: 
        condition = cpu->B != 0;
        if(condition)
        {
            jumpRelative(cpu, memory, condition);
            cpu->B--;
            cycles = 13;
        }
        else
        {
            cpu->PC++;
            cycles = 8;
        }
        break;
    }
}

static int instructionRst(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    int cycles = 0;

    switch(instruction)
    {
        case MAIN_RST_00H: rst(cpu, memory, 0x00); cycles = 11; break;
        case MAIN_RST_08H: rst(cpu, memory, 0x08); cycles = 11; break;
        case MAIN_RST_10H: rst(cpu, memory, 0x10); cycles = 11; break;
        case MAIN_RST_18H: rst(cpu, memory, 0x18); cycles = 11; break;
        case MAIN_RST_20H: rst(cpu, memory, 0x20); cycles = 11; break;
        case MAIN_RST_28H: rst(cpu, memory, 0x28); cycles = 11; break;
        case MAIN_RST_30H: rst(cpu, memory, 0x30); cycles = 11; break;
        case MAIN_RST_38H: rst(cpu, memory, 0x38); cycles = 11; break;
    }
}

static int instructionDi(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    // TODO: Implement
    return 0;
}

static int instructionEi(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    // TODO: Implement
    return 0;
}

static int instructionEx(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    int cycles = 0;

    word_t word1, word2;

    switch(instruction)
    {
        case MAIN_EX_DE_HL: 
            word1 = TO_WORD(cpu->D, cpu->E); 
            word2 = TO_WORD(cpu->H, cpu->L); 
            exWord(cpu, &word1, &word2);
            cpu->D = UPPER_BYTE(word1);
            cpu->E = LOWER_BYTE(word1);
            cpu->H = UPPER_BYTE(word2);
            cpu->L = LOWER_BYTE(word2);
            cycles = 4;
            break;
        case MAIN_EX_AF_AF_ALT: 
            word1 = TO_WORD(cpu->A, flagsToByte(cpu->F));
            word2 = TO_WORD(cpu->A_ALT, flagsToByte(cpu->F_ALT));
            exWord(cpu, &word1, &word2);
            cpu->A = UPPER_BYTE(word1);
            byteToFlags(&cpu->F, LOWER_BYTE(word1));
            cpu->A_ALT = UPPER_BYTE(word2);
            byteToFlags(&cpu->F_ALT, LOWER_BYTE(word2));
            cycles = 4;
            break;
        case MAIN_EX_SP_HL: 
            word1 = fetchWord(memory, cpu->SP);
            word2 = TO_WORD(cpu->H, cpu->L);
            exWord(cpu, &word1, &word2);
            storeByte(memory, cpu->SP, UPPER_BYTE(word1));
            storeByte(memory, cpu->SP + 1, LOWER_BYTE(word1));
            cpu->H = UPPER_BYTE(word2);
            cpu->L = LOWER_BYTE(word2);
            cycles = 19;
            break;
            break;
        case MAIN_EXX: 
            ex(cpu, &cpu->B, &cpu->B_ALT);
            ex(cpu, &cpu->C, &cpu->C_ALT);
            ex(cpu, &cpu->D, &cpu->D_ALT);
            ex(cpu, &cpu->E, &cpu->E_ALT);
            ex(cpu, &cpu->H, &cpu->H_ALT);
            ex(cpu, &cpu->L, &cpu->L_ALT);
            cycles = 4;
            break;
    }
}

static int instructionLd(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    int cycles = 0;

    word_t word;
    byte_t operand;

    switch (instruction)
    {
        // LD A
        case MAIN_LD_A_A: ld(cpu, &cpu->A, cpu->A); cycles = 4; break;
        case MAIN_LD_A_B: ld(cpu, &cpu->A, cpu->B); cycles = 4; break;
        case MAIN_LD_A_C: ld(cpu, &cpu->A, cpu->C); cycles = 4; break;
        case MAIN_LD_A_D: ld(cpu, &cpu->A, cpu->D); cycles = 4; break;
        case MAIN_LD_A_E: ld(cpu, &cpu->A, cpu->E); cycles = 4; break;
        case MAIN_LD_A_H: ld(cpu, &cpu->A, cpu->H); cycles = 4; break;
        case MAIN_LD_A_L: ld(cpu, &cpu->A, cpu->L); cycles = 4; break;
        case MAIN_LD_A_HL: operand = fetchByte(memory, TO_WORD(cpu->H, cpu->L)); ld(cpu, &cpu->A, operand); cycles = 7; break;
        case MAIN_LD_A_n: operand = fetchByte(memory, cpu->PC); ld(cpu, &cpu->A, operand); cycles = 7; break;

        // LD B
        case MAIN_LD_B_A: ld(cpu, &cpu->B, cpu->A); cycles = 4; break;
        case MAIN_LD_B_B: ld(cpu, &cpu->B, cpu->B); cycles = 4; break;
        case MAIN_LD_B_C: ld(cpu, &cpu->B, cpu->C); cycles = 4; break;
        case MAIN_LD_B_D: ld(cpu, &cpu->B, cpu->D); cycles = 4; break;
        case MAIN_LD_B_E: ld(cpu, &cpu->B, cpu->E); cycles = 4; break;
        case MAIN_LD_B_H: ld(cpu, &cpu->B, cpu->H); cycles = 4; break;
        case MAIN_LD_B_L: ld(cpu, &cpu->B, cpu->L); cycles = 4; break;
        case MAIN_LD_B_HL: operand = fetchByte(memory, TO_WORD(cpu->H, cpu->L)); ld(cpu, &cpu->B, operand); cycles = 7; break;
        case MAIN_LD_B_n: operand = fetchByte(memory, cpu->PC); ld(cpu, &cpu->B, operand); cycles = 7; break;

        // LD C
        case MAIN_LD_C_A: ld(cpu, &cpu->C, cpu->A); cycles = 4; break;
        case MAIN_LD_C_B: ld(cpu, &cpu->C, cpu->B); cycles = 4; break;
        case MAIN_LD_C_C: ld(cpu, &cpu->C, cpu->C); cycles = 4; break;
        case MAIN_LD_C_D: ld(cpu, &cpu->C, cpu->D); cycles = 4; break;
        case MAIN_LD_C_E: ld(cpu, &cpu->C, cpu->E); cycles = 4; break;
        case MAIN_LD_C_H: ld(cpu, &cpu->C, cpu->H); cycles = 4; break;
        case MAIN_LD_C_L: ld(cpu, &cpu->C, cpu->L); cycles = 4; break;
        case MAIN_LD_C_HL: operand = fetchByte(memory, TO_WORD(cpu->H, cpu->L)); ld(cpu, &cpu->C, operand); cycles = 7; break;
        case MAIN_LD_C_n: operand = fetchByte(memory, cpu->PC); ld(cpu, &cpu->C, operand); cycles = 7; break;

        // LD D
        case MAIN_LD_D_A: ld(cpu, &cpu->D, cpu->A); cycles = 4; break;
        case MAIN_LD_D_B: ld(cpu, &cpu->D, cpu->B); cycles = 4; break;
        case MAIN_LD_D_C: ld(cpu, &cpu->D, cpu->C); cycles = 4; break;
        case MAIN_LD_D_D: ld(cpu, &cpu->D, cpu->D); cycles = 4; break;
        case MAIN_LD_D_E: ld(cpu, &cpu->D, cpu->E); cycles = 4; break;
        case MAIN_LD_D_H: ld(cpu, &cpu->D, cpu->H); cycles = 4; break;
        case MAIN_LD_D_L: ld(cpu, &cpu->D, cpu->L); cycles = 4; break;
        case MAIN_LD_D_HL: operand = fetchByte(memory, TO_WORD(cpu->H, cpu->L)); ld(cpu, &cpu->D, operand); cycles = 7; break;
        case MAIN_LD_D_n: operand = fetchByte(memory, cpu->PC); ld(cpu, &cpu->D, operand); cycles = 7; break;

        // LD E
        case MAIN_LD_E_A: ld(cpu, &cpu->E, cpu->A); cycles = 4; break;
        case MAIN_LD_E_B: ld(cpu, &cpu->E, cpu->B); cycles = 4; break;
        case MAIN_LD_E_C: ld(cpu, &cpu->E, cpu->C); cycles = 4; break;
        case MAIN_LD_E_D: ld(cpu, &cpu->E, cpu->D); cycles = 4; break;
        case MAIN_LD_E_E: ld(cpu, &cpu->E, cpu->E); cycles = 4; break;
        case MAIN_LD_E_H: ld(cpu, &cpu->E, cpu->H); cycles = 4; break;
        case MAIN_LD_E_L: ld(cpu, &cpu->E, cpu->L); cycles = 4; break;
        case MAIN_LD_E_HL: operand = fetchByte(memory, TO_WORD(cpu->H, cpu->L)); ld(cpu, &cpu->E, operand); cycles = 7; break;
        case MAIN_LD_E_n: operand = fetchByte(memory, cpu->PC); ld(cpu, &cpu->E, operand); cycles = 7; break;

        // LD H
        case MAIN_LD_H_A: ld(cpu, &cpu->H, cpu->A); cycles = 4; break;
        case MAIN_LD_H_B: ld(cpu, &cpu->H, cpu->B); cycles = 4; break;
        case MAIN_LD_H_C: ld(cpu, &cpu->H, cpu->C); cycles = 4; break;
        case MAIN_LD_H_D: ld(cpu, &cpu->H, cpu->D); cycles = 4; break;
        case MAIN_LD_H_E: ld(cpu, &cpu->H, cpu->E); cycles = 4; break;
        case MAIN_LD_H_H: ld(cpu, &cpu->H, cpu->H); cycles = 4; break;
        case MAIN_LD_H_L: ld(cpu, &cpu->H, cpu->L); cycles = 4; break;
        case MAIN_LD_H_HL: operand = fetchByte(memory, TO_WORD(cpu->H, cpu->L)); ld(cpu, &cpu->H, operand); cycles = 7; break;
        case MAIN_LD_H_n: operand = fetchByte(memory, cpu->PC); ld(cpu, &cpu->H, operand); cycles = 7; break;

        // LD L
        case MAIN_LD_L_A: ld(cpu, &cpu->L, cpu->A); cycles = 4; break;
        case MAIN_LD_L_B: ld(cpu, &cpu->L, cpu->B); cycles = 4; break;
        case MAIN_LD_L_C: ld(cpu, &cpu->L, cpu->C); cycles = 4; break;
        case MAIN_LD_L_D: ld(cpu, &cpu->L, cpu->D); cycles = 4; break;
        case MAIN_LD_L_E: ld(cpu, &cpu->L, cpu->E); cycles = 4; break;
        case MAIN_LD_L_H: ld(cpu, &cpu->L, cpu->H); cycles = 4; break;
        case MAIN_LD_L_L: ld(cpu, &cpu->L, cpu->L); cycles = 4; break;
        case MAIN_LD_L_HL: operand = fetchByte(memory, TO_WORD(cpu->H, cpu->L)); ld(cpu, &cpu->L, operand); cycles = 7; break;
        case MAIN_LD_L_n: operand = fetchByte(memory, cpu->PC); ld(cpu, &cpu->L, operand); cycles = 7; break;

        // LD (HL)
        case MAIN_LD_HL_A_ADDR: operand = cpu->A; storeByte(memory, TO_WORD(cpu->H, cpu->L), operand); cycles = 7; break;
        case MAIN_LD_HL_B_ADDR: operand = cpu->B; storeByte(memory, TO_WORD(cpu->H, cpu->L), operand); cycles = 7; break;
        case MAIN_LD_HL_C_ADDR: operand = cpu->C; storeByte(memory, TO_WORD(cpu->H, cpu->L), operand); cycles = 7; break;
        case MAIN_LD_HL_D_ADDR: operand = cpu->D; storeByte(memory, TO_WORD(cpu->H, cpu->L), operand); cycles = 7; break;
        case MAIN_LD_HL_E_ADDR: operand = cpu->E; storeByte(memory, TO_WORD(cpu->H, cpu->L), operand); cycles = 7; break;
        case MAIN_LD_HL_H_ADDR: operand = cpu->H; storeByte(memory, TO_WORD(cpu->H, cpu->L), operand); cycles = 7; break;
        case MAIN_LD_HL_L_ADDR: operand = cpu->L; storeByte(memory, TO_WORD(cpu->H, cpu->L), operand); cycles = 7; break;

        case MAIN_LD_BC_A_ADDR: operand = cpu->A; storeByte(memory, TO_WORD(cpu->B, cpu->C), operand); cycles = 7; break;
        case MAIN_LD_DE_A_ADDR: operand = cpu->A; storeByte(memory, TO_WORD(cpu->D, cpu->E), operand); cycles = 7; break;
        case MAIN_LD_nn_A: 
            word = fetchWord(memory, cpu->PC);
            operand = cpu->A;
            storeByte(memory, word, operand);
            cycles = 13;
            break;
        case MAIN_LD_nn_HL: 
            word = fetchWord(memory, cpu->PC);
            operand = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
            storeByte(memory, word, operand);
            cycles = 16;
            break;

        // LD Pair
        case MAIN_LD_HL_nn: word = fetchWord(memory, cpu->PC); ldPair(cpu, &cpu->H, &cpu->L, word); cycles = 10; break;
        case MAIN_LD_DE_nn: word = fetchWord(memory, cpu->PC); ldPair(cpu, &cpu->D, &cpu->E, word); cycles = 10; break;
        case MAIN_LD_BC_nn: word = fetchWord(memory, cpu->PC); ldPair(cpu, &cpu->B, &cpu->C, word); cycles = 10; break;
        case MAIN_LD_SP_nn: word = fetchWord(memory, cpu->PC); cpu->SP = word; cycles = 10; break;
        case MAIN_LD_SP_HL: cpu->SP = TO_WORD(cpu->H, cpu->L); cycles = 6; break;

        case MAIN_LD_A_BC: ld(cpu, &cpu->A, fetchByte(memory, TO_WORD(cpu->B, cpu->C))); cycles = 7; break;
        case MAIN_LD_A_DE: ld(cpu, &cpu->A, fetchByte(memory, TO_WORD(cpu->D, cpu->E))); cycles = 7; break;
        case MAIN_LD_A_nn: 
            word = fetchWord(memory, cpu->PC);
            ld(cpu, &cpu->A, fetchByte(memory, word));
            cycles = 13;
            break;
    }
}
// -----------------------------------------------------------------------------