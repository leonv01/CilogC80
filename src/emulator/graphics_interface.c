#include "graphics_interface.h"

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#undef RAYGUI_IMPLEMENTATION            // Avoid including raygui implementation again
#define GUI_WINDOW_FILE_DIALOG_IMPLEMENTATION
#include "gui_components/custom_filedialog.h"

// Global variables
// -----------------------------------------------------------
// Emulator objects
static CPU_t                *cpu;
static Memory_t             *memory;
// -----------------------------------------------------------


// Forward declarations
// -----------------------------------------------------------
enum UiSize
{
    UI_SIZE_SMALL = 0,
    UI_SIZE_MEDIUM,
    UI_SIZE_LARGE
};
// -----------------------------------------------------------

// Callback functions
// -----------------------------------------------------------

// -----------------------------------------------------------

// Function definitions
// -----------------------------------------------------------
int graphicsInit(int argc, char **argv, CPU_t *cpu, Memory_t *memory)
{
    
    int status;

    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "Cilog C80 - Emulator");
    
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);

    int showMessageBox = 0;

    GuiWindowFileDialogState fileDialogState = InitGuiWindowFileDialog(GetWorkingDirectory());
    char fileNameToLoad[512] = { 0 };
    Texture texture = { 0 };

    bool exitWindow = false;
    int buttonSize = 30;
    while(exitWindow == false)
    {
        exitWindow = WindowShouldClose();

        if(fileDialogState.SelectFilePressed)
        {
            if(IsFileExtension(fileDialogState.fileNameText, ".bin"))
            {
                strcpy(fileNameToLoad, TextFormat("%s" PATH_SEPERATOR "%s", fileDialogState.dirPathText, fileDialogState.fileNameText));
                UnloadTexture(texture);
                texture = LoadTexture(fileNameToLoad);
            }

            fileDialogState.SelectFilePressed = false;
        }

        /* --------------------------- Begin Draw Routine --------------------------- */
        BeginDrawing();
        {
            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

            DrawTexture(
                texture, 
                GetScreenWidth() / 2 - texture.width / 2,
                GetScreenHeight() / 2 - texture.height / 2 - 5,
                WHITE);

            DrawRectangleLines(
                GetScreenWidth() / 2 - texture.width / 2,
                GetScreenHeight() / 2 - texture.height / 2 - 5,
                texture.width,
                texture.height,
                BLACK);

            DrawText(fileNameToLoad, 208, GetScreenHeight() - 20, 10, GRAY);

            if(fileDialogState.windowActive == true)
            {
                GuiLock();
            }

            /* --------------------------------- Menubar -------------------------------- */
            DrawRectangle(0, 0, GetScreenWidth(), 50, Fade(GRAY, 0.5f));
            if(GuiButton((Rectangle){ 10, 10, buttonSize * 2, buttonSize }, GuiIconText(ICON_FILE_OPEN, "Open")))
            {
                fileDialogState.windowActive = true;
            }
            //DrawLine(0, 50, GetScreenWidth(), 50, BLACK);
            /* -------------------------------------------------------------------------- */

            GuiUnlock();

            GuiWindowFileDialog(&fileDialogState);
        }
        EndDrawing();
        /* ---------------------------- End Draw Routine ---------------------------- */
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