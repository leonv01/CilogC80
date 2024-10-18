#ifndef CILOG_C80_INSTRUCTIONS_H
#define CILOG_C80_INSTRUCTIONS_H

// NOP
#define MAIN_NOP 0x00

// HALT
#define MAIN_HALT 0x76

// Add instructions
#define MAIN_ADD_A_n     0xC6
#define MAIN_ADD_A_HL    0x86
#define MAIN_ADD_A_B     0x80
#define MAIN_ADD_A_C     0x81
#define MAIN_ADD_A_D     0x82
#define MAIN_ADD_A_E     0x83
#define MAIN_ADD_A_H     0x84
#define MAIN_ADD_A_L     0x85
#define MAIN_ADD_A_A     0x87

// Add (HL) instructions
#define MAIN_ADD_HL_BC   0x09
#define MAIN_ADD_HL_DE   0x19
#define MAIN_ADD_HL_HL   0x29
#define MAIN_ADD_HL_SP   0x39

// Add with carry instructions
#define MAIN_ADC_A_n     0xCE
#define MAIN_ADC_A_HL    0x8E
#define MAIN_ADC_A_B     0x88
#define MAIN_ADC_A_C     0x89
#define MAIN_ADC_A_D     0x8A
#define MAIN_ADC_A_E     0x8B
#define MAIN_ADC_A_H     0x8C
#define MAIN_ADC_A_L     0x8D
#define MAIN_ADC_A_A     0x8F

// Increment instructions
#define MAIN_INC_A       0x3C
#define MAIN_INC_B       0x04
#define MAIN_INC_C       0x0C
#define MAIN_INC_D       0x14
#define MAIN_INC_E       0x1C
#define MAIN_INC_H       0x24
#define MAIN_INC_L       0x2C
#define MAIN_INC_HL_ADDR 0x34

// Increment register pair instructions
#define MAIN_INC_BC      0x03
#define MAIN_INC_DE      0x13
#define MAIN_INC_HL      0x23
#define MAIN_INC_SP      0x33

// Subtract instructions
#define MAIN_SUB_A_n       0xD6
#define MAIN_SUB_A_HL      0x96
#define MAIN_SUB_A_B       0x90
#define MAIN_SUB_A_C       0x91
#define MAIN_SUB_A_D       0x92
#define MAIN_SUB_A_E       0x93
#define MAIN_SUB_A_H       0x94
#define MAIN_SUB_A_L       0x95
#define MAIN_SUB_A_A       0x97

// Subtract with carry instructions
#define MAIN_SBC_A_n     0xDE
#define MAIN_SBC_A_HL    0x9E
#define MAIN_SBC_A_B     0x98
#define MAIN_SBC_A_C     0x99
#define MAIN_SBC_A_D     0x9A
#define MAIN_SBC_A_E     0x9B
#define MAIN_SBC_A_H     0x9C
#define MAIN_SBC_A_L     0x9D
#define MAIN_SBC_A_A     0x9F

// Decrement instructions
#define MAIN_DEC_A       0x3D
#define MAIN_DEC_B       0x05
#define MAIN_DEC_C       0x0D
#define MAIN_DEC_D       0x15
#define MAIN_DEC_E       0x1D
#define MAIN_DEC_H       0x25
#define MAIN_DEC_L       0x2D
#define MAIN_DEC_HL_ADDR 0x35

#define MAIN_DEC_BC      0x0B
#define MAIN_DEC_DE      0x1B
#define MAIN_DEC_HL      0x2B
#define MAIN_DEC_SP      0x3B

// OR instructions
#define MAIN_OR_n        0xF6
#define MAIN_OR_HL       0xB6
#define MAIN_OR_B        0xB0
#define MAIN_OR_C        0xB1
#define MAIN_OR_D        0xB2
#define MAIN_OR_E        0xB3
#define MAIN_OR_H        0xB4
#define MAIN_OR_L        0xB5
#define MAIN_OR_A        0xB7

// AND instructions
#define MAIN_AND_n       0xE6
#define MAIN_AND_HL      0xA6
#define MAIN_AND_B       0xA0
#define MAIN_AND_C       0xA1
#define MAIN_AND_D       0xA2
#define MAIN_AND_E       0xA3
#define MAIN_AND_H       0xA4
#define MAIN_AND_L       0xA5
#define MAIN_AND_A       0xA7

// XOR instructions
#define MAIN_XOR_n       0xEE
#define MAIN_XOR_HL      0xAE
#define MAIN_XOR_B       0xA8
#define MAIN_XOR_C       0xA9
#define MAIN_XOR_D       0xAA
#define MAIN_XOR_E       0xAB
#define MAIN_XOR_H       0xAC
#define MAIN_XOR_L       0xAD
#define MAIN_XOR_A       0xAF

// CP instructions
#define MAIN_CP_n        0xFE
#define MAIN_CP_HL       0xBE
#define MAIN_CP_B        0xB8
#define MAIN_CP_C        0xB9
#define MAIN_CP_D        0xBA
#define MAIN_CP_E        0xBB
#define MAIN_CP_H        0xBC
#define MAIN_CP_L        0xBD
#define MAIN_CP_A        0xBF

// Load instructions
// Immediate
#define MAIN_LD_A_n      0x3E
#define MAIN_LD_B_n      0x06
#define MAIN_LD_C_n      0x0E
#define MAIN_LD_D_n      0x16
#define MAIN_LD_E_n      0x1E
#define MAIN_LD_H_n      0x26
#define MAIN_LD_L_n      0x2E

#define MAIN_LD_B_B      0x40
#define MAIN_LD_B_C      0x41
#define MAIN_LD_B_D      0x42
#define MAIN_LD_B_E      0x43
#define MAIN_LD_B_H      0x44
#define MAIN_LD_B_L      0x45
#define MAIN_LD_B_HL     0x46
#define MAIN_LD_B_A      0x47

#define MAIN_LD_C_B      0x48
#define MAIN_LD_C_C      0x49
#define MAIN_LD_C_D      0x4A
#define MAIN_LD_C_E      0x4B
#define MAIN_LD_C_H      0x4C
#define MAIN_LD_C_L      0x4D
#define MAIN_LD_C_HL     0x4E
#define MAIN_LD_C_A      0x4F

#define MAIN_LD_D_B      0x50
#define MAIN_LD_D_C      0x51
#define MAIN_LD_D_D      0x52
#define MAIN_LD_D_E      0x53
#define MAIN_LD_D_H      0x54
#define MAIN_LD_D_L      0x55
#define MAIN_LD_D_HL     0x56
#define MAIN_LD_D_A      0x57

#define MAIN_LD_E_B      0x58
#define MAIN_LD_E_C      0x59
#define MAIN_LD_E_D      0x5A
#define MAIN_LD_E_E      0x5B
#define MAIN_LD_E_H      0x5C
#define MAIN_LD_E_L      0x5D
#define MAIN_LD_E_HL     0x5E
#define MAIN_LD_E_A      0x5F

#define MAIN_LD_H_B      0x60
#define MAIN_LD_H_C      0x61
#define MAIN_LD_H_D      0x62
#define MAIN_LD_H_E      0x63
#define MAIN_LD_H_H      0x64
#define MAIN_LD_H_L      0x65
#define MAIN_LD_H_HL     0x66
#define MAIN_LD_H_A      0x67

#define MAIN_LD_L_B      0x68
#define MAIN_LD_L_C      0x69
#define MAIN_LD_L_D      0x6A
#define MAIN_LD_L_E      0x6B
#define MAIN_LD_L_H      0x6C
#define MAIN_LD_L_L      0x6D
#define MAIN_LD_L_HL     0x6E
#define MAIN_LD_L_A      0x6F

#define MAIN_LD_H_B      0x60
#define MAIN_LD_H_C      0x61
#define MAIN_LD_H_D      0x62
#define MAIN_LD_H_E      0x63
#define MAIN_LD_H_H      0x64
#define MAIN_LD_H_L      0x65
#define MAIN_LD_H_HL     0x66
#define MAIN_LD_H_A      0x67

#define MAIN_LD_A_nn     0x3A
#define MAIN_LD_HL_nn    0x2A

#define MAIN_LD_BC_IMMEDIATE 0x01
#define MAIN_LD_DE_IMMEDIATE 0x11
#define MAIN_LD_HL_IMMEDIATE 0x21
#define MAIN_LD_SP_IMMEDIATE 0x31


#define BIT_INSTRUCTIONS 0xCB

#define IX_INSTRUCTIONS 0xDD
#define IY_INSTRUCTIONS 0xFD
#define MISC_INSTRUCTIONS 0xED

#define MISC_LD_HL_IMMEDIATE 0x21
#define MISC_LD_DE_IMMEDIATE 0x11
#define MISC_LD_BC_IMMEDIATE 0x01
#define MISC_LD_SP_IMMEDIATE 0x31


#endif //CILOG_C80_INSTRUCTIONS_H