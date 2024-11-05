#ifndef FILE_READER_H
#define FILE_READER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mem.h"

int loadFile(Memory_t *memory, const char *filename);
int saveFile(Memory_t *memory, const char *filename);

int loadFileIntoBuffer(char **lines, int *lineCount, const char *filename);
#endif // FILE_READER_H