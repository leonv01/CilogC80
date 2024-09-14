#include <stdio.h>

#include "cpu.h"

int main() {
    CPU_t cpu;
    cpuInit(&cpu);

    printf("%d\n", cpu.A);

    return 0;
}
