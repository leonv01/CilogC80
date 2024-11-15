#include "graphics_interface.h"

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

// Global variables
// -----------------------------------------------------------
// Emulator objects
static CPU_t                *cpu;
static Memory_t             *memory;
// -----------------------------------------------------------


// Forward declarations
// -----------------------------------------------------------

// -----------------------------------------------------------

// Callback functions
// -----------------------------------------------------------

// -----------------------------------------------------------

// Function definitions
// -----------------------------------------------------------
int graphicsInit(int argc, char **argv, CPU_t *cpu, Memory_t *memory)
{
    int status;

    InitWindow(800, 600, "Cilog C80 - Emulator");
    SetTargetFPS(60);

    int showMessageBox = 0;

    while(!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        {
            GuiWindowF
        }
        EndDrawing();
    }

    CloseWindow();

    return status;
}
void graphicsDestroy()
{

}
// -----------------------------------------------------------

// GUI creation functions
// -----------------------------------------------------------

// -----------------------------------------------------------

// Callback functions
// -----------------------------------------------------------

// -----------------------------------------------------------