//
// Created by leonv on 11.09.2024.
//

#include "register.h"

byte_t getFlags(Flags_t* flags){
    byte_t ret;

    if(flags == NULL){
        ret = 0;
    }
    else{
        ret = (
                flags->C | (flags->N << 1) | (flags->P << 2) |
                (0 << 3) | (flags->H << 4) | (0 << 5) |
                (flags->Z << 6) | (flags->S << 7)
                );
    }
    return ret;
}

Register_t* registerInit(void){
    Register_t* reg = (Register_t*) malloc(sizeof(Register_t));

    if(reg == NULL){
        reg = NULL;
    }
    else{
        Flags_t flags = { .C = 0, .N = 0, .P = 0, .H = 0, .Z = 0, .S = 0 };
        reg->A = 0x00;
        reg->F = flags;
        reg->B = 0x00;
        reg->C = 0x00;
        reg->D = 0x00;
        reg->H = 0x00;
        reg->L = 0x00;
    }

    return reg;
}

byte_t AF(Register_t* reg){
    word_t ret;
    if(reg == NULL){
        ret = 0x0000;
    }
    else{
        ret = TO_WORD(reg->A, getFlags(&reg->F));
    }

    return ret;
}

byte_t BC(Register_t* reg){
    word_t ret;
    if(reg == NULL){
        ret = 0x0000;
    }
    else{
        ret = TO_WORD(reg->B, reg->C);
    }

    return ret;
}

byte_t DE(Register_t* reg){
    word_t ret;
    if(reg == NULL){
        ret = 0x0000;
    }
    else{
        ret = TO_WORD(reg->D, reg->E);
    }

    return ret;
}

byte_t HL(Register_t* reg){
    word_t ret;
    if(reg == NULL){
        ret = 0x0000;
    }
    else{
        ret = TO_WORD(reg->H, reg->L);
    }

    return ret;
}