#include "cpu.h"
#include "instructions.h"
#include "time.h"
#include <stdio.h>

#include <stdbool.h>

#define CYCLES_PER_FRAME(x) ((int)((x * 1000000) / 60))
#define MAX_ITERATIONS 1000000

#define MAX_INSTRUCTION_COUNT 0xFF

typedef int (*InstructionHandler)(CPU_t *, Memory_t *);


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


// NOP      -----------------------------------------------------------------------------
static int nop(CPU_t *cpu, Memory_t *memory);

// HALT     -----------------------------------------------------------------------------
static int halt(CPU_t *cpu, Memory_t *memory);

// ADD      -----------------------------------------------------------------------------
static int add_hl_bc_imm(CPU_t *cpu, Memory_t *memory);
static int add_hl_de_imm(CPU_t *cpu, Memory_t *memory);
static int add_hl_hl_imm(CPU_t *cpu, Memory_t *memory);
static int add_hl_sp_imm(CPU_t *cpu, Memory_t *memory);

static int add_a_n(CPU_t *cpu, Memory_t *memory);
static int add_a_a(CPU_t *cpu, Memory_t *memory);
static int add_a_b(CPU_t *cpu, Memory_t *memory);
static int add_a_c(CPU_t *cpu, Memory_t *memory);
static int add_a_d(CPU_t *cpu, Memory_t *memory);
static int add_a_e(CPU_t *cpu, Memory_t *memory);
static int add_a_h(CPU_t *cpu, Memory_t *memory);
static int add_a_l(CPU_t *cpu, Memory_t *memory);
static int add_a_hl_addr(CPU_t *cpu, Memory_t *memory);

// ADC      -----------------------------------------------------------------------------
static int adc_a_n(CPU_t *cpu, Memory_t *memory);
static int adc_a_a(CPU_t *cpu, Memory_t *memory);
static int adc_a_b(CPU_t *cpu, Memory_t *memory);
static int adc_a_c(CPU_t *cpu, Memory_t *memory);
static int adc_a_d(CPU_t *cpu, Memory_t *memory);
static int adc_a_e(CPU_t *cpu, Memory_t *memory);
static int adc_a_h(CPU_t *cpu, Memory_t *memory);
static int adc_a_l(CPU_t *cpu, Memory_t *memory);
static int adc_a_hl_addr(CPU_t *cpu, Memory_t *memory);

// INC      -----------------------------------------------------------------------------
static int inc_bc(CPU_t *cpu, Memory_t *memory);
static int inc_de(CPU_t *cpu, Memory_t *memory);
static int inc_hl(CPU_t *cpu, Memory_t *memory);
static int inc_sp(CPU_t *cpu, Memory_t *memory);

static int inc_a(CPU_t *cpu, Memory_t *memory);
static int inc_b(CPU_t *cpu, Memory_t *memory);
static int inc_c(CPU_t *cpu, Memory_t *memory);
static int inc_d(CPU_t *cpu, Memory_t *memory);
static int inc_e(CPU_t *cpu, Memory_t *memory);
static int inc_h(CPU_t *cpu, Memory_t *memory);
static int inc_l(CPU_t *cpu, Memory_t *memory);
static int inc_hl_addr(CPU_t *cpu, Memory_t *memory);

// SUB      -----------------------------------------------------------------------------
static int sub_n(CPU_t *cpu, Memory_t *memory);
static int sub_a(CPU_t *cpu, Memory_t *memory);
static int sub_b(CPU_t *cpu, Memory_t *memory);
static int sub_c(CPU_t *cpu, Memory_t *memory);
static int sub_d(CPU_t *cpu, Memory_t *memory);
static int sub_e(CPU_t *cpu, Memory_t *memory);
static int sub_h(CPU_t *cpu, Memory_t *memory);
static int sub_l(CPU_t *cpu, Memory_t *memory);
static int sub_hl_addr(CPU_t *cpu, Memory_t *memory);

// SBC      -----------------------------------------------------------------------------
static int sbc_a_n(CPU_t *cpu, Memory_t *memory);
static int sbc_a_a(CPU_t *cpu, Memory_t *memory);
static int sbc_b(CPU_t *cpu, Memory_t *memory);
static int sbc_c(CPU_t *cpu, Memory_t *memory);
static int sbc_d(CPU_t *cpu, Memory_t *memory);
static int sbc_e(CPU_t *cpu, Memory_t *memory);
static int sbc_h(CPU_t *cpu, Memory_t *memory);
static int sbc_l(CPU_t *cpu, Memory_t *memory);
static int sbc_hl_addr(CPU_t *cpu, Memory_t *memory);

// DEC      -----------------------------------------------------------------------------
static int dec_bc(CPU_t *cpu, Memory_t *memory);
static int dec_de(CPU_t *cpu, Memory_t *memory);
static int dec_hl(CPU_t *cpu, Memory_t *memory);
static int dec_sp(CPU_t *cpu, Memory_t *memory);

static int dec_a(CPU_t *cpu, Memory_t *memory);
static int dec_b(CPU_t *cpu, Memory_t *memory);
static int dec_c(CPU_t *cpu, Memory_t *memory);
static int dec_d(CPU_t *cpu, Memory_t *memory);
static int dec_e(CPU_t *cpu, Memory_t *memory);
static int dec_h(CPU_t *cpu, Memory_t *memory);
static int dec_l(CPU_t *cpu, Memory_t *memory);
static int dec_hl_addr(CPU_t *cpu, Memory_t *memory);

// AND      -----------------------------------------------------------------------------
static int and_n(CPU_t *cpu, Memory_t *memory);
static int and_a(CPU_t *cpu, Memory_t *memory);
static int and_b(CPU_t *cpu, Memory_t *memory);
static int and_c(CPU_t *cpu, Memory_t *memory);
static int and_d(CPU_t *cpu, Memory_t *memory);
static int and_e(CPU_t *cpu, Memory_t *memory);
static int and_h(CPU_t *cpu, Memory_t *memory);
static int and_l(CPU_t *cpu, Memory_t *memory);
static int and_hl_addr(CPU_t *cpu, Memory_t *memory);

// OR       -----------------------------------------------------------------------------
static int or_n(CPU_t *cpu, Memory_t *memory);
static int or_a(CPU_t *cpu, Memory_t *memory);
static int or_b(CPU_t *cpu, Memory_t *memory);
static int or_c(CPU_t *cpu, Memory_t *memory);
static int or_d(CPU_t *cpu, Memory_t *memory);
static int or_e(CPU_t *cpu, Memory_t *memory);
static int or_h(CPU_t *cpu, Memory_t *memory);
static int or_l(CPU_t *cpu, Memory_t *memory);
static int or_hl_addr(CPU_t *cpu, Memory_t *memory);

// XOR      -----------------------------------------------------------------------------
static int xor_n(CPU_t *cpu, Memory_t *memory);
static int xor_a(CPU_t *cpu, Memory_t *memory);
static int xor_b(CPU_t *cpu, Memory_t *memory);
static int xor_c(CPU_t *cpu, Memory_t *memory);
static int xor_d(CPU_t *cpu, Memory_t *memory);
static int xor_e(CPU_t *cpu, Memory_t *memory);
static int xor_h(CPU_t *cpu, Memory_t *memory);
static int xor_l(CPU_t *cpu, Memory_t *memory);
static int xor_hl_addr(CPU_t *cpu, Memory_t *memory);

// CP       -----------------------------------------------------------------------------
static int cp_n(CPU_t *cpu, Memory_t *memory);
static int cp_a(CPU_t *cpu, Memory_t *memory);
static int cp_b(CPU_t *cpu, Memory_t *memory);
static int cp_c(CPU_t *cpu, Memory_t *memory);
static int cp_d(CPU_t *cpu, Memory_t *memory);
static int cp_e(CPU_t *cpu, Memory_t *memory);
static int cp_h(CPU_t *cpu, Memory_t *memory);
static int cp_l(CPU_t *cpu, Memory_t *memory);
static int cp_hl_addr(CPU_t *cpu, Memory_t *memory);

// PUSH     -----------------------------------------------------------------------------
static int push_bc(CPU_t *cpu, Memory_t *memory);
static int push_de(CPU_t *cpu, Memory_t *memory);
static int push_hl(CPU_t *cpu, Memory_t *memory);
static int push_af(CPU_t *cpu, Memory_t *memory);

// POP      -----------------------------------------------------------------------------
static int pop_bc(CPU_t *cpu, Memory_t *memory);
static int pop_de(CPU_t *cpu, Memory_t *memory);
static int pop_hl(CPU_t *cpu, Memory_t *memory);
static int pop_af(CPU_t *cpu, Memory_t *memory);

// CALL     -----------------------------------------------------------------------------
static int call_nz_nn(CPU_t *cpu, Memory_t *memory);
static int call_z_nn(CPU_t *cpu, Memory_t *memory);
static int call_nn(CPU_t *cpu, Memory_t *memory);
static int call_nc_nn(CPU_t *cpu, Memory_t *memory);
static int call_c_nn(CPU_t *cpu, Memory_t *memory);
static int call_po_nn(CPU_t *cpu, Memory_t *memory);
static int call_pe_nn(CPU_t *cpu, Memory_t *memory);
static int call_p_nn(CPU_t *cpu, Memory_t *memory);
static int call_m_nn(CPU_t *cpu, Memory_t *memory);

// RET      -----------------------------------------------------------------------------
static int ret_nz(CPU_t *cpu, Memory_t *memory);
static int ret_z(CPU_t *cpu, Memory_t *memory);
static int ret_nc(CPU_t *cpu, Memory_t *memory);
static int ret_c(CPU_t *cpu, Memory_t *memory);
static int ret_po(CPU_t *cpu, Memory_t *memory);
static int ret_pe(CPU_t *cpu, Memory_t *memory);
static int ret_p(CPU_t *cpu, Memory_t *memory);
static int ret_m(CPU_t *cpu, Memory_t *memory);

// ROTATE   -----------------------------------------------------------------------------
static int rlca(CPU_t *cpu, Memory_t *memory);
static int rrca(CPU_t *cpu, Memory_t *memory);
static int rla(CPU_t *cpu, Memory_t *memory);
static int rra(CPU_t *cpu, Memory_t *memory);

// RST      -----------------------------------------------------------------------------
static int rst_00h(CPU_t *cpu, Memory_t *memory);
static int rst_08h(CPU_t *cpu, Memory_t *memory);
static int rst_10h(CPU_t *cpu, Memory_t *memory);
static int rst_18h(CPU_t *cpu, Memory_t *memory);
static int rst_20h(CPU_t *cpu, Memory_t *memory);
static int rst_28h(CPU_t *cpu, Memory_t *memory);
static int rst_30h(CPU_t *cpu, Memory_t *memory);
static int rst_38h(CPU_t *cpu, Memory_t *memory);

// JUMP     -----------------------------------------------------------------------------
static int djnz_d(CPU_t *cpu, Memory_t *memory);
static int jr_d(CPU_t *cpu, Memory_t *memory);
static int jr_nz_d(CPU_t *cpu, Memory_t *memory);
static int jr_z_b(CPU_t *cpu, Memory_t *memory);
static int jr_nc_d(CPU_t *cpu, Memory_t *memory);
static int jr_c_b(CPU_t *cpu, Memory_t *memory);
static int jp_nz_nn(CPU_t *cpu, Memory_t *memory);
static int jp_nn(CPU_t *cpu, Memory_t *memory);
static int jp_z_nn(CPU_t *cpu, Memory_t *memory);
static int jp_nc_nn(CPU_t *cpu, Memory_t *memory);
static int jp_c_nn(CPU_t *cpu, Memory_t *memory);
static int jp_po_nn(CPU_t *cpu, Memory_t *memory);
static int jp_hl_addr(CPU_t *cpu, Memory_t *memory);
static int jp_hl_addr(CPU_t *cpu, Memory_t *memory);
static int jp_p_nn(CPU_t *cpu, Memory_t *memory);
static int jp_m_nn(CPU_t *cpu, Memory_t *memory);
// EXCHANGE -----------------------------------------------------------------------------
static int ex_af_af_(CPU_t *cpu, Memory_t *memory);
static int exx(CPU_t *cpu, Memory_t *memory);
static int ex_sp_hl_addr(CPU_t *cpu, Memory_t *memory);
static int ex_de_hl(CPU_t *cpu, Memory_t *memory);
// LD       -----------------------------------------------------------------------------
static int ld_a_n(CPU_t *cpu, Memory_t *memory);
static int ld_a_a(CPU_t *cpu, Memory_t *memory);
static int ld_a_b(CPU_t *cpu, Memory_t *memory);
static int ld_a_c(CPU_t *cpu, Memory_t *memory);
static int ld_a_d(CPU_t *cpu, Memory_t *memory);
static int ld_a_e(CPU_t *cpu, Memory_t *memory);
static int ld_a_h(CPU_t *cpu, Memory_t *memory);
static int ld_a_l(CPU_t *cpu, Memory_t *memory);
static int ld_a_hl_addr(CPU_t *cpu, Memory_t *memory);

static int ld_b_n(CPU_t *cpu, Memory_t *memory);
static int ld_b_a(CPU_t *cpu, Memory_t *memory);
static int ld_b_b(CPU_t *cpu, Memory_t *memory);
static int ld_b_c(CPU_t *cpu, Memory_t *memory);
static int ld_b_d(CPU_t *cpu, Memory_t *memory);
static int ld_b_e(CPU_t *cpu, Memory_t *memory);
static int ld_b_h(CPU_t *cpu, Memory_t *memory);
static int ld_b_l(CPU_t *cpu, Memory_t *memory);
static int ld_b_hl_addr(CPU_t *cpu, Memory_t *memory);

static int ld_c_n(CPU_t *cpu, Memory_t *memory);
static int ld_c_a(CPU_t *cpu, Memory_t *memory);
static int ld_c_b(CPU_t *cpu, Memory_t *memory);
static int ld_c_c(CPU_t *cpu, Memory_t *memory);
static int ld_c_d(CPU_t *cpu, Memory_t *memory);
static int ld_c_e(CPU_t *cpu, Memory_t *memory);
static int ld_c_h(CPU_t *cpu, Memory_t *memory);
static int ld_c_l(CPU_t *cpu, Memory_t *memory);
static int ld_c_hl_addr(CPU_t *cpu, Memory_t *memory);

static int ld_d_n(CPU_t *cpu, Memory_t *memory);
static int ld_d_a(CPU_t *cpu, Memory_t *memory);
static int ld_d_b(CPU_t *cpu, Memory_t *memory);
static int ld_d_c(CPU_t *cpu, Memory_t *memory);
static int ld_d_d(CPU_t *cpu, Memory_t *memory);
static int ld_d_e(CPU_t *cpu, Memory_t *memory);
static int ld_d_h(CPU_t *cpu, Memory_t *memory);
static int ld_d_l(CPU_t *cpu, Memory_t *memory);
static int ld_d_hl_addr(CPU_t *cpu, Memory_t *memory);

static int ld_e_n(CPU_t *cpu, Memory_t *memory);
static int ld_e_a(CPU_t *cpu, Memory_t *memory);
static int ld_e_b(CPU_t *cpu, Memory_t *memory);
static int ld_e_c(CPU_t *cpu, Memory_t *memory);
static int ld_e_d(CPU_t *cpu, Memory_t *memory);
static int ld_e_e(CPU_t *cpu, Memory_t *memory);
static int ld_e_h(CPU_t *cpu, Memory_t *memory);
static int ld_e_l(CPU_t *cpu, Memory_t *memory);
static int ld_e_hl_addr(CPU_t *cpu, Memory_t *memory);

static int ld_h_n(CPU_t *cpu, Memory_t *memory);
static int ld_h_a(CPU_t *cpu, Memory_t *memory);
static int ld_h_b(CPU_t *cpu, Memory_t *memory);
static int ld_h_c(CPU_t *cpu, Memory_t *memory);
static int ld_h_d(CPU_t *cpu, Memory_t *memory);
static int ld_h_e(CPU_t *cpu, Memory_t *memory);
static int ld_h_h(CPU_t *cpu, Memory_t *memory);
static int ld_h_l(CPU_t *cpu, Memory_t *memory);
static int ld_h_hl_addr(CPU_t *cpu, Memory_t *memory);

static int ld_l_n(CPU_t *cpu, Memory_t *memory);
static int ld_l_a(CPU_t *cpu, Memory_t *memory);
static int ld_l_b(CPU_t *cpu, Memory_t *memory);
static int ld_l_c(CPU_t *cpu, Memory_t *memory);
static int ld_l_d(CPU_t *cpu, Memory_t *memory);
static int ld_l_e(CPU_t *cpu, Memory_t *memory);
static int ld_l_h(CPU_t *cpu, Memory_t *memory);
static int ld_l_l(CPU_t *cpu, Memory_t *memory);
static int ld_l_hl_addr(CPU_t *cpu, Memory_t *memory);

static int ld_hl_n_addr(CPU_t *cpu, Memory_t *memory);
static int ld_hl_a_addr(CPU_t *cpu, Memory_t *memory);
static int ld_hl_b_addr(CPU_t *cpu, Memory_t *memory);
static int ld_hl_c_addr(CPU_t *cpu, Memory_t *memory);
static int ld_hl_d_addr(CPU_t *cpu, Memory_t *memory);
static int ld_hl_e_addr(CPU_t *cpu, Memory_t *memory);
static int ld_hl_h_addr(CPU_t *cpu, Memory_t *memory);
static int ld_hl_l_addr(CPU_t *cpu, Memory_t *memory);
static int ld_hl_hl_addr(CPU_t *cpu, Memory_t *memory);

static int ld_hl_nn_addr(CPU_t *cpu, Memory_t *memory);
static int ld_a_nn_addr(CPU_t *cpu, Memory_t *memory);

static int ld_nn_hl_addr(CPU_t *cpu, Memory_t *memory);
static int ld_nn_a_addr(CPU_t *cpu, Memory_t *memory);

static int ld_de_nn_imm(CPU_t *cpu, Memory_t *memory);
static int ld_de_a_addr(CPU_t *cpu, Memory_t *memory);
static int ld_a_de_addr(CPU_t *cpu, Memory_t *memory);

static int ld_bc_nn_imm(CPU_t *cpu, Memory_t *memory);
static int ld_bc_a_addr(CPU_t *cpu, Memory_t *memory);
static int ld_a_bc_addr(CPU_t *cpu, Memory_t *memory);

static int ld_hl_nn_imm(CPU_t *cpu, Memory_t *memory);
static int ld_hl_a_addr(CPU_t *cpu, Memory_t *memory);
static int ld_a_hl_addr(CPU_t *cpu, Memory_t *memory);

static int ld_sp_nn_imm(CPU_t *cpu, Memory_t *memory);

static int ld_sp_hl(CPU_t *cpu, Memory_t *memory);

// OTHER INSTRUCTION    -----------------------------------------------------------------------------
static int bit_op(CPU_t *cpu, Memory_t *memory);
static int ix_op(CPU_t *cpu, Memory_t *memory);
static int misc_op(CPU_t *cpu, Memory_t *memory);
static int iy_op(CPU_t *cpu, Memory_t *memory);

// INTERRUPTS           -----------------------------------------------------------------------------
static int di(CPU_t *cpu, Memory_t *memory);
static int ei(CPU_t *cpu, Memory_t *memory);

// PORT     -----------------------------------------------------------------------------
static int in_a_n(CPU_t *cpu, Memory_t *memory);
static int out_n_a_addr(CPU_t *cpu, Memory_t *memory);
static int daa(CPU_t *cpu, Memory_t *memory);
static int scf(CPU_t *cpu, Memory_t *memory);

// EXTRA
static int cpl(CPU_t *cpu, Memory_t *memory);
static int ccf(CPU_t *cpu, Memory_t *memory);

static const InstructionHandler mainInstructionTable[MAX_INSTRUCTION_COUNT] = {
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
/*0xE*/ ret_po,         pop_hl,         jp_po_nn,       ex_sp_hl_addr,  call_po_nn,     push_hl,        and_n,          rst_20h,        ret_pe,         jp_hl_addr,         jp_hl_addr,         ex_de_hl,       call_pe_nn,     misc_op,    xor_n,          rst_28h,
/*0xF*/ ret_p,          pop_af,         jp_p_nn,        di,             call_p_nn,      push_af,        or_n,           rst_30h,        ret_m,          ld_sp_hl,           jp_m_nn,            ei,             call_m_nn,      iy_op,      cp_n,           rst_38h
};

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

static int mainInstructions(CPU_t *cpu, Memory_t *memory, byte_t instruction)
{
    byte_t cycles = mainInstructionTable[instruction](cpu, memory);
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
static int nop(CPU_t *cpu, Memory_t *memory)
{
    return 4;
}

// EXCHANGE -----------------------------------------------------------------------------
static int ex_af_af_(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}

// ADD      -----------------------------------------------------------------------------
static int add_hl_bc_imm(CPU_t *cpu, Memory_t *memory)
{
    //
}




// INC      -----------------------------------------------------------------------------
static int inc_bc(CPU_t *cpu, Memory_t *memory)
{
    incrementRegisterPair(cpu, cpu->B, cpu->C);
    return 6;
}
static int inc_de(CPU_t *cpu, Memory_t *memory)
{
    incrementRegisterPair(cpu, cpu->D, cpu->E);
    return 6;
}
static int inc_hl(CPU_t *cpu, Memory_t *memory)
{
    incrementRegisterPair(cpu, cpu->H, cpu->L);
    return 6;
}
static int inc_sp(CPU_t *cpu, Memory_t *memory)
{
    byte_t s = UPPER_BYTE(cpu->SP);
    byte_t p = LOWER_BYTE(cpu->SP);
    incrementRegisterPair(cpu, &s, &p);
    cpu->SP = TO_WORD(s, p);
}

static int inc_a(CPU_t *cpu, Memory_t *memory)
{
    incrementRegister(cpu, cpu->A);
    return 4;
}
static int inc_b(CPU_t *cpu, Memory_t *memory)
{
    incrementRegister(cpu, cpu->B);
    return 4;
}
static int inc_c(CPU_t *cpu, Memory_t *memory)
{
    incrementRegister(cpu, cpu->C);
    return 4;
}
static int inc_d(CPU_t *cpu, Memory_t *memory)
{
    incrementRegister(cpu, cpu->D);
    return 4;
}
static int inc_e(CPU_t *cpu, Memory_t *memory)
{
    incrementRegister(cpu, cpu->E);
    return 4;
}
static int inc_h(CPU_t *cpu, Memory_t *memory)
{
    incrementRegister(cpu, cpu->H);
    return 4;
}
static int inc_l(CPU_t *cpu, Memory_t *memory)
{
    incrementRegister(cpu, cpu->L);
    return 4;
}
static int inc_hl_addr(CPU_t *cpu, Memory_t *memory)
{
    byte_t val = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
    incrementRegister(cpu, &val);
    storeByte(memory, TO_WORD(cpu->H, cpu->L), val);
    return 11;
}

// DEC      -----------------------------------------------------------------------------
static int dec_bc(CPU_t *cpu, Memory_t *memory)
{
    decrementRegisterPair(cpu, cpu->B, cpu->C);
    return 6;
}
static int dec_de(CPU_t *cpu, Memory_t *memory)
{
    decrementRegisterPair(cpu, cpu->D, cpu->E);
    return 6;
}
static int dec_hl(CPU_t *cpu, Memory_t *memory)
{
    decrementRegisterPair(cpu, cpu->D, cpu->E);
    return 6;
}
static int dec_sp(CPU_t *cpu, Memory_t *memory)
{
    byte_t s = UPPER_BYTE(cpu->SP);
    byte_t p = LOWER_BYTE(cpu->SP);
    decrementRegisterPair(cpu, &s, &p);
    cpu->SP = TO_WORD(s, p);
    return 6;
}

static int dec_a(CPU_t *cpu, Memory_t *memory)
{
    decrementRegister(cpu, cpu->A);
    return 4;
}
static int dec_b(CPU_t *cpu, Memory_t *memory)
{
    decrementRegister(cpu, cpu->B);
    return 4;
}
static int dec_c(CPU_t *cpu, Memory_t *memory)
{
    decrementRegister(cpu, cpu->C);
    return 4;
}
static int dec_d(CPU_t *cpu, Memory_t *memory)
{
    decrementRegister(cpu, cpu->D);
    return 4;
}
static int dec_e(CPU_t *cpu, Memory_t *memory)
{
    decrementRegister(cpu, cpu->E);
    return 4;
}
static int dec_h(CPU_t *cpu, Memory_t *memory)
{
    decrementRegister(cpu, cpu->H);
    return 4;
}
static int dec_l(CPU_t *cpu, Memory_t *memory)
{
    decrementRegister(cpu, cpu->L);
    return 4;
}
static int dec_hl_addr(CPU_t *cpu, Memory_t *memory)
{
    byte_t val = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
    decrementRegister(cpu, &val);
    storeByte(memory, TO_WORD(cpu->H, cpu->L), val);
    return 11;
}

// ROTATE   -----------------------------------------------------------------------------
static int rlca(CPU_t *cpu, Memory_t *memory)
{
    // TODO: Rotate instruction
    return 4;
}
static int rrca(CPU_t *cpu, Memory_t *memory)
{
    return 4;
}

// LD       -----------------------------------------------------------------------------
static int ld_a_bc_addr(CPU_t *cpu, Memory_t *memory)
{

}
static int ld_c_n_imm(CPU_t *cpu, Memory_t *memory)
{

}
static int ld_b_n_imm(CPU_t *cpu, Memory_t *memory)
{

}
static int ld_bc_nn_imm(CPU_t *cpu, Memory_t *memory)
{
    word_t word = fetchWord(memory, cpu->PC);
    cpu->B = UPPER_BYTE(word);
    cpu->C = LOWER_BYTE(word);
    cpu->PC += 2;
    return 10;
}
static int ld_bc_a_addr(CPU_t *cpu, Memory_t *memory)
{
    storeByte(memory, TO_WORD(cpu->B, cpu->C), cpu->A);
    return 7;
}

// -----------------------------------------------------------------------------