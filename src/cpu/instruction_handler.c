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

// CPU helper functions -------------------------------------------------------
static byte_t flagsToByte(F_t flags);
static void byteToFlags(F_t *flags, byte_t value);

static int calculateParity(word_t value);
static void setFlags(ZilogZ80_t *cpu, byte_t regA, byte_t operand, word_t result, bool isSubstraction);
static void setFlagsWord(ZilogZ80_t *cpu, word_t reg1, word_t reg2, dword_t result);
// -----------------------------------------------------------------------------

// Helper functions ------------------------------------------------------------
static void addToRegister(ZilogZ80_t *cpu, byte_t *reg, byte_t value);
static void addToRegisterPair(ZilogZ80_t *cpu, word_t value1, word_t value2);
static void addToRegisterWithCarry(ZilogZ80_t *cpu, byte_t *reg, byte_t value);
static void incrementRegister(ZilogZ80_t *cpu, byte_t *reg);
static void incrementRegisterPair(ZilogZ80_t *cpu, byte_t* upperByte, byte_t* lowerByte);

static void subtractFromRegister(ZilogZ80_t *cpu, byte_t value);
static void subtractFromRegisterWithCarry(ZilogZ80_t *cpu, byte_t value);
static void decrementRegister(ZilogZ80_t *cpu, byte_t *reg);
static void decrementRegisterPair(ZilogZ80_t *cpu, byte_t* upperByte, byte_t* lowerByte);

static void andWithRegister(ZilogZ80_t *cpu, byte_t value);
static void orWithRegister(ZilogZ80_t *cpu, byte_t value);
static void xorWithRegister(ZilogZ80_t *cpu, byte_t value);
static void cpWithRegister(ZilogZ80_t *cpu, byte_t value);

static void pushWord(ZilogZ80_t *cpu, word_t value);
static void popWord(ZilogZ80_t *cpu, byte_t *upperByte, byte_t *lowerByte);

static int returnHelper(ZilogZ80_t *cpu, bool condition);
static int callHelper(ZilogZ80_t *cpu, bool condition);
static int jumpHelper(ZilogZ80_t *cpu, bool condition);
static int jumpRelativeHelper(ZilogZ80_t *cpu, bool condition);

static void rst(ZilogZ80_t *cpu, byte_t address);

static void exWord(ZilogZ80_t *cpu, word_t *reg1, word_t *reg2);

static void ld(ZilogZ80_t *cpu, byte_t *reg, byte_t value);
static void ldPair(ZilogZ80_t *cpu, byte_t *upperByte, byte_t *lowerByte, word_t value);
// -----------------------------------------------------------------------------

// NO Func  -----------------------------------------------------------------------------
static int noFunc(ZilogZ80_t *cpu);

// NOP      -----------------------------------------------------------------------------
static int nop(ZilogZ80_t *cpu);

// HALT     -----------------------------------------------------------------------------
static int halt(ZilogZ80_t *cpu);

// ADD  -----------------------------------------------------------------------------
static int add_hl_bc_imm(ZilogZ80_t *cpu);
static int add_hl_de_imm(ZilogZ80_t *cpu);
static int add_hl_hl_imm(ZilogZ80_t *cpu);
static int add_hl_sp_imm(ZilogZ80_t *cpu);

static int add_a_n(ZilogZ80_t *cpu);
static int add_a_a(ZilogZ80_t *cpu);
static int add_a_b(ZilogZ80_t *cpu);
static int add_a_c(ZilogZ80_t *cpu);
static int add_a_d(ZilogZ80_t *cpu);
static int add_a_e(ZilogZ80_t *cpu);
static int add_a_h(ZilogZ80_t *cpu);
static int add_a_l(ZilogZ80_t *cpu);
static int add_a_hl_addr(ZilogZ80_t *cpu);

// ADC      -----------------------------------------------------------------------------
static int adc_a_n(ZilogZ80_t *cpu);
static int adc_a_a(ZilogZ80_t *cpu);
static int adc_a_b(ZilogZ80_t *cpu);
static int adc_a_c(ZilogZ80_t *cpu);
static int adc_a_d(ZilogZ80_t *cpu);
static int adc_a_e(ZilogZ80_t *cpu);
static int adc_a_h(ZilogZ80_t *cpu);
static int adc_a_l(ZilogZ80_t *cpu);
static int adc_a_hl_addr(ZilogZ80_t *cpu);

static int adc_hl_bc(ZilogZ80_t *cpu);
static int adc_hl_de(ZilogZ80_t *cpu);
static int adc_hl_hl(ZilogZ80_t *cpu);
static int adc_hl_sp(ZilogZ80_t *cpu);

// INC      -----------------------------------------------------------------------------
static int inc_bc(ZilogZ80_t *cpu);
static int inc_de(ZilogZ80_t *cpu);
static int inc_hl(ZilogZ80_t *cpu);
static int inc_sp(ZilogZ80_t *cpu);

static int inc_a(ZilogZ80_t *cpu);
static int inc_b(ZilogZ80_t *cpu);
static int inc_c(ZilogZ80_t *cpu);
static int inc_d(ZilogZ80_t *cpu);
static int inc_e(ZilogZ80_t *cpu);
static int inc_h(ZilogZ80_t *cpu);
static int inc_l(ZilogZ80_t *cpu);
static int inc_hl_addr(ZilogZ80_t *cpu);

// SUB      -----------------------------------------------------------------------------
static int sub_n(ZilogZ80_t *cpu);
static int sub_a(ZilogZ80_t *cpu);
static int sub_b(ZilogZ80_t *cpu);
static int sub_c(ZilogZ80_t *cpu);
static int sub_d(ZilogZ80_t *cpu);
static int sub_e(ZilogZ80_t *cpu);
static int sub_h(ZilogZ80_t *cpu);
static int sub_l(ZilogZ80_t *cpu);
static int sub_hl_addr(ZilogZ80_t *cpu);

// SBC      -----------------------------------------------------------------------------
static int sbc_a_n(ZilogZ80_t *cpu);
static int sbc_a_a(ZilogZ80_t *cpu);
static int sbc_b(ZilogZ80_t *cpu);
static int sbc_c(ZilogZ80_t *cpu);
static int sbc_d(ZilogZ80_t *cpu);
static int sbc_e(ZilogZ80_t *cpu);
static int sbc_h(ZilogZ80_t *cpu);
static int sbc_l(ZilogZ80_t *cpu);
static int sbc_hl_addr(ZilogZ80_t *cpu);

static int sbc_hl_bc(ZilogZ80_t *cpu);
static int sbc_hl_de(ZilogZ80_t *cpu);
static int sbc_hl_hl(ZilogZ80_t *cpu);
static int sbc_hl_sp(ZilogZ80_t *cpu);

// DEC      -----------------------------------------------------------------------------
static int dec_bc(ZilogZ80_t *cpu);
static int dec_de(ZilogZ80_t *cpu);
static int dec_hl(ZilogZ80_t *cpu);
static int dec_sp(ZilogZ80_t *cpu);

static int dec_a(ZilogZ80_t *cpu);
static int dec_b(ZilogZ80_t *cpu);
static int dec_c(ZilogZ80_t *cpu);
static int dec_d(ZilogZ80_t *cpu);
static int dec_e(ZilogZ80_t *cpu);
static int dec_h(ZilogZ80_t *cpu);
static int dec_l(ZilogZ80_t *cpu);
static int dec_hl_addr(ZilogZ80_t *cpu);

// AND      -----------------------------------------------------------------------------
static int and_n(ZilogZ80_t *cpu);
static int and_a(ZilogZ80_t *cpu);
static int and_b(ZilogZ80_t *cpu);
static int and_c(ZilogZ80_t *cpu);
static int and_d(ZilogZ80_t *cpu);
static int and_e(ZilogZ80_t *cpu);
static int and_h(ZilogZ80_t *cpu);
static int and_l(ZilogZ80_t *cpu);
static int and_hl_addr(ZilogZ80_t *cpu);

// OR       -----------------------------------------------------------------------------
static int or_n(ZilogZ80_t *cpu);
static int or_a(ZilogZ80_t *cpu);
static int or_b(ZilogZ80_t *cpu);
static int or_c(ZilogZ80_t *cpu);
static int or_d(ZilogZ80_t *cpu);
static int or_e(ZilogZ80_t *cpu);
static int or_h(ZilogZ80_t *cpu);
static int or_l(ZilogZ80_t *cpu);
static int or_hl_addr(ZilogZ80_t *cpu);

// XOR      -----------------------------------------------------------------------------
static int xor_n(ZilogZ80_t *cpu);
static int xor_a(ZilogZ80_t *cpu);
static int xor_b(ZilogZ80_t *cpu);
static int xor_c(ZilogZ80_t *cpu);
static int xor_d(ZilogZ80_t *cpu);
static int xor_e(ZilogZ80_t *cpu);
static int xor_h(ZilogZ80_t *cpu);
static int xor_l(ZilogZ80_t *cpu);
static int xor_hl_addr(ZilogZ80_t *cpu);

// CP       -----------------------------------------------------------------------------
static int cp_n(ZilogZ80_t *cpu);
static int cp_a(ZilogZ80_t *cpu);
static int cp_b(ZilogZ80_t *cpu);
static int cp_c(ZilogZ80_t *cpu);
static int cp_d(ZilogZ80_t *cpu);
static int cp_e(ZilogZ80_t *cpu);
static int cp_h(ZilogZ80_t *cpu);
static int cp_l(ZilogZ80_t *cpu);
static int cp_hl_addr(ZilogZ80_t *cpu);
static int cpi(ZilogZ80_t *cpu);
static int cpir(ZilogZ80_t *cpu);

// PUSH     -----------------------------------------------------------------------------
static int push_bc(ZilogZ80_t *cpu);
static int push_de(ZilogZ80_t *cpu);
static int push_hl(ZilogZ80_t *cpu);
static int push_af(ZilogZ80_t *cpu);

// POP      -----------------------------------------------------------------------------
static int pop_bc(ZilogZ80_t *cpu);
static int pop_de(ZilogZ80_t *cpu);
static int pop_hl(ZilogZ80_t *cpu);
static int pop_af(ZilogZ80_t *cpu);

// CALL     -----------------------------------------------------------------------------
static int call_nz_nn(ZilogZ80_t *cpu);
static int call_z_nn(ZilogZ80_t *cpu);
static int call_nn(ZilogZ80_t *cpu);
static int call_nc_nn(ZilogZ80_t *cpu);
static int call_c_nn(ZilogZ80_t *cpu);
static int call_po_nn(ZilogZ80_t *cpu);
static int call_pe_nn(ZilogZ80_t *cpu);
static int call_p_nn(ZilogZ80_t *cpu);
static int call_m_nn(ZilogZ80_t *cpu);

// RET      -----------------------------------------------------------------------------
static int ret_nz(ZilogZ80_t *cpu);
static int ret_z(ZilogZ80_t *cpu);
static int ret_nc(ZilogZ80_t *cpu);
static int ret_c(ZilogZ80_t *cpu);
static int ret_po(ZilogZ80_t *cpu);
static int ret_pe(ZilogZ80_t *cpu);
static int ret_p(ZilogZ80_t *cpu);
static int ret_m(ZilogZ80_t *cpu);
static int ret(ZilogZ80_t *cpu);
static int retn(ZilogZ80_t *cpu);
static int reti(ZilogZ80_t *cpu);

// ROTATE   -----------------------------------------------------------------------------
static int rlca(ZilogZ80_t *cpu);
static int rrca(ZilogZ80_t *cpu);
static int rla(ZilogZ80_t *cpu);
static int rra(ZilogZ80_t *cpu);

// RST      -----------------------------------------------------------------------------
static int rst_00h(ZilogZ80_t *cpu);
static int rst_08h(ZilogZ80_t *cpu);
static int rst_10h(ZilogZ80_t *cpu);
static int rst_18h(ZilogZ80_t *cpu);
static int rst_20h(ZilogZ80_t *cpu);
static int rst_28h(ZilogZ80_t *cpu);
static int rst_30h(ZilogZ80_t *cpu);
static int rst_38h(ZilogZ80_t *cpu);

// JUMP     -----------------------------------------------------------------------------
static int djnz_d(ZilogZ80_t *cpu);
static int jr_d(ZilogZ80_t *cpu);
static int jr_nz_d(ZilogZ80_t *cpu);
static int jr_z_b(ZilogZ80_t *cpu);
static int jr_nc_d(ZilogZ80_t *cpu);
static int jr_c_b(ZilogZ80_t *cpu);
static int jp_nz_nn(ZilogZ80_t *cpu);
static int jp_nn(ZilogZ80_t *cpu);
static int jp_z_nn(ZilogZ80_t *cpu);
static int jp_nc_nn(ZilogZ80_t *cpu);
static int jp_c_nn(ZilogZ80_t *cpu);
static int jp_po_nn(ZilogZ80_t *cpu);
static int jp_hl_addr(ZilogZ80_t *cpu);
static int jp_p_nn(ZilogZ80_t *cpu);
static int jp_m_nn(ZilogZ80_t *cpu);

// EXCHANGE -----------------------------------------------------------------------------
static int exx(ZilogZ80_t *cpu);
static int ex_af_af_(ZilogZ80_t *cpu);
static int ex_sp_hl_addr(ZilogZ80_t *cpu);
static int ex_de_hl(ZilogZ80_t *cpu);

// LD       -----------------------------------------------------------------------------
static int ld_a_n(ZilogZ80_t *cpu);
static int ld_a_a(ZilogZ80_t *cpu);
static int ld_a_b(ZilogZ80_t *cpu);
static int ld_a_c(ZilogZ80_t *cpu);
static int ld_a_d(ZilogZ80_t *cpu);
static int ld_a_e(ZilogZ80_t *cpu);
static int ld_a_h(ZilogZ80_t *cpu);
static int ld_a_l(ZilogZ80_t *cpu);
static int ld_a_hl_addr(ZilogZ80_t *cpu);

static int ld_b_n(ZilogZ80_t *cpu);
static int ld_b_a(ZilogZ80_t *cpu);
static int ld_b_b(ZilogZ80_t *cpu);
static int ld_b_c(ZilogZ80_t *cpu);
static int ld_b_d(ZilogZ80_t *cpu);
static int ld_b_e(ZilogZ80_t *cpu);
static int ld_b_h(ZilogZ80_t *cpu);
static int ld_b_l(ZilogZ80_t *cpu);
static int ld_b_hl_addr(ZilogZ80_t *cpu);

static int ld_c_n(ZilogZ80_t *cpu);
static int ld_c_a(ZilogZ80_t *cpu);
static int ld_c_b(ZilogZ80_t *cpu);
static int ld_c_c(ZilogZ80_t *cpu);
static int ld_c_d(ZilogZ80_t *cpu);
static int ld_c_e(ZilogZ80_t *cpu);
static int ld_c_h(ZilogZ80_t *cpu);
static int ld_c_l(ZilogZ80_t *cpu);
static int ld_c_hl_addr(ZilogZ80_t *cpu);

static int ld_d_n(ZilogZ80_t *cpu);
static int ld_d_a(ZilogZ80_t *cpu);
static int ld_d_b(ZilogZ80_t *cpu);
static int ld_d_c(ZilogZ80_t *cpu);
static int ld_d_d(ZilogZ80_t *cpu);
static int ld_d_e(ZilogZ80_t *cpu);
static int ld_d_h(ZilogZ80_t *cpu);
static int ld_d_l(ZilogZ80_t *cpu);
static int ld_d_hl_addr(ZilogZ80_t *cpu);

static int ld_e_n(ZilogZ80_t *cpu);
static int ld_e_a(ZilogZ80_t *cpu);
static int ld_e_b(ZilogZ80_t *cpu);
static int ld_e_c(ZilogZ80_t *cpu);
static int ld_e_d(ZilogZ80_t *cpu);
static int ld_e_e(ZilogZ80_t *cpu);
static int ld_e_h(ZilogZ80_t *cpu);
static int ld_e_l(ZilogZ80_t *cpu);
static int ld_e_hl_addr(ZilogZ80_t *cpu);

static int ld_h_n(ZilogZ80_t *cpu);
static int ld_h_a(ZilogZ80_t *cpu);
static int ld_h_b(ZilogZ80_t *cpu);
static int ld_h_c(ZilogZ80_t *cpu);
static int ld_h_d(ZilogZ80_t *cpu);
static int ld_h_e(ZilogZ80_t *cpu);
static int ld_h_h(ZilogZ80_t *cpu);
static int ld_h_l(ZilogZ80_t *cpu);
static int ld_h_hl_addr(ZilogZ80_t *cpu);

static int ld_l_n(ZilogZ80_t *cpu);
static int ld_l_a(ZilogZ80_t *cpu);
static int ld_l_b(ZilogZ80_t *cpu);
static int ld_l_c(ZilogZ80_t *cpu);
static int ld_l_d(ZilogZ80_t *cpu);
static int ld_l_e(ZilogZ80_t *cpu);
static int ld_l_h(ZilogZ80_t *cpu);
static int ld_l_l(ZilogZ80_t *cpu);
static int ld_l_hl_addr(ZilogZ80_t *cpu);

static int ld_hl_n_addr(ZilogZ80_t *cpu);
static int ld_hl_a_addr(ZilogZ80_t *cpu);
static int ld_hl_b_addr(ZilogZ80_t *cpu);
static int ld_hl_c_addr(ZilogZ80_t *cpu);
static int ld_hl_d_addr(ZilogZ80_t *cpu);
static int ld_hl_e_addr(ZilogZ80_t *cpu);
static int ld_hl_h_addr(ZilogZ80_t *cpu);
static int ld_hl_l_addr(ZilogZ80_t *cpu);
static int ld_hl_hl_addr(ZilogZ80_t *cpu);

static int ld_hl_nn_addr(ZilogZ80_t *cpu);
static int ld_a_nn_addr(ZilogZ80_t *cpu);
static int ld_bc_nn_addr(ZilogZ80_t *cpu);
static int ld_de_nn_addr(ZilogZ80_t *cpu);
static int ld_sp_nn_addr(ZilogZ80_t *cpu);

static int ld_nn_hl_addr(ZilogZ80_t *cpu);
static int ld_nn_a_addr(ZilogZ80_t *cpu);
static int ld_nn_bc_addr(ZilogZ80_t *cpu);
static int ld_nn_de_addr(ZilogZ80_t *cpu);
static int ld_nn_sp_addr(ZilogZ80_t *cpu);

static int ld_de_nn_imm(ZilogZ80_t *cpu);
static int ld_de_a_addr(ZilogZ80_t *cpu);
static int ld_a_de_addr(ZilogZ80_t *cpu);

static int ld_bc_nn_imm(ZilogZ80_t *cpu);
static int ld_bc_a_addr(ZilogZ80_t *cpu);
static int ld_a_bc_addr(ZilogZ80_t *cpu);

static int ld_hl_nn_imm(ZilogZ80_t *cpu);

static int ld_sp_nn_imm(ZilogZ80_t *cpu);

static int ld_sp_hl(ZilogZ80_t *cpu);

static int ldi(ZilogZ80_t *cpu);
static int ldir(ZilogZ80_t *cpu);
static int ldd(ZilogZ80_t *cpu);
static int lddr(ZilogZ80_t *cpu);

static int ld_nn_bc_imm(ZilogZ80_t *cpu);
static int ld_nn_de_imm(ZilogZ80_t *cpu);
static int ld_nn_hl_imm(ZilogZ80_t *cpu);
static int ld_nn_sp_imm(ZilogZ80_t *cpu);

static int ld_r_a(ZilogZ80_t *cpu);
static int ld_a_r(ZilogZ80_t *cpu);


// OTHER INSTRUCTION    -----------------------------------------------------------------------------
static int bit_op(ZilogZ80_t *cpu);
static int ix_op(ZilogZ80_t *cpu);
static int misc_op(ZilogZ80_t *cpu);
static int iy_op(ZilogZ80_t *cpu);

// INTERRUPTS           -----------------------------------------------------------------------------
static int di(ZilogZ80_t *cpu);
static int ei(ZilogZ80_t *cpu);

// PORT     -----------------------------------------------------------------------------
static int in_a_n(ZilogZ80_t *cpu);
static int in_b_c(ZilogZ80_t *cpu);
static int in_d_c(ZilogZ80_t *cpu);
static int in_e_c(ZilogZ80_t *cpu);
static int in_h_c(ZilogZ80_t *cpu);
static int in_l_c(ZilogZ80_t *cpu);
static int ini(ZilogZ80_t *cpu);
static int inir(ZilogZ80_t *cpu);
static int in_c_c(ZilogZ80_t *cpu);
static int in_a_c(ZilogZ80_t *cpu);

static int in0_c_n(ZilogZ80_t *cpu);
static int in0_e_n(ZilogZ80_t *cpu);
static int in0_l_n(ZilogZ80_t *cpu);
static int in0_a_n(ZilogZ80_t *cpu);

static int out_n_a_addr(ZilogZ80_t *cpu);
static int out_c_b(ZilogZ80_t *cpu);
static int out_c_d(ZilogZ80_t *cpu);
static int out_c_h(ZilogZ80_t *cpu);
static int out_c_0(ZilogZ80_t *cpu);
static int out_c_c(ZilogZ80_t *cpu);
static int out_c_e(ZilogZ80_t *cpu);
static int out_c_l(ZilogZ80_t *cpu);
static int out_c_a(ZilogZ80_t *cpu);
static int outi(ZilogZ80_t *cpu);
static int otir(ZilogZ80_t *cpu);

static int out0_c_n(ZilogZ80_t *cpu);
static int out0_e_n(ZilogZ80_t *cpu);
static int out0_l_n(ZilogZ80_t *cpu);
static int out0_a_n(ZilogZ80_t *cpu);

static int daa(ZilogZ80_t *cpu);
static int scf(ZilogZ80_t *cpu);

static int otim(ZilogZ80_t *cpu);
static int otimr(ZilogZ80_t *cpu);
static int otdm(ZilogZ80_t *cpu);
static int otdmr(ZilogZ80_t *cpu);
static int outd(ZilogZ80_t *cpu);
static int otdr(ZilogZ80_t *cpu);

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

static void exWord(ZilogZ80_t *cpu, word_t *reg1, word_t *reg2)
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
static int noFunc(ZilogZ80_t *cpu)
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
    // TODO
}
static int add_hl_de_imm(ZilogZ80_t *cpu)
{
    // TODO
}
static int add_hl_hl_imm(ZilogZ80_t *cpu)
{
    // TODO
}
static int add_hl_sp_imm(ZilogZ80_t *cpu)
{
    // TODO
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
    decrementRegisterPair(cpu, &cpu->D, &cpu->E);
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
    // TODO
}
static int cpir(ZilogZ80_t *cpu)
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
    storeByteAddressSpace(&cpu->ram, &cpu->rom, address, cpu->L);
    storeByteAddressSpace(&cpu->ram, &cpu->rom, address + 1, cpu->H);
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
    // TODO:
}
static int ld_de_nn_addr(ZilogZ80_t *cpu)
{
    // TODO:
}
static int ld_sp_nn_addr(ZilogZ80_t *cpu)
{
    // TODO:
}
static int ld_nn_bc_addr(ZilogZ80_t *cpu)
{
    // TODO:
}
static int ld_nn_de_addr(ZilogZ80_t *cpu)
{
    // TODO:
}
static int ld_nn_sp_addr(ZilogZ80_t *cpu)
{
    // TODO:
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
    // TODO: Port
    return 11;
}
static int out_n_a_addr(ZilogZ80_t *cpu)
{
    // TODO: Port
    byte_t port = fetchByteAddressSpace(&cpu->ram, &cpu->rom, cpu->PC);
    cpu->outputCallback(port, cpu->A);
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
static int in_b_c(ZilogZ80_t *cpu)
{
    // TODO
}
static int in_d_c(ZilogZ80_t *cpu)
{
    // TODO
}
static int in_e_c(ZilogZ80_t *cpu)
{
    // TODO
}
static int in_h_c(ZilogZ80_t *cpu)
{
    // TODO
}
static int in_l_c(ZilogZ80_t *cpu)
{
    // TODO
}
static int ini(ZilogZ80_t *cpu)
{
    // TODO
}
static int inir(ZilogZ80_t *cpu)
{
    // TODO
}
static int in_c_c(ZilogZ80_t *cpu)
{
    // TODO
}
static int in_a_c(ZilogZ80_t *cpu)
{
    // TODO
}

static int in0_c_n(ZilogZ80_t *cpu)
{
    // TODO
}
static int in0_e_n(ZilogZ80_t *cpu)
{
    // TODO
}
static int in0_l_n(ZilogZ80_t *cpu)
{
    // TODO
}
static int in0_a_n(ZilogZ80_t *cpu)
{
    // TODO
}
static int out_c_b(ZilogZ80_t *cpu)
{
    // TODO
}
static int out_c_d(ZilogZ80_t *cpu)
{
    // TODO
}
static int out_c_h(ZilogZ80_t *cpu)
{
    // TODO
}
static int out_c_0(ZilogZ80_t *cpu)
{
    // TODO
}
static int out_c_c(ZilogZ80_t *cpu)
{
    // TODO
}
static int out_c_e(ZilogZ80_t *cpu)
{
    // TODO
}
static int out_c_l(ZilogZ80_t *cpu)
{
    // TODO
}
static int out_c_a(ZilogZ80_t *cpu)
{
    // TODO
}
static int outi(ZilogZ80_t *cpu)
{
    // TODO
}
static int otir(ZilogZ80_t *cpu)
{
    // TODO
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
    // TODO
}
static int otdr(ZilogZ80_t *cpu)
{
    // TODO
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
// -----------------------------------------------------------------------------