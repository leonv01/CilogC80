#include "error_handler.h"

#include <stdio.h>

#include "utils.h"

#define MAX_ERRORS 128

typedef struct
{
    C80_Error_t error;
} Error_t;

typedef struct
{
    Error_t errors[MAX_ERRORS];
    int topIndex;
} ErrorStack_t;

static ErrorStack_t errorStack;
static const char *errorMessages[] = 
{
    "No error",

    // Memory errors
    "Error initializing memory",
    "Error destroying memory",
    "Error storing byte in memory",
    "Error storing word in memory",
    "Error fetching byte from memory",
    "Error fetching word from memory",

    // ROM errors
    "ROM file not found",
    "ROM file is empty",
    "ROM file is too large",
    "Error reading ROM file",
    "Error writing ROM file",
    "Error trying to store byte in ROM"

    // CPU errors
    "Error initializing CPU",
    "Error destroying CPU",
    "Invalid opcode"
};

void errorStackInit()
{
    errorStack.topIndex = -1;
}

void pollError()
{
    if(errorStack.topIndex >= 0)
    {
        printf("Error: %s\n", errorMessages[errorStack.errors[errorStack.topIndex].error]);
    }
}
void setError(C80_Error_t error)
{
    errorStack.topIndex++;
    errorStack.errors[errorStack.topIndex].error = error;

#if defined(DEBUG_MODE) && (DEBUG_MODE == 1)
    pollError();
#endif
}
void clearError(C80_Error_t error)
{
    for(int i = 0; i <= errorStack.topIndex; i++)
    {
        if(errorStack.errors[i].error == error)
        {
            for(int j = i; j < errorStack.topIndex; j++)
            {
                errorStack.errors[j] = errorStack.errors[j + 1];
            }
            errorStack.topIndex--;
            break;
        }
    }
}
void clearAllErrors()
{
    errorStack.topIndex = -1;
}
bool hasError(C80_Error_t error)
{
    for(int i = 0; i <= errorStack.topIndex; i++)
    {
        if(errorStack.errors[i].error == error)
        {
            return true;
        }
    }
    return false;
}
int getErrorCount()
{
    return errorStack.topIndex + 1;
}