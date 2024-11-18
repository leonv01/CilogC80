#include "graphics_interface.h"

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_CUSTOM_ICONS
#include "gui_icons.h"
#include "raygui.h"

#undef RAYGUI_IMPLEMENTATION            // Avoid including raygui implementation again
#define GUI_WINDOW_FILE_DIALOG_IMPLEMENTATION
#include "gui_components/gui_file_dialog.h"

#define GUI_WINDOW_PREFERENCES_IMPLEMENTATION
#include "gui_components/gui_preferences.h"

#define GUI_TOOLTIPTTEXT_IMPLEMENTATION
#include "gui_components/gui_tooltiptext.h"

#define GUI_CPU_VIEW_IMPLEMENTATION
#include "gui_components/gui_cpu_view.h"

#define GUI_MENUBAR_IMPLEMENTATION
#include "gui_components/gui_menubar.h"



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

typedef struct
{
    Vector2 position;
    char *tooltip;
    bool isActive;
    int iconId;
} GuiMenuButton;

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
    GuiMenubarState menubarState = InitGuiMenubar();

    GuiCpuViewState cpuViewState = InitGuiCpuView();

    // Variables for the portable window
    Vector2 mousePosition = { 0 };
    Vector2 windowPosition = { 500, 200 };
    Vector2 panOffset = mousePosition;
    bool dragWindow = false;
    bool exitWindow = false;
    
    SetWindowPosition(windowPosition.x, windowPosition.y);
    
    char ** guiIconsName = GuiLoadIcons("./resources/icons.rgi", true);
    if(guiIconsName == NULL)
    {
        TraceLog(LOG_ERROR, "Failed to load icons");
        return 1;
    }

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
        //----------------------------------------------------------------------------------

        /* ------------------------------ Begin Drawing ----------------------------- */
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            exitWindow = GuiWindowBox((Rectangle){ 0, 0, screenWidth, screenHeight }, "#198# Cilog C80");

            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
            //GuiSetStyle(DEFAULT, TEXT_SIZE, 20); // Change font size to 20
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
            if(menubarState.fileNewFilePressed == true)
            {
                fileDialogState.windowActive = true;
            }
            if(menubarState.fileOpenFilePressed == true)
            {
                fileDialogState.windowActive = true;
            }
            if(menubarState.fileSaveFilePressed == true)
            {
                fileDialogState.windowActive = true;
            }
            if(menubarState.fileCloseFilePressed == true)
            {
                fileDialogState.windowActive = false;
            }
            if(menubarState.viewCpuPressed == true)
            {
                cpuViewState.cpuAndRegisterWindowActive = !cpuViewState.cpuAndRegisterWindowActive;
            }
            /* -------------------------------------------------------------------------- */

            GuiUnlock();

            GuiMenubar(&menubarState);

            GuiCpuView(&cpuViewState);

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