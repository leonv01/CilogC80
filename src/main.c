#include <stdio.h>

#include "cpu/zilogZ80.h"
#include "memory/ram.h"

int main() {
    ZilogZ80_t* zilogZ80 = zilogInit();
    RAM_t* ram = ramInit(0xFFFF);


    if(zilogZ80 != NULL){
        printf("Successful\n");
        zilogZ80->reg->A = 0x20;
        zilogZ80->reg->F.Z = 0x01;

        printf("%d\n", AF(zilogZ80->reg));
    }
    else{
        printf("Not Successful\n");
    }

    return 0;
}
