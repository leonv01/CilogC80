#include "graphics_interface.h"

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#undef RAYGUI_IMPLEMENTATION            // Avoid including raygui implementation again
#define GUI_WINDOW_FILE_DIALOG_IMPLEMENTATION
#include "gui_components/gui_file_dialog.h"

#define GUI_WINDOW_PREFERENCES_IMPLEMENTATION
#include "gui_components/gui_preferences.h"

#define GUI_TOOLTIPTTEXT_IMPLEMENTATION
#include "gui_components/gui_tooltiptext.h"

// Global variables
// -----------------------------------------------------------
// Emulator objects
static CPU_t                *cpu;
static Memory_t             *memory;
// -----------------------------------------------------------


/* -------------------------------------------------------------------------- */
/*                            Forward declarations                            */
/* -------------------------------------------------------------------------- */
enum UiSize
{
    UI_SIZE_SMALL = 0,
    UI_SIZE_MEDIUM,
    UI_SIZE_LARGE
};

/* -------------------------------------------------------------------------- */
/*                             Callback functions                             */
/* -------------------------------------------------------------------------- */


/* -------------------------------------------------------------------------- */
/*                            Function definitions                            */
/* -------------------------------------------------------------------------- */
int graphicsInit(int argc, char **argv, CPU_t *cpu, Memory_t *memory)
{
    
    int status;

    const int screenWidth = 1280;
    const int screenHeight = 720;
    
    SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_VSYNC_HINT);
    InitWindow(screenWidth, screenHeight, "Cilog C80 - Emulator");

    GuiTooltipTextState tooltip = InitGuiToolTipText("Create new file", (Rectangle){ 0, 0, 150, 30 });

    // Variables for file dialog
    Texture texture = { 0 };
    char fileNameToLoad[512] = { 0 };
    GuiWindowFileDialogState fileDialogState = InitGuiWindowFileDialog(GetWorkingDirectory());

    // Variables for the portable window
    Vector2 mousePosition = { 0 };
    Vector2 windowPosition = { 500, 200 };
    Vector2 panOffset = mousePosition;
    bool dragWindow = false;
    bool exitWindow = false;
    
    SetWindowPosition(windowPosition.x, windowPosition.y);
    
    int buttonSize = 30;
    
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!exitWindow && !WindowShouldClose())    // Detect window close button or ESC key
    {
        /* --------------------------------- Update --------------------------------- */
        mousePosition = GetMousePosition();
        
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !dragWindow)
        {
            if (CheckCollisionPointRec(mousePosition, (Rectangle){ 0, 0, screenWidth, 20 }))
            {
                dragWindow = true;
                panOffset = mousePosition;
            }
        }

        if (dragWindow)
        {            
            windowPosition.x += (mousePosition.x - panOffset.x);
            windowPosition.y += (mousePosition.y - panOffset.y);

            SetWindowPosition((int)windowPosition.x, (int)windowPosition.y);
            
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) dragWindow = false;
        }

        if(CheckCollisionPointRec(mousePosition, (Rectangle){ 10, 34, 30, 30 }))
        {
            tooltip.isActive = true;    
        }
        else
        {
            tooltip.isActive = false;
        }


        //----------------------------------------------------------------------------------

        /* ------------------------------ Begin Drawing ----------------------------- */
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            exitWindow = GuiWindowBox((Rectangle){ 0, 0, screenWidth, screenHeight }, "#198# Cilog C80");

            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
            GuiSetStyle(DEFAULT, TEXT_SIZE, 20); // Change font size to 20
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
            const int menuHeightOffset = 24;
            const int menuButtonOffset = 10;
            DrawRectangle(0, menuHeightOffset, GetScreenWidth(), 50, Fade(GRAY, 0.8f));
            GuiGroupBox((Rectangle){ 0, menuHeightOffset, GetScreenWidth(), 50 }, "");
            // New file button
            if(GuiButton((Rectangle){ menuButtonOffset, menuButtonOffset + menuHeightOffset, buttonSize, buttonSize }, GuiIconText(ICON_FILE_NEW, "")))
            {
                fileDialogState.windowActive = true;
            }

            GuiUnlock();

            GuiWindowFileDialog(&fileDialogState);

            GuiToolTipText(&tooltip);
        }
        EndDrawing();
        /* ------------------------------ End Drawing ----------------------------- */
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