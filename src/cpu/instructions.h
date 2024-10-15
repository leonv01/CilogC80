#ifndef CILOG_C80_INSTRUCTIONS_H
#define CILOG_C80_INSTRUCTIONS_H

// NOP
#define NOP 0x00

// Add instructions
#define ADD_A_n 0xC6
#define ADD_A_HL 0x86
#define ADD_A_B 0x80
#define ADD_A_C 0x81
#define ADD_A_D 0x82
#define ADD_A_E 0x83
#define ADD_A_H 0x84
#define ADD_A_L 0x85
#define ADD_A_A 0x87

// Subtract instructions
#define SUB_n 0xD6
#define SUB_HL 0x96
#define SUB_B 0x90
#define SUB_C 0x91
#define SUB_D 0x92
#define SUB_E 0x93
#define SUB_H 0x94
#define SUB_L 0x95
#define SUB_A 0x97

// Load instructions
// Immediate
#define LD_A_n 0x3E
#define LD_B_n 0x06
#define LD_C_n 0x0E
#define LD_D_n 0x16
#define LD_E_n 0x1E
#define LD_H_n 0x26
#define LD_L_n 0x2E

#define LD_A_nn 0x3A
#define LD_HL_nn 0x21

#define LD_BC_IMMEDIATE 0x01
#define LD_DE_IMMEDIATE 0x11
#define LD_HL_IMMEDIATE 0x2A
#define LD_SP_IMMEDIATE 0x31

#define BIT_INSTRUCTIONS 0xCB

#define IX_INSTRUCTIONS 0xDD
#define IY_INSTRUCTIONS 0xFD
#define MISC_INSTRUCTIONS 0xED

#define MISC_LD_HL_IMMEDIATE 0x21
#define MISC_LD_DE_IMMEDIATE 0x11
#define MISC_LD_BC_IMMEDIATE 0x01
#define MISC_LD_SP_IMMEDIATE 0x31


#endif //CILOG_C80_INSTRUCTIONS_H