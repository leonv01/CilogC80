#ifndef FILE_PARSER_H
#define FILE_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mem.h"
#include "utils.h"

int loadFile(Memory_t *memory, const char *filename);

#endif // FILE_PARSER_H