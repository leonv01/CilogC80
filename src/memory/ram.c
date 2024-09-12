//
// Created by leonv on 11.09.2024.
//

#include "ram.h"

#include <stdlib.h>
#include <string.h>

RAM_t* ramInit(dword_t size){
    RAM_t* ram = (RAM_t*) malloc(sizeof(RAM_t));

    byte_t* buffer = (byte_t*) malloc(sizeof(byte_t) * size);

    if(buffer == NULL){
        buffer = NULL;
    }
    else{
        memset(buffer, 0xFF, size);
        ram->ramMemory = buffer;
    }
    return ram;
}