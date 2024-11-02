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
        /*

        if(fileSize > memory->size)
        {
            returnCode = -2;
        }
        else
        {
            fread(memory->data, sizeof(uint8_t), fileSize, file);
            fclose(file);
        }
        */
    }
}

int saveFile(Memory_t *memory, const char *filename)
{

}