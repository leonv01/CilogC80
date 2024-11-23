#include "file_grabber.h"

bool loadFile(const char *filename, byte_t *buffer, size_t bufferSize)
{
    FILE *file = fopen(filename, "rb");
    if (file == NULL)
    {
        return false;
    }

    fread(buffer, sizeof(byte_t), bufferSize, file);
    fclose(file);

    return true;
}

bool saveFile(const char *filename, byte_t *buffer, size_t bufferSize)
{
    return false;
}