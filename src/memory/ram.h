//
// Created by leonv on 11.09.2024.
//

#ifndef CILOGC80_RAM_H
#define CILOGC80_RAM_H

#include "utils.h"

typedef struct RAM_t{
    byte_t* ramMemory;
} RAM_t;

RAM_t* ramInit(dword_t size);

#endif //CILOGC80_RAM_H
