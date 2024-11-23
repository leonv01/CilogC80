#include "graphics_interface.h"

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
//#define RAYGUI_CUSTOM_ICONS
//#include "gui_icons.h"
#include "raygui.h"

#undef RAYGUI_IMPLEMENTATION         
#define GUI_WINDOW_FILE_DIALOG_IMPLEMENTATION
#include "gui_components/gui_file_dialog.h"

#define GUI_MENU_BAR_IMPLEMENTATION
#include "gui_components/gui_menu_bar.h"

#define GUI_TOOLTIPTTEXT_IMPLEMENTATION
#include "gui_components/gui_tooltiptext.h"

#define GUI_CPU_VIEW_IMPLEMENTATION
#include "gui_components/gui_cpu_view.h"

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

    int screenWidth = 1280;
    int screenHeight = 720;
    
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_MINIMIZED);  // Set window configuration flags
    InitWindow(screenWidth, screenHeight, "Cilog C80 - Emulator");

    /* -------------------------------- GUI Items ------------------------------- */
    GuiMenuBarState menuBarState = InitGuiMenuBar((Vector2){ 0, 0 }, screenWidth);
    GuiWindowFileDialogState fileDialogState = InitGuiWindowFileDialog(GetWorkingDirectory());
    GuiTooltipTextState tooltipTextState = InitGuiToolTipText("Tooltip text", (Rectangle){ 0, 0, 100, 20 });
    GuiCpuViewState cpuViewState = InitGuiCpuView((Vector2){ screenWidth / 2, screenHeight / 2 }, 350, 400);
    //--------------------------------------------------------------------------------------

    /* -------------------------------- Main loop ------------------------------- */
    while (WindowShouldClose() == false)
    {
        /* -------------------------- Handle window resize -------------------------- */
        if(IsWindowResized())
        {
            screenWidth = GetScreenWidth();
            screenHeight = GetScreenHeight();
            GuiMenuBarResize(&menuBarState, screenWidth);
        }
        /* -------------------------------------------------------------------------- */

        /* ------------------------------ Logic updates ----------------------------- */
        if(menuBarState.openButtonActive)
        {
            fileDialogState.windowActive = !fileDialogState.windowActive;
        }

        if(menuBarState.cpuButtonActive)
        {
            cpuViewState.isWindowActive = !cpuViewState.isWindowActive;
        }

        bool isAnyHovered = false;
        char *toolTipText = GuiMenuBarGetTooltip(&menuBarState, &isAnyHovered);
        if(isAnyHovered == false)
        {
            tooltipTextState.isActive = false;
        }
        else
        {
            tooltipTextState.text = toolTipText;
            tooltipTextState.isActive = true;
        }
        /* -------------------------------------------------------------------------- */

        /* ------------------------------ Begin Drawing ----------------------------- */
        BeginDrawing();
        {
            ClearBackground(GRAY); 

            /* -------------------------------- Draw GUI -------------------------------- */
            GuiMenuBar(&menuBarState);
            GuiCpuView(&cpuViewState);
            GuiWindowFileDialog(&fileDialogState);
            GuiToolTipText(&tooltipTextState);
            /* -------------------------------------------------------------------------- */
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