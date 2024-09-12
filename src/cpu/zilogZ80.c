//
// Created by leonv on 11.09.2024.
//

#include "zilogZ80.h"

ZilogZ80_t* zilogInit(void){
    ZilogZ80_t* zilogZ80 = (ZilogZ80_t*) malloc(sizeof(ZilogZ80_t));

    if(zilogZ80 == NULL){
        zilogZ80 = NULL;
    }
    else{
        zilogZ80->reg = registerInit();
        zilogZ80->SP = 0x00;
        zilogZ80->PC = 0x00;
        zilogZ80->I = 0x00;
        zilogZ80->R = 0x00;
        zilogZ80->IX = 0x0000;
        zilogZ80->IY = 0x0000;
    }

    return zilogZ80;
}