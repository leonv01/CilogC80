#include "cpu.h"
#include "instructions.h"
#include "time.h"

#define CYCLES_PER_FRAME(x) ((int) ((x * 1000000 ) / 60)) 
#define MAX_ITERATIONS 1000000


static int instruction_add(CPU_t* cpu, Memory_t* memory, byte_t instruction);
static int instruction_adc(CPU_t* cpu, Memory_t* memory, byte_t instruction);
static int instruction_inc(CPU_t* cpu, Memory_t* memory, byte_t instruction);

static int instruction_sub(CPU_t* cpu, Memory_t* memory, byte_t instruction);
static int instruction_sbc(CPU_t* cpu, Memory_t* memory, byte_t instruction);
static int instruction_dec(CPU_t* cpu, Memory_t* memory, byte_t instruction);

static int instruction_and(CPU_t* cpu, Memory_t* memory, byte_t instruction);
static int instruction_or(CPU_t* cpu, Memory_t* memory, byte_t instruction);
static int instruction_xor(CPU_t* cpu, Memory_t* memory, byte_t instruction);

static int instruction_ld(CPU_t* cpu, Memory_t* memory, byte_t instruction);

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
    cpu->F.H = isSubstraction ? ((regA & 0x0F) - (operand & 0x0F)) < 0 : ((regA & 0x0F) + (operand & 0x0F)) > 0x0F;    
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

    typedef int (*InstructionHandler)(CPU_t*, Memory_t*, byte_t);
    InstructionHandler handler = NULL;

    switch(instruction)
    {
        // NOP
        case NOP:
            cycles = 4;
            break;

        // ADD
        case ADD_A_A:
        case ADD_A_B:
        case ADD_A_C:
        case ADD_A_D:
        case ADD_A_E:
        case ADD_A_H:
        case ADD_A_L:
        case ADD_A_HL:
        case ADD_A_n:
            handler = &instruction_add;
            break;

        // ADC
        case ADC_A_A:
        case ADC_A_B:
        case ADC_A_C:
        case ADC_A_D:
        case ADC_A_E:
        case ADC_A_H:
        case ADC_A_L:
        case ADC_A_HL:
        case ADC_A_n:
            handler = &instruction_adc;
            break;

        // INC
        case INC_A:
        case INC_B:
        case INC_C:
        case INC_D:
        case INC_E:
        case INC_H:
        case INC_L:
        case INC_HL:
            handler = &instruction_inc;
            break;

        // SUB
        case SUB_n:
        case SUB_HL:
        case SUB_B:
        case SUB_C:
        case SUB_D:
        case SUB_E:
        case SUB_H:
        case SUB_L:
        case SUB_A:
            handler = &instruction_sub;
            break;

        // SBC
        case SBC_A_A:
        case SBC_A_B:
        case SBC_A_C:
        case SBC_A_D:
        case SBC_A_E:
        case SBC_A_H:
        case SBC_A_L:
        case SBC_A_HL:
        case SBC_A_n:
            handler = &instruction_sbc;
            break;

        // DEC
        case DEC_A:
        case DEC_B:
        case DEC_C:
        case DEC_D:
        case DEC_E:
        case DEC_H:
        case DEC_L:
        case DEC_HL:
            handler = &instruction_dec;
            break;

        case LD_A_n:
        case LD_B_n:
        case LD_C_n:
        case LD_D_n:
        case LD_E_n:
        case LD_H_n:
        case LD_L_n:
        case LD_B_B:
        case LD_B_C:
        case LD_B_D:  
        case LD_B_E:
        case LD_B_H:
        case LD_B_L:
        case LD_B_HL:
        case LD_B_A:
        case LD_C_B:
        case LD_C_C:
        case LD_C_D:
        case LD_C_E:
        case LD_C_H:
        case LD_C_L:
        case LD_C_HL:
        case LD_C_A:
        case LD_D_B:
        case LD_D_C:
        case LD_D_D:
        case LD_D_E:
        case LD_D_H:
        case LD_D_L:
        case LD_D_HL:
        case LD_D_A:
        case LD_E_B:
        case LD_E_C:
        case LD_E_D:
        case LD_E_E:
        case LD_E_H:
        case LD_E_L:
        case LD_E_HL:
        case LD_E_A:
        case LD_H_B:
        case LD_H_C:
        case LD_H_D:
        case LD_H_E:
        case LD_H_H:
        case LD_H_L:
        case LD_H_HL:
        case LD_H_A:
        case LD_L_B:
        case LD_L_C:
        case LD_L_D:
        case LD_L_E:
        case LD_L_H:
        case LD_L_L:
        case LD_L_HL:
        case LD_L_A:
        case LD_A_nn:
        case LD_HL_nn:
        case LD_BC_IMMEDIATE:
        case LD_DE_IMMEDIATE:
        case LD_HL_IMMEDIATE:
        case LD_SP_IMMEDIATE:
            handler = &instruction_ld;
            break;
        
    }
    if(handler != NULL)
    {
        cycles = handler(cpu, memory, instruction);
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
    int cycles = 0;

    word_t result;
    word_t address;
    byte_t operand;

    switch(instruction)
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

static int instruction_add(CPU_t* cpu, Memory_t* memory, byte_t instruction)
{
    int cycles = 0;

    word_t result;
    word_t address;
    byte_t operand;

    switch(instruction)
    {
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
        default:
            break;
    }
    return cycles;
}

static int instruction_adc(CPU_t* cpu, Memory_t* memory, byte_t instruction)
{
    return 0;
}

static int instruction_inc(CPU_t* cpu, Memory_t* memory, byte_t instruction)
{
    return 0;
}

static int instruction_sub(CPU_t* cpu, Memory_t* memory, byte_t instruction)
{
    int cycles = 0;

    word_t result;
    word_t address;
    byte_t operand;

    switch(instruction)
    {
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
        default:
            break;
    }
    return cycles;
}

static int instruction_and(CPU_t* cpu, Memory_t* memory, byte_t instruction)
{
    return 0;
}

static int instruction_or(CPU_t* cpu, Memory_t* memory, byte_t instruction)
{
    return 0;
}

static int instruction_xor(CPU_t* cpu, Memory_t* memory, byte_t instruction)
{
    return 0;
}


static int instruction_ld(CPU_t* cpu, Memory_t* memory, byte_t instruction)
{
    int cycles = 0;

    word_t address;
    byte_t operand;

    switch(instruction)
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