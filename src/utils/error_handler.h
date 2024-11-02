#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <stdbool.h>

typedef enum
{
    C80_ERROR_NONE = 0,

    // Memory errors
    C80_ERROR_MEMORY_INIT_ERROR,
    C80_ERROR_MEMORY_DESTROY_ERROR,
    C80_ERROR_MEMORY_STORE_BYTE_ERROR,
    C80_ERROR_MEMORY_STORE_WORD_ERROR,
    C80_ERROR_MEMORY_FETCH_BYTE_ERROR,
    C80_ERROR_MEMORY_FETCH_WORD_ERROR,

    // ROM errors
    C80_ERROR_ROM_FILE_NOT_FOUND,
    C80_ERROR_ROM_FILE_EMPTY,
    C80_ERROR_ROM_FILE_TOO_LARGE,
    C80_ERROR_ROM_FILE_READ_ERROR,
    C80_ERROR_ROM_FILE_WRITE_ERROR,
    C80_ERROR_ROM_STORE_BYTE_ERROR,
    
    // CPU errors
    C80_ERROR_CPU_INIT_ERROR,
    C80_ERROR_CPU_DESTROY_ERROR,
    C80_ERROR_CPU_INVALID_OPCODE

} C80_Error_t;

void errorStackInit();

void pollError();
void setError(C80_Error_t error);
void clearError(C80_Error_t error);
void clearAllErrors();
bool hasError(C80_Error_t error);
int getErrorCount();

#endif //ERROR_HANDLER_H