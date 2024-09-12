//
// Created by leonv on 11.09.2024.
//

#ifndef CILOGC80_REGISTER_H
#define CILOGC80_REGISTER_H

#include <stdlib.h>

#include "utils.h"

typedef struct Flags_t{
    byte_t C : 1;
    byte_t N : 1;
    byte_t P : 1;
    byte_t H : 1;
    byte_t Z : 1;
    byte_t S : 1;
} Flags_t;

byte_t getFlags(Flags_t* flags);

typedef struct Register_t{
    byte_t A;
    Flags_t F;
    byte_t B;
    byte_t C;
    byte_t D;
    byte_t E;
    byte_t H;
    byte_t L;
} Register_t;

Register_t* registerInit(void);

byte_t AF(Register_t* reg);
byte_t BC(Register_t* reg);
byte_t DE(Register_t* reg);
byte_t HL(Register_t* reg);


#endif //CILOGC80_REGISTER_H
