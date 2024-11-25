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

#define GUI_MEMORY_VIEW_IMPLEMENTATION
#include "gui_components/gui_memory_view.h"

#include "file_grabber.h"

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
    GuiMemoryViewState memoryViewState = InitGuiMemoryView((Vector2){ screenWidth / 2, screenHeight / 2 }, 600, 500);   
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

        if(menuBarState.memoryButtonActive)
        {
            memoryViewState.isWindowActive = !memoryViewState.isWindowActive;
        }

        if(fileDialogState.SelectFilePressed == true)
        {
            char selectedFileBuffer[2048];
            createFilePath(fileDialogState.dirPathText, fileDialogState.fileNameText, selectedFileBuffer, sizeof(selectedFileBuffer));

            fileDialogState.windowActive = false;
            bool fileLoaded = loadFile(selectedFileBuffer, memory->data, MEMORY_SIZE);

            if(fileLoaded == true)
            {
                GuiMemoryViewAddressUpdate(&memoryViewState, 0, memory->data, MEMORY_SIZE);
                printf("File loaded\n");
            }

            fileDialogState.SelectFilePressed = false;
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
        if(IsKeyPressed(KEY_SPACE) == true)
        {
            printf("Space key pressed\n");
            cpuStep(cpu, memory);

            /* ----------------------------- CPU view update ---------------------------- */
            GuiCpuViewUpdateRegisters(&cpuViewState, cpu->A, cpu->B, cpu->C, cpu->D, cpu->E, cpu->H, cpu->L);
            GuiCpuViewUpdateFlags(&cpuViewState, cpu->F.C, cpu->F.N, cpu->F.P, cpu->F.H, cpu->F.Z, cpu->F.S);
            GuiCpuViewUpdatePointers(&cpuViewState, cpu->PC, cpu->SP);
            /* -------------------------------------------------------------------------- */

            /* --------------------------- Memory view update --------------------------- */
            GuiMemoryViewAddressUpdate(&memoryViewState, 0, memory->data, MEMORY_SIZE);
            /* -------------------------------------------------------------------------- */
        }
        /* -------------------------------------------------------------------------- */

        /* ------------------------------ Begin Drawing ----------------------------- */
        BeginDrawing();
        {
            ClearBackground(GRAY); 

            /* -------------------------------- Draw GUI -------------------------------- */
            GuiMenuBar(&menuBarState);
            GuiCpuView(&cpuViewState);
            GuiMemoryView(&memoryViewState);
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