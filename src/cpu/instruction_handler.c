#include "instruction_handler.h"

#include <stdio.h>
#include <stdbool.h>

#include "utils/error_handler.h"

#define MAX_INSTRUCTION_COUNT 256

/**
 * @brief Instruction function pointer
 * 
 */
typedef int (*InstructionHandler_t)(CPU_t *, Memory_t *);

// CPU helper functions -------------------------------------------------------
static byte_t flagsToByte(F_t flags);
static void byteToFlags(F_t *flags, byte_t value);

static int calculateParity(word_t value);
static void setFlags(CPU_t *cpu, byte_t regA, byte_t operand, word_t result, bool isSubstraction);
static void setFlagsWord(CPU_t *cpu, word_t reg1, word_t reg2, dword_t result);
// -----------------------------------------------------------------------------

// Helper functions ------------------------------------------------------------
static void addToRegister(CPU_t *cpu, byte_t *reg, byte_t value);
static void addToRegisterPair(CPU_t *cpu, word_t value1, word_t value2);
static void addToRegisterWithCarry(CPU_t *cpu, byte_t *reg, byte_t value);
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

static int returnHelper(CPU_t *cpu, Memory_t *memory, bool condition);
static int callHelper(CPU_t *cpu, Memory_t *memory, bool condition);
static int jumpHelper(CPU_t *cpu, Memory_t *memory, bool condition);
static int jumpRelativeHelper(CPU_t *cpu, Memory_t *memory, bool condition);

static void rst(CPU_t *cpu, Memory_t *memory, byte_t address);

static void exWord(CPU_t *cpu, word_t *reg1, word_t *reg2);

static void ld(CPU_t *cpu, byte_t *reg, byte_t value);
static void ldPair(CPU_t *cpu, byte_t *upperByte, byte_t *lowerByte, word_t value);
// -----------------------------------------------------------------------------

// NO Func  -----------------------------------------------------------------------------
static int noFunc(CPU_t *cpu, Memory_t *memory);

// NOP      -----------------------------------------------------------------------------
static int nop(CPU_t *cpu, Memory_t *memory);

// HALT     -----------------------------------------------------------------------------
static int halt(CPU_t *cpu, Memory_t *memory);

// ADD  -----------------------------------------------------------------------------
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

static int adc_hl_bc(CPU_t *cpu, Memory_t *memory);
static int adc_hl_de(CPU_t *cpu, Memory_t *memory);
static int adc_hl_hl(CPU_t *cpu, Memory_t *memory);
static int adc_hl_sp(CPU_t *cpu, Memory_t *memory);

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

static int sbc_hl_bc(CPU_t *cpu, Memory_t *memory);
static int sbc_hl_de(CPU_t *cpu, Memory_t *memory);
static int sbc_hl_hl(CPU_t *cpu, Memory_t *memory);
static int sbc_hl_sp(CPU_t *cpu, Memory_t *memory);

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
static int cpi(CPU_t *cpu, Memory_t *memory);
static int cpir(CPU_t *cpu, Memory_t *memory);

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
static int ret(CPU_t *cpu, Memory_t *memory);
static int retn(CPU_t *cpu, Memory_t *memory);
static int reti(CPU_t *cpu, Memory_t *memory);

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
static int jp_p_nn(CPU_t *cpu, Memory_t *memory);
static int jp_m_nn(CPU_t *cpu, Memory_t *memory);

// EXCHANGE -----------------------------------------------------------------------------
static int exx(CPU_t *cpu, Memory_t *memory);
static int ex_af_af_(CPU_t *cpu, Memory_t *memory);
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
static int ld_bc_nn_addr(CPU_t *cpu, Memory_t *memory);
static int ld_de_nn_addr(CPU_t *cpu, Memory_t *memory);
static int ld_sp_nn_addr(CPU_t *cpu, Memory_t *memory);

static int ld_nn_hl_addr(CPU_t *cpu, Memory_t *memory);
static int ld_nn_a_addr(CPU_t *cpu, Memory_t *memory);
static int ld_nn_bc_addr(CPU_t *cpu, Memory_t *memory);
static int ld_nn_de_addr(CPU_t *cpu, Memory_t *memory);
static int ld_nn_sp_addr(CPU_t *cpu, Memory_t *memory);

static int ld_de_nn_imm(CPU_t *cpu, Memory_t *memory);
static int ld_de_a_addr(CPU_t *cpu, Memory_t *memory);
static int ld_a_de_addr(CPU_t *cpu, Memory_t *memory);

static int ld_bc_nn_imm(CPU_t *cpu, Memory_t *memory);
static int ld_bc_a_addr(CPU_t *cpu, Memory_t *memory);
static int ld_a_bc_addr(CPU_t *cpu, Memory_t *memory);

static int ld_hl_nn_imm(CPU_t *cpu, Memory_t *memory);

static int ld_sp_nn_imm(CPU_t *cpu, Memory_t *memory);

static int ld_sp_hl(CPU_t *cpu, Memory_t *memory);

static int ldi(CPU_t *cpu, Memory_t *memory);
static int ldir(CPU_t *cpu, Memory_t *memory);
static int ldd(CPU_t *cpu, Memory_t *memory);
static int lddr(CPU_t *cpu, Memory_t *memory);

static int ld_nn_bc_imm(CPU_t *cpu, Memory_t *memory);
static int ld_nn_de_imm(CPU_t *cpu, Memory_t *memory);
static int ld_nn_hl_imm(CPU_t *cpu, Memory_t *memory);
static int ld_nn_sp_imm(CPU_t *cpu, Memory_t *memory);

static int ld_r_a(CPU_t *cpu, Memory_t *memory);
static int ld_a_r(CPU_t *cpu, Memory_t *memory);


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
static int in_b_c(CPU_t *cpu, Memory_t *memory);
static int in_d_c(CPU_t *cpu, Memory_t *memory);
static int in_e_c(CPU_t *cpu, Memory_t *memory);
static int in_h_c(CPU_t *cpu, Memory_t *memory);
static int in_l_c(CPU_t *cpu, Memory_t *memory);
static int ini(CPU_t *cpu, Memory_t *memory);
static int inir(CPU_t *cpu, Memory_t *memory);
static int in_c_c(CPU_t *cpu, Memory_t *memory);
static int in_a_c(CPU_t *cpu, Memory_t *memory);

static int in0_c_n(CPU_t *cpu, Memory_t *memory);
static int in0_e_n(CPU_t *cpu, Memory_t *memory);
static int in0_l_n(CPU_t *cpu, Memory_t *memory);
static int in0_a_n(CPU_t *cpu, Memory_t *memory);

static int out_n_a_addr(CPU_t *cpu, Memory_t *memory);
static int out_c_b(CPU_t *cpu, Memory_t *memory);
static int out_c_d(CPU_t *cpu, Memory_t *memory);
static int out_c_h(CPU_t *cpu, Memory_t *memory);
static int out_c_0(CPU_t *cpu, Memory_t *memory);
static int out_c_c(CPU_t *cpu, Memory_t *memory);
static int out_c_e(CPU_t *cpu, Memory_t *memory);
static int out_c_l(CPU_t *cpu, Memory_t *memory);
static int out_c_a(CPU_t *cpu, Memory_t *memory);
static int outi(CPU_t *cpu, Memory_t *memory);
static int otir(CPU_t *cpu, Memory_t *memory);

static int out0_c_n(CPU_t *cpu, Memory_t *memory);
static int out0_e_n(CPU_t *cpu, Memory_t *memory);
static int out0_l_n(CPU_t *cpu, Memory_t *memory);
static int out0_a_n(CPU_t *cpu, Memory_t *memory);

static int daa(CPU_t *cpu, Memory_t *memory);
static int scf(CPU_t *cpu, Memory_t *memory);

static int otim(CPU_t *cpu, Memory_t *memory);
static int otimr(CPU_t *cpu, Memory_t *memory);
static int otdm(CPU_t *cpu, Memory_t *memory);
static int otdmr(CPU_t *cpu, Memory_t *memory);
static int outd(CPU_t *cpu, Memory_t *memory);
static int otdr(CPU_t *cpu, Memory_t *memory);

// TST      -----------------------------------------------------------------------------
static int tst_b(CPU_t *cpu, Memory_t *memory);
static int tst_d(CPU_t *cpu, Memory_t *memory);
static int tst_h(CPU_t *cpu, Memory_t *memory);
static int tst_c(CPU_t *cpu, Memory_t *memory);
static int tst_e(CPU_t *cpu, Memory_t *memory);
static int tst_l(CPU_t *cpu, Memory_t *memory);
static int tst_a(CPU_t *cpu, Memory_t *memory);
static int tst_hl_addr(CPU_t *cpu, Memory_t *memory);
static int tst_n(CPU_t *cpu, Memory_t *memory);
static int tstio_n(CPU_t *cpu, Memory_t *memory);

// MULT     -----------------------------------------------------------------------------
static int mlt_bc(CPU_t *cpu, Memory_t *memory);
static int mlt_de(CPU_t *cpu, Memory_t *memory);
static int mlt_hl(CPU_t *cpu, Memory_t *memory);
static int mlt_sp(CPU_t *cpu, Memory_t *memory);

// IM       -----------------------------------------------------------------------------
static int im_0(CPU_t *cpu, Memory_t *memory);
static int im_1(CPU_t *cpu, Memory_t *memory);
static int im_2(CPU_t *cpu, Memory_t *memory);

// EXTRA    -----------------------------------------------------------------------------
static int cpl(CPU_t *cpu, Memory_t *memory);
static int ccf(CPU_t *cpu, Memory_t *memory);
static int neg(CPU_t *cpu, Memory_t *memory);
static int slp(CPU_t *cpu, Memory_t *memory);
static int rld(CPU_t *cpu, Memory_t *memory);

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
/*0xE*/ ret_po,         pop_hl,         jp_po_nn,       ex_sp_hl_addr,  call_po_nn,     push_hl,        and_n,          rst_20h,        ret_pe,         ret,                jp_hl_addr,         ex_de_hl,       call_pe_nn,     misc_op,    xor_n,          rst_28h,
/*0xF*/ ret_p,          pop_af,         jp_p_nn,        di,             call_p_nn,      push_af,        or_n,           rst_30h,        ret_m,          ld_sp_hl,           jp_m_nn,            ei,             call_m_nn,      iy_op,      cp_n,           rst_38h
};

static const InstructionHandler_t miscInstructionTable[MAX_INSTRUCTION_COUNT] =
{
/*      0               1               2               3               4               5               6               7               8               9                   A                   B               C               D           E               F*/
/*0x0*/ noFunc,         noFunc,         noFunc,         noFunc,         tst_b,          noFunc,         noFunc,         noFunc,         in0_c_n,        out0_c_n,           noFunc,             noFunc,         tst_c,          noFunc,     noFunc,         noFunc,
/*0x1*/ noFunc,         noFunc,         noFunc,         noFunc,         tst_d,          noFunc,         noFunc,         rla,            in0_e_n,        out0_e_n,           noFunc,             noFunc,         tst_e,          noFunc,     noFunc,         noFunc,
/*0x2*/ noFunc,         noFunc,         noFunc,         noFunc,         tst_h,          noFunc,         noFunc,         daa,            in0_l_n,        out0_l_n,           noFunc,             noFunc,         tst_l,          noFunc,     noFunc,         noFunc,
/*0x3*/ noFunc,         noFunc,         noFunc,         noFunc,         tst_hl_addr,    noFunc,         noFunc,         scf,            in0_a_n,        out0_a_n,           noFunc,             noFunc,         tst_a,          noFunc,     noFunc,         noFunc,
/*0x4*/ in_b_c,         out_c_b,        sbc_hl_bc,      ld_nn_bc_addr,  neg,            retn,           im_0,           ld_b_a,         in_c_c,         out_c_c,            adc_hl_bc,          ld_bc_nn_addr,  mlt_bc,         reti,       noFunc,         ld_r_a,
/*0x5*/ in_d_c,         out_c_d,        sbc_hl_de,      ld_nn_de_addr,  noFunc,         noFunc,         im_1,           ld_d_a,         in_e_c,         out_c_e,            adc_hl_de,          ld_de_nn_addr,  mlt_de,         noFunc,     im_2,           ld_a_r,
/*0x6*/ in_h_c,         out_c_h,        sbc_hl_hl,      ld_nn_hl_addr,  tst_n,          noFunc,         noFunc,         ld_h_a,         in_l_c,         out_c_l,            adc_hl_hl,          ld_hl_nn_addr,  mlt_hl,         noFunc,     noFunc,         rld,
/*0x7*/ noFunc,         out_c_0,        sbc_hl_sp,      ld_nn_sp_addr,  tstio_n,        noFunc,         slp,            ld_hl_a_addr,   in_a_c,         out_c_a,            adc_hl_sp,          ld_sp_nn_addr,  mlt_sp,         noFunc,     noFunc,         noFunc,
/*0x8*/ noFunc,         noFunc,         noFunc,         otim,           noFunc,         noFunc,         noFunc,         add_a_a,        noFunc,         noFunc,             noFunc,             adc_a_e,        noFunc,         noFunc,     noFunc,         noFunc,
/*0x9*/ noFunc,         noFunc,         noFunc,         otimr,          noFunc,         noFunc,         noFunc,         sub_hl_addr,    noFunc,         noFunc,             noFunc,             otdm,           noFunc,         noFunc,     noFunc,         noFunc,
/*0xA*/ ldi,            cpi,            ini,            outi,           noFunc,         noFunc,         noFunc,         and_a,          noFunc,         ldd,                noFunc,             otdmr,          noFunc,         noFunc,     noFunc,         noFunc,
/*0xB*/ ldir,           cpir,           inir,           otir,           noFunc,         noFunc,         noFunc,         or_a,           noFunc,         lddr,               noFunc,             outd,           noFunc,         noFunc,     noFunc,         noFunc,
/*0xC*/ noFunc,         noFunc,         noFunc,         noFunc,         noFunc,         noFunc,         noFunc,         rst_00h,        noFunc,         noFunc,             noFunc,             otdr,           noFunc,         noFunc,     noFunc,         noFunc,
/*0xD*/ noFunc,         noFunc,         noFunc,         noFunc,         noFunc,         noFunc,         noFunc,         rst_10h,        noFunc,         noFunc,             noFunc,             noFunc,         noFunc,         noFunc,     noFunc,         noFunc,
/*0xE*/ noFunc,         noFunc,         noFunc,         noFunc,         noFunc,         noFunc,         noFunc,         rst_20h,        noFunc,         noFunc,             noFunc,             noFunc,         noFunc,         noFunc,     noFunc,         noFunc,
/*0xF*/ noFunc,         noFunc,         noFunc,         noFunc,         noFunc,         noFunc,         noFunc,         rst_30h,        noFunc,         noFunc,             noFunc,             noFunc,         noFunc,         noFunc,     noFunc,         noFunc,
};

int executeInstruction(CPU_t *cpu, Memory_t *memory)
{
    byte_t opcode = fetchByte(memory, cpu->PC);
    cpu->PC++;

    int cycles = mainInstructionTable[opcode](cpu, memory);

    cpu->currentCycles = cycles;
    cpu->totalCycles += cycles;
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

// Helper functions ------------------------------------------------------------
static void addToRegister(CPU_t *cpu, byte_t *reg, byte_t value)
{
    word_t result = (word_t)(*reg + value);
    setFlags(cpu, *reg, value, result, false);
    *reg = result & 0xFF;
}
static void addToRegisterPair(CPU_t *cpu, word_t value1, word_t value2)
{
    dword_t result = (dword_t)(value1 + value2);
    setFlagsWord(cpu, value1, value2, result);
    cpu->H = cpu->H + value1;   // TODO: May change
    cpu->L = cpu->L + value2;
}
static void addToRegisterWithCarry(CPU_t *cpu, byte_t *reg, byte_t value)
{
    word_t result = (word_t)(*reg + value + cpu->F.C);
    setFlags(cpu, *reg, value, result, false);
    *reg = result & 0xFF;
}
static void addToRegisterPairWithCarry(CPU_t *cpu, word_t value1, word_t value2)
{
    dword_t result = (dword_t)(value1 + value2 + cpu->F.C);
    setFlagsWord(cpu, value1, value2, result);
    cpu->H = cpu->H + value1;   // TODO: May change
    cpu->L = cpu->L + value2;
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
static void subtractFromRegisterPairWithCarry(CPU_t *cpu, word_t val1, word_t val2)
{
    dword_t result = (dword_t)(val1 - val2 - cpu->F.C);
    setFlagsWord(cpu, val1, val2, result);
    cpu->H = val1 - val2;   // TODO: May change
    cpu->L = val1 - val2;
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

static int returnHelper(CPU_t *cpu, Memory_t *memory, bool condition)
{
    int cycles = 5;

    if(condition)
    {
        byte_t lowerByte, upperByte;
        popWord(cpu, memory, &upperByte, &lowerByte);
        cpu->PC = TO_WORD(upperByte, lowerByte);

        cycles = 11;
    }

    return cycles;
}
static int callHelper(CPU_t *cpu, Memory_t *memory, bool condition)
{
    int cycles = 10;
    word_t address = fetchWord(memory, cpu->PC);
    cpu->PC += 2;

    if(condition)
    {
        pushWord(cpu, memory, cpu->PC);
        cpu->PC = address;

        cycles = 17;
    }

    return cycles;
}

static int jumpHelper(CPU_t *cpu, Memory_t *memory, bool condition)
{
    if(condition)
    {
        word_t address = fetchWord(memory, cpu->PC);
        cpu->PC = address;
    }

    return 10;
}
static int jumpRelativeHelper(CPU_t *cpu, Memory_t *memory, bool condition)
{
    int cycles = 7;

    if(condition)
    {
        byte_t offset = fetchByte(memory, cpu->PC);
        cpu->PC += offset;
        cycles = 12;
    }

    return cycles;
}

static void rst(CPU_t *cpu, Memory_t *memory, byte_t address)
{
    pushWord(cpu, memory, cpu->PC);
    cpu->PC = address;
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
static int noFunc(CPU_t *cpu, Memory_t *memory)
{
    setError(C80_ERROR_CPU_INVALID_OPCODE);
    return 0;
}

// NOP      -----------------------------------------------------------------------------
static int nop(CPU_t *cpu, Memory_t *memory)
{
    return 4;
}

// HALT     -----------------------------------------------------------------------------
static int halt(CPU_t *cpu, Memory_t *memory)
{
    return 4;
}


// ADD      -----------------------------------------------------------------------------
static int add_hl_bc_imm(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int add_hl_de_imm(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int add_hl_hl_imm(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int add_hl_sp_imm(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}

static int add_a_n(CPU_t *cpu, Memory_t *memory)
{
    byte_t val = fetchByte(memory, cpu->PC);
    addToRegister(cpu, &cpu->A, val);
    cpu->PC++;
    return 7;
}
static int add_a_a(CPU_t *cpu, Memory_t *memory)
{
    addToRegister(cpu, &cpu->A, cpu->A);
    return 4;
}
static int add_a_b(CPU_t *cpu, Memory_t *memory)
{
    addToRegister(cpu, &cpu->A, cpu->B);
    return 4;
}
static int add_a_c(CPU_t *cpu, Memory_t *memory)
{
    addToRegister(cpu, &cpu->A, cpu->C);
    return 4;
}
static int add_a_d(CPU_t *cpu, Memory_t *memory)
{
    addToRegister(cpu, &cpu->A, cpu->D);
    return 4;
}
static int add_a_e(CPU_t *cpu, Memory_t *memory)
{
    addToRegister(cpu, &cpu->A, cpu->E);
    return 4;
}
static int add_a_h(CPU_t *cpu, Memory_t *memory)
{
    addToRegister(cpu, &cpu->A, cpu->H);
    return 4;
}
static int add_a_l(CPU_t *cpu, Memory_t *memory)
{
    addToRegister(cpu, &cpu->A, cpu->L);
    return 4;
}
static int add_a_hl_addr(CPU_t *cpu, Memory_t *memory)
{
    byte_t val = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
    addToRegister(cpu, &cpu->A, val);
    cpu->PC++;
    return 7;
}


// ADC      -----------------------------------------------------------------------------
static int adc_a_n(CPU_t *cpu, Memory_t *memory)
{
    byte_t val = fetchByte(memory, cpu->PC);
    addToRegisterWithCarry(cpu, &cpu->A, val);
    cpu->PC++;
    return 7;
}
static int adc_a_a(CPU_t *cpu, Memory_t *memory)
{
    addToRegisterWithCarry(cpu, &cpu->A, cpu->A);
    return 4;
}
static int adc_a_b(CPU_t *cpu, Memory_t *memory)
{
    addToRegisterWithCarry(cpu, &cpu->A, cpu->A);
    return 4;
}
static int adc_a_c(CPU_t *cpu, Memory_t *memory)
{
    addToRegisterWithCarry(cpu, &cpu->A, cpu->A);
    return 4;
}
static int adc_a_d(CPU_t *cpu, Memory_t *memory)
{
    addToRegisterWithCarry(cpu, &cpu->A, cpu->A);
    return 4;
}
static int adc_a_e(CPU_t *cpu, Memory_t *memory)
{
    addToRegisterWithCarry(cpu, &cpu->A, cpu->A);
    return 4;
}
static int adc_a_h(CPU_t *cpu, Memory_t *memory)
{
    addToRegisterWithCarry(cpu, &cpu->A, cpu->A);
    return 4;
}
static int adc_a_l(CPU_t *cpu, Memory_t *memory)
{
    addToRegisterWithCarry(cpu, &cpu->A, cpu->A);
    return 4;
}
static int adc_a_hl_addr(CPU_t *cpu, Memory_t *memory)
{
    byte_t val = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
    addToRegisterWithCarry(cpu, &cpu->A, val);
    cpu->PC++;
    return 7;
}


static int adc_hl_bc(CPU_t *cpu, Memory_t *memory)
{
    addToRegisterPairWithCarry(cpu, TO_WORD(cpu->H, cpu->L), TO_WORD(cpu->B, cpu->C));
    return 11;
}
static int adc_hl_de(CPU_t *cpu, Memory_t *memory)
{
    addToRegisterPairWithCarry(cpu, TO_WORD(cpu->H, cpu->L), TO_WORD(cpu->D, cpu->E));
    return 11;
}
static int adc_hl_hl(CPU_t *cpu, Memory_t *memory)
{
    addToRegisterPairWithCarry(cpu, TO_WORD(cpu->H, cpu->L), TO_WORD(cpu->H, cpu->L));
    return 11;
}
static int adc_hl_sp(CPU_t *cpu, Memory_t *memory)
{
    addToRegisterPairWithCarry(cpu, TO_WORD(cpu->H, cpu->L), cpu->SP);
    return 11;
}

// INC      -----------------------------------------------------------------------------
static int inc_bc(CPU_t *cpu, Memory_t *memory)
{
    incrementRegisterPair(cpu, &cpu->B, &cpu->C);
    return 6;
}
static int inc_de(CPU_t *cpu, Memory_t *memory)
{
    incrementRegisterPair(cpu, &cpu->D, &cpu->E);
    return 6;
}
static int inc_hl(CPU_t *cpu, Memory_t *memory)
{
    incrementRegisterPair(cpu, &cpu->H, &cpu->L);
    return 6;
}
static int inc_sp(CPU_t *cpu, Memory_t *memory)
{
    byte_t upperByte = UPPER_BYTE(cpu->SP);
    byte_t lowerByte = LOWER_BYTE(cpu->SP);

    incrementRegisterPair(cpu, &upperByte, &lowerByte);

    cpu->SP = TO_WORD(upperByte, lowerByte);
    return 6;
}

static int inc_a(CPU_t *cpu, Memory_t *memory)
{
    incrementRegister(cpu, &cpu->A);
    return 4;
}
static int inc_b(CPU_t *cpu, Memory_t *memory)
{
    incrementRegister(cpu, &cpu->B);
    return 4;
}
static int inc_c(CPU_t *cpu, Memory_t *memory)
{
    incrementRegister(cpu, &cpu->C);
    return 4;
}
static int inc_d(CPU_t *cpu, Memory_t *memory)
{
    incrementRegister(cpu, &cpu->D);
    return 4;
}
static int inc_e(CPU_t *cpu, Memory_t *memory)
{
    incrementRegister(cpu, &cpu->E);
    return 4;
}
static int inc_h(CPU_t *cpu, Memory_t *memory)
{
    incrementRegister(cpu, &cpu->H);
    return 4;
}
static int inc_l(CPU_t *cpu, Memory_t *memory)
{
    incrementRegister(cpu, &cpu->L);
    return 4;
}
static int inc_hl_addr(CPU_t *cpu, Memory_t *memory)
{
    byte_t val = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
    cpu->PC++;
    incrementRegister(cpu, &val);
    storeByte(memory, TO_WORD(cpu->H, cpu->L), val);
    cpu->PC++;
    return 11;
}

// SUB      -----------------------------------------------------------------------------
static int sub_n(CPU_t *cpu, Memory_t *memory)
{
    byte_t val = fetchByte(memory, cpu->PC);
    subtractFromRegister(cpu, val);
    cpu->PC++;
    return 7;
}
static int sub_a(CPU_t *cpu, Memory_t *memory)
{
    subtractFromRegister(cpu, cpu->A);
    return 4;
}
static int sub_b(CPU_t *cpu, Memory_t *memory)
{
    subtractFromRegister(cpu, cpu->B);
    return 4;
}
static int sub_c(CPU_t *cpu, Memory_t *memory)
{
    subtractFromRegister(cpu, cpu->C);
    return 4;
}
static int sub_d(CPU_t *cpu, Memory_t *memory)
{
    subtractFromRegister(cpu, cpu->D);
    return 4;
}
static int sub_e(CPU_t *cpu, Memory_t *memory)
{
    subtractFromRegister(cpu, cpu->E);
    return 4;
}
static int sub_h(CPU_t *cpu, Memory_t *memory)
{
    subtractFromRegister(cpu, cpu->H);
    return 4;
}
static int sub_l(CPU_t *cpu, Memory_t *memory)
{
    subtractFromRegister(cpu, cpu->L);
    return 4;
}
static int sub_hl_addr(CPU_t *cpu, Memory_t *memory)
{
    byte_t val = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
    subtractFromRegister(cpu, val);
    cpu->PC++;
    return 7;
}

// SBC      -----------------------------------------------------------------------------
static int sbc_a_n(CPU_t *cpu, Memory_t *memory)
{
    byte_t val = fetchByte(memory, cpu->PC);
    subtractFromRegisterWithCarry(cpu, val);
    cpu->PC++;
    return 7;
}
static int sbc_a_a(CPU_t *cpu, Memory_t *memory)
{
    subtractFromRegisterWithCarry(cpu, cpu->A);
    return 4;
}
static int sbc_b(CPU_t *cpu, Memory_t *memory)
{
    subtractFromRegisterWithCarry(cpu, cpu->B);
    return 4;
}
static int sbc_c(CPU_t *cpu, Memory_t *memory)
{
    subtractFromRegisterWithCarry(cpu, cpu->C);
    return 4;
}
static int sbc_d(CPU_t *cpu, Memory_t *memory)
{
    subtractFromRegisterWithCarry(cpu, cpu->D);
    return 4;
}
static int sbc_e(CPU_t *cpu, Memory_t *memory)
{
    subtractFromRegisterWithCarry(cpu, cpu->E);
    return 4;
}
static int sbc_h(CPU_t *cpu, Memory_t *memory)
{
    subtractFromRegisterWithCarry(cpu, cpu->H);
    return 4;
}
static int sbc_l(CPU_t *cpu, Memory_t *memory)
{
    subtractFromRegisterWithCarry(cpu, cpu->L);
    return 4;
}
static int sbc_hl_addr(CPU_t *cpu, Memory_t *memory)
{
    byte_t val = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
    subtractFromRegisterWithCarry(cpu, val);
    cpu->PC++;
    return 7;
}

static int sbc_hl_bc(CPU_t *cpu, Memory_t *memory)
{
    subtractFromRegisterPairWithCarry(cpu, TO_WORD(cpu->H, cpu->L), TO_WORD(cpu->B, cpu->C));
    return 11;
}
static int sbc_hl_de(CPU_t *cpu, Memory_t *memory)
{
    subtractFromRegisterPairWithCarry(cpu, TO_WORD(cpu->H, cpu->L), TO_WORD(cpu->D, cpu->E));
    return 11;
}
static int sbc_hl_hl(CPU_t *cpu, Memory_t *memory)
{
    subtractFromRegisterPairWithCarry(cpu, TO_WORD(cpu->H, cpu->L), TO_WORD(cpu->H, cpu->L));
    return 11;
}
static int sbc_hl_sp(CPU_t *cpu, Memory_t *memory)
{
    subtractFromRegisterPairWithCarry(cpu, TO_WORD(cpu->H, cpu->L), cpu->SP);
    return 11;
}


// SBC      -----------------------------------------------------------------------------
static int dec_bc(CPU_t *cpu, Memory_t *memory)
{
    decrementRegisterPair(cpu, &cpu->B, &cpu->C);
    return 6;
}
static int dec_de(CPU_t *cpu, Memory_t *memory)
{
    decrementRegisterPair(cpu, &cpu->D, &cpu->E);
    return 6;
}
static int dec_hl(CPU_t *cpu, Memory_t *memory)
{
    decrementRegisterPair(cpu, &cpu->D, &cpu->E);
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
    decrementRegister(cpu, &cpu->A);
    return 4;
}
static int dec_b(CPU_t *cpu, Memory_t *memory)
{
    decrementRegister(cpu, &cpu->B);
    return 4;
}
static int dec_c(CPU_t *cpu, Memory_t *memory)
{
    decrementRegister(cpu, &cpu->C);
    return 4;
}
static int dec_d(CPU_t *cpu, Memory_t *memory)
{
    decrementRegister(cpu, &cpu->D);
    return 4;
}
static int dec_e(CPU_t *cpu, Memory_t *memory)
{
    decrementRegister(cpu, &cpu->E);
    return 4;
}
static int dec_h(CPU_t *cpu, Memory_t *memory)
{
    decrementRegister(cpu, &cpu->H);
    return 4;
}
static int dec_l(CPU_t *cpu, Memory_t *memory)
{
    decrementRegister(cpu, &cpu->L);
    return 4;
}
static int dec_hl_addr(CPU_t *cpu, Memory_t *memory)
{
    byte_t val = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
    cpu->PC++;
    decrementRegister(cpu, &val);
    storeByte(memory, TO_WORD(cpu->H, cpu->L), val);
    cpu->PC++;
    return 11;
}

// AND      -----------------------------------------------------------------------------
static int and_n(CPU_t *cpu, Memory_t *memory)
{
    byte_t val = fetchByte(memory, cpu->PC);
    andWithRegister(cpu, val);
    cpu->PC++;
    return 7;
}
static int and_a(CPU_t *cpu, Memory_t *memory)
{
    andWithRegister(cpu, cpu->A);
    return 4;
}
static int and_b(CPU_t *cpu, Memory_t *memory)
{
    andWithRegister(cpu, cpu->B);
    return 4;
}
static int and_c(CPU_t *cpu, Memory_t *memory)
{
    andWithRegister(cpu, cpu->C);
    return 4;
}
static int and_d(CPU_t *cpu, Memory_t *memory)
{
    andWithRegister(cpu, cpu->D);
    return 4;
}
static int and_e(CPU_t *cpu, Memory_t *memory)
{
    andWithRegister(cpu, cpu->E);
    return 4;
}
static int and_h(CPU_t *cpu, Memory_t *memory)
{
    andWithRegister(cpu, cpu->H);
    return 4;
}
static int and_l(CPU_t *cpu, Memory_t *memory)
{
    andWithRegister(cpu, cpu->L);
    return 4;
}
static int and_hl_addr(CPU_t *cpu, Memory_t *memory)
{
    byte_t val = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
    andWithRegister(cpu, val);
    cpu->PC++;
    return 7;
}

// OR       -----------------------------------------------------------------------------
static int or_n(CPU_t *cpu, Memory_t *memory)
{
    byte_t val = fetchByte(memory, cpu->PC);
    orWithRegister(cpu, val);
    cpu->PC++;
    return 7;
}
static int or_a(CPU_t *cpu, Memory_t *memory)
{
    orWithRegister(cpu, cpu->A);
    return 4;
}
static int or_b(CPU_t *cpu, Memory_t *memory)
{
    orWithRegister(cpu, cpu->B);
    return 4;
}
static int or_c(CPU_t *cpu, Memory_t *memory)
{
    orWithRegister(cpu, cpu->C);
    return 4;
}
static int or_d(CPU_t *cpu, Memory_t *memory)
{
    orWithRegister(cpu, cpu->D);
    return 4;
}
static int or_e(CPU_t *cpu, Memory_t *memory)
{
    orWithRegister(cpu, cpu->E);
    return 4;
}
static int or_h(CPU_t *cpu, Memory_t *memory)
{
    orWithRegister(cpu, cpu->H);
    return 4;
}
static int or_l(CPU_t *cpu, Memory_t *memory)
{
    orWithRegister(cpu, cpu->L);
    return 4;
}
static int or_hl_addr(CPU_t *cpu, Memory_t *memory)
{
    byte_t val = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
    andWithRegister(cpu, val);
    cpu->PC++;
    return 7;
}

// XOR      -----------------------------------------------------------------------------
static int xor_n(CPU_t *cpu, Memory_t *memory)
{
    byte_t val = fetchByte(memory, cpu->PC);
    xorWithRegister(cpu, val);
    cpu->PC++;
    return 7;
}
static int xor_a(CPU_t *cpu, Memory_t *memory)
{
    xorWithRegister(cpu, cpu->A);
    return 4;
}
static int xor_b(CPU_t *cpu, Memory_t *memory)
{
    xorWithRegister(cpu, cpu->B);
    return 4;
}
static int xor_c(CPU_t *cpu, Memory_t *memory)
{
    xorWithRegister(cpu, cpu->C);
    return 4;
}
static int xor_d(CPU_t *cpu, Memory_t *memory)
{
    xorWithRegister(cpu, cpu->D);
    return 4;
}
static int xor_e(CPU_t *cpu, Memory_t *memory)
{
    xorWithRegister(cpu, cpu->E);
    return 4;
}
static int xor_h(CPU_t *cpu, Memory_t *memory)
{
    xorWithRegister(cpu, cpu->H);
    return 4;
}
static int xor_l(CPU_t *cpu, Memory_t *memory)
{
    xorWithRegister(cpu, cpu->L);
    return 4;
}
static int xor_hl_addr(CPU_t *cpu, Memory_t *memory)
{
    byte_t val = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
    xorWithRegister(cpu, val);
    cpu->PC++;
    return 7;
}

// CP       -----------------------------------------------------------------------------
static int cp_n(CPU_t *cpu, Memory_t *memory)
{
    byte_t val = fetchByte(memory, cpu->PC);
    cpWithRegister(cpu, val);
    cpu->PC++;
    return 7;
}
static int cp_a(CPU_t *cpu, Memory_t *memory)
{
    cpWithRegister(cpu, cpu->A);
    return 4;
}
static int cp_b(CPU_t *cpu, Memory_t *memory)
{
    cpWithRegister(cpu, cpu->A);
    return 4;
}
static int cp_c(CPU_t *cpu, Memory_t *memory)
{
    cpWithRegister(cpu, cpu->A);
    return 4;
}
static int cp_d(CPU_t *cpu, Memory_t *memory)
{
    cpWithRegister(cpu, cpu->A);
    return 4;
}
static int cp_e(CPU_t *cpu, Memory_t *memory)
{
    cpWithRegister(cpu, cpu->A);
    return 4;
}
static int cp_h(CPU_t *cpu, Memory_t *memory)
{
    cpWithRegister(cpu, cpu->A);
    return 4;
}
static int cp_l(CPU_t *cpu, Memory_t *memory)
{
    cpWithRegister(cpu, cpu->A);
    return 4;
}
static int cp_hl_addr(CPU_t *cpu, Memory_t *memory)
{
    byte_t val = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
    cpWithRegister(cpu, val);
    cpu->PC++;
    return 7;
}
static int cpi(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int cpir(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}

// PUSH     -----------------------------------------------------------------------------
static int push_bc(CPU_t *cpu, Memory_t *memory)
{
    pushWord(cpu, memory, TO_WORD(cpu->B, cpu->C));
    return 11;
}
static int push_de(CPU_t *cpu, Memory_t *memory)
{
    pushWord(cpu, memory, TO_WORD(cpu->D, cpu->E));
    return 11;
}
static int push_hl(CPU_t *cpu, Memory_t *memory)
{
    pushWord(cpu, memory, TO_WORD(cpu->H, cpu->L));
    return 11;
}
static int push_af(CPU_t *cpu, Memory_t *memory)
{
    pushWord(cpu, memory, TO_WORD(cpu->A, flagsToByte(cpu->F)));
    return 11;
}

// POP      -----------------------------------------------------------------------------
static int pop_bc(CPU_t *cpu, Memory_t *memory)
{
    popWord(cpu, memory, &cpu->B, &cpu->C);
    return 10;
}
static int pop_de(CPU_t *cpu, Memory_t *memory)
{
    popWord(cpu, memory, &cpu->D, &cpu->E);
    return 10;
}
static int pop_hl(CPU_t *cpu, Memory_t *memory)
{
    popWord(cpu, memory, &cpu->H, &cpu->L);
    return 10;
}
static int pop_af(CPU_t *cpu, Memory_t *memory)
{
    byte_t f = flagsToByte(cpu->F);
    popWord(cpu, memory, &cpu->A, &f);
    byteToFlags(&cpu->F, f);
    return 10;
}

// CALL     -----------------------------------------------------------------------------
static int call_nz_nn(CPU_t *cpu, Memory_t *memory)
{
    bool condition = cpu->F.N == 0;
    int cycles = callHelper(cpu, memory, condition);

    return cycles;
}
static int call_z_nn(CPU_t *cpu, Memory_t *memory)
{
    bool condition = cpu->F.Z == 1;
    int cycles = callHelper(cpu, memory, condition);

    return cycles;
}
static int call_nn(CPU_t *cpu, Memory_t *memory)
{
    int cycles = callHelper(cpu, memory, true);

    return cycles;
}
static int call_nc_nn(CPU_t *cpu, Memory_t *memory)
{
    bool condition = cpu->F.C == 0;
    int cycles = callHelper(cpu, memory, condition);

    return cycles;
}
static int call_c_nn(CPU_t *cpu, Memory_t *memory)
{
    bool condition = cpu->F.C == 1;
    int cycles = callHelper(cpu, memory, condition);

    return cycles;
}
static int call_po_nn(CPU_t *cpu, Memory_t *memory)
{
    bool condition = cpu->F.P == 0;
    int cycles = callHelper(cpu, memory, condition);

    return cycles;
}
static int call_pe_nn(CPU_t *cpu, Memory_t *memory)
{
    bool condition = cpu->F.P == 1;
    int cycles = callHelper(cpu, memory, condition);

    return cycles;
}
static int call_p_nn(CPU_t *cpu, Memory_t *memory)
{
    bool condition = cpu->F.S == 0;
    int cycles = callHelper(cpu, memory, condition);

    return cycles;
}
static int call_m_nn(CPU_t *cpu, Memory_t *memory)
{
    bool condition = cpu->F.S == 0;
    int cycles = callHelper(cpu, memory, condition);

    return cycles;
}

// RET      -----------------------------------------------------------------------------
static int ret_nz(CPU_t *cpu, Memory_t *memory)
{
    bool condition = cpu->F.Z == 0;
    int cycles = returnHelper(cpu, memory, condition);

    return cycles;
}
static int ret_z(CPU_t *cpu, Memory_t *memory)
{
    bool condition = cpu->F.Z == 1;
    int cycles = returnHelper(cpu, memory, condition);

    return cycles;
}
static int ret_nc(CPU_t *cpu, Memory_t *memory)
{
    bool condition = cpu->F.C == 0;
    int cycles = returnHelper(cpu, memory, condition);

    return cycles;
}
static int ret_c(CPU_t *cpu, Memory_t *memory)
{
    bool condition = cpu->F.C == 1;
    int cycles = returnHelper(cpu, memory, condition);

    return cycles;
}
static int ret_po(CPU_t *cpu, Memory_t *memory)
{
    bool condition = cpu->F.P == 0;
    int cycles = returnHelper(cpu, memory, condition);

    return cycles;
}
static int ret_pe(CPU_t *cpu, Memory_t *memory)
{
    bool condition = cpu->F.P == 1;
    int cycles = returnHelper(cpu, memory, condition);

    return cycles;
}
static int ret_p(CPU_t *cpu, Memory_t *memory)
{
    bool condition = cpu->F.S == 0;
    int cycles = returnHelper(cpu, memory, condition);

    return cycles;
}
static int ret_m(CPU_t *cpu, Memory_t *memory)
{
    bool condition = cpu->F.S == 1;
    int cycles = returnHelper(cpu, memory, condition);

    return cycles;
}
static int ret(CPU_t *cpu, Memory_t *memory)
{
    byte_t lowerByte, upperByte;
    popWord(cpu, memory, &upperByte, &lowerByte);
    cpu->PC = TO_WORD(upperByte, lowerByte);

    return 10;
}
static int retn(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int reti(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}

// ROTATE   -----------------------------------------------------------------------------
static int rlca(CPU_t *cpu, Memory_t *memory)
{
    byte_t bit7 = (cpu->A & 0b10000000) >> 7;
    cpu->A = (cpu->A << 1) | bit7;
    cpu->F.C = bit7;
    cpu->F.H = 0;
    cpu->F.N = 0;
    return 4;
}
static int rrca(CPU_t *cpu, Memory_t *memory)
{
    byte_t bit0 = (cpu->A & 0b00000001);
    cpu->A = (bit0 << 7) | (cpu->A >> 1); 
    cpu->F.C = bit0;
    cpu->F.H = 0;
    cpu->F.N = 0;
    return 4;
}
static int rla(CPU_t *cpu, Memory_t *memory)
{
    byte_t bit7 = (cpu->A & 0b10000000) >> 7;
    cpu->A = (cpu->A << 1) | cpu->F.C;
    cpu->F.C = bit7;
    cpu->F.H = 0;
    cpu->F.N = 0;
    return 4;
}
static int rra(CPU_t *cpu, Memory_t *memory)
{
    byte_t bit0 = (cpu->A & 0b00000001);
    cpu->A = (cpu->F.C << 7) | (cpu->A >> 1);
    cpu->F.C = bit0;
    cpu->F.H = 0;
    cpu->F.N = 0;
    return 4;
}

// RST      -----------------------------------------------------------------------------
static int rst_00h(CPU_t *cpu, Memory_t *memory)
{
    pushWord(cpu, memory, cpu->PC);
    cpu->PC = 0x00;
    return 11;
}
static int rst_08h(CPU_t *cpu, Memory_t *memory)
{
    pushWord(cpu, memory, cpu->PC);
    cpu->PC = 0x08;
    return 11;
}
static int rst_10h(CPU_t *cpu, Memory_t *memory)
{
    pushWord(cpu, memory, cpu->PC);
    cpu->PC = 0x10;
    return 11;
}
static int rst_18h(CPU_t *cpu, Memory_t *memory)
{
    pushWord(cpu, memory, cpu->PC);
    cpu->PC = 0x18;
    return 11;
}
static int rst_20h(CPU_t *cpu, Memory_t *memory)
{
    pushWord(cpu, memory, cpu->PC);
    cpu->PC = 0x20;
    return 11;
}
static int rst_28h(CPU_t *cpu, Memory_t *memory)
{
    pushWord(cpu, memory, cpu->PC);
    cpu->PC = 0x28;
    return 11;
}
static int rst_30h(CPU_t *cpu, Memory_t *memory)
{
    pushWord(cpu, memory, cpu->PC);
    cpu->PC = 0x30;
    return 11;
}
static int rst_38h(CPU_t *cpu, Memory_t *memory)
{
    pushWord(cpu, memory, cpu->PC);
    cpu->PC = 0x38;
    return 11;
}

// JUMP     -----------------------------------------------------------------------------
static int djnz_d(CPU_t *cpu, Memory_t *memory)
{
    int cycles = 8;
    cpu->B--;
    bool condition = cpu->B != 0;

    jumpRelativeHelper(cpu, memory, condition);

    if(condition)
    {
        cycles = 13;
    }

    return cycles;
}
static int jr_d(CPU_t *cpu, Memory_t *memory)
{
    int cycles = jumpRelativeHelper(cpu, memory, true);
    return cycles;
}
static int jr_nz_d(CPU_t *cpu, Memory_t *memory)
{
    bool condition = cpu->F.Z == 0;
    int cycles = jumpRelativeHelper(cpu, memory, condition);

    return cycles;
}
static int jr_z_b(CPU_t *cpu, Memory_t *memory)
{
    bool condition = cpu->F.Z == 1;
    int cycles = jumpRelativeHelper(cpu, memory, condition);

    return cycles;
}
static int jr_nc_d(CPU_t *cpu, Memory_t *memory)
{
    bool condition = cpu->F.C == 0;
    int cycles = jumpRelativeHelper(cpu, memory, condition);

    return cycles;
}
static int jr_c_b(CPU_t *cpu, Memory_t *memory)
{
    bool condition = cpu->F.C == 1;
    int cycles = jumpRelativeHelper(cpu, memory, condition);

    return cycles;
}
static int jp_nz_nn(CPU_t *cpu, Memory_t *memory)
{
    bool condition = cpu->F.Z == 0;
    int cycles = jumpHelper(cpu, memory, condition);

    return cycles;
}
static int jp_nn(CPU_t *cpu, Memory_t *memory)
{
    int cycles = jumpHelper(cpu, memory, true);

    return cycles;
}
static int jp_z_nn(CPU_t *cpu, Memory_t *memory)
{
    bool condition = cpu->F.Z == 1;
    int cycles = jumpHelper(cpu, memory, condition);

    return cycles;
}
static int jp_nc_nn(CPU_t *cpu, Memory_t *memory)
{
    bool condition = cpu->F.C == 0;
    int cycles = jumpHelper(cpu, memory, condition);

    return cycles;
}
static int jp_c_nn(CPU_t *cpu, Memory_t *memory)
{
    bool condition = cpu->F.C == 1;
    int cycles = jumpHelper(cpu, memory, condition);

    return cycles;
}
static int jp_po_nn(CPU_t *cpu, Memory_t *memory)
{
    bool condition = cpu->F.P == 0;
    int cycles = jumpHelper(cpu, memory, condition);

    return cycles;
}
static int jp_hl_addr(CPU_t *cpu, Memory_t *memory)
{
    cpu->PC = TO_WORD(cpu->H, cpu->L);
    return 4;
}
static int jp_p_nn(CPU_t *cpu, Memory_t *memory)
{
    bool condition = cpu->F.S == 0;
    int cycles = jumpHelper(cpu, memory, condition);

    return cycles;
}
static int jp_m_nn(CPU_t *cpu, Memory_t *memory)
{
    bool condition = cpu->F.S == 1;
    int cycles = jumpHelper(cpu, memory, condition);

    return cycles;
}

// EXCHANGE -----------------------------------------------------------------------------
static int exx(CPU_t *cpu, Memory_t *memory)
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
static int ex_af_af_(CPU_t *cpu, Memory_t *memory)
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
static int ex_sp_hl_addr(CPU_t *cpu, Memory_t *memory)
{
    word_t address = cpu->SP;

    byte_t lowerByte = fetchByte(memory, address);
    byte_t upperByte = fetchByte(memory, address + 1);

    storeByte(memory, address, cpu->L);
    storeByte(memory, address + 1, cpu->H);

    cpu->H = upperByte;
    cpu->L = lowerByte;

    return 19;
}
static int ex_de_hl(CPU_t *cpu, Memory_t *memory)
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
static int ld_a_n(CPU_t *cpu, Memory_t *memory)
{
    cpu->A = fetchByte(memory, cpu->PC);
    cpu->PC++;
    return 7;
}
static int ld_a_a(CPU_t *cpu, Memory_t *memory)
{
    cpu->A = cpu->A;
    return 4;
}
static int ld_a_b(CPU_t *cpu, Memory_t *memory)
{
    cpu->A = cpu->B;
    return 4;
}
static int ld_a_c(CPU_t *cpu, Memory_t *memory)
{
    cpu->A = cpu->C;
    return 4;
}
static int ld_a_d(CPU_t *cpu, Memory_t *memory)
{
    cpu->A = cpu->D;
    return 4;
}
static int ld_a_e(CPU_t *cpu, Memory_t *memory)
{
    cpu->A = cpu->E;
    return 4;
}
static int ld_a_h(CPU_t *cpu, Memory_t *memory)
{
    cpu->A = cpu->H;
    return 4;
}
static int ld_a_l(CPU_t *cpu, Memory_t *memory)
{
    cpu->A = cpu->L;
    return 4;
}
static int ld_a_hl_addr(CPU_t *cpu, Memory_t *memory)
{
    byte_t val = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
    cpu->A = val;
    cpu->PC++;
    return 7;
}

static int ld_b_n(CPU_t *cpu, Memory_t *memory)
{
    cpu->B = fetchByte(memory, cpu->PC);
    cpu->PC++;
    return 7;
}
static int ld_b_a(CPU_t *cpu, Memory_t *memory)
{
    cpu->B = cpu->A;
    return 4;
}
static int ld_b_b(CPU_t *cpu, Memory_t *memory)
{
    cpu->B = cpu->B;
    return 4;
}
static int ld_b_c(CPU_t *cpu, Memory_t *memory)
{
    cpu->B = cpu->C;
    return 4;
}
static int ld_b_d(CPU_t *cpu, Memory_t *memory)
{
    cpu->B = cpu->D;
    return 4;
}
static int ld_b_e(CPU_t *cpu, Memory_t *memory)
{
    cpu->B = cpu->E;
    return 4;
}
static int ld_b_h(CPU_t *cpu, Memory_t *memory)
{
    cpu->B = cpu->H;
    return 4;
}
static int ld_b_l(CPU_t *cpu, Memory_t *memory)
{
    cpu->B = cpu->L;
    return 4;
}
static int ld_b_hl_addr(CPU_t *cpu, Memory_t *memory)
{
    byte_t val = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
    cpu->B = val;
    cpu->PC++;
    return 7;
}

static int ld_c_n(CPU_t *cpu, Memory_t *memory)
{
    cpu->C = fetchByte(memory, cpu->PC);
    cpu->PC++;
    return 7;
}
static int ld_c_a(CPU_t *cpu, Memory_t *memory)
{
    cpu->C = cpu->A;
    return 4;
}
static int ld_c_b(CPU_t *cpu, Memory_t *memory)
{
    cpu->C = cpu->B;
    return 4;
}
static int ld_c_c(CPU_t *cpu, Memory_t *memory)
{
    cpu->C = cpu->C;
    return 4;
}
static int ld_c_d(CPU_t *cpu, Memory_t *memory)
{
    cpu->C = cpu->D;
    return 4;
}
static int ld_c_e(CPU_t *cpu, Memory_t *memory)
{
    cpu->C = cpu->E;
    return 4;
}
static int ld_c_h(CPU_t *cpu, Memory_t *memory)
{
    cpu->C = cpu->H;
    return 4;
}
static int ld_c_l(CPU_t *cpu, Memory_t *memory)
{
    cpu->C = cpu->L;
    return 4;
}
static int ld_c_hl_addr(CPU_t *cpu, Memory_t *memory)
{
    byte_t val = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
    cpu->C = val;
    cpu->PC++;
    return 7;
}

static int ld_d_n(CPU_t *cpu, Memory_t *memory)
{
    cpu->D = fetchByte(memory, cpu->PC);
    cpu->PC++;
    return 7;
}
static int ld_d_a(CPU_t *cpu, Memory_t *memory)
{
    cpu->D = cpu->A;
    return 4;
}
static int ld_d_b(CPU_t *cpu, Memory_t *memory)
{
    cpu->D = cpu->B;
    return 4;
}
static int ld_d_c(CPU_t *cpu, Memory_t *memory)
{
    cpu->D = cpu->C;
    return 4;
}
static int ld_d_d(CPU_t *cpu, Memory_t *memory)
{
    cpu->D = cpu->D;
    return 4;
}
static int ld_d_e(CPU_t *cpu, Memory_t *memory)
{
    cpu->D = cpu->E;
    return 4;
}
static int ld_d_h(CPU_t *cpu, Memory_t *memory)
{
    cpu->D = cpu->H;
    return 4;
}
static int ld_d_l(CPU_t *cpu, Memory_t *memory)
{
    cpu->D = cpu->L;
    return 4;
}
static int ld_d_hl_addr(CPU_t *cpu, Memory_t *memory)
{
    byte_t val = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
    cpu->D = val;
    cpu->PC++;
    return 7;
}

static int ld_e_n(CPU_t *cpu, Memory_t *memory)
{
    cpu->E = fetchByte(memory, cpu->PC);
    cpu->PC++;
    return 7;
}
static int ld_e_a(CPU_t *cpu, Memory_t *memory)
{
    cpu->E = cpu->A;
    return 4;
}
static int ld_e_b(CPU_t *cpu, Memory_t *memory)
{
    cpu->E = cpu->B;
    return 4;
}
static int ld_e_c(CPU_t *cpu, Memory_t *memory)
{
    cpu->E = cpu->C;
    return 4;
}
static int ld_e_d(CPU_t *cpu, Memory_t *memory)
{
    cpu->E = cpu->D;
    return 4;
}
static int ld_e_e(CPU_t *cpu, Memory_t *memory)
{
    cpu->E = cpu->E;
    return 4;
}
static int ld_e_h(CPU_t *cpu, Memory_t *memory)
{
    cpu->E = cpu->H;
    return 4;
}
static int ld_e_l(CPU_t *cpu, Memory_t *memory)
{
    cpu->E = cpu->L;
    return 4;
}
static int ld_e_hl_addr(CPU_t *cpu, Memory_t *memory)
{
    byte_t val = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
    cpu->E = val;
    cpu->PC++;
    return 7;
}

static int ld_h_n(CPU_t *cpu, Memory_t *memory)
{
    cpu->H = fetchByte(memory, cpu->PC);
    cpu->PC++;
    return 7;
}
static int ld_h_a(CPU_t *cpu, Memory_t *memory)
{
    cpu->H = cpu->A;
    return 4;
}
static int ld_h_b(CPU_t *cpu, Memory_t *memory)
{
    cpu->H = cpu->B;
    return 4;
}
static int ld_h_c(CPU_t *cpu, Memory_t *memory)
{
    cpu->H = cpu->C;
    return 4;
}
static int ld_h_d(CPU_t *cpu, Memory_t *memory)
{
    cpu->H = cpu->D;
    return 4;
}
static int ld_h_e(CPU_t *cpu, Memory_t *memory)
{
    cpu->H = cpu->E;
    return 4;
}
static int ld_h_h(CPU_t *cpu, Memory_t *memory)
{
    cpu->H = cpu->H;
    return 4;
}
static int ld_h_l(CPU_t *cpu, Memory_t *memory)
{
    cpu->H = cpu->L;
    return 4;
}
static int ld_h_hl_addr(CPU_t *cpu, Memory_t *memory)
{
    byte_t val = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
    cpu->H = val;
    cpu->PC++;
    return 7;
}

static int ld_l_n(CPU_t *cpu, Memory_t *memory)
{
    cpu->L = fetchByte(memory, cpu->PC);
    cpu->PC++;
    return 7;
}
static int ld_l_a(CPU_t *cpu, Memory_t *memory)
{
    cpu->L = cpu->A;
    return 4;
}
static int ld_l_b(CPU_t *cpu, Memory_t *memory)
{
    cpu->L = cpu->B;
    return 4;
}
static int ld_l_c(CPU_t *cpu, Memory_t *memory)
{
    cpu->L = cpu->C;
    return 4;
}
static int ld_l_d(CPU_t *cpu, Memory_t *memory)
{
    cpu->L = cpu->D;
    return 4;
}
static int ld_l_e(CPU_t *cpu, Memory_t *memory)
{
    cpu->L = cpu->E;
    return 4;
}
static int ld_l_h(CPU_t *cpu, Memory_t *memory)
{
    cpu->L = cpu->H;
    return 4;
}
static int ld_l_l(CPU_t *cpu, Memory_t *memory)
{
    cpu->L = cpu->L;
    return 4;
}
static int ld_l_hl_addr(CPU_t *cpu, Memory_t *memory)
{
    byte_t val = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
    cpu->L = val;
    cpu->PC++;
    return 7;
}

static int ld_hl_n_addr(CPU_t *cpu, Memory_t *memory)
{
    byte_t val = fetchByte(memory, cpu->PC);
    storeByte(memory, TO_WORD(cpu->H, cpu->L), val);
    cpu->PC++;
    return 10;
}
static int ld_hl_a_addr(CPU_t *cpu, Memory_t *memory)
{
    storeByte(memory, TO_WORD(cpu->H, cpu->L), cpu->A);
    return 7;
}
static int ld_hl_b_addr(CPU_t *cpu, Memory_t *memory)
{
    storeByte(memory, TO_WORD(cpu->H, cpu->L), cpu->B);
    return 7;
}
static int ld_hl_c_addr(CPU_t *cpu, Memory_t *memory)
{
    storeByte(memory, TO_WORD(cpu->H, cpu->L), cpu->C);
    return 7;
}
static int ld_hl_d_addr(CPU_t *cpu, Memory_t *memory)
{
    storeByte(memory, TO_WORD(cpu->H, cpu->L), cpu->D);
    return 7;
}
static int ld_hl_e_addr(CPU_t *cpu, Memory_t *memory)
{
    storeByte(memory, TO_WORD(cpu->H, cpu->L), cpu->E);
    return 7;
}
static int ld_hl_h_addr(CPU_t *cpu, Memory_t *memory)
{
    storeByte(memory, TO_WORD(cpu->H, cpu->L), cpu->H);
    return 7;
}
static int ld_hl_l_addr(CPU_t *cpu, Memory_t *memory)
{
    storeByte(memory, TO_WORD(cpu->H, cpu->L), cpu->L);
    return 7;
}
static int ld_hl_hl_addr(CPU_t *cpu, Memory_t *memory)
{
    byte_t val = fetchByte(memory, TO_WORD(cpu->H, cpu->L));
    storeByte(memory, TO_WORD(cpu->H, cpu->L), val);
    return 7;
}

static int ld_hl_nn_addr(CPU_t *cpu, Memory_t *memory)
{
    word_t address = fetchWord(memory, cpu->PC);
    
    byte_t lowerByte = fetchByte(memory, address);
    byte_t upperByte = fetchByte(memory, address + 1);

    cpu->H = upperByte;
    cpu->L = lowerByte;

    cpu->PC += 2;

    return 16;
}
static int ld_a_nn_addr(CPU_t *cpu, Memory_t *memory)
{
    word_t address = fetchWord(memory, cpu->PC);
    cpu->A = fetchByte(memory, address);
    cpu->PC += 2;
    return 13;
}

static int ld_nn_hl_addr(CPU_t *cpu, Memory_t *memory)
{
    word_t address = fetchWord(memory, cpu->PC);
    storeByte(memory, address, cpu->L);
    storeByte(memory, address + 1, cpu->H);
    cpu->PC += 2;
    return 16;
}
static int ld_nn_a_addr(CPU_t *cpu, Memory_t *memory)
{
    word_t address = fetchWord(memory, cpu->PC);
    storeByte(memory, address, cpu->A);
    cpu->PC += 2;
    return 13;
}

static int ld_de_nn_imm(CPU_t *cpu, Memory_t *memory)
{
    word_t address = fetchWord(memory, cpu->PC);
    storeByte(memory, address, cpu->E);
    storeByte(memory, address + 1, cpu->D);
    cpu->PC += 2;
    return 16;
}
static int ld_de_a_addr(CPU_t *cpu, Memory_t *memory)
{
    storeByte(memory, TO_WORD(cpu->D, cpu->E), cpu->A);
    return 7;
}
static int ld_a_de_addr(CPU_t *cpu, Memory_t *memory)
{
    cpu->A = fetchByte(memory, TO_WORD(cpu->D, cpu->E));
    return 7;
}

static int ld_bc_nn_imm(CPU_t *cpu, Memory_t *memory)
{
    word_t immediate = fetchWord(memory, cpu->PC);
    cpu->B = UPPER_BYTE(immediate);
    cpu->C = LOWER_BYTE(immediate);
    cpu->PC += 2;

    return 10;

}
static int ld_bc_a_addr(CPU_t *cpu, Memory_t *memory)
{
    storeByte(memory, TO_WORD(cpu->B, cpu->C), cpu->A);
    return 7;
}
static int ld_a_bc_addr(CPU_t *cpu, Memory_t *memory)
{
    cpu->A = fetchByte(memory, TO_WORD(cpu->B, cpu->C));
    return 7;
}

static int ld_hl_nn_imm(CPU_t *cpu, Memory_t *memory)
{
    word_t immediate = fetchWord(memory, cpu->PC);
    cpu->H = UPPER_BYTE(immediate);
    cpu->L = LOWER_BYTE(immediate);
    cpu->PC += 2;

    return 10;
}

static int ld_sp_nn_imm(CPU_t *cpu, Memory_t *memory)
{
    word_t immediate = fetchWord(memory, cpu->PC);
    cpu->SP = immediate;
    cpu->PC += 2;

    return 10;
}

static int ld_sp_hl(CPU_t *cpu, Memory_t *memory)
{
    cpu->SP = TO_WORD(cpu->H, cpu->L);
    return 6;
}

static int ld_bc_nn_addr(CPU_t *cpu, Memory_t *memory)
{
    // TODO:
}
static int ld_de_nn_addr(CPU_t *cpu, Memory_t *memory)
{
    // TODO:
}
static int ld_sp_nn_addr(CPU_t *cpu, Memory_t *memory)
{
    // TODO:
}
static int ld_nn_bc_addr(CPU_t *cpu, Memory_t *memory)
{
    // TODO:
}
static int ld_nn_de_addr(CPU_t *cpu, Memory_t *memory)
{
    // TODO:
}
static int ld_nn_sp_addr(CPU_t *cpu, Memory_t *memory)
{
    // TODO:
}
static int ldi(CPU_t *cpu, Memory_t *memory)
{
    // TODO:
}
static int ldir(CPU_t *cpu, Memory_t *memory)
{
    // TODO:
}
static int ldd(CPU_t *cpu, Memory_t *memory)
{
    // TODO:
}
static int lddr(CPU_t *cpu, Memory_t *memory)
{
    // TODO:
}

static int ld_nn_bc_imm(CPU_t *cpu, Memory_t *memory)
{
    // TODO:
}
static int ld_nn_de_imm(CPU_t *cpu, Memory_t *memory)
{
    // TODO:
}
static int ld_nn_hl_imm(CPU_t *cpu, Memory_t *memory)
{
    // TODO:
}
static int ld_nn_sp_imm(CPU_t *cpu, Memory_t *memory)
{
    // TODO:
}

static int ld_r_a(CPU_t *cpu, Memory_t *memory)
{
    // TODO:
}
static int ld_a_r(CPU_t *cpu, Memory_t *memory)
{
    // TODO:
}

// OTHER INSTRUCTION    -----------------------------------------------------------------------------
static int bit_op(CPU_t *cpu, Memory_t *memory)
{
    return 0;
}
static int ix_op(CPU_t *cpu, Memory_t *memory)
{
    return 0;
}
static int misc_op(CPU_t *cpu, Memory_t *memory)
{
    return 0;
}
static int iy_op(CPU_t *cpu, Memory_t *memory)
{
    return 0;
}

// INTERRUPTS           -----------------------------------------------------------------------------
static int di(CPU_t *cpu, Memory_t *memory)
{
    // TODO: Interrupts
    return 4;
}
static int ei(CPU_t *cpu, Memory_t *memory)
{
    // TODO: Interrupts
    return 4;
}

// PORT     -----------------------------------------------------------------------------
static int in_a_n(CPU_t *cpu, Memory_t *memory)
{
    // TODO: Port
    return 11;
}
static int out_n_a_addr(CPU_t *cpu, Memory_t *memory)
{
    // TODO: Port
    return 11;
}
static int daa(CPU_t *cpu, Memory_t *memory)
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
static int scf(CPU_t *cpu, Memory_t *memory)
{
    cpu->F.C = 1;
    cpu->F.H = 0;
    cpu->F.N = 0;

    return 4;
}
static int in_b_c(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int in_d_c(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int in_e_c(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int in_h_c(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int in_l_c(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int ini(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int inir(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int in_c_c(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int in_a_c(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}

static int in0_c_n(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int in0_e_n(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int in0_l_n(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int in0_a_n(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int out_c_b(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int out_c_d(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int out_c_h(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int out_c_0(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int out_c_c(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int out_c_e(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int out_c_l(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int out_c_a(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int outi(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int otir(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}

static int out0_c_n(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int out0_e_n(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int out0_l_n(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int out0_a_n(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}

static int otim(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int otimr(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int otdm(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int otdmr(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int outd(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int otdr(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}

// TST      -----------------------------------------------------------------------------
static int tst_b(CPU_t *cpu, Memory_t *memory)
{
    // TODO:    
}
static int tst_d(CPU_t *cpu, Memory_t *memory)
{
    // TODO:    
}
static int tst_h(CPU_t *cpu, Memory_t *memory)
{
    // TODO:    
}
static int tst_c(CPU_t *cpu, Memory_t *memory)
{
    // TODO:    
}
static int tst_e(CPU_t *cpu, Memory_t *memory)
{
    // TODO:    
}
static int tst_l(CPU_t *cpu, Memory_t *memory)
{
    // TODO:    
}
static int tst_a(CPU_t *cpu, Memory_t *memory)
{
    // TODO:    
}
static int tst_hl_addr(CPU_t *cpu, Memory_t *memory)
{
    // TODO:    
}
static int tst_n(CPU_t *cpu, Memory_t *memory)
{
    // TODO:    
}
static int tstio_n(CPU_t *cpu, Memory_t *memory)
{
    // TODO:    
}

// MULT     -----------------------------------------------------------------------------
static int mlt_bc(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int mlt_de(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int mlt_hl(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int mlt_sp(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}

// IM       -----------------------------------------------------------------------------
static int im_0(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int im_1(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int im_2(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}

// EXTRA    -----------------------------------------------------------------------------
static int cpl(CPU_t *cpu, Memory_t *memory)
{
    cpu->A = ~cpu->A;
    cpu->F.H = 1;
    cpu->F.N = 1;

    return 4;
}
static int ccf(CPU_t *cpu, Memory_t *memory)
{
    cpu->F.C = !cpu->F.C;
    cpu->F.H = 0;
    cpu->F.N = 0;

    return 4;
}
static int neg(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int slp(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
static int rld(CPU_t *cpu, Memory_t *memory)
{
    // TODO
}
// -----------------------------------------------------------------------------