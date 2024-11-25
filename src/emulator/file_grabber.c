#include "file_grabber.h"

#ifdef _WIN32
#define PATH_SEPERATOR "\\"
#else
#define PATH_SEPERATOR "/"
#endif

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

void createFilePath(const char *dir, const char *fileName, char *buffer, size_t bufferSize)
{
    snprintf(buffer, bufferSize, "%s%s%s", dir, PATH_SEPERATOR, fileName);
}