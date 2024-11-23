#ifndef FILE_GRABBER_H
#define FILE_GRABBER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "utils/utils.h"

bool loadFile(const char *filename, byte_t *buffer, size_t bufferSize);
bool saveFile(const char *filename, byte_t *buffer, size_t bufferSize);

#endif // FILE_GRABBER_H