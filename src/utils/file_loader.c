#include "file_loader.h"

int loadFile(Memory_t *memory, const char *filename)
{
    FILE *file = fopen(filename, "rb");

    if(file != NULL)
    {
        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        fseek(file, 0, SEEK_SET);

        if(fileSize > MEMORY_SIZE)
        {
            fclose(file);
            return -1;
        }

        fread(memory->data, fileSize, 1, file);
        fclose(file);

        return 0;
    }
    else
    {
        return -1;
    }
}