//
// Created by leonv on 11.09.2024.
//

#ifndef CILOGC80_ZILOGZ80_H
#define CILOGC80_ZILOGZ80_H

#include <stdint.h>

#include "utils.h"
#include "register.h"

typedef struct ZilogZ80_t
{
    Register_t* reg;

    word_t PC;
    word_t SP;

    byte_t I;
    byte_t R;

    word_t IX;
    word_t IY;
} ZilogZ80_t;

ZilogZ80_t* zilogInit(void);

#endif //CILOGC80_ZILOGZ80_H
