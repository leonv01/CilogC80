//
// Created by leonv on 11.09.2024.
//

#ifndef CILOGC80_UTILS_H
#define CILOGC80_UTILS_H

#include <stdint.h>

#define UPPER_NIBBLE(x) ((x >> 4) & 0x0F)
#define LOWER_NIBBLE(x) (x & 0x0F)
#define UPPER_BYTE(x) ((x >> 8) & 0xFF)
#define LOWER_BYTE(x) (x & 0xFF)
#define TO_WORD(x, y)(((x << 8) & 0xFF00) | (y & 0x00FF))

#define ERROR_NONE 0x00
#define ERROR_INSTRUCTION 0x01

typedef int8_t bool_t;
#define true 1
#define false 0

typedef uint8_t byte_t;
typedef uint16_t word_t;
typedef uint32_t dword_t;
typedef uint64_t qword_t;

#endif //CILOGC80_UTILS_H
