#ifndef CILOG_C80_INSTRUCTIONS_H
#define CILOG_C80_INSTRUCTIONS_H

// NOP      -----------------------------------------------------------
#define MAIN_NOP                0x00

// HALT     -----------------------------------------------------------
#define MAIN_HALT               0x76

// ADD      -----------------------------------------------------------
#define MAIN_ADD_A_n_IMM        0xC6
#define MAIN_ADD_A_HL_ADDR      0x86
#define MAIN_ADD_A_B            0x80
#define MAIN_ADD_A_C            0x81
#define MAIN_ADD_A_D            0x82
#define MAIN_ADD_A_E            0x83
#define MAIN_ADD_A_H            0x84
#define MAIN_ADD_A_L            0x85
#define MAIN_ADD_A_A            0x87

#define MAIN_ADD_HL_BC_IMM      0x09
#define MAIN_ADD_HL_DE_IMM      0x19
#define MAIN_ADD_HL_HL_IMM      0x29
#define MAIN_ADD_HL_SP_IMM      0x39

// ADC      -----------------------------------------------------------
#define MAIN_ADC_A_n_IMM        0xCE
#define MAIN_ADC_A_HL_ADDR      0x8E
#define MAIN_ADC_A_B            0x88
#define MAIN_ADC_A_C            0x89
#define MAIN_ADC_A_D            0x8A
#define MAIN_ADC_A_E            0x8B
#define MAIN_ADC_A_H            0x8C
#define MAIN_ADC_A_L            0x8D
#define MAIN_ADC_A_A            0x8F

#define MISC_ADC_HL_BC_IMM      0x4A
#define MISC_ADC_HL_DE_IMM      0x5A
#define MISC_ADC_HL_HL_IMM      0x6A
#define MISC_ADC_HL_SP_IMM      0x7A

#define IX_ADC_A_IX_d           0x8E
#define IY_ADC_A_IY_d           0x8E

// INC      -----------------------------------------------------------
#define MAIN_INC_A              0x3C
#define MAIN_INC_B              0x04
#define MAIN_INC_C              0x0C
#define MAIN_INC_D              0x14
#define MAIN_INC_E              0x1C
#define MAIN_INC_H              0x24
#define MAIN_INC_L              0x2C
#define MAIN_INC_HL_ADDR        0x34

#define IX_INC_IX               0x23
#define IX_INC_IX_d             0x34

#define IY_INC_IY               0x23
#define IY_INC_IY_d             0x34

#define MAIN_INC_BC_IMM         0x03
#define MAIN_INC_DE_IMM         0x13
#define MAIN_INC_HL_IMM         0x23
#define MAIN_INC_SP_IMM         0x33

// SUB      -----------------------------------------------------------
#define MAIN_SUB_A_n_IMM        0xD6
#define MAIN_SUB_A_HL_ADDR      0x96
#define MAIN_SUB_A_B            0x90
#define MAIN_SUB_A_C            0x91
#define MAIN_SUB_A_D            0x92
#define MAIN_SUB_A_E            0x93
#define MAIN_SUB_A_H            0x94
#define MAIN_SUB_A_L            0x95
#define MAIN_SUB_A_A            0x97

#define IX_SUB_IX_d             0x96
#define IY_SUB_IY_d             0x96

// SBC      -----------------------------------------------------------
#define MAIN_SBC_A_n_IMM        0xDE
#define MAIN_SBC_A_HL_ADDR      0x9E
#define MAIN_SBC_A_B            0x98
#define MAIN_SBC_A_C            0x99
#define MAIN_SBC_A_D            0x9A
#define MAIN_SBC_A_E            0x9B
#define MAIN_SBC_A_H            0x9C
#define MAIN_SBC_A_L            0x9D
#define MAIN_SBC_A_A            0x9F

#define MISC_SBC_HL_BC_IMM      0x42
#define MISC_SBC_HL_DE_IMM      0x52
#define MISC_SBC_HL_HL_IMM      0x62
#define MISC_SBC_HL_SP_IMM      0x72

#define IX_SBC_A_IX_d           0x9E
#define IY_SBC_A_IY_d           0x9E

// DEC      -----------------------------------------------------------
#define MAIN_DEC_A              0x3D
#define MAIN_DEC_B              0x05
#define MAIN_DEC_C              0x0D
#define MAIN_DEC_D              0x15
#define MAIN_DEC_E              0x1D
#define MAIN_DEC_H              0x25
#define MAIN_DEC_L              0x2D
#define MAIN_DEC_HL_ADDR        0x35

#define MAIN_DEC_BC_IMM         0x0B
#define MAIN_DEC_DE_IMM         0x1B
#define MAIN_DEC_HL_IMM         0x2B
#define MAIN_DEC_SP_IMM         0x3B

#define IX_DEC_IX_d             0x35
#define IX_DEC_IX               0x2B

#define IY_DEC_IY_d             0x35
#define IY_DEC_IY               0x2B

// OR       -----------------------------------------------------------
#define MAIN_OR_n_IMM           0xF6
#define MAIN_OR_HL_ADDR         0xB6
#define MAIN_OR_B               0xB0
#define MAIN_OR_C               0xB1
#define MAIN_OR_D               0xB2
#define MAIN_OR_E               0xB3
#define MAIN_OR_H               0xB4
#define MAIN_OR_L               0xB5
#define MAIN_OR_A               0xB7

#define IX_OR_IX_d              0xB6
#define IY_OR_IY_d              0xB6

// AND      -----------------------------------------------------------
#define MAIN_AND_n_IMM          0xE6
#define MAIN_AND_HL_ADDR        0xA6
#define MAIN_AND_B              0xA0
#define MAIN_AND_C              0xA1
#define MAIN_AND_D              0xA2
#define MAIN_AND_E              0xA3
#define MAIN_AND_H              0xA4
#define MAIN_AND_L              0xA5
#define MAIN_AND_A              0xA7

#define IX_AND_IX_d             0xA6
#define IY_AND_IY_d             0xA6

// XOR      -----------------------------------------------------------
#define MAIN_XOR_n_IMM          0xEE
#define MAIN_XOR_HL_ADDR        0xAE
#define MAIN_XOR_B              0xA8
#define MAIN_XOR_C              0xA9
#define MAIN_XOR_D              0xAA
#define MAIN_XOR_E              0xAB
#define MAIN_XOR_H              0xAC
#define MAIN_XOR_L              0xAD
#define MAIN_XOR_A              0xAF

#define IX_XOR_IX_d             0xB6
#define IY_XOR_IY_d             0xB6

// CP       -----------------------------------------------------------
#define MAIN_CP_n_IMM           0xFE
#define MAIN_CP_HL_ADDR         0xBE
#define MAIN_CP_B               0xB8
#define MAIN_CP_C               0xB9
#define MAIN_CP_D               0xBA
#define MAIN_CP_E               0xBB
#define MAIN_CP_H               0xBC
#define MAIN_CP_L               0xBD
#define MAIN_CP_A               0xBF

#define MISC_CPI                0xA2
#define MISC_CPIR               0xB2
#define MISC_CPD                0xA9
#define MISC_CPDR               0xB9

#define CP_IX_d                 0xBE
#define CP_IY_d                 0xBE

// RET      -----------------------------------------------------------
#define MAIN_RET                0xC9
#define MAIN_RET_NZ             0xC0
#define MAIN_RET_Z              0xC8
#define MAIN_RET_NC             0xD0
#define MAIN_RET_C              0xD8
#define MAIN_RET_PO             0xE0
#define MAIN_RET_PE             0xE8
#define MAIN_RET_P              0xF0
#define MAIN_RET_M              0xF8

#define MISC_RETN               0x45
#define MISC_RETI               0x4D

// CALL     -----------------------------------------------------------
#define MAIN_CALL_nn_IMM        0xCD
#define MAIN_CALL_NZ_nn_IMM     0xC4
#define MAIN_CALL_Z_nn_IMM      0xCC
#define MAIN_CALL_NC_nn_IMM     0xD4
#define MAIN_CALL_C_nn_IMM      0xDC
#define MAIN_CALL_PO_nn_IMM     0xE4
#define MAIN_CALL_PE_nn_IMM     0xEC
#define MAIN_CALL_P_nn_IMM      0xF4
#define MAIN_CALL_M_nn_IMM      0xFC

// PUSH     -----------------------------------------------------------
#define MAIN_PUSH_BC            0xC5
#define MAIN_PUSH_DE            0xD5
#define MAIN_PUSH_HL            0xE5
#define MAIN_PUSH_AF            0xF5

#define IX_PUSH_IX              0xE5
#define IY_PUSH_IY              0xE5

// POP      -----------------------------------------------------------
#define MAIN_POP_BC_IMM         0xC1
#define MAIN_POP_DE_IMM         0xD1
#define MAIN_POP_HL_IMM         0xE1
#define MAIN_POP_AF_IMM         0xF1

// JP     -----------------------------------------------------------
#define MAIN_JP_nn_IMM          0xC3
#define MAIN_JP_NZ_nn_IMM       0xC2
#define MAIN_JP_Z_nn_IMM        0xCA
#define MAIN_JP_NC_nn_IMM       0xD2
#define MAIN_JP_C_nn_IMM        0xDA
#define MAIN_JP_PO_nn_IMM       0xE2
#define MAIN_JP_PE_nn_IMM       0xEA
#define MAIN_JP_P_nn_IMM        0xF2
#define MAIN_JP_M_nn_IMM        0xFA
#define MAIN_JP_HL_IMM          0xE9

#define IX_JP_IX_ADDR           0xE9    // TODO: Check if maybe imm
#define IY_JP_IY_ADDR           0xE9    // TODO: Check if maybe imm

// JR   -----------------------------------------------------------
#define MAIN_JR_d               0x18
#define MAIN_JR_NZ_d            0x20
#define MAIN_JR_Z_d             0x28
#define MAIN_JR_NC_d            0x30
#define MAIN_JR_C_d             0x38
#define MAIN_DJNZ_d             0x10

// RST instructions
#define MAIN_RST_00H            0xC7
#define MAIN_RST_08H            0xCF
#define MAIN_RST_10H            0xD7
#define MAIN_RST_18H            0xDF
#define MAIN_RST_20H            0xE7
#define MAIN_RST_28H            0xEF
#define MAIN_RST_30H            0xF7
#define MAIN_RST_38H            0xFF

// Disable interrupt instructions
#define MAIN_DI          0xF3

// Enable interrupt instructions
#define MAIN_EI          0xFB

// EX instructions
#define MAIN_EX_DE_HL_IMM       0xEB
#define MAIN_EX_AF_AF_ALT       0x08
#define MAIN_EX_SP_HL_ADDR      0xE3
#define MAIN_EXX                0xD9

#define IX_EX_SP_IX_ADDR        0xE3
#define IY_EX_SP_IY_ADDR        0xE3

// Load instructions
// LD A
#define MAIN_LD_A_n      0x3E
#define MAIN_LD_A_A      0x7F
#define MAIN_LD_A_B      0x78
#define MAIN_LD_A_C      0x79
#define MAIN_LD_A_D      0x7A
#define MAIN_LD_A_E      0x7B
#define MAIN_LD_A_H      0x7C
#define MAIN_LD_A_L      0x7D

#define MAIN_LD_A_HL     0x7E
#define MAIN_LD_A_BC     0x0A
#define MAIN_LD_A_DE     0x1A
#define MAIN_LD_A_nn     0x3A

// LD B
#define MAIN_LD_B_n      0x06
#define MAIN_LD_B_A      0x47
#define MAIN_LD_B_B      0x40
#define MAIN_LD_B_C      0x41
#define MAIN_LD_B_D      0x42
#define MAIN_LD_B_E      0x43
#define MAIN_LD_B_H      0x44
#define MAIN_LD_B_L      0x45
#define MAIN_LD_B_HL     0x46

// LD C
#define MAIN_LD_C_n      0x0E
#define MAIN_LD_C_A      0x4F
#define MAIN_LD_C_B      0x48
#define MAIN_LD_C_C      0x49
#define MAIN_LD_C_D      0x4A
#define MAIN_LD_C_E      0x4B
#define MAIN_LD_C_H      0x4C
#define MAIN_LD_C_L      0x4D
#define MAIN_LD_C_HL     0x4E

// LD D
#define MAIN_LD_D_n      0x16
#define MAIN_LD_D_A      0x57
#define MAIN_LD_D_B      0x50
#define MAIN_LD_D_C      0x51
#define MAIN_LD_D_D      0x52
#define MAIN_LD_D_E      0x53
#define MAIN_LD_D_H      0x54
#define MAIN_LD_D_L      0x55
#define MAIN_LD_D_HL     0x56

// LD E
#define MAIN_LD_E_n      0x1E
#define MAIN_LD_E_A      0x5F
#define MAIN_LD_E_B      0x58
#define MAIN_LD_E_C      0x59
#define MAIN_LD_E_D      0x5A
#define MAIN_LD_E_E      0x5B
#define MAIN_LD_E_H      0x5C
#define MAIN_LD_E_L      0x5D
#define MAIN_LD_E_HL     0x5E

// LD H
#define MAIN_LD_H_n      0x26
#define MAIN_LD_H_A      0x67
#define MAIN_LD_H_B      0x60
#define MAIN_LD_H_C      0x61
#define MAIN_LD_H_D      0x62
#define MAIN_LD_H_E      0x63
#define MAIN_LD_H_H      0x64
#define MAIN_LD_H_L      0x65
#define MAIN_LD_H_HL     0x66

// LD L
#define MAIN_LD_L_n      0x2E
#define MAIN_LD_L_A      0x6F
#define MAIN_LD_L_B      0x68
#define MAIN_LD_L_C      0x69
#define MAIN_LD_L_D      0x6A
#define MAIN_LD_L_E      0x6B
#define MAIN_LD_L_H      0x6C
#define MAIN_LD_L_L      0x6D
#define MAIN_LD_L_HL     0x6E

// LD (HL)
#define MAIN_LD_HL_A_ADDR 0x77
#define MAIN_LD_HL_B_ADDR 0x70
#define MAIN_LD_HL_C_ADDR 0x71
#define MAIN_LD_HL_D_ADDR 0x72
#define MAIN_LD_HL_E_ADDR 0x73
#define MAIN_LD_HL_H_ADDR 0x74
#define MAIN_LD_HL_L_ADDR 0x75

#define MAIN_LD_BC_A_ADDR    0x02
#define MAIN_LD_DE_A_ADDR    0x12
#define MAIN_LD_HL_A_ADDR    0x77
#define MAIN_LD_nn_A        0x32
#define MAIN_LD_nn_HL       0x22

// LD Pair instructions
#define MAIN_LD_HL_nn       0x22
#define MAIN_LD_DE_nn       0x11
#define MAIN_LD_BC_nn       0x01
#define MAIN_LD_SP_nn       0x31
#define MAIN_LD_SP_HL       0xF9
#define MAIN_LD_HL_nn_ADDR  0x2A
//#define MAIN_LD_HL_nn       0x21

#define BIT_INSTRUCTIONS 0xCB

#define IX_INSTRUCTIONS 0xDD
#define IY_INSTRUCTIONS 0xFD
#define MISC_INSTRUCTIONS 0xED

#define MISC_LD_HL_IMMEDIATE 0x21
#define MISC_LD_DE_IMMEDIATE 0x11
#define MISC_LD_BC_IMMEDIATE 0x01
#define MISC_LD_SP_IMMEDIATE 0x31

// -----------------------------------------------------------------------------

#endif //CILOG_C80_INSTRUCTIONS_H