#include "file_reader.h"

int loadFile(Memory_t *memory, const char *filename)
{
    int returnCode = 0;
    FILE *file = fopen(filename, "rb");
    if(file == NULL)
    {
        returnCode = -1;
    }
    else
    {
        fseek(file, 0, SEEK_END);
        long fileSize = ftell(file);
        rewind(file);

        byte_t *buffer = (byte_t*) malloc(fileSize);

        if(buffer == NULL)
        {
            returnCode = -2;
        }
        else
        {
            fread(buffer, 1, fileSize, file);
            fclose(file);
        }

        for(int i = 0; i < fileSize; i++)
        {
            storeByte(memory, i, buffer[i]);
        }

        //loadFileToMemory(memory, buffer, fileSize);
    }
}

int saveFile(Memory_t *memory, const char *filename)
{

}