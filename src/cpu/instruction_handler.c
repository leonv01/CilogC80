#include "instruction_handler.h"

#include <stdio.h>
#include <stdbool.h>
#include "utils/utils.h"

#include "utils/error_handler.h"

#define MAX_INSTRUCTION_COUNT 256

/**
 * @brief Instruction function pointer
 * 
 */
typedef int (*InstructionHandler_t)(ZilogZ80_t *);

/* -------------------------- CPU helper functions -------------------------- */
/**
 * @brief Transforms the values of the flags into a byte
 * 
 * @param flags 
 * @return byte_t Flags as a byte
 */
static byte_t flagsToByte(F_t flags);
/**
 * @brief Takes a byte and sets the corresponding flags
 * 
 * @param flags 
 * @param value Byte with values for the flags
 */
static void byteToFlags(F_t *flags, byte_t value);

/**
 * @brief Calculates the parity of a value.
 * If the number of bits set to 1 is even, the parity is true, otherwise it is false
 * 
 * @param value 
 * @return bool 
 */
static bool calculateParity(word_t value);
/**
 * @brief Set the Flags of the CPU depending on the result of an operation with a byte
 * 
 * @param cpu 
 * @param regA 
 * @param operand Operand of the operation
 * @param result Result to set the flags
 * @param isSubstraction Flag to indicate if the operation is a substraction
 */
static void setFlags(ZilogZ80_t *cpu, byte_t regA, byte_t operand, word_t result, bool isSubstraction);
/**
 * @brief Set the Flags of the CPU depending on the result of an operation with a word
 * 
 * @param cpu 
 * @param reg1 
 * @param reg2 
 * @param result 
 */
static void setFlagsWord(ZilogZ80_t *cpu, word_t reg1, word_t reg2, dword_t result);
/* -------------------------------------------------------------------------- */

/* ---------------------------- Helper functions ---------------------------- */
/**
 * @brief Helper function to add a value to a register and set the flags
 * 
 * @param cpu 
 * @param reg 
 * @param value 
 */
static void addToRegister(ZilogZ80_t *cpu, byte_t *reg, byte_t value);
/**
 * @brief Helper function to add two values to a register pair and set the flags
 * 
 * @param cpu 
 * @param value1
 * @param value2 
 */
static void addToRegisterPair(ZilogZ80_t *cpu, word_t value1, word_t value2);
/**
 * @brief Helper function to add a value to a register with carry bit and set the flags
 * 
 * @param cpu 
 * @param reg 
 * @param value 
 */
static void addToRegisterWithCarry(ZilogZ80_t *cpu, byte_t *reg, byte_t value);
/**
 * @brief Helper function to increment a register and set the flags
 * 
 * @param cpu 
 * @param reg 
 */
static void incrementRegister(ZilogZ80_t *cpu, byte_t *reg);
/**
 * @brief Helper function to increment a register pair and set the flags
 * 
 * @param cpu 
 * @param upperByte 
 * @param lowerByte 
 */
static void incrementRegisterPair(ZilogZ80_t *cpu, byte_t* upperByte, byte_t* lowerByte);

/**
 * @brief Helper function to subtract a value from a register and set the flags
 * 
 * @param cpu 
 * @param reg 
 * @param value 
 */
static void subtractFromRegister(ZilogZ80_t *cpu, byte_t value);
/**
 * @brief Helper function to subtract two values from a register pair and set the flags
 * 
 * @param cpu 
 * @param value1 
 * @param value2 
 */
static void subtractFromRegisterWithCarry(ZilogZ80_t *cpu, byte_t value);
/**
 * @brief Helper function to decrement a register and set the flags
 * 
 * @param cpu 
 * @param reg 
 */
static void decrementRegister(ZilogZ80_t *cpu, byte_t *reg);
/**
 * @brief Helper function to decrement a register pair and set the flags
 * 
 * @param cpu 
 * @param upperByte 
 * @param lowerByte 
 */
static void decrementRegisterPair(ZilogZ80_t *cpu, byte_t* upperByte, byte_t* lowerByte);

/**
 * @brief Helper function to and a value with a register and set the flags
 * 
 * @param cpu 
 * @param value 
 */
static void andWithRegister(ZilogZ80_t *cpu, byte_t value);
/**
 * @brief Helper function to or a value with a register and set the flags
 * 
 * @param cpu 
 * @param value 
 */
static void orWithRegister(ZilogZ80_t *cpu, byte_t value);
/**
 * @brief Helper function to xor a value with a register and set the flags
 * 
 * @param cpu 
 * @param value 
 */
static void xorWithRegister(ZilogZ80_t *cpu, byte_t value);
/**
 * @brief Helper function to compare a value with a register and set the flags
 * 
 * @param cpu 
 * @param value 
 */
static void cpWithRegister(ZilogZ80_t *cpu, byte_t value);

/**
 * @brief Helper function to push a word to the stack
 * 
 * @param cpu 
 * @param value 
 */
static void pushWord(ZilogZ80_t *cpu, word_t value);
/**
 * @brief Helper function to pop a word from the stack
 * 
 * @param cpu 
 * @param upperByte 
 * @param lowerByte 
 */
static void popWord(ZilogZ80_t *cpu, byte_t *upperByte, byte_t *lowerByte);

/**
 * @brief Helper function to return if a condition is met
 * 
 * @param cpu 
 * @param address 
 */
static int returnHelper(ZilogZ80_t *cpu, bool condition);
/**
 * @brief Helper function to call a function if a condition is met
 * 
 * @param cpu 
 * @param condition 
 */
static int callHelper(ZilogZ80_t *cpu, bool condition);
/**
 * @brief Helper function to jump to an address if a condition is met
 * 
 * @param cpu 
 * @param condition 
 * @return int 
 */
static int jumpHelper(ZilogZ80_t *cpu, bool condition);
/**
 * @brief Helper function to jump to a relative address if a condition is met
 * 
 * @param cpu 
 * @param condition 
 * @return int 
 */
static int jumpRelativeHelper(ZilogZ80_t *cpu, bool condition);

// TODO: Document this function
static void rst(ZilogZ80_t *cpu, byte_t address);

/**
 * @brief Helper function to exchange two registers
 * 
 * @param cpu 
 * @param reg1 
 * @param reg2 
 */
static void exWordHelper(ZilogZ80_t *cpu, word_t *reg1, word_t *reg2);

// TODO: Document this function
static void ld(ZilogZ80_t *cpu, byte_t *reg, byte_t value);
static void ldPair(ZilogZ80_t *cpu, byte_t *upperByte, byte_t *lowerByte, word_t value);
/* -------------------------------------------------------------------------- */

/* ---------------------------- Instruction functions ---------------------------- */

/**
 * @brief Instruction function that reports an invalid opcode
 * 
 * @param cpu 
 * @return int Cycle count (-1)
 */
static int no_func(ZilogZ80_t *cpu);

/* ----------------------------------- NOP ---------------------------------- */
/**
 * @brief Instruction function that does nothing
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int nop(ZilogZ80_t *cpu);

/* ---------------------------------- HALT ---------------------------------- */
/**
 * @brief Instruction function that halts the CPU
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int halt(ZilogZ80_t *cpu);

/* ----------------------------------- ADD ---------------------------------- */
/**
 * @brief Instruction function that adds to register pair HL the value of BC
 * 
 * @param cpu 
 * @return int Cycle count (11)
 */
static int add_hl_bc_imm(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that adds to register pair HL the value of DE
 * 
 * @param cpu 
 * @return int Cycle count (11)
 */
static int add_hl_de_imm(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that adds to register pair HL the value of HL
 * 
 * @param cpu 
 * @return int Cycle count (11)
 */
static int add_hl_hl_imm(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that adds to register pair HL the value of SP
 * 
 * @param cpu 
 * @return int Cycle count (11)
 */
static int add_hl_sp_imm(ZilogZ80_t *cpu);

/**
 * @brief Instruction function that adds to register A an immediate value
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int add_a_n(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that adds to register A the value of register A
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int add_a_a(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that adds to register A the value of register B
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int add_a_b(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that adds to register A the value of register C
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int add_a_c(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that adds to register A the value of register D
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int add_a_d(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that adds to register A the value of register E
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int add_a_e(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that adds to register A the value of register H
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int add_a_h(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that adds to register A the value of register L
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int add_a_l(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that adds to register A the value of the memory address in HL
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int add_a_hl_addr(ZilogZ80_t *cpu);

/* ----------------------------------- ADC ---------------------------------- */
/**
 * @brief Instruction function that adds to register A an immediate value with carry
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int adc_a_n(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that adds to register A the value of register A with carry
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int adc_a_a(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that adds to register A the value of register B with carry
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int adc_a_b(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that adds to register A the value of register C with carry
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int adc_a_c(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that adds to register A the value of register D with carry
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int adc_a_d(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that adds to register A the value of register E with carry
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int adc_a_e(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that adds to register A the value of register H with carry
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int adc_a_h(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that adds to register A the value of register L with carry
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int adc_a_l(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that adds to register A the value of the memory address in HL with carry
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int adc_a_hl_addr(ZilogZ80_t *cpu);

/**
 * @brief Instruction function that adds to register pair HL the value of BC with carry
 * 
 * @param cpu 
 * @return int Cycle count (11)
 */
static int adc_hl_bc(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that adds to register pair HL the value of DE with carry
 * 
 * @param cpu 
 * @return int Cycle count (11)
 */
static int adc_hl_de(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that adds to register pair HL the value of HL with carry
 * 
 * @param cpu 
 * @return int Cycle count (11)
 */
static int adc_hl_hl(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that adds to register pair HL the value of SP with carry
 * 
 * @param cpu 
 * @return int Cycle count (11)
 */
static int adc_hl_sp(ZilogZ80_t *cpu);

/* ----------------------------------- INC ---------------------------------- */
/**
 * @brief Instruction function that increments register pair BC
 * 
 * @param cpu 
 * @return int Cycle count (6)
 */
static int inc_bc(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that increments register pair DE
 * 
 * @param cpu 
 * @return int Cycle count (6)
 */
static int inc_de(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that increments register pair HL
 * 
 * @param cpu 
 * @return int Cycle count (6)
 */
static int inc_hl(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that increments register pair SP
 * 
 * @param cpu 
 * @return int Cycle count (6)
 */
static int inc_sp(ZilogZ80_t *cpu);

/**
 * @brief Instruction function that increments register A
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int inc_a(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that increments register B
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int inc_b(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that increments register C
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int inc_c(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that increments register D
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int inc_d(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that increments register E
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int inc_e(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that increments register H
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int inc_h(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that increments register L
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int inc_l(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that increments the memory address in HL
 * 
 * @param cpu 
 * @return int Cycle count (11)
 */
static int inc_hl_addr(ZilogZ80_t *cpu);

/* ----------------------------------- SUB ---------------------------------- */
/**
 * @brief Instruction function that subtracts from register A an immediate value
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int sub_n(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that subtracts from register A the value of register A
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int sub_a(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that subtracts from register A the value of register B
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int sub_b(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that subtracts from register A the value of register C
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int sub_c(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that subtracts from register A the value of register D
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int sub_d(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that subtracts from register A the value of register E
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int sub_e(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that subtracts from register A the value of register H
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int sub_h(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that subtracts from register A the value of register L
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int sub_l(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that subtracts from register A the value of the memory address in HL
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int sub_hl_addr(ZilogZ80_t *cpu);

/* ----------------------------------- SBC ---------------------------------- */
/**
 * @brief Instruction function that subtracts from register A an immediate value with carry
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int sbc_a_n(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that subtracts from register A the value of register A with carry
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int sbc_a_a(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that subtracts from register A the value of register B with carry
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int sbc_b(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that subtracts from register A the value of register C with carry
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int sbc_c(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that subtracts from register A the value of register D with carry
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int sbc_d(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that subtracts from register A the value of register E with carry
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int sbc_e(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that subtracts from register A the value of register H with carry
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int sbc_h(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that subtracts from register A the value of register L with carry
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int sbc_l(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that subtracts from register A the value of the memory address in HL with carry
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int sbc_hl_addr(ZilogZ80_t *cpu);

/**
 * @brief Instruction function that subtracts from register pair HL the value of BC with carry
 * 
 * @param cpu 
 * @return int Cycle count (11)
 */
static int sbc_hl_bc(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that subtracts from register pair HL the value of DE with carry
 * 
 * @param cpu 
 * @return int Cycle count (11)
 */
static int sbc_hl_de(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that subtracts from register pair HL the value of HL with carry
 * 
 * @param cpu 
 * @return int Cycle count (11)
 */
static int sbc_hl_hl(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that subtracts from register pair HL the value of SP with carry
 * 
 * @param cpu 
 * @return int Cycle count (11)
 */
static int sbc_hl_sp(ZilogZ80_t *cpu);

/* ----------------------------------- DEC ---------------------------------- */
/**
 * @brief Instruction function that decrements register pair BC
 * 
 * @param cpu 
 * @return int Cycle count (6)
 */
static int dec_bc(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that decrements register pair DE
 * 
 * @param cpu 
 * @return int Cycle count (6)
 */
static int dec_de(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that decrements register pair HL
 * 
 * @param cpu 
 * @return int Cycle count (6)
 */
static int dec_hl(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that decrements register pair SP
 * 
 * @param cpu 
 * @return int Cycle count (6)
 */
static int dec_sp(ZilogZ80_t *cpu);

/**
 * @brief Instruction function that decrements register A
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int dec_a(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that decrements register B
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int dec_b(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that decrements register C
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int dec_c(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that decrements register D
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int dec_d(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that decrements register E
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int dec_e(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that decrements register H
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int dec_h(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that decrements register L
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int dec_l(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that decrements the memory address in HL
 * 
 * @param cpu 
 * @return int Cycle count (11)
 */
static int dec_hl_addr(ZilogZ80_t *cpu);

/* ----------------------------------- AND ---------------------------------- */
/**
 * @brief Instruction function that ands register A with an immediate value
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int and_n(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that ands register A with register A
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int and_a(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that ands register A with register B
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int and_b(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that ands register A with register C
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int and_c(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that ands register A with register D
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int and_d(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that ands register A with register E
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int and_e(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that ands register A with register H
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int and_h(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that ands register A with register L
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int and_l(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that ands register A with the memory address in HL
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int and_hl_addr(ZilogZ80_t *cpu);

/* ----------------------------------- OR ----------------------------------- */
/**
 * @brief Instruction function that ors register A with an immediate value
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int or_n(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that ors register A with register A
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int or_a(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that ors register A with register B
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int or_b(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that ors register A with register C
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int or_c(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that ors register A with register D
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int or_d(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that ors register A with register E
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int or_e(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that ors register A with register H
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int or_h(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that ors register A with register L
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int or_l(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that ors register A with the memory address in HL
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int or_hl_addr(ZilogZ80_t *cpu);

/* ----------------------------------- XOR ---------------------------------- */
/**
 * @brief Instruction function that xors register A with an immediate value
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int xor_n(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that xors register A with register A
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int xor_a(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that xors register A with register B
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int xor_b(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that xors register A with register C
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int xor_c(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that xors register A with register D
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int xor_d(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that xors register A with register E
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int xor_e(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that xors register A with register H
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int xor_h(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that xors register A with register L
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int xor_l(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that xors register A with the memory address in HL
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int xor_hl_addr(ZilogZ80_t *cpu);

/* ----------------------------------- CP ----------------------------------- */
/**
 * @brief Instruction function that compares register A with an immediate value
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int cp_n(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that compares register A with register A
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int cp_a(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that compares register A with register B
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int cp_b(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that compares register A with register C
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int cp_c(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that compares register A with register D
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int cp_d(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that compares register A with register E
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int cp_e(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that compares register A with register H
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int cp_h(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that compares register A with register L
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int cp_l(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that compares register A with the memory address in HL
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int cp_hl_addr(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that compares register A with the memory address in HL and increments HL and decrements BC. If BC is not zero, it repeats the process
 * ! This function is not implemented
 * 
 * @param cpu 
 * @return int Cycle count (16)
 */
static int cpi(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that compares register A with the memory address in HL and increments HL and decrements BC. If BC is not zero and the value of A is different from the value in the memory address, it repeats the process
 * ! This function is not implemented
 * 
 * @param cpu 
 * @return int Cycle count (21/16)
 */
static int cpir(ZilogZ80_t *cpu);
static int cpd(ZilogZ80_t *cpu);
static int cpdr(ZilogZ80_t *cpu);

/* ---------------------------------- PUSH ---------------------------------- */
/**
 * @brief Instruction function that pushes the register pair BC to the stack
 * 
 * @param cpu 
 * @return int Cycle count (11)
 */
static int push_bc(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that pushes the register pair DE to the stack
 * 
 * @param cpu 
 * @return int Cycle count (11)
 */
static int push_de(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that pushes the register pair HL to the stack
 * 
 * @param cpu 
 * @return int Cycle count (11)
 */
static int push_hl(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that pushes the register pair AF to the stack
 * 
 * @param cpu 
 * @return int Cycle count (11)
 */
static int push_af(ZilogZ80_t *cpu);

/* ----------------------------------- POP ---------------------------------- */
/**
 * @brief Instruction function that pops the stack to the register pair BC
 * 
 * @param cpu 
 * @return int Cycle count (10)
 */
static int pop_bc(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that pops the stack to the register pair DE
 * 
 * @param cpu 
 * @return int Cycle count (10)
 */
static int pop_de(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that pops the stack to the register pair HL
 * 
 * @param cpu 
 * @return int Cycle count (10)
 */
static int pop_hl(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that pops the stack to the register pair AF
 * 
 * @param cpu 
 * @return int Cycle count (10)
 */
static int pop_af(ZilogZ80_t *cpu);

/* ---------------------------------- CALL ---------------------------------- */
/**
 * @brief Instruction function that calls a function if the zero flag is not set
 * 
 * @param cpu 
 * @return int Cycle count (10/17)
 */
static int call_nz_nn(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that calls a function if the zero flag is set
 * 
 * @param cpu 
 * @return int Cycle count (10/17)
 */
static int call_z_nn(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that calls a function if the carry flag is not set
 * 
 * @param cpu 
 * @return int Cycle count (10/17)
 */
static int call_nn(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that calls a function if the carry flag is set
 * 
 * @param cpu 
 * @return int Cycle count (10/17)
 */
static int call_nc_nn(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that calls a function if the parity flag is odd
 * 
 * @param cpu 
 * @return int Cycle count (10/17)
 */
static int call_c_nn(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that calls a function if the parity flag is even
 * 
 * @param cpu 
 * @return int Cycle count (10/17)
 */
static int call_po_nn(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that calls a function if the parity flag is odd
 * 
 * @param cpu 
 * @return int Cycle count (10/17)
 */
static int call_pe_nn(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that calls a function if the sign flag is positive
 * 
 * @param cpu 
 * @return int Cycle count (10/17)
 */
static int call_p_nn(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that calls a function if the sign flag is negative
 * 
 * @param cpu 
 * @return int Cycle count (10/17)
 */
static int call_m_nn(ZilogZ80_t *cpu);

/* ----------------------------------- RET ---------------------------------- */
/**
 * @brief Instruction function that returns if the zero flag is not set
 * 
 * @param cpu 
 * @return int Cycle count (5/11)
 */
static int ret_nz(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that returns if the zero flag is set
 * 
 * @param cpu 
 * @return int Cycle count (5/11)
 */
static int ret_z(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that returns if the carry flag is not set
 * 
 * @param cpu 
 * @return int Cycle count (5/11)
 */
static int ret_nc(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that returns if the carry flag is set
 * 
 * @param cpu 
 * @return int Cycle count (5/11)
 */
static int ret_c(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that returns if the parity flag is odd
 * 
 * @param cpu 
 * @return int Cycle count (5/11)
 */
static int ret_po(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that returns if the parity flag is even
 * 
 * @param cpu 
 * @return int Cycle count (5/11)
 */
static int ret_pe(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that returns if the sign flag is positive
 * 
 * @param cpu 
 * @return int Cycle count (5/11)
 */
static int ret_p(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that returns if the sign flag is negative
 * 
 * @param cpu 
 * @return int Cycle count (5/11)
 */
static int ret_m(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that returns if the interrupt is enabled
 * 
 * @param cpu 
 * @return int Cycle count (10)
 */
static int ret(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that returns if the interrupt is enabled and enables the interrupt
 * 
 * @param cpu 
 * @return int Cycle count (14)
 */
static int retn(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that returns if the interrupt is enabled and enables the interrupt
 * 
 * @param cpu 
 * @return int Cycle count (14)
 */
static int reti(ZilogZ80_t *cpu);

/* --------------------------------- ROTATE --------------------------------- */
/**
 * @brief Instruction function that rotates register A to the left
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int rlca(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that rotates register A to the right
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int rrca(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that rotates register A to the left through the carry
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int rla(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that rotates register A to the right through the carry
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int rra(ZilogZ80_t *cpu);

/* ----------------------------------- RST ---------------------------------- */
/**
 * @brief Instruction function that calls the function at address 0x00
 * 
 * @param cpu 
 * @return int Cycle count (11)
 */
static int rst_00h(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that calls the function at address 0x08
 * 
 * @param cpu 
 * @return int Cycle count (11)
 */
static int rst_08h(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that calls the function at address 0x10
 * 
 * @param cpu 
 * @return int Cycle count (11)
 */
static int rst_10h(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that calls the function at address 0x18
 * 
 * @param cpu 
 * @return int Cycle count (11)
 */
static int rst_18h(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that calls the function at address 0x20
 * 
 * @param cpu 
 * @return int Cycle count (11)
 */
static int rst_20h(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that calls the function at address 0x28
 * 
 * @param cpu 
 * @return int Cycle count (11)
 */
static int rst_28h(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that calls the function at address 0x30
 * 
 * @param cpu 
 * @return int Cycle count (11)
 */
static int rst_30h(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that calls the function at address 0x38
 * 
 * @param cpu 
 * @return int Cycle count (11)
 */
static int rst_38h(ZilogZ80_t *cpu);

/* ---------------------------------- JUMP ---------------------------------- */

/**
 * @brief Instruction function that decrements register B and jumps to an address if B is not zero
 * 
 * @param cpu 
 * @return int Cycle count (13/8)
 */
static int djnz_d(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that adds an immediate value to the program counter and jumps to the new address
 * 
 * @param cpu 
 * @return int Cycle count (12)
 */ 
static int jr_d(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that adds an immediate value to the program counter and jumps to the new address if the zero flag is not set
 * 
 * @param cpu 
 * @return int Cycle count (12/7)
 */
static int jr_nz_d(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that adds an immediate value to the program counter and jumps to the new address if the zero flag is set
 * 
 * @param cpu 
 * @return int Cycle count (12/7)
 */
static int jr_z_b(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that adds an immediate value to the program counter and jumps to the new address if the carry flag is not set
 * 
 * @param cpu 
 * @return int Cycle count (12/7)
 */
static int jr_nc_d(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that adds an immediate value to the program counter and jumps to the new address if the carry flag is set
 * 
 * @param cpu 
 * @return int Cycle count (12/7)
 */
static int jr_c_b(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads an immediate value to the program counter if the zero flag is not set
 * 
 * @param cpu 
 * @return int Cycle count (10)
 */
static int jp_nz_nn(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads an immediate value to the program counter
 * 
 * @param cpu 
 * @return int Cycle count (10)
 */
static int jp_nn(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads an immediate value to the program counter if the zero flag is set
 * 
 * @param cpu 
 * @return int Cycle count (10)
 */
static int jp_z_nn(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads an immediate value to the program counter if the carry flag is not set
 * 
 * @param cpu 
 * @return int Cycle count (10)
 */
static int jp_nc_nn(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads an immediate value to the program counter if the carry flag is set
 * 
 * @param cpu 
 * @return int Cycle count (10)
 */
static int jp_c_nn(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads an immediate value to the program counter if the parity flag is odd
 * 
 * @param cpu 
 * @return int Cycle count (10)
 */
static int jp_po_nn(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads an immediate value to the program counter if the parity flag is even
 * 
 * @param cpu 
 * @return int Cycle count (10)
 */
static int jp_pe_nn(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of address HL to the program counter
 * 
 * @param cpu 
 * @return int Cycle count (10)
 */
static int jp_hl_addr(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads an immediate value to the program counter if the sign flag is positive
 * 
 * @param cpu 
 * @return int Cycle count (10)
 */
static int jp_p_nn(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads an immediate value to the program counter if the sign flag is set
 * 
 * @param cpu 
 * @return int Cycle count (10)
 */
static int jp_m_nn(ZilogZ80_t *cpu);

/* -------------------------------- EXCHANGE -------------------------------- */
/**
 * @brief Instruction function that exchanges the values of register pairs BC, DE and HL with BC', DE' and HL'
 * 
 * @param cpu 
 * @return int Cycle count (4) 
 */
static int exx(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that exchanges the values of register pairs AF and AF'
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ex_af_af_(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that exchanges the values at address SP with L and SP+1 with H       
 * 
 * @param cpu 
 * @return int Cycle count (19)
 */
static int ex_sp_hl_addr(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that exchanges the values of register pairs DE and HL
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ex_de_hl(ZilogZ80_t *cpu);

/* ----------------------------------- LD ----------------------------------- */
/**
 * @brief Instruction function that loads an immediate value to register A
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int ld_a_n(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register A to register A
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_a_a(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register A to register B
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_a_b(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register A to register C
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_a_c(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register A to register D
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_a_d(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register A to register E
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_a_e(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register A to register H
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_a_h(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register A to register L
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_a_l(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register A to the memory address in HL
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */     
static int ld_a_hl_addr(ZilogZ80_t *cpu);

/**
 * @brief Instruction function that loads an immediate value to register B
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int ld_b_n(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register A to register B
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_b_a(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register B to register B
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_b_b(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register C to register B
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_b_c(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register D to register B
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_b_d(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register E to register B
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_b_e(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register H to register B
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_b_h(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register L to register B
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_b_l(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of the memory address in HL to register B
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int ld_b_hl_addr(ZilogZ80_t *cpu);

/**
 * @brief Instruction function that loads an immediate value to register C
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int ld_c_n(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register A to register C
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_c_a(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register B to register C
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_c_b(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register C to register C
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_c_c(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register D to register C
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_c_d(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register E to register C
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_c_e(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register H to register C
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_c_h(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register L to register C
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_c_l(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of the memory address in HL to register C
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int ld_c_hl_addr(ZilogZ80_t *cpu);

/**
 * @brief Instruction function that loads an immediate value to register D
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int ld_d_n(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register A to register D
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_d_a(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register B to register D
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_d_b(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register C to register D
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_d_c(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register D to register D
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_d_d(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register E to register D
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_d_e(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register H to register D
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_d_h(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register L to register D
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_d_l(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of the memory address in HL to register D
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int ld_d_hl_addr(ZilogZ80_t *cpu);

/**
 * @brief Instruction function that loads an immediate value to register E
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int ld_e_n(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register A to register E
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_e_a(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register B to register E
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_e_b(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register C to register E
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_e_c(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register D to register E
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_e_d(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register E to register E
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_e_e(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register H to register E
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_e_h(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register L to register E
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_e_l(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of the memory address in HL to register E
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int ld_e_hl_addr(ZilogZ80_t *cpu);

/**
 * @brief Instruction function that loads an immediate value to register H
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int ld_h_n(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register A to register H
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_h_a(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register B to register H
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_h_b(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register C to register H
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_h_c(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register D to register H
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_h_d(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register E to register H
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_h_e(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register H to register H
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_h_h(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register L to register H
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_h_l(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of the memory address in HL to register H
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int ld_h_hl_addr(ZilogZ80_t *cpu);

/**
 * @brief Instruction function that loads an immediate value to register L
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int ld_l_n(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register A to register L
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_l_a(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register B to register L
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_l_b(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register C to register L
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_l_c(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register D to register L
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_l_d(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register E to register L
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_l_e(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register H to register L
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_l_h(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register L to register L
 * 
 * @param cpu 
 * @return int Cycle count (4)
 */
static int ld_l_l(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of the memory address in HL to register L
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int ld_l_hl_addr(ZilogZ80_t *cpu);

/**
 * @brief Instruction function that loads an immediate value to the memory address in HL
 * 
 * @param cpu 
 * @return int Cycle count (10)
 */
static int ld_hl_n_addr(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register A to the memory address in HL
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int ld_hl_a_addr(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register B to the memory address in HL
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int ld_hl_b_addr(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register C to the memory address in HL
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int ld_hl_c_addr(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register D to the memory address in HL
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int ld_hl_d_addr(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register E to the memory address in HL
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int ld_hl_e_addr(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register H to the memory address in HL
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int ld_hl_h_addr(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register L to the memory address in HL
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int ld_hl_l_addr(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of the memory address in HL to register A
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */ 
static int ld_hl_hl_addr(ZilogZ80_t *cpu);

/**
 * @brief Instruction function that loads the value of the memory address in NN to register BC
 * 
 * @param cpu 
 * @return int Cycle count (10)
 */
static int ld_hl_nn_addr(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of the memory address in NN to register DE
 * 
 * @param cpu 
 * @return int Cycle count (10)
 */
static int ld_a_nn_addr(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of the memory address in NN to register HL
 * 
 * @param cpu 
 * @return int Cycle count (10)
 */
static int ld_bc_nn_addr(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of the memory address in NN to register SP
 * 
 * @param cpu 
 * @return int Cycle count (10)
 */
static int ld_de_nn_addr(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of the memory address in NN to register SP
 * 
 * @param cpu 
 * @return int Cycle count (10)
 */
static int ld_sp_nn_addr(ZilogZ80_t *cpu);

/**
 * @brief Instruction function that loads the value of register HL to the memory address in NN
 * 
 * @param cpu 
 * @return int Cycle count (13)
 */
static int ld_nn_hl_addr(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register A to the memory address in NN
 * 
 * @param cpu 
 * @return int Cycle count (13)
 */
static int ld_nn_a_addr(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register BC to the memory address in NN
 * 
 * @param cpu 
 * @return int Cycle count (13)
 */
static int ld_nn_bc_addr(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register DE to the memory address in NN
 * 
 * @param cpu 
 * @return int Cycle count (13)
 */
static int ld_nn_de_addr(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register SP to the memory address in NN
 * 
 * @param cpu 
 * @return int Cycle count (13)
 */
static int ld_nn_sp_addr(ZilogZ80_t *cpu);

/**
 * @brief Instruction function that loads the value of register A to the memory address in DE
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int ld_de_nn_imm(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of the memory address in DE to register A
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int ld_de_a_addr(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register A to the memory address in DE
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int ld_a_de_addr(ZilogZ80_t *cpu);

/**
 * @brief Instruction function that loads the value of register A to the memory address in BC
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int ld_bc_nn_imm(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of the memory address in BC to register A
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int ld_bc_a_addr(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register A to the memory address in BC
 * 
 * @param cpu 
 * @return int Cycle count (7)
 */
static int ld_a_bc_addr(ZilogZ80_t *cpu);

/**
 * @brief Instruction function that loads an immediate value to register SP
 * 
 * @param cpu 
 * @return int Cycle count (10)
 */
static int ld_hl_nn_imm(ZilogZ80_t *cpu);

/**
 * @brief Instruction function that loads the value of register SP to register HL
 * 
 * @param cpu 
 * @return int Cycle count (6)
 */
static int ld_sp_nn_imm(ZilogZ80_t *cpu);

/**
 * @brief Instruction function that loads the value of register SP to register HL
 * 
 * @param cpu 
 * @return int Cycle count (6)
 */
static int ld_sp_hl(ZilogZ80_t *cpu);

/**
 * @brief Instruction function that loads the value of memory address in HL to memory address in DE. Then HL and DE are incremented and BC is decremented. If BC is zero, flag p/v is reset
 * 
 * @param cpu 
 * @return int Cycle count (6)
 */
static int ldi(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of memory address in HL to memory address in DE. Then HL and DE are incremented and BC is decremented. If BC is not zero, the function is repeated (Interrupts are still processed)
 * 
 * @param cpu 
 * @return int Cycle count (6)
 */
static int ldir(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of memory address in HL to memory address in DE. Then HL and DE are decremented and BC is decremented. If BC is zero, flag p/v is reset
 * !Check if this is correct
 * 
 * @param cpu 
 * @return int Cycle count (6)
 */
static int ldd(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that loads the value of register SP to register HL
 * !Check if this is correct
 * 
 * @param cpu 
 * @return int Cycle count (6)
 */
static int lddr(ZilogZ80_t *cpu);

static int ld_nn_bc_imm(ZilogZ80_t *cpu);
static int ld_nn_de_imm(ZilogZ80_t *cpu);
static int ld_nn_hl_imm(ZilogZ80_t *cpu);
static int ld_nn_sp_imm(ZilogZ80_t *cpu);

static int ld_r_a(ZilogZ80_t *cpu);
static int ld_a_r(ZilogZ80_t *cpu);

static int ld_i_a(ZilogZ80_t *cpu);
static int ld_a_i(ZilogZ80_t *cpu);

/* ---------------------------- OTHER INSTRUCTION --------------------------- */
static int bit_op(ZilogZ80_t *cpu);
static int ix_op(ZilogZ80_t *cpu);
static int misc_op(ZilogZ80_t *cpu);
static int iy_op(ZilogZ80_t *cpu);

// INTERRUPTS           -----------------------------------------------------------------------------
static int di(ZilogZ80_t *cpu);
static int ei(ZilogZ80_t *cpu);

/* ---------------------------------- PORTS --------------------------------- */
/**
 * @brief Instruction function that reads a byte from an I/O port and loads it to register A
 * 
 * @param cpu 
 * @return int Cycle count (11)
 */
static int in_a_n(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that reads a byte from an I/O port and loads it to register B
 * 
 * @param cpu 
 * @return int Cycle count (11)
 */
static int in_b_c(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that reads a byte from an I/O port and loads it to register D
 * 
 * @param cpu 
 * @return int Cycle count (11)
 */
static int in_d_c(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that reads a byte from an I/O port and loads it to register E
 * 
 * @param cpu 
 * @return int Cycle count (11)
 */
static int in_e_c(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that reads a byte from an I/O port and loads it to register H
 * 
 * @param cpu 
 * @return int Cycle count (11)
 */
static int in_h_c(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that reads a byte from an I/O port and loads it to register L
 * 
 * @param cpu 
 * @return int Cycle count (11)
 */
static int in_l_c(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that reads a byte from I/O port C to the memory address in HL and increments HL and decrements B
 * 
 * @param cpu 
 * @return int Cycle count (16)
 */
static int ini(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that reads a byte from I/O port C to the memory address in HL and increments HL and decrements B. If B is not zero, this function is repeated (Interrupts are still processed)
 * 
 * @param cpu 
 * @return int Cycle count (21/16)
 */
static int inir(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that reads a byte from I/O port C to register C
 * 
 * @param cpu 
 * @return int Cycle count (12)
 */
static int in_c_c(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that reads a byte from I/O port C to register A
 * 
 * @param cpu 
 * @return int Cycle count (12)
 */
static int in_a_c(ZilogZ80_t *cpu);
static int ind(ZilogZ80_t *cpu);
static int indr(ZilogZ80_t *cpu);

static int in0_a_n(ZilogZ80_t *cpu);
static int in0_b_n(ZilogZ80_t *cpu);
static int in0_c_n(ZilogZ80_t *cpu);
static int in0_d_n(ZilogZ80_t *cpu);
static int in0_e_n(ZilogZ80_t *cpu);
static int in0_h_n(ZilogZ80_t *cpu);
static int in0_l_n(ZilogZ80_t *cpu);
static int in_c(ZilogZ80_t *cpu);


static int out_n_a_addr(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that writes a byte from register A to I/O port n
 * 
 * @param cpu 
 * @return int Cycle count (11)
 */
static int out_c_a(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that writes a byte from register B to I/O port C
 * 
 * @param cpu 
 * @return int Cycle count (12)
 */
static int out_c_b(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that writes a byte from register C to I/O port C
 * 
 * @param cpu 
 * @return int Cycle count (12)
 */
static int out_c_c(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that writes a byte from register D to I/O port C
 * 
 * @param cpu 
 * @return int Cycle count (12)
 */
static int out_c_d(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that writes a byte from register E to I/O port C
 * 
 * @param cpu 
 * @return int Cycle count (12)
 */
static int out_c_e(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that writes a byte from register H to I/O port C
 * 
 * @param cpu 
 * @return int Cycle count (12)
 */
static int out_c_h(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that writes a byte from register L to I/O port C
 * 
 * @param cpu 
 * @return int Cycle count (12)
 */
static int out_c_l(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that decrements B and writes a byte from memory location HL to I/O port C. Then HL is incremented
 * 
 * @param cpu 
 * @return int Cycle count (16)
 */
static int outi(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that decrements B and writes a byte from memory location HL to I/O port C. Then HL is incremented. If B is not zero, this function is repeated (Interrupts are still processed)
 * 
 * @param cpu 
 * @return int Cycle count (21/16)
 */
static int otir(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that decrements B and writes a byte from memory address HL to I/O port C. Then HL and C are incremented
 * 
 * @param cpu 
 * @return int Cycle count (14)
 */
static int otim(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that decrements B and writes a byte from memory address HL to I/O port C. Then HL and C are incremented. If B is not zero, this function is repeated (Interrupts are still processed)
 * 
 * @param cpu 
 * @return int Cycle count (16/14)
 */
static int otimr(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that decrements B and writes a byte from memory address HL to I/O port C. Then HL and C are decremented
 * 
 * @param cpu 
 * @return int Cycle count (14)
 */
static int otdm(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that decrements B and writes a byte from memory address HL to I/O port C. Then HL and C are decremented. If B is not zero, this function is repeated (Interrupts are still processed)
 * 
 * @param cpu 
 * @return int Cycle count (16/14)
 */
static int otdmr(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that decrements B and writes a byte from memory location HL to I/O port C. Then HL is decremented
 * 
 * @param cpu 
 * @return int Cycle count (16)
 */
static int outd(ZilogZ80_t *cpu);
/**
 * @brief Instruction function that decrements B and writes a byte from memory location HL to I/O port C. Then HL is decremented. If B is not zero, this function is repeated (Interrupts are still processed)
 * 
 * @param cpu 
 * @return int Cycle count (21/16)
 */
static int otdr(ZilogZ80_t *cpu);

static int out_c_0(ZilogZ80_t *cpu);
static int out0_c_n(ZilogZ80_t *cpu);
static int out0_e_n(ZilogZ80_t *cpu);
static int out0_l_n(ZilogZ80_t *cpu);
static int out0_a_n(ZilogZ80_t *cpu);
static int out0_n_b(ZilogZ80_t *cpu);
static int out0_n_d(ZilogZ80_t *cpu);
static int out0_n_h(ZilogZ80_t *cpu);

static int daa(ZilogZ80_t *cpu);
static int scf(ZilogZ80_t *cpu);

// TST      -----------------------------------------------------------------------------
static int tst_b(ZilogZ80_t *cpu);
static int tst_d(ZilogZ80_t *cpu);
static int tst_h(ZilogZ80_t *cpu);
static int tst_c(ZilogZ80_t *cpu);
static int tst_e(ZilogZ80_t *cpu);
static int tst_l(ZilogZ80_t *cpu);
static int tst_a(ZilogZ80_t *cpu);
static int tst_hl_addr(ZilogZ80_t *cpu);
static int tst_n(ZilogZ80_t *cpu);
static int tstio_n(ZilogZ80_t *cpu);

// MULT     -----------------------------------------------------------------------------
static int mlt_bc(ZilogZ80_t *cpu);
static int mlt_de(ZilogZ80_t *cpu);
static int mlt_hl(ZilogZ80_t *cpu);
static int mlt_sp(ZilogZ80_t *cpu);

// IM       -----------------------------------------------------------------------------
static int im_0(ZilogZ80_t *cpu);
static int im_1(ZilogZ80_t *cpu);
static int im_2(ZilogZ80_t *cpu);

// EXTRA    -----------------------------------------------------------------------------
static int cpl(ZilogZ80_t *cpu);
static int ccf(ZilogZ80_t *cpu);
static int neg(ZilogZ80_t *cpu);
static int slp(ZilogZ80_t *cpu);
static int rld(ZilogZ80_t *cpu);
static int rrd(ZilogZ80_t *cpu);

// Instruction table -----------------------------------------------------------------
static const InstructionHandler_t mainInstructionTable[MAX_INSTRUCTION_COUNT] = 
{
/*      0               1               2               3               4               5               6               7               8               9                   A                   B               C               D           E               F*/
/*0x0*/ nop,            ld_bc_nn_imm,   ld_bc_a_addr,   inc_bc,         inc_b,          dec_b,          ld_b_n,         rlca,           ex_af_af_,      add_hl_bc_imm,      ld_a_bc_addr,       dec_bc,         inc_c,          dec_c,      ld_c_n,         rrca,
/*0x1*/ djnz_d,         ld_de_nn_imm,   ld_de_a_addr,   inc_de,         inc_d,          dec_d,          ld_d_n,         rla,            jr_d,           add_hl_de_imm,      ld_a_de_addr,       dec_de,         inc_e,          dec_e,      ld_e_n,         rra,
/*0x2*/ jr_nz_d,        ld_hl_nn_imm,   ld_nn_hl_addr,  inc_hl,         inc_h,          dec_h,          ld_h_n,         daa,            jr_z_b,         add_hl_hl_imm,      ld_hl_nn_addr,      dec_hl,         inc_l,          dec_l,      ld_l_n,         cpl,
/*0x3*/ jr_nc_d,        ld_sp_nn_imm,   ld_nn_a_addr,   inc_sp,         inc_hl_addr,    dec_hl_addr,    ld_hl_n_addr,   scf,            jr_c_b,         add_hl_sp_imm,      ld_a_nn_addr,       dec_sp,         inc_a,          dec_a,      ld_a_n,         ccf,
/*0x4*/ ld_b_b,         ld_b_c,         ld_b_d,         ld_b_e,         ld_b_h,         ld_b_l,         ld_b_hl_addr,   ld_b_a,         ld_c_b,         ld_c_c,             ld_c_d,             ld_c_e,         ld_c_h,         ld_c_l,     ld_c_hl_addr,   ld_c_a,
/*0x5*/ ld_d_b,         ld_d_c,         ld_d_d,         ld_d_e,         ld_d_h,         ld_d_l,         ld_d_hl_addr,   ld_d_a,         ld_d_b,         ld_e_c,             ld_e_d,             ld_e_e,         ld_e_h,         ld_e_l,     ld_e_hl_addr,   ld_e_a,
/*0x6*/ ld_h_b,         ld_h_c,         ld_h_d,         ld_h_e,         ld_h_h,         ld_h_l,         ld_h_hl_addr,   ld_h_a,         ld_h_b,         ld_l_c,             ld_l_d,             ld_l_e,         ld_l_h,         ld_l_l,     ld_l_hl_addr,   ld_l_a,
/*0x7*/ ld_hl_b_addr,   ld_hl_c_addr,   ld_hl_d_addr,   ld_hl_e_addr,   ld_hl_h_addr,   ld_hl_l_addr,   halt,           ld_hl_a_addr,   ld_hl_b_addr,   ld_a_c,             ld_a_d,             ld_a_e,         ld_a_h,         ld_a_l,     ld_a_hl_addr,   ld_a_a,
/*0x8*/ add_a_b,        add_a_c,        add_a_d,        add_a_e,        add_a_h,        add_a_l,        add_a_hl_addr,  add_a_a,        adc_a_b,        adc_a_c,            adc_a_d,            adc_a_e,        adc_a_h,        adc_a_l,    adc_a_hl_addr,  adc_a_a,
/*0x9*/ sub_b,          sub_c,          sub_d,          sub_e,          sub_h,          sub_l,          sub_hl_addr,    sub_a,          sbc_b,          sbc_c,              sbc_d,              sbc_e,          sbc_h,          sbc_l,      sbc_hl_addr,    sbc_a_a,
/*0xA*/ and_b,          and_c,          and_d,          and_e,          and_h,          and_l,          and_hl_addr,    and_a,          xor_b,          xor_c,              xor_d,              xor_e,          xor_h,          xor_l,      xor_hl_addr,    xor_a,
/*0xB*/ or_b,           or_c,           or_d,           or_e,           or_h,           or_l,           or_hl_addr,     or_a,           cp_b,           cp_c,               cp_d,               cp_e,           cp_h,           cp_l,       cp_hl_addr,     cp_a,
/*0xC*/ ret_nz,         pop_bc,         jp_nz_nn,       jp_nn,          call_nz_nn,     push_bc,        add_a_n,        rst_00h,        ret_z,          ret,                jp_z_nn,            bit_op,         call_z_nn,      call_nn,    adc_a_n,        rst_08h,
/*0xD*/ ret_nc,         pop_de,         jp_nc_nn,       out_n_a_addr,   call_nc_nn,     push_de,        sub_n,          rst_10h,        ret_c,          exx,                jp_c_nn,            in_a_n,         call_c_nn,      ix_op,      sbc_a_n,        rst_18h,
/*0xE*/ ret_po,         pop_hl,         jp_po_nn,       ex_sp_hl_addr,  call_po_nn,     push_hl,        and_n,          rst_20h,        ret_pe,         jp_hl_addr,         jp_pe_nn,           ex_de_hl,       call_pe_nn,     misc_op,    xor_n,          rst_28h,
/*0xF*/ ret_p,          pop_af,         jp_p_nn,        di,             call_p_nn,      push_af,        or_n,           rst_30h,        ret_m,          ld_sp_hl,           jp_m_nn,            ei,             call_m_nn,      iy_op,      cp_n,           rst_38h
};

static const InstructionHandler_t miscInstructionTable[MAX_INSTRUCTION_COUNT] =
{
/*      0               1               2               3               4               5               6               7               8               9                   A                   B               C               D           E               F*/
/*0x0*/ in0_b_n,        out0_n_b,       no_func,        no_func,        tst_b,          no_func,        no_func,        no_func,        in0_c_n,        out0_c_n,           no_func,            no_func,        tst_c,          no_func,    no_func,        no_func,
/*0x1*/ in0_d_n,        out0_n_d,       no_func,        no_func,        tst_d,          no_func,        no_func,        no_func,        in0_e_n,        out0_e_n,           no_func,            no_func,        tst_e,          no_func,    no_func,        no_func,
/*0x2*/ in0_h_n,        out0_n_h,       no_func,        no_func,        tst_h,          no_func,        no_func,        no_func,        in0_l_n,        out0_l_n,           no_func,            no_func,        tst_l,          no_func,    no_func,        no_func,
/*0x3*/ no_func,        no_func,        no_func,        no_func,        tst_hl_addr,    no_func,        no_func,        no_func,        in0_a_n,        out0_a_n,           no_func,            no_func,        tst_a,          no_func,    no_func,        no_func,
/*0x4*/ in_b_c,         out_c_b,        sbc_hl_bc,      ld_nn_bc_addr,  neg,            retn,           im_0,           ld_i_a,         in_c_c,         out_c_c,            adc_hl_bc,          ld_bc_nn_addr,  mlt_bc,         reti,       no_func,        ld_r_a,
/*0x5*/ in_d_c,         out_c_d,        sbc_hl_de,      ld_nn_de_addr,  no_func,        no_func,        im_1,           ld_a_i,         in_e_c,         out_c_e,            adc_hl_de,          ld_de_nn_addr,  mlt_de,         no_func,    im_2,           ld_a_r,
/*0x6*/ in_h_c,         out_c_h,        sbc_hl_hl,      ld_nn_hl_addr,  tst_n,          no_func,        no_func,        rrd,            in_l_c,         out_c_l,            adc_hl_hl,          ld_hl_nn_addr,  mlt_hl,         no_func,    no_func,        rld,
/*0x7*/ in_c,           out_c_0,        sbc_hl_sp,      ld_nn_sp_addr,  tstio_n,        no_func,        slp,            no_func,        in_a_c,         out_c_a,            adc_hl_sp,          ld_sp_nn_addr,  mlt_sp,         no_func,    no_func,        no_func,
/*0x8*/ no_func,        no_func,        no_func,        otim,           no_func,        no_func,        no_func,        no_func,        no_func,        no_func,            no_func,            otdm,           no_func,        no_func,    no_func,        no_func,
/*0x9*/ no_func,        no_func,        no_func,        otimr,          no_func,        no_func,        no_func,        no_func,        no_func,        no_func,            no_func,            otdmr,          no_func,        no_func,    no_func,        no_func,
/*0xA*/ ldi,            cpi,            ini,            outi,           no_func,        no_func,        no_func,        no_func,        ldd,            cpd,                ind,                outd,           no_func,        no_func,    no_func,        no_func,
/*0xB*/ ldir,           cpir,           inir,           otir,           no_func,        no_func,        no_func,        no_func,        lddr,           cpdr,               indr,               otdr,           no_func,        no_func,    no_func,        no_func,
/*0xC*/ no_func,        no_func,        no_func,        no_func,        no_func,        no_func,        no_func,        no_func,        no_func,        no_func,            no_func,            otdr,           no_func,        no_func,    no_func,        no_func,
/*0xD*/ no_func,        no_func,        no_func,        no_func,        no_func,        no_func,        no_func,        no_func,        no_func,        no_func,            no_func,            no_func,        no_func,        no_func,    no_func,        no_func,
/*0xE*/ no_func,        no_func,        no_func,        no_func,        no_func,        no_func,        no_func,        no_func,        no_func,        no_func,            no_func,            no_func,        no_func,        no_func,    no_func,        no_func,
/*0xF*/ no_func,        no_func,        no_func,        no_func,        no_func,        no_func,        no_func,        no_func,        no_func,        no_func,            no_func,            no_func,        no_func,        no_func,    no_func,        no_func,
};

int executeInstruction(ZilogZ80_t *cpu)
{
    byte_t opcode = fetchByteAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
    cpu->PC++;

    int cycles = mainInstructionTable[opcode](cpu);

    cpu->currentCycles = cycles;
    cpu->totalCycles += cycles;

    return 0;
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

static bool calculateParity(word_t value)
{
    int count = 0;
    bool parity = false;

    while (value)
    {
        count += value & 1;
        value >>= 1;
    }

    if(count % 2 == 0)
    {
        parity = true;
    }
    
    return parity;
}

static void setFlags(ZilogZ80_t *cpu, byte_t regA, byte_t operand, word_t result, bool isSubstraction)
{
    cpu->F.Z = (result & 0xFF) == 0;
    cpu->F.S = (result & 0x80) >> 7;
    cpu->F.H = isSubstraction ? ((regA & 0x0F) - (operand & 0x0F)) < 0 : ((regA & 0x0F) + (operand & 0x0F)) > 0x0F;
    cpu->F.P = calculateParity(result & 0xFF);
    cpu->F.N = isSubstraction;
    cpu->F.C = result > 0xFF;
}
static void setFlagsWord(ZilogZ80_t *cpu, word_t reg1, word_t reg2, dword_t result)
{
    cpu->F.Z = (result & 0xFFFF) == 0;
    cpu->F.S = (result & 0x8000) >> 15;

    cpu->F.H = ((reg1 & 0x0FFF) + (reg2 & 0x0FFF)) > 0x0FFF;

    cpu->F.C = result > 0xFFFF;

    cpu->F.N = 0;
}

// Helper functions ------------------------------------------------------------
static void addToRegister(ZilogZ80_t *cpu, byte_t *reg, byte_t value)
{
    word_t result = (word_t)(*reg + value);
    setFlags(cpu, *reg, value, result, false);
    *reg = result & 0xFF;
}
static void addToRegisterPair(ZilogZ80_t *cpu, word_t value1, word_t value2)
{
    dword_t result = (dword_t)(value1 + value2);
    setFlagsWord(cpu, value1, value2, result);
    cpu->H = cpu->H + value1;   // TODO: May change
    cpu->L = cpu->L + value2;
}
static void addToRegisterWithCarry(ZilogZ80_t *cpu, byte_t *reg, byte_t value)
{
    word_t result = (word_t)(*reg + value + cpu->F.C);
    setFlags(cpu, *reg, value, result, false);
    *reg = result & 0xFF;
}
static void addToRegisterPairWithCarry(ZilogZ80_t *cpu, word_t value1, word_t value2)
{
    dword_t result = (dword_t)(value1 + value2 + cpu->F.C);
    setFlagsWord(cpu, value1, value2, result);
    cpu->H = cpu->H + value1;   // TODO: May change
    cpu->L = cpu->L + value2;
}
static void incrementRegister(ZilogZ80_t *cpu, byte_t *reg)
{
    word_t result = (word_t)(*reg + 1);
    setFlags(cpu, *reg, 1, result, false);
    *reg = result & 0xFF;
}
static void incrementRegisterPair(ZilogZ80_t *cpu, byte_t* upperByte, byte_t* lowerByte)
{
    word_t result = (word_t)(TO_WORD(*upperByte, *lowerByte) + 1);
    *upperByte = UPPER_BYTE(result);
    *lowerByte = LOWER_BYTE(result);
}

static void subtractFromRegister(ZilogZ80_t *cpu, byte_t value)
{
    word_t result = (word_t)(cpu->A - value);
    setFlags(cpu, cpu->A, value, result, true);
    cpu->A = result & 0xFF;
}
static void subtractFromRegisterWithCarry(ZilogZ80_t *cpu, byte_t value)
{
    word_t result = (word_t)(cpu->A - value - cpu->F.C);
    setFlags(cpu, cpu->A, value, result, true);
    cpu->A = result & 0xFF;
}
static void subtractFromRegisterPairWithCarry(ZilogZ80_t *cpu, word_t val1, word_t val2)
{
    dword_t result = (dword_t)(val1 - val2 - cpu->F.C);
    setFlagsWord(cpu, val1, val2, result);
    cpu->H = val1 - val2;   // TODO: May change
    cpu->L = val1 - val2;
}
static void decrementRegister(ZilogZ80_t *cpu, byte_t *reg)
{
    word_t result = (word_t)(*reg - 1);
    setFlags(cpu, *reg, 1, result, true);
    *reg = result & 0xFF;
}
static void decrementRegisterPair(ZilogZ80_t *cpu, byte_t* upperByte, byte_t* lowerByte)
{
    word_t result = (word_t)(TO_WORD(*upperByte, *lowerByte) - 1);
    *upperByte = UPPER_BYTE(result);
    *lowerByte = LOWER_BYTE(result);
}

static void andWithRegister(ZilogZ80_t *cpu, byte_t value)
{
    word_t result = (word_t)(cpu->A & value);
    setFlags(cpu, cpu->A, value, result, false);
    cpu->A = result & 0xFF;
}
static void orWithRegister(ZilogZ80_t *cpu, byte_t value)
{
    word_t result = (word_t)(cpu->A | value);
    setFlags(cpu, cpu->A, value, result, false);
    cpu->A = result & 0xFF;
}
static void xorWithRegister(ZilogZ80_t *cpu, byte_t value)
{
    word_t result = (word_t)(cpu->A ^ value);
    setFlags(cpu, cpu->A, value, result, false);
    cpu->A = result & 0xFF;
}
static void cpWithRegister(ZilogZ80_t *cpu, byte_t value)
{
    word_t result = (word_t)(cpu->A - value);
    setFlags(cpu, cpu->A, value, result, true);
}

static void pushWord(ZilogZ80_t *cpu, word_t value)
{
    storeByteAddressSpace(&cpu->ram, &cpu->rom, cpu->SP - 1, UPPER_BYTE(value));
    storeByteAddressSpace(&cpu->ram, &cpu->rom, cpu->SP - 2, LOWER_BYTE(value));
    cpu->SP -= 2;
}
static void popWord(ZilogZ80_t *cpu, byte_t *upperByte, byte_t *lowerByte)
{
    *lowerByte = fetchByteAddressSpace(&cpu->ram, &cpu->rom, cpu->SP);
    *upperByte = fetchByteAddressSpace(&cpu->ram, &cpu->rom, cpu->SP + 1);
    cpu->SP += 2;
}

static int returnHelper(ZilogZ80_t *cpu, bool condition)
{
    int cycles = 5;

    if(condition)
    {
        byte_t lowerByte, upperByte;
        popWord(cpu, &upperByte, &lowerByte);
        cpu->PC = TO_WORD(upperByte, lowerByte);

        cycles = 11;
    }

    return cycles;
}
static int callHelper(ZilogZ80_t *cpu, bool condition)
{
    int cycles = 10;
    word_t address = fetchWordAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
    cpu->PC += 2;

    if(condition)
    {
        pushWord(cpu, cpu->PC);
        cpu->PC = address;

        cycles = 17;
    }

    return cycles;
}

static int jumpHelper(ZilogZ80_t *cpu, bool condition)
{
    if(condition)
    {
        word_t address = fetchWordAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
        cpu->PC = address;
    }

    return 10;
}
static int jumpRelativeHelper(ZilogZ80_t *cpu, bool condition)
{
    int cycles = 7;

    if(condition)
    {
        byte_t offset = fetchByteAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
        cpu->PC += offset;
        cycles = 12;
    }

    return cycles;
}

static void rst(ZilogZ80_t *cpu, byte_t address)
{
    pushWord(cpu, cpu->PC);
    cpu->PC = address;
}

static void exWordHelper(ZilogZ80_t *cpu, word_t *reg1, word_t *reg2)
{
    word_t temp = *reg1;
    *reg1 = *reg2;
    *reg2 = temp;
}

static void ld(ZilogZ80_t *cpu, byte_t *reg, byte_t value)
{
    *reg = value;
}
static void ldPair(ZilogZ80_t *cpu, byte_t *upperByte, byte_t *lowerByte, word_t value)
{
    *upperByte = UPPER_BYTE(value);
    *lowerByte = LOWER_BYTE(value);
}
// -----------------------------------------------------------------------------
static int no_func(ZilogZ80_t *cpu)
{
    setError(C80_ERROR_CPU_INVALID_OPCODE);
    return 0;
}

// NOP      -----------------------------------------------------------------------------
static int nop(ZilogZ80_t *cpu)
{
    return 4;
}

// HALT     -----------------------------------------------------------------------------
static int halt(ZilogZ80_t *cpu)
{
    cpu->isHaltered = true;
    return 4;
}


// ADD      -----------------------------------------------------------------------------
static int add_hl_bc_imm(ZilogZ80_t *cpu)
{
    addToRegisterPair(cpu, TO_WORD(cpu->H, cpu->L), TO_WORD(cpu->B, cpu->C));

    return 7;
}
static int add_hl_de_imm(ZilogZ80_t *cpu)
{
    addToRegisterPair(cpu, TO_WORD(cpu->H, cpu->L), TO_WORD(cpu->D, cpu->E));

    return 7;
}
static int add_hl_hl_imm(ZilogZ80_t *cpu)
{
    addToRegisterPair(cpu, TO_WORD(cpu->H, cpu->L), TO_WORD(cpu->H, cpu->L));

    return 7;
}
static int add_hl_sp_imm(ZilogZ80_t *cpu)
{
    addToRegisterPair(cpu, TO_WORD(cpu->H, cpu->L), cpu->SP);

    return 7;
}

static int add_a_n(ZilogZ80_t *cpu)
{
    byte_t val = fetchByteAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
    addToRegister(cpu, &cpu->A, val);
    cpu->PC++;
    return 7;
}
static int add_a_a(ZilogZ80_t *cpu)
{
    addToRegister(cpu, &cpu->A, cpu->A);
    return 4;
}
static int add_a_b(ZilogZ80_t *cpu)
{
    addToRegister(cpu, &cpu->A, cpu->B);
    return 4;
}
static int add_a_c(ZilogZ80_t *cpu)
{
    addToRegister(cpu, &cpu->A, cpu->C);
    return 4;
}
static int add_a_d(ZilogZ80_t *cpu)
{
    addToRegister(cpu, &cpu->A, cpu->D);
    return 4;
}
static int add_a_e(ZilogZ80_t *cpu)
{
    addToRegister(cpu, &cpu->A, cpu->E);
    return 4;
}
static int add_a_h(ZilogZ80_t *cpu)
{
    addToRegister(cpu, &cpu->A, cpu->H);
    return 4;
}
static int add_a_l(ZilogZ80_t *cpu)
{
    addToRegister(cpu, &cpu->A, cpu->L);
    return 4;
}
static int add_a_hl_addr(ZilogZ80_t *cpu)
{
    byte_t val = fetchByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L));
    addToRegister(cpu, &cpu->A, val);
    cpu->PC++;
    return 7;
}


// ADC      -----------------------------------------------------------------------------
static int adc_a_n(ZilogZ80_t *cpu)
{
    byte_t val = fetchByteAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
    addToRegisterWithCarry(cpu, &cpu->A, val);
    cpu->PC++;
    return 7;
}
static int adc_a_a(ZilogZ80_t *cpu)
{
    addToRegisterWithCarry(cpu, &cpu->A, cpu->A);
    return 4;
}
static int adc_a_b(ZilogZ80_t *cpu)
{
    addToRegisterWithCarry(cpu, &cpu->A, cpu->A);
    return 4;
}
static int adc_a_c(ZilogZ80_t *cpu)
{
    addToRegisterWithCarry(cpu, &cpu->A, cpu->A);
    return 4;
}
static int adc_a_d(ZilogZ80_t *cpu)
{
    addToRegisterWithCarry(cpu, &cpu->A, cpu->A);
    return 4;
}
static int adc_a_e(ZilogZ80_t *cpu)
{
    addToRegisterWithCarry(cpu, &cpu->A, cpu->A);
    return 4;
}
static int adc_a_h(ZilogZ80_t *cpu)
{
    addToRegisterWithCarry(cpu, &cpu->A, cpu->A);
    return 4;
}
static int adc_a_l(ZilogZ80_t *cpu)
{
    addToRegisterWithCarry(cpu, &cpu->A, cpu->A);
    return 4;
}
static int adc_a_hl_addr(ZilogZ80_t *cpu)
{
    byte_t val = fetchByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L));
    addToRegisterWithCarry(cpu, &cpu->A, val);
    cpu->PC++;
    return 7;
}


static int adc_hl_bc(ZilogZ80_t *cpu)
{
    addToRegisterPairWithCarry(cpu, TO_WORD(cpu->H, cpu->L), TO_WORD(cpu->B, cpu->C));
    return 11;
}
static int adc_hl_de(ZilogZ80_t *cpu)
{
    addToRegisterPairWithCarry(cpu, TO_WORD(cpu->H, cpu->L), TO_WORD(cpu->D, cpu->E));
    return 11;
}
static int adc_hl_hl(ZilogZ80_t *cpu)
{
    addToRegisterPairWithCarry(cpu, TO_WORD(cpu->H, cpu->L), TO_WORD(cpu->H, cpu->L));
    return 11;
}
static int adc_hl_sp(ZilogZ80_t *cpu)
{
    addToRegisterPairWithCarry(cpu, TO_WORD(cpu->H, cpu->L), cpu->SP);
    return 11;
}

// INC      -----------------------------------------------------------------------------
static int inc_bc(ZilogZ80_t *cpu)
{
    incrementRegisterPair(cpu, &cpu->B, &cpu->C);
    return 6;
}
static int inc_de(ZilogZ80_t *cpu)
{
    incrementRegisterPair(cpu, &cpu->D, &cpu->E);
    return 6;
}
static int inc_hl(ZilogZ80_t *cpu)
{
    incrementRegisterPair(cpu, &cpu->H, &cpu->L);
    return 6;
}
static int inc_sp(ZilogZ80_t *cpu)
{
    byte_t upperByte = UPPER_BYTE(cpu->SP);
    byte_t lowerByte = LOWER_BYTE(cpu->SP);

    incrementRegisterPair(cpu, &upperByte, &lowerByte);

    cpu->SP = TO_WORD(upperByte, lowerByte);
    return 6;
}

static int inc_a(ZilogZ80_t *cpu)
{
    incrementRegister(cpu, &cpu->A);
    return 4;
}
static int inc_b(ZilogZ80_t *cpu)
{
    incrementRegister(cpu, &cpu->B);
    return 4;
}
static int inc_c(ZilogZ80_t *cpu)
{
    incrementRegister(cpu, &cpu->C);
    return 4;
}
static int inc_d(ZilogZ80_t *cpu)
{
    incrementRegister(cpu, &cpu->D);
    return 4;
}
static int inc_e(ZilogZ80_t *cpu)
{
    incrementRegister(cpu, &cpu->E);
    return 4;
}
static int inc_h(ZilogZ80_t *cpu)
{
    incrementRegister(cpu, &cpu->H);
    return 4;
}
static int inc_l(ZilogZ80_t *cpu)
{
    incrementRegister(cpu, &cpu->L);
    return 4;
}
static int inc_hl_addr(ZilogZ80_t *cpu)
{
    byte_t val = fetchByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L));
    cpu->PC++;
    incrementRegister(cpu, &val);
    storeByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L), val);
    cpu->PC++;
    return 11;
}

// SUB      -----------------------------------------------------------------------------
static int sub_n(ZilogZ80_t *cpu)
{
    byte_t val = fetchByteAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
    subtractFromRegister(cpu, val);
    cpu->PC++;
    return 7;
}
static int sub_a(ZilogZ80_t *cpu)
{
    subtractFromRegister(cpu, cpu->A);
    return 4;
}
static int sub_b(ZilogZ80_t *cpu)
{
    subtractFromRegister(cpu, cpu->B);
    return 4;
}
static int sub_c(ZilogZ80_t *cpu)
{
    subtractFromRegister(cpu, cpu->C);
    return 4;
}
static int sub_d(ZilogZ80_t *cpu)
{
    subtractFromRegister(cpu, cpu->D);
    return 4;
}
static int sub_e(ZilogZ80_t *cpu)
{
    subtractFromRegister(cpu, cpu->E);
    return 4;
}
static int sub_h(ZilogZ80_t *cpu)
{
    subtractFromRegister(cpu, cpu->H);
    return 4;
}
static int sub_l(ZilogZ80_t *cpu)
{
    subtractFromRegister(cpu, cpu->L);
    return 4;
}
static int sub_hl_addr(ZilogZ80_t *cpu)
{
    byte_t val = fetchByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L));
    subtractFromRegister(cpu, val);
    cpu->PC++;
    return 7;
}

// SBC      -----------------------------------------------------------------------------
static int sbc_a_n(ZilogZ80_t *cpu)
{
    byte_t val = fetchByteAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
    subtractFromRegisterWithCarry(cpu, val);
    cpu->PC++;
    return 7;
}
static int sbc_a_a(ZilogZ80_t *cpu)
{
    subtractFromRegisterWithCarry(cpu, cpu->A);
    return 4;
}
static int sbc_b(ZilogZ80_t *cpu)
{
    subtractFromRegisterWithCarry(cpu, cpu->B);
    return 4;
}
static int sbc_c(ZilogZ80_t *cpu)
{
    subtractFromRegisterWithCarry(cpu, cpu->C);
    return 4;
}
static int sbc_d(ZilogZ80_t *cpu)
{
    subtractFromRegisterWithCarry(cpu, cpu->D);
    return 4;
}
static int sbc_e(ZilogZ80_t *cpu)
{
    subtractFromRegisterWithCarry(cpu, cpu->E);
    return 4;
}
static int sbc_h(ZilogZ80_t *cpu)
{
    subtractFromRegisterWithCarry(cpu, cpu->H);
    return 4;
}
static int sbc_l(ZilogZ80_t *cpu)
{
    subtractFromRegisterWithCarry(cpu, cpu->L);
    return 4;
}
static int sbc_hl_addr(ZilogZ80_t *cpu)
{
    byte_t val = fetchByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L));
    subtractFromRegisterWithCarry(cpu, val);
    cpu->PC++;
    return 7;
}

static int sbc_hl_bc(ZilogZ80_t *cpu)
{
    subtractFromRegisterPairWithCarry(cpu, TO_WORD(cpu->H, cpu->L), TO_WORD(cpu->B, cpu->C));
    return 11;
}
static int sbc_hl_de(ZilogZ80_t *cpu)
{
    subtractFromRegisterPairWithCarry(cpu, TO_WORD(cpu->H, cpu->L), TO_WORD(cpu->D, cpu->E));
    return 11;
}
static int sbc_hl_hl(ZilogZ80_t *cpu)
{
    subtractFromRegisterPairWithCarry(cpu, TO_WORD(cpu->H, cpu->L), TO_WORD(cpu->H, cpu->L));
    return 11;
}
static int sbc_hl_sp(ZilogZ80_t *cpu)
{
    subtractFromRegisterPairWithCarry(cpu, TO_WORD(cpu->H, cpu->L), cpu->SP);
    return 11;
}


// SBC      -----------------------------------------------------------------------------
static int dec_bc(ZilogZ80_t *cpu)
{
    decrementRegisterPair(cpu, &cpu->B, &cpu->C);
    return 6;
}
static int dec_de(ZilogZ80_t *cpu)
{
    decrementRegisterPair(cpu, &cpu->D, &cpu->E);
    return 6;
}
static int dec_hl(ZilogZ80_t *cpu)
{
    decrementRegisterPair(cpu, &cpu->H, &cpu->L);
    return 6;
}
static int dec_sp(ZilogZ80_t *cpu)
{
    byte_t s = UPPER_BYTE(cpu->SP);
    byte_t p = LOWER_BYTE(cpu->SP);
    decrementRegisterPair(cpu, &s, &p);
    cpu->SP = TO_WORD(s, p);
    return 6;
}

static int dec_a(ZilogZ80_t *cpu)
{
    decrementRegister(cpu, &cpu->A);
    return 4;
}
static int dec_b(ZilogZ80_t *cpu)
{
    decrementRegister(cpu, &cpu->B);
    return 4;
}
static int dec_c(ZilogZ80_t *cpu)
{
    decrementRegister(cpu, &cpu->C);
    return 4;
}
static int dec_d(ZilogZ80_t *cpu)
{
    decrementRegister(cpu, &cpu->D);
    return 4;
}
static int dec_e(ZilogZ80_t *cpu)
{
    decrementRegister(cpu, &cpu->E);
    return 4;
}
static int dec_h(ZilogZ80_t *cpu)
{
    decrementRegister(cpu, &cpu->H);
    return 4;
}
static int dec_l(ZilogZ80_t *cpu)
{
    decrementRegister(cpu, &cpu->L);
    return 4;
}
static int dec_hl_addr(ZilogZ80_t *cpu)
{
    byte_t val = fetchByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L));
    cpu->PC++;
    decrementRegister(cpu, &val);
    storeByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L), val);
    cpu->PC++;
    return 11;
}

// AND      -----------------------------------------------------------------------------
static int and_n(ZilogZ80_t *cpu)
{
    byte_t val = fetchByteAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
    andWithRegister(cpu, val);
    cpu->PC++;
    return 7;
}
static int and_a(ZilogZ80_t *cpu)
{
    andWithRegister(cpu, cpu->A);
    return 4;
}
static int and_b(ZilogZ80_t *cpu)
{
    andWithRegister(cpu, cpu->B);
    return 4;
}
static int and_c(ZilogZ80_t *cpu)
{
    andWithRegister(cpu, cpu->C);
    return 4;
}
static int and_d(ZilogZ80_t *cpu)
{
    andWithRegister(cpu, cpu->D);
    return 4;
}
static int and_e(ZilogZ80_t *cpu)
{
    andWithRegister(cpu, cpu->E);
    return 4;
}
static int and_h(ZilogZ80_t *cpu)
{
    andWithRegister(cpu, cpu->H);
    return 4;
}
static int and_l(ZilogZ80_t *cpu)
{
    andWithRegister(cpu, cpu->L);
    return 4;
}
static int and_hl_addr(ZilogZ80_t *cpu)
{
    byte_t val = fetchByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L));
    andWithRegister(cpu, val);
    cpu->PC++;
    return 7;
}

// OR       -----------------------------------------------------------------------------
static int or_n(ZilogZ80_t *cpu)
{
    byte_t val = fetchByteAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
    orWithRegister(cpu, val);
    cpu->PC++;
    return 7;
}
static int or_a(ZilogZ80_t *cpu)
{
    orWithRegister(cpu, cpu->A);
    return 4;
}
static int or_b(ZilogZ80_t *cpu)
{
    orWithRegister(cpu, cpu->B);
    return 4;
}
static int or_c(ZilogZ80_t *cpu)
{
    orWithRegister(cpu, cpu->C);
    return 4;
}
static int or_d(ZilogZ80_t *cpu)
{
    orWithRegister(cpu, cpu->D);
    return 4;
}
static int or_e(ZilogZ80_t *cpu)
{
    orWithRegister(cpu, cpu->E);
    return 4;
}
static int or_h(ZilogZ80_t *cpu)
{
    orWithRegister(cpu, cpu->H);
    return 4;
}
static int or_l(ZilogZ80_t *cpu)
{
    orWithRegister(cpu, cpu->L);
    return 4;
}
static int or_hl_addr(ZilogZ80_t *cpu)
{
    byte_t val = fetchByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L));
    andWithRegister(cpu, val);
    cpu->PC++;
    return 7;
}

// XOR      -----------------------------------------------------------------------------
static int xor_n(ZilogZ80_t *cpu)
{
    byte_t val = fetchByteAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
    xorWithRegister(cpu, val);
    cpu->PC++;
    return 7;
}
static int xor_a(ZilogZ80_t *cpu)
{
    xorWithRegister(cpu, cpu->A);
    return 4;
}
static int xor_b(ZilogZ80_t *cpu)
{
    xorWithRegister(cpu, cpu->B);
    return 4;
}
static int xor_c(ZilogZ80_t *cpu)
{
    xorWithRegister(cpu, cpu->C);
    return 4;
}
static int xor_d(ZilogZ80_t *cpu)
{
    xorWithRegister(cpu, cpu->D);
    return 4;
}
static int xor_e(ZilogZ80_t *cpu)
{
    xorWithRegister(cpu, cpu->E);
    return 4;
}
static int xor_h(ZilogZ80_t *cpu)
{
    xorWithRegister(cpu, cpu->H);
    return 4;
}
static int xor_l(ZilogZ80_t *cpu)
{
    xorWithRegister(cpu, cpu->L);
    return 4;
}
static int xor_hl_addr(ZilogZ80_t *cpu)
{
    byte_t val = fetchByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L));
    xorWithRegister(cpu, val);
    cpu->PC++;
    return 7;
}

// CP       -----------------------------------------------------------------------------
static int cp_n(ZilogZ80_t *cpu)
{
    byte_t val = fetchByteAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
    cpWithRegister(cpu, val);
    cpu->PC++;
    return 7;
}
static int cp_a(ZilogZ80_t *cpu)
{
    cpWithRegister(cpu, cpu->A);
    return 4;
}
static int cp_b(ZilogZ80_t *cpu)
{
    cpWithRegister(cpu, cpu->A);
    return 4;
}
static int cp_c(ZilogZ80_t *cpu)
{
    cpWithRegister(cpu, cpu->A);
    return 4;
}
static int cp_d(ZilogZ80_t *cpu)
{
    cpWithRegister(cpu, cpu->A);
    return 4;
}
static int cp_e(ZilogZ80_t *cpu)
{
    cpWithRegister(cpu, cpu->A);
    return 4;
}
static int cp_h(ZilogZ80_t *cpu)
{
    cpWithRegister(cpu, cpu->A);
    return 4;
}
static int cp_l(ZilogZ80_t *cpu)
{
    cpWithRegister(cpu, cpu->A);
    return 4;
}
static int cp_hl_addr(ZilogZ80_t *cpu)
{
    byte_t val = fetchByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L));
    cpWithRegister(cpu, val);
    cpu->PC++;
    return 7;
}
static int cpi(ZilogZ80_t *cpu)
{
    byte_t value = fetchByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L));

    andWithRegister(cpu, value);

    incrementRegisterPair(cpu, &cpu->H, &cpu->L);

    decrementRegisterPair(cpu, &cpu->B, &cpu->C);

    cpu->F.P = calculateParity(TO_WORD(cpu->B, cpu->C));

    return 16;
}
static int cpir(ZilogZ80_t *cpu)
{
    // TODO
    return 16;
}
static int cpd(ZilogZ80_t *cpu)
{
    byte_t value = fetchByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L));

    andWithRegister(cpu, value);

    decrementRegisterPair(cpu, &cpu->H, &cpu->L);

    decrementRegisterPair(cpu, &cpu->B, &cpu->C);

    cpu->F.P = calculateParity(TO_WORD(cpu->B, cpu->C));

    return 16;
}
static int cpdr(ZilogZ80_t *cpu)
{
    // TODO
}


// PUSH     -----------------------------------------------------------------------------
static int push_bc(ZilogZ80_t *cpu)
{
    pushWord(cpu, TO_WORD(cpu->B, cpu->C));
    return 11;
}
static int push_de(ZilogZ80_t *cpu)
{
    pushWord(cpu, TO_WORD(cpu->D, cpu->E));
    return 11;
}
static int push_hl(ZilogZ80_t *cpu)
{
    pushWord(cpu, TO_WORD(cpu->H, cpu->L));
    return 11;
}
static int push_af(ZilogZ80_t *cpu)
{
    pushWord(cpu, TO_WORD(cpu->A, flagsToByte(cpu->F)));
    return 11;
}

// POP      -----------------------------------------------------------------------------
static int pop_bc(ZilogZ80_t *cpu)
{
    popWord(cpu, &cpu->B, &cpu->C);
    return 10;
}
static int pop_de(ZilogZ80_t *cpu)
{
    popWord(cpu, &cpu->D, &cpu->E);
    return 10;
}
static int pop_hl(ZilogZ80_t *cpu)
{
    popWord(cpu, &cpu->H, &cpu->L);
    return 10;
}
static int pop_af(ZilogZ80_t *cpu)
{
    byte_t f = flagsToByte(cpu->F);
    popWord(cpu, &cpu->A, &f);
    byteToFlags(&cpu->F, f);
    return 10;
}

// CALL     -----------------------------------------------------------------------------
static int call_nz_nn(ZilogZ80_t *cpu)
{
    bool condition = cpu->F.N == 0;
    int cycles = callHelper(cpu, condition);

    return cycles;
}
static int call_z_nn(ZilogZ80_t *cpu)
{
    bool condition = cpu->F.Z == 1;
    int cycles = callHelper(cpu, condition);

    return cycles;
}
static int call_nn(ZilogZ80_t *cpu)
{
    int cycles = callHelper(cpu, true);

    return cycles;
}
static int call_nc_nn(ZilogZ80_t *cpu)
{
    bool condition = cpu->F.C == 0;
    int cycles = callHelper(cpu, condition);

    return cycles;
}
static int call_c_nn(ZilogZ80_t *cpu)
{
    bool condition = cpu->F.C == 1;
    int cycles = callHelper(cpu, condition);

    return cycles;
}
static int call_po_nn(ZilogZ80_t *cpu)
{
    bool condition = cpu->F.P == 0;
    int cycles = callHelper(cpu, condition);

    return cycles;
}
static int call_pe_nn(ZilogZ80_t *cpu)
{
    bool condition = cpu->F.P == 1;
    int cycles = callHelper(cpu, condition);

    return cycles;
}
static int call_p_nn(ZilogZ80_t *cpu)
{
    bool condition = cpu->F.S == 0;
    int cycles = callHelper(cpu, condition);

    return cycles;
}
static int call_m_nn(ZilogZ80_t *cpu)
{
    bool condition = cpu->F.S == 0;
    int cycles = callHelper(cpu, condition);

    return cycles;
}

// RET      -----------------------------------------------------------------------------
static int ret_nz(ZilogZ80_t *cpu)
{
    bool condition = cpu->F.Z == 0;
    int cycles = returnHelper(cpu, condition);

    return cycles;
}
static int ret_z(ZilogZ80_t *cpu)
{
    bool condition = cpu->F.Z == 1;
    int cycles = returnHelper(cpu, condition);

    return cycles;
}
static int ret_nc(ZilogZ80_t *cpu)
{
    bool condition = cpu->F.C == 0;
    int cycles = returnHelper(cpu, condition);

    return cycles;
}
static int ret_c(ZilogZ80_t *cpu)
{
    bool condition = cpu->F.C == 1;
    int cycles = returnHelper(cpu, condition);

    return cycles;
}
static int ret_po(ZilogZ80_t *cpu)
{
    bool condition = cpu->F.P == 0;
    int cycles = returnHelper(cpu, condition);

    return cycles;
}
static int ret_pe(ZilogZ80_t *cpu)
{
    bool condition = cpu->F.P == 1;
    int cycles = returnHelper(cpu, condition);

    return cycles;
}
static int ret_p(ZilogZ80_t *cpu)
{
    bool condition = cpu->F.S == 0;
    int cycles = returnHelper(cpu, condition);

    return cycles;
}
static int ret_m(ZilogZ80_t *cpu)
{
    bool condition = cpu->F.S == 1;
    int cycles = returnHelper(cpu, condition);

    return cycles;
}
static int ret(ZilogZ80_t *cpu)
{
    byte_t lowerByte, upperByte;
    popWord(cpu, &upperByte, &lowerByte);
    cpu->PC = TO_WORD(upperByte, lowerByte);

    return 10;
}
static int retn(ZilogZ80_t *cpu)
{
    // TODO
}
static int reti(ZilogZ80_t *cpu)
{
    // TODO
}

// ROTATE   -----------------------------------------------------------------------------
static int rlca(ZilogZ80_t *cpu)
{
    byte_t bit7 = (cpu->A & 0b10000000) >> 7;
    cpu->A = (cpu->A << 1) | bit7;
    cpu->F.C = bit7;
    cpu->F.H = 0;
    cpu->F.N = 0;
    return 4;
}
static int rrca(ZilogZ80_t *cpu)
{
    byte_t bit0 = (cpu->A & 0b00000001);
    cpu->A = (bit0 << 7) | (cpu->A >> 1); 
    cpu->F.C = bit0;
    cpu->F.H = 0;
    cpu->F.N = 0;
    return 4;
}
static int rla(ZilogZ80_t *cpu)
{
    byte_t bit7 = (cpu->A & 0b10000000) >> 7;
    cpu->A = (cpu->A << 1) | cpu->F.C;
    cpu->F.C = bit7;
    cpu->F.H = 0;
    cpu->F.N = 0;
    return 4;
}
static int rra(ZilogZ80_t *cpu)
{
    byte_t bit0 = (cpu->A & 0b00000001);
    cpu->A = (cpu->F.C << 7) | (cpu->A >> 1);
    cpu->F.C = bit0;
    cpu->F.H = 0;
    cpu->F.N = 0;
    return 4;
}

// RST      -----------------------------------------------------------------------------
static int rst_00h(ZilogZ80_t *cpu)
{
    pushWord(cpu, cpu->PC);
    cpu->PC = 0x00;
    return 11;
}
static int rst_08h(ZilogZ80_t *cpu)
{
    pushWord(cpu, cpu->PC);
    cpu->PC = 0x08;
    return 11;
}
static int rst_10h(ZilogZ80_t *cpu)
{
    pushWord(cpu, cpu->PC);
    cpu->PC = 0x10;
    return 11;
}
static int rst_18h(ZilogZ80_t *cpu)
{
    pushWord(cpu, cpu->PC);
    cpu->PC = 0x18;
    return 11;
}
static int rst_20h(ZilogZ80_t *cpu)
{
    pushWord(cpu, cpu->PC);
    cpu->PC = 0x20;
    return 11;
}
static int rst_28h(ZilogZ80_t *cpu)
{
    pushWord(cpu, cpu->PC);
    cpu->PC = 0x28;
    return 11;
}
static int rst_30h(ZilogZ80_t *cpu)
{
    pushWord(cpu, cpu->PC);
    cpu->PC = 0x30;
    return 11;
}
static int rst_38h(ZilogZ80_t *cpu)
{
    pushWord(cpu, cpu->PC);
    cpu->PC = 0x38;
    return 11;
}

// JUMP     -----------------------------------------------------------------------------
static int djnz_d(ZilogZ80_t *cpu)
{
    int cycles = 8;
    cpu->B--;
    bool condition = cpu->B != 0;

    jumpRelativeHelper(cpu, condition);

    if(condition)
    {
        cycles = 13;
    }

    return cycles;
}
static int jr_d(ZilogZ80_t *cpu)
{
    int cycles = jumpRelativeHelper(cpu, true);
    return cycles;
}
static int jr_nz_d(ZilogZ80_t *cpu)
{
    bool condition = cpu->F.Z == 0;
    int cycles = jumpRelativeHelper(cpu, condition);

    return cycles;
}
static int jr_z_b(ZilogZ80_t *cpu)
{
    bool condition = cpu->F.Z == 1;
    int cycles = jumpRelativeHelper(cpu, condition);

    return cycles;
}
static int jr_nc_d(ZilogZ80_t *cpu)
{
    bool condition = cpu->F.C == 0;
    int cycles = jumpRelativeHelper(cpu, condition);

    return cycles;
}
static int jr_c_b(ZilogZ80_t *cpu)
{
    bool condition = cpu->F.C == 1;
    int cycles = jumpRelativeHelper(cpu, condition);

    return cycles;
}
static int jp_nz_nn(ZilogZ80_t *cpu)
{
    bool condition = cpu->F.Z == 0;
    int cycles = jumpHelper(cpu, condition);

    return cycles;
}
static int jp_nn(ZilogZ80_t *cpu)
{
    int cycles = jumpHelper(cpu, true);

    return cycles;
}
static int jp_z_nn(ZilogZ80_t *cpu)
{
    bool condition = cpu->F.Z == 1;
    int cycles = jumpHelper(cpu, condition);

    return cycles;
}
static int jp_nc_nn(ZilogZ80_t *cpu)
{
    bool condition = cpu->F.C == 0;
    int cycles = jumpHelper(cpu, condition);

    return cycles;
}
static int jp_c_nn(ZilogZ80_t *cpu)
{
    bool condition = cpu->F.C == 1;
    int cycles = jumpHelper(cpu, condition);

    return cycles;
}
static int jp_po_nn(ZilogZ80_t *cpu)
{
    bool condition = cpu->F.P == 0;
    int cycles = jumpHelper(cpu, condition);

    return cycles;
}
static int jp_pe_nn(ZilogZ80_t *cpu)
{
    bool condition = cpu->F.P == 1;
    int cycles = jumpHelper(cpu, condition);

    return cycles;
}
static int jp_hl_addr(ZilogZ80_t *cpu)
{
    cpu->PC = TO_WORD(cpu->H, cpu->L);
    return 4;
}
static int jp_p_nn(ZilogZ80_t *cpu)
{
    bool condition = cpu->F.S == 0;
    int cycles = jumpHelper(cpu, condition);

    return cycles;
}
static int jp_m_nn(ZilogZ80_t *cpu)
{
    bool condition = cpu->F.S == 1;
    int cycles = jumpHelper(cpu, condition);

    return cycles;
}

// EXCHANGE -----------------------------------------------------------------------------
static int exx(ZilogZ80_t *cpu)
{
    byte_t temp;

    temp = cpu->B;
    cpu->B = cpu->B_;
    cpu->B_ = temp;

    temp = cpu->C;
    cpu->C = cpu->C_;
    cpu->C_ = temp;

    temp = cpu->D;
    cpu->D = cpu->D_;
    cpu->D_ = temp;

    temp = cpu->E;
    cpu->E = cpu->E_;
    cpu->E_ = temp;

    temp = cpu->H;
    cpu->H = cpu->H_;
    cpu->H_ = temp;

    return 4;
}
static int ex_af_af_(ZilogZ80_t *cpu)
{
    byte_t temp;

    temp = cpu->A;
    cpu->A = cpu->A_;
    cpu->A_ = temp;

    temp = flagsToByte(cpu->F);
    byte_t temp2 = flagsToByte(cpu->F_);

    byteToFlags(&cpu->F, temp2);
    byteToFlags(&cpu->F_, temp);

    return 4;
}
static int ex_sp_hl_addr(ZilogZ80_t *cpu)
{
    word_t address = cpu->SP;

    byte_t lowerByte = fetchByteAddressSpace(&cpu->ram, &cpu->rom, address);
    byte_t upperByte = fetchByteAddressSpace(&cpu->ram, &cpu->rom, address + 1);

    storeByteAddressSpace(&cpu->ram, &cpu->rom, address, cpu->L);
    storeByteAddressSpace(&cpu->ram, &cpu->rom, address + 1, cpu->H);

    cpu->H = upperByte;
    cpu->L = lowerByte;

    return 19;
}
static int ex_de_hl(ZilogZ80_t *cpu)
{
    byte_t temp = cpu->D;
    cpu->D = cpu->H;
    cpu->H = temp;

    temp = cpu->E;
    cpu->E = cpu->L;
    cpu->L = temp;

    return 4;
}

// LD       -----------------------------------------------------------------------------
static int ld_a_n(ZilogZ80_t *cpu)
{
    cpu->A = fetchByteAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
    cpu->PC++;
    return 7;
}
static int ld_a_a(ZilogZ80_t *cpu)
{
    cpu->A = cpu->A;
    return 4;
}
static int ld_a_b(ZilogZ80_t *cpu)
{
    cpu->A = cpu->B;
    return 4;
}
static int ld_a_c(ZilogZ80_t *cpu)
{
    cpu->A = cpu->C;
    return 4;
}
static int ld_a_d(ZilogZ80_t *cpu)
{
    cpu->A = cpu->D;
    return 4;
}
static int ld_a_e(ZilogZ80_t *cpu)
{
    cpu->A = cpu->E;
    return 4;
}
static int ld_a_h(ZilogZ80_t *cpu)
{
    cpu->A = cpu->H;
    return 4;
}
static int ld_a_l(ZilogZ80_t *cpu)
{
    cpu->A = cpu->L;
    return 4;
}
static int ld_a_hl_addr(ZilogZ80_t *cpu)
{
    byte_t val = fetchByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L));
    cpu->A = val;
    return 7;
}

static int ld_b_n(ZilogZ80_t *cpu)
{
    cpu->B = fetchByteAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
    cpu->PC++;
    return 7;
}
static int ld_b_a(ZilogZ80_t *cpu)
{
    cpu->B = cpu->A;
    return 4;
}
static int ld_b_b(ZilogZ80_t *cpu)
{
    cpu->B = cpu->B;
    return 4;
}
static int ld_b_c(ZilogZ80_t *cpu)
{
    cpu->B = cpu->C;
    return 4;
}
static int ld_b_d(ZilogZ80_t *cpu)
{
    cpu->B = cpu->D;
    return 4;
}
static int ld_b_e(ZilogZ80_t *cpu)
{
    cpu->B = cpu->E;
    return 4;
}
static int ld_b_h(ZilogZ80_t *cpu)
{
    cpu->B = cpu->H;
    return 4;
}
static int ld_b_l(ZilogZ80_t *cpu)
{
    cpu->B = cpu->L;
    return 4;
}
static int ld_b_hl_addr(ZilogZ80_t *cpu)
{
    byte_t val = fetchByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L));
    cpu->B = val;
    cpu->PC++;
    return 7;
}

static int ld_c_n(ZilogZ80_t *cpu)
{
    cpu->C = fetchByteAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
    cpu->PC++;
    return 7;
}
static int ld_c_a(ZilogZ80_t *cpu)
{
    cpu->C = cpu->A;
    return 4;
}
static int ld_c_b(ZilogZ80_t *cpu)
{
    cpu->C = cpu->B;
    return 4;
}
static int ld_c_c(ZilogZ80_t *cpu)
{
    cpu->C = cpu->C;
    return 4;
}
static int ld_c_d(ZilogZ80_t *cpu)
{
    cpu->C = cpu->D;
    return 4;
}
static int ld_c_e(ZilogZ80_t *cpu)
{
    cpu->C = cpu->E;
    return 4;
}
static int ld_c_h(ZilogZ80_t *cpu)
{
    cpu->C = cpu->H;
    return 4;
}
static int ld_c_l(ZilogZ80_t *cpu)
{
    cpu->C = cpu->L;
    return 4;
}
static int ld_c_hl_addr(ZilogZ80_t *cpu)
{
    byte_t val = fetchByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L));
    cpu->C = val;
    cpu->PC++;
    return 7;
}

static int ld_d_n(ZilogZ80_t *cpu)
{
    cpu->D = fetchByteAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
    cpu->PC++;
    return 7;
}
static int ld_d_a(ZilogZ80_t *cpu)
{
    cpu->D = cpu->A;
    return 4;
}
static int ld_d_b(ZilogZ80_t *cpu)
{
    cpu->D = cpu->B;
    return 4;
}
static int ld_d_c(ZilogZ80_t *cpu)
{
    cpu->D = cpu->C;
    return 4;
}
static int ld_d_d(ZilogZ80_t *cpu)
{
    cpu->D = cpu->D;
    return 4;
}
static int ld_d_e(ZilogZ80_t *cpu)
{
    cpu->D = cpu->E;
    return 4;
}
static int ld_d_h(ZilogZ80_t *cpu)
{
    cpu->D = cpu->H;
    return 4;
}
static int ld_d_l(ZilogZ80_t *cpu)
{
    cpu->D = cpu->L;
    return 4;
}
static int ld_d_hl_addr(ZilogZ80_t *cpu)
{
    byte_t val = fetchByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L));
    cpu->D = val;
    cpu->PC++;
    return 7;
}

static int ld_e_n(ZilogZ80_t *cpu)
{
    cpu->E = fetchByteAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
    cpu->PC++;
    return 7;
}
static int ld_e_a(ZilogZ80_t *cpu)
{
    cpu->E = cpu->A;
    return 4;
}
static int ld_e_b(ZilogZ80_t *cpu)
{
    cpu->E = cpu->B;
    return 4;
}
static int ld_e_c(ZilogZ80_t *cpu)
{
    cpu->E = cpu->C;
    return 4;
}
static int ld_e_d(ZilogZ80_t *cpu)
{
    cpu->E = cpu->D;
    return 4;
}
static int ld_e_e(ZilogZ80_t *cpu)
{
    cpu->E = cpu->E;
    return 4;
}
static int ld_e_h(ZilogZ80_t *cpu)
{
    cpu->E = cpu->H;
    return 4;
}
static int ld_e_l(ZilogZ80_t *cpu)
{
    cpu->E = cpu->L;
    return 4;
}
static int ld_e_hl_addr(ZilogZ80_t *cpu)
{
    byte_t val = fetchByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L));
    cpu->E = val;
    cpu->PC++;
    return 7;
}

static int ld_h_n(ZilogZ80_t *cpu)
{
    cpu->H = fetchByteAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
    cpu->PC++;
    return 7;
}
static int ld_h_a(ZilogZ80_t *cpu)
{
    cpu->H = cpu->A;
    return 4;
}
static int ld_h_b(ZilogZ80_t *cpu)
{
    cpu->H = cpu->B;
    return 4;
}
static int ld_h_c(ZilogZ80_t *cpu)
{
    cpu->H = cpu->C;
    return 4;
}
static int ld_h_d(ZilogZ80_t *cpu)
{
    cpu->H = cpu->D;
    return 4;
}
static int ld_h_e(ZilogZ80_t *cpu)
{
    cpu->H = cpu->E;
    return 4;
}
static int ld_h_h(ZilogZ80_t *cpu)
{
    cpu->H = cpu->H;
    return 4;
}
static int ld_h_l(ZilogZ80_t *cpu)
{
    cpu->H = cpu->L;
    return 4;
}
static int ld_h_hl_addr(ZilogZ80_t *cpu)
{
    byte_t val = fetchByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L));
    cpu->H = val;
    cpu->PC++;
    return 7;
}

static int ld_l_n(ZilogZ80_t *cpu)
{
    cpu->L = fetchByteAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
    cpu->PC++;
    return 7;
}
static int ld_l_a(ZilogZ80_t *cpu)
{
    cpu->L = cpu->A;
    return 4;
}
static int ld_l_b(ZilogZ80_t *cpu)
{
    cpu->L = cpu->B;
    return 4;
}
static int ld_l_c(ZilogZ80_t *cpu)
{
    cpu->L = cpu->C;
    return 4;
}
static int ld_l_d(ZilogZ80_t *cpu)
{
    cpu->L = cpu->D;
    return 4;
}
static int ld_l_e(ZilogZ80_t *cpu)
{
    cpu->L = cpu->E;
    return 4;
}
static int ld_l_h(ZilogZ80_t *cpu)
{
    cpu->L = cpu->H;
    return 4;
}
static int ld_l_l(ZilogZ80_t *cpu)
{
    cpu->L = cpu->L;
    return 4;
}
static int ld_l_hl_addr(ZilogZ80_t *cpu)
{
    byte_t val = fetchByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L));
    cpu->L = val;
    cpu->PC++;
    return 7;
}

static int ld_hl_n_addr(ZilogZ80_t *cpu)
{
    byte_t val = fetchByteAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
    storeByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L), val);
    cpu->PC++;
    return 10;
}
static int ld_hl_a_addr(ZilogZ80_t *cpu)
{
    storeByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L), cpu->A);
    return 7;
}
static int ld_hl_b_addr(ZilogZ80_t *cpu)
{
    storeByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L), cpu->B);
    return 7;
}
static int ld_hl_c_addr(ZilogZ80_t *cpu)
{
    storeByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L), cpu->C);
    return 7;
}
static int ld_hl_d_addr(ZilogZ80_t *cpu)
{
    storeByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L), cpu->D);
    return 7;
}
static int ld_hl_e_addr(ZilogZ80_t *cpu)
{
    storeByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L), cpu->E);
    return 7;
}
static int ld_hl_h_addr(ZilogZ80_t *cpu)
{
    storeByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L), cpu->H);
    return 7;
}
static int ld_hl_l_addr(ZilogZ80_t *cpu)
{
    storeByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L), cpu->L);
    return 7;
}
static int ld_hl_hl_addr(ZilogZ80_t *cpu)
{
    byte_t val = fetchByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L));
    storeByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L), val);
    return 7;
}

static int ld_hl_nn_addr(ZilogZ80_t *cpu)
{
    word_t address = fetchWordAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
    
    byte_t lowerByte = fetchByteAddressSpace(&cpu->ram, &cpu->rom, address);
    byte_t upperByte = fetchByteAddressSpace(&cpu->ram, &cpu->rom, address + 1);

    cpu->H = upperByte;
    cpu->L = lowerByte;

    cpu->PC += 2;

    return 16;
}
static int ld_a_nn_addr(ZilogZ80_t *cpu)
{
    word_t address = fetchWordAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
    cpu->A = fetchByteAddressSpace(&cpu->ram, &cpu->rom, address);
    cpu->PC += 2;
    return 13;
}

static int ld_nn_hl_addr(ZilogZ80_t *cpu)
{
    word_t address = fetchWordAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
    storeWordAddressSpace(&cpu->ram, &cpu->rom, address, TO_WORD(cpu->H, cpu->L));

    cpu->PC += 2;

    return 16;
}
static int ld_nn_a_addr(ZilogZ80_t *cpu)
{
    word_t address = fetchWordAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
    storeByteAddressSpace(&cpu->ram, &cpu->rom, address, cpu->A);
    cpu->PC += 2;
    return 13;
}

static int ld_de_nn_imm(ZilogZ80_t *cpu)
{
    word_t address = fetchWordAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
    storeByteAddressSpace(&cpu->ram, &cpu->rom, address, cpu->E);
    storeByteAddressSpace(&cpu->ram, &cpu->rom, address + 1, cpu->D);
    cpu->PC += 2;
    return 16;
}
static int ld_de_a_addr(ZilogZ80_t *cpu)
{
    storeByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->D, cpu->E), cpu->A);
    return 7;
}
static int ld_a_de_addr(ZilogZ80_t *cpu)
{
    cpu->A = fetchByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->D, cpu->E));
    return 7;
}

static int ld_bc_nn_imm(ZilogZ80_t *cpu)
{
    word_t immediate = fetchWordAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
    cpu->B = UPPER_BYTE(immediate);
    cpu->C = LOWER_BYTE(immediate);
    cpu->PC += 2;

    return 10;

}
static int ld_bc_a_addr(ZilogZ80_t *cpu)
{
    storeByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->B, cpu->C), cpu->A);
    return 7;
}
static int ld_a_bc_addr(ZilogZ80_t *cpu)
{
    cpu->A = fetchByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->B, cpu->C));
    return 7;
}

static int ld_hl_nn_imm(ZilogZ80_t *cpu)
{
    word_t immediate = fetchWordAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
    cpu->H = UPPER_BYTE(immediate);
    cpu->L = LOWER_BYTE(immediate);
    cpu->PC += 2;

    return 10;
}

static int ld_sp_nn_imm(ZilogZ80_t *cpu)
{
    word_t immediate = fetchWordAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
    cpu->SP = immediate;
    cpu->PC += 2;

    return 10;
}

static int ld_sp_hl(ZilogZ80_t *cpu)
{
    cpu->SP = TO_WORD(cpu->H, cpu->L);
    return 6;
}

static int ld_bc_nn_addr(ZilogZ80_t *cpu)
{
    word_t address = (word_t) fetchByteAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
    cpu->PC += 2;
    
    word_t value = (word_t) fetchWordAddressSpace(&cpu->ram, &cpu->rom, address);

    cpu->B = UPPER_BYTE(value);
    cpu->C = UPPER_BYTE(value);

    return 20;
}
static int ld_de_nn_addr(ZilogZ80_t *cpu)
{
    word_t address = (word_t) fetchByteAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
    cpu->PC += 2;
    
    word_t value = (word_t) fetchWordAddressSpace(&cpu->ram, &cpu->rom, address);

    cpu->D = UPPER_BYTE(value);
    cpu->E = UPPER_BYTE(value);

    return 20;
}
static int ld_sp_nn_addr(ZilogZ80_t *cpu)
{
    word_t address = (word_t) fetchByteAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
    cpu->PC += 2;
    
    cpu->SP = (word_t) fetchWordAddressSpace(&cpu->ram, &cpu->rom, address);

    return 20;
}
static int ld_nn_bc_addr(ZilogZ80_t *cpu)
{
    word_t address = (word_t) fetchByteAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
    cpu->PC += 2;
    
    storeWordAddressSpace(&cpu->ram, &cpu->rom, address, TO_WORD(cpu->B, cpu->C));

    return 20;
}
static int ld_nn_de_addr(ZilogZ80_t *cpu)
{
    word_t address = (word_t) fetchByteAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
    cpu->PC += 2;
    
    storeWordAddressSpace(&cpu->ram, &cpu->rom, address, TO_WORD(cpu->D, cpu->E));

    return 20;
}
static int ld_nn_sp_addr(ZilogZ80_t *cpu)
{
    word_t address = (word_t) fetchByteAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
    cpu->PC += 2;
    
    storeWordAddressSpace(&cpu->ram, &cpu->rom, address, cpu->SP);

    return 20;
}
static int ldi(ZilogZ80_t *cpu)
{
    // TODO:
}
static int ldir(ZilogZ80_t *cpu)
{
    // TODO:
}
static int ldd(ZilogZ80_t *cpu)
{
    // TODO:
}
static int lddr(ZilogZ80_t *cpu)
{
    // TODO:
}

static int ld_nn_bc_imm(ZilogZ80_t *cpu)
{
    // TODO:
}
static int ld_nn_de_imm(ZilogZ80_t *cpu)
{
    // TODO:
}
static int ld_nn_hl_imm(ZilogZ80_t *cpu)
{
    // TODO:
}
static int ld_nn_sp_imm(ZilogZ80_t *cpu)
{
    // TODO:
}

static int ld_r_a(ZilogZ80_t *cpu)
{
    // TODO:
}
static int ld_a_r(ZilogZ80_t *cpu)
{
    // TODO:
}
static int ld_i_a(ZilogZ80_t *cpu)
{
    // TODO
}
static int ld_a_i(ZilogZ80_t *cpu)
{
    // TODO
}

// OTHER INSTRUCTION    -----------------------------------------------------------------------------
static int bit_op(ZilogZ80_t *cpu)
{
    return 0;
}
static int ix_op(ZilogZ80_t *cpu)
{
    return 0;
}
static int misc_op(ZilogZ80_t *cpu)
{
    return 0;
}
static int iy_op(ZilogZ80_t *cpu)
{
    return 0;
}

// INTERRUPTS           -----------------------------------------------------------------------------
static int di(ZilogZ80_t *cpu)
{
    // TODO: Interrupts
    return 4;
}
static int ei(ZilogZ80_t *cpu)
{
    // TODO: Interrupts
    return 4;
}

// PORT     -----------------------------------------------------------------------------
static int in_a_n(ZilogZ80_t *cpu)
{
    byte_t port = fetchByteAddressSpace(&cpu->ram, &cpu->rom, (word_t) cpu->PC);
    cpu->inputCallback[port](&cpu->A);
    cpu->PC++;
    
    return 11;
}
static int out_n_a_addr(ZilogZ80_t *cpu)
{
    // TODO: Port
    byte_t port = fetchByteAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
    cpu->outputCallback[port](cpu->A);
    cpu->PC++;
    return 11;
}
static int daa(ZilogZ80_t *cpu)
{
    byte_t halfCarry = cpu->F.H;
    byte_t carry = cpu->F.C;
    byte_t subtract = cpu->F.N;
    byte_t correction = 0;
    byte_t a = cpu->A;

    if(subtract == 0)
    {
        if(halfCarry == 1 || LOWER_NIBBLE(a) > 9)
        {
            correction |= 0x06;
        }
        if(carry == 1 || a > 0x99)
        {
            correction |= 0x60;
            cpu->F.C = 1;
        }
    }
    else
    {
        if(halfCarry == 1 || LOWER_NIBBLE(a) > 9)
        {
            correction |= 0x06;
        }
        if(carry == 1 || a > 0x99)
        {
            correction |= 0x60;
            cpu->F.C = 1;
        }
    }

    if(subtract == 0)
    {
        cpu->A += correction;
    }
    else
    {
        cpu->A -= correction;
    }

    cpu->F.Z = cpu->A == 0;

    cpu->F.H = 0;

    return 4;
}
static int scf(ZilogZ80_t *cpu)
{
    cpu->F.C = 1;
    cpu->F.H = 0;
    cpu->F.N = 0;

    return 4;
}

static int in_a_c(ZilogZ80_t *cpu)
{
    cpu->inputCallback[cpu->C](&cpu->A);

    return 12;
}
static int in_b_c(ZilogZ80_t *cpu)
{
    cpu->inputCallback[cpu->C](&cpu->B);
    
    return 12;
}
static int in_c_c(ZilogZ80_t *cpu)
{
    cpu->inputCallback[cpu->C](&cpu->C);
    
    return 12;
}
static int in_d_c(ZilogZ80_t *cpu)
{
    cpu->inputCallback[cpu->C](&cpu->D);
    
    return 12;
}
static int in_e_c(ZilogZ80_t *cpu)
{
    cpu->inputCallback[cpu->C](&cpu->E);
    
    return 12;
}
static int in_h_c(ZilogZ80_t *cpu)
{
    cpu->inputCallback[cpu->C](&cpu->H);
    
    return 12;
}
static int in_l_c(ZilogZ80_t *cpu)
{
    cpu->inputCallback[cpu->C](&cpu->L);
    
    return 12;
}
static int ini(ZilogZ80_t *cpu)
{
    byte_t value;

    // Get value of I/O pointed by register C
    cpu->inputCallback[cpu->C](&value);
    
    // Store value in memory address (HL)
    storeByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L), value);

    // Increment HL
    incrementRegisterPair(cpu, &cpu->H, &cpu->L);
    // Decrement B
    cpu->B--;

    return 16;
}
static int inir(ZilogZ80_t *cpu)
{
    byte_t cycles = 16;
    byte_t value;

    do
    {        
        cpu->inputCallback[cpu->C](&value);
        
        storeByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L), value);

        incrementRegisterPair(cpu, &cpu->H, &cpu->L);
        cpu->B--;

        if(cpu->B > 0)
        {
            cycles = 21;
        }

        //TODO: Handle interrupts
    } while (cpu->B != 0);

    return 16;
}

static int in0_a_n(ZilogZ80_t *cpu)
{
    // TODO
}
static int in0_b_n(ZilogZ80_t *cpu)
{
    // TODO
}
static int in0_c_n(ZilogZ80_t *cpu)
{
    // TODO
}
static int in0_d_n(ZilogZ80_t *cpu)
{
    // TODO
}
static int in0_e_n(ZilogZ80_t *cpu)
{
    // TODO
}
static int in0_h_n(ZilogZ80_t *cpu)
{
    // TODO
}
static int in0_l_n(ZilogZ80_t *cpu)
{
    // TODO
}
static int in_c(ZilogZ80_t *cpu)
{
    // TODO
}
static int ind(ZilogZ80_t *cpu)
{
    // TODO
}
static int indr(ZilogZ80_t *cpu)
{
    // TODO
}

static int out_c_0(ZilogZ80_t *cpu)
{
    // TODO
}
static int out_c_a(ZilogZ80_t *cpu)
{
    cpu->outputCallback[cpu->C](cpu->A);
    return 12;
}
static int out_c_b(ZilogZ80_t *cpu)
{
    cpu->outputCallback[cpu->C](cpu->B);
    return 12;
}
static int out_c_c(ZilogZ80_t *cpu)
{
    cpu->outputCallback[cpu->C](cpu->C);
    return 12;
}
static int out_c_d(ZilogZ80_t *cpu)
{
    cpu->outputCallback[cpu->C](cpu->D);
    return 12;
}
static int out_c_e(ZilogZ80_t *cpu)
{
    cpu->outputCallback[cpu->C](cpu->E);
    return 12;
}
static int out_c_h(ZilogZ80_t *cpu)
{
    cpu->outputCallback[cpu->C](cpu->H);
    return 12;
}
static int out_c_l(ZilogZ80_t *cpu)
{
    cpu->outputCallback[cpu->C](cpu->L);
    return 12;
}
static int outi(ZilogZ80_t *cpu)
{
    // Decrement register B
    cpu->B--;

    // Get byte pointed by (HL)
    byte_t portValue = fetchByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L));

    // Write value to port pointed by C
    cpu->outputCallback[cpu->C](portValue);

    // Increment HL
    incrementRegisterPair(cpu, &cpu->H, &cpu->L);

    return 16;
}
static int otir(ZilogZ80_t *cpu)
{
    int cycles = 16;
    do
    {
        // Decrement register B
        cpu->B--;

        // Get byte pointed by (HL)
        byte_t portValue = fetchByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L));

        // Write value to port pointed by C
        cpu->outputCallback[cpu->C](portValue);

        // Increment HL
        incrementRegisterPair(cpu, &cpu->H, &cpu->L);

        if(cpu->B > 0)
        {
            cycles = 21;
        }

        // TODO: Handle interrupts
    } while(cpu->B != 0);

    return cycles;
}

static int out0_n_b(ZilogZ80_t *cpu)
{

}
static int out0_n_d(ZilogZ80_t *cpu)
{

}
static int out0_n_h(ZilogZ80_t *cpu)
{

}
static int out0_c_n(ZilogZ80_t *cpu)
{
    // TODO
}
static int out0_e_n(ZilogZ80_t *cpu)
{
    // TODO
}
static int out0_l_n(ZilogZ80_t *cpu)
{
    // TODO
}
static int out0_a_n(ZilogZ80_t *cpu)
{
    // TODO
}

static int otim(ZilogZ80_t *cpu)
{
    // TODO
}
static int otimr(ZilogZ80_t *cpu)
{
    // TODO
}
static int otdm(ZilogZ80_t *cpu)
{
    // TODO
}
static int otdmr(ZilogZ80_t *cpu)
{
    // TODO
}
static int outd(ZilogZ80_t *cpu)
{
    // Decrement register B
    cpu->B++;

    // Get byte pointed by (HL)
    byte_t portValue = fetchByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L));

    // Write value to port pointed by C
    cpu->outputCallback[cpu->C](portValue);

    // Decrement HL
    decrementRegisterPair(cpu, &cpu->H, &cpu->L);
    
    return 16;
}
static int otdr(ZilogZ80_t *cpu)
{
    int cycles = 16;
    do
    {
        // Decrement register B
        cpu->B--;

        // Get byte pointed by (HL)
        byte_t portValue = fetchByteAddressSpace(&cpu->ram, &cpu->rom, TO_WORD(cpu->H, cpu->L));

        // Write value to port pointed by C
        cpu->outputCallback[cpu->C](portValue);

        // Decrement HL
        decrementRegisterPair(cpu, &cpu->H, &cpu->L);

        if(cpu->B > 0)
        {
            cycles = 21;
        }
        
        // TODO: Handle interrupts
    } while(cpu->B != 0);

    return cycles;
}

// TST      -----------------------------------------------------------------------------
static int tst_b(ZilogZ80_t *cpu)
{
    // TODO:    
}
static int tst_d(ZilogZ80_t *cpu)
{
    // TODO:    
}
static int tst_h(ZilogZ80_t *cpu)
{
    // TODO:    
}
static int tst_c(ZilogZ80_t *cpu)
{
    // TODO:    
}
static int tst_e(ZilogZ80_t *cpu)
{
    // TODO:    
}
static int tst_l(ZilogZ80_t *cpu)
{
    // TODO:    
}
static int tst_a(ZilogZ80_t *cpu)
{
    // TODO:    
}
static int tst_hl_addr(ZilogZ80_t *cpu)
{
    // TODO:    
}
static int tst_n(ZilogZ80_t *cpu)
{
    // TODO:    
}
static int tstio_n(ZilogZ80_t *cpu)
{
    // TODO:    
}

// MULT     -----------------------------------------------------------------------------
static int mlt_bc(ZilogZ80_t *cpu)
{
    // TODO
}
static int mlt_de(ZilogZ80_t *cpu)
{
    // TODO
}
static int mlt_hl(ZilogZ80_t *cpu)
{
    // TODO
}
static int mlt_sp(ZilogZ80_t *cpu)
{
    // TODO
}

// IM       -----------------------------------------------------------------------------
static int im_0(ZilogZ80_t *cpu)
{
    // TODO
}
static int im_1(ZilogZ80_t *cpu)
{
    // TODO
}
static int im_2(ZilogZ80_t *cpu)
{
    // TODO
}

// EXTRA    -----------------------------------------------------------------------------
static int cpl(ZilogZ80_t *cpu)
{
    cpu->A = ~cpu->A;
    cpu->F.H = 1;
    cpu->F.N = 1;

    return 4;
}
static int ccf(ZilogZ80_t *cpu)
{
    cpu->F.C = !cpu->F.C;
    cpu->F.H = 0;
    cpu->F.N = 0;

    return 4;
}
static int neg(ZilogZ80_t *cpu)
{
    // TODO
}
static int slp(ZilogZ80_t *cpu)
{
    // TODO
}
static int rld(ZilogZ80_t *cpu)
{
    // TODO
}
static int rrd(ZilogZ80_t *cpu)
{
    // TODO
}
// -----------------------------------------------------------------------------