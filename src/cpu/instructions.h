#ifndef CILOG_C80_INSTRUCTIONS_H
#define CILOG_C80_INSTRUCTIONS_H

// NOP
#define NOP 0x00

// HALT
#define HALT 0x76

// Add instructions
#define ADD_A_n     0xC6
#define ADD_A_HL    0x86
#define ADD_A_B     0x80
#define ADD_A_C     0x81
#define ADD_A_D     0x82
#define ADD_A_E     0x83
#define ADD_A_H     0x84
#define ADD_A_L     0x85
#define ADD_A_A     0x87

// Add (HL) instructions
#define ADD_HL_BC   0x09
#define ADD_HL_DE   0x19
#define ADD_HL_HL   0x29
#define ADD_HL_SP   0x39

// Add with carry instructions
#define ADC_A_n     0xCE
#define ADC_A_HL    0x8E
#define ADC_A_B     0x88
#define ADC_A_C     0x89
#define ADC_A_D     0x8A
#define ADC_A_E     0x8B
#define ADC_A_H     0x8C
#define ADC_A_L     0x8D
#define ADC_A_A     0x8F

// Increment instructions
#define INC_A       0x3C
#define INC_B       0x04
#define INC_C       0x0C
#define INC_D       0x14
#define INC_E       0x1C
#define INC_H       0x24
#define INC_L       0x2C
#define INC_HL      0x34

// Subtract instructions
#define SUB_A_n       0xD6
#define SUB_A_HL      0x96
#define SUB_A_B       0x90
#define SUB_A_C       0x91
#define SUB_A_D       0x92
#define SUB_A_E       0x93
#define SUB_A_H       0x94
#define SUB_A_L       0x95
#define SUB_A_A       0x97

// Subtract with carry instructions
#define SBC_A_n     0xDE
#define SBC_A_HL    0x9E
#define SBC_A_B     0x98
#define SBC_A_C     0x99
#define SBC_A_D     0x9A
#define SBC_A_E     0x9B
#define SBC_A_H     0x9C
#define SBC_A_L     0x9D
#define SBC_A_A     0x9F

// Decrement instructions
#define DEC_A       0x3D
#define DEC_B       0x05
#define DEC_C       0x0D
#define DEC_D       0x15
#define DEC_E       0x1D
#define DEC_H       0x25
#define DEC_L       0x2D
#define DEC_HL      0x35    // TODO: Check this

#define DEC_BC      0x0B
#define DEC_DE      0x1B
#define DEC_HL      0x2B
#define DEC_SP      0x3B

// OR instructions
#define OR_n        0xF6
#define OR_HL       0xB6
#define OR_B        0xB0
#define OR_C        0xB1
#define OR_D        0xB2
#define OR_E        0xB3
#define OR_H        0xB4
#define OR_L        0xB5
#define OR_A        0xB7

// AND instructions
#define AND_n       0xE6
#define AND_HL      0xA6
#define AND_B       0xA0
#define AND_C       0xA1
#define AND_D       0xA2
#define AND_E       0xA3
#define AND_H       0xA4
#define AND_L       0xA5
#define AND_A       0xA7

// XOR instructions
#define XOR_n       0xEE
#define XOR_HL      0xAE
#define XOR_B       0xA8
#define XOR_C       0xA9
#define XOR_D       0xAA
#define XOR_E       0xAB
#define XOR_H       0xAC
#define XOR_L       0xAD
#define XOR_A       0xAF

// CP instructions
#define CP_n        0xFE
#define CP_HL       0xBE
#define CP_B        0xB8
#define CP_C        0xB9
#define CP_D        0xBA
#define CP_E        0xBB
#define CP_H        0xBC
#define CP_L        0xBD
#define CP_A        0xBF

// Load instructions
// Immediate
#define LD_A_n      0x3E
#define LD_B_n      0x06
#define LD_C_n      0x0E
#define LD_D_n      0x16
#define LD_E_n      0x1E
#define LD_H_n      0x26
#define LD_L_n      0x2E

#define LD_B_B      0x40
#define LD_B_C      0x41
#define LD_B_D      0x42
#define LD_B_E      0x43
#define LD_B_H      0x44
#define LD_B_L      0x45
#define LD_B_HL     0x46
#define LD_B_A      0x47

#define LD_C_B      0x48
#define LD_C_C      0x49
#define LD_C_D      0x4A
#define LD_C_E      0x4B
#define LD_C_H      0x4C
#define LD_C_L      0x4D
#define LD_C_HL     0x4E
#define LD_C_A      0x4F

#define LD_D_B      0x50
#define LD_D_C      0x51
#define LD_D_D      0x52
#define LD_D_E      0x53
#define LD_D_H      0x54
#define LD_D_L      0x55
#define LD_D_HL     0x56
#define LD_D_A      0x57

#define LD_E_B      0x58
#define LD_E_C      0x59
#define LD_E_D      0x5A
#define LD_E_E      0x5B
#define LD_E_H      0x5C
#define LD_E_L      0x5D
#define LD_E_HL     0x5E
#define LD_E_A      0x5F

#define LD_H_B      0x60
#define LD_H_C      0x61
#define LD_H_D      0x62
#define LD_H_E      0x63
#define LD_H_H      0x64
#define LD_H_L      0x65
#define LD_H_HL     0x66
#define LD_H_A      0x67

#define LD_L_B      0x68
#define LD_L_C      0x69
#define LD_L_D      0x6A
#define LD_L_E      0x6B
#define LD_L_H      0x6C
#define LD_L_L      0x6D
#define LD_L_HL     0x6E
#define LD_L_A      0x6F

#define LD_H_B      0x60
#define LD_H_C      0x61
#define LD_H_D      0x62
#define LD_H_E      0x63
#define LD_H_H      0x64
#define LD_H_L      0x65
#define LD_H_HL     0x66
#define LD_H_A      0x67

#define LD_A_nn     0x3A
#define LD_HL_nn    0x2A

#define LD_BC_IMMEDIATE 0x01
#define LD_DE_IMMEDIATE 0x11
#define LD_HL_IMMEDIATE 0x21
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