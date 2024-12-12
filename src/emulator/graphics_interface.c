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

#define GUI_RAM_MEMORY_VIEW_IMPLEMENTATION
#include "gui_components/gui_ram_memory_view.h"

#define GUI_ROM_MEMORY_VIEW_IMPLEMENTATION
#include "gui_components/gui_rom_memory_view.h"

#define GUI_PREFERENCES_IMPLEMENTATION
#include "gui_components/gui_preferences.h"

#define GUI_TOAST_IMPLEMENTATION
#include "gui_components/gui_toast.h"

#define GUI_EMULATOR_INFO_VIEW_IMPLEMENTATION
#include "gui_components/gui_emulator_info_view.h"

#include "file_grabber.h"

/* -------------------------------------------------------------------------- */
/*                                   Defines                                  */
/* -------------------------------------------------------------------------- */
#define FONT_SIZE 10
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/*                              Global variables                              */
/* -------------------------------------------------------------------------- */
static ZilogZ80_t                *cpu;
/* -------------------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/*                             Struct declarations                            */
/* -------------------------------------------------------------------------- */
typedef struct RenderObject
{
    void* state;
    void (*renderFunction)(void* state);
    size_t priority;
} RenderObject;

typedef enum EmulationState
{
    EMULATION_RUNNING,
    EMULATION_STEP,
    EMULATION_PAUSED,
    EMULATION_STOPPED
} EmulationState;
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/*                            Forward declarations                            */
/* -------------------------------------------------------------------------- */
static inline void renderCpuViewCallback(void *state);
static inline void renderRamMemoryViewCallback(void *state);
static inline void renderRomMemoryViewCallback(void *state);
static inline void renderPreferencesCallback(void *state);
static inline void renderMenuBarCallback(void *state);
static inline void renderFileDialogCallback(void *state);
static inline void renderToolTipTextCallback(void *state);
static inline void renderToastCallback(void *state);

static void checkPriority(RenderObject *renderObjects, size_t *renderObjectsPriority, const size_t renderStatesCount);
/* -------------------------------------------------------------------------- */
/*                             Callback functions                             */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/*                            Function definitions                            */
/* -------------------------------------------------------------------------- */
int graphicsInit(int argc, char **argv, ZilogZ80_t *cpu)
{
    int status;

    int screenWidth = 1280;
    int screenHeight = 720;

    bool isProgramLoaded = false;

    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_MINIMIZED);  // Set window configuration flags
    InitWindow(screenWidth, screenHeight, "Cilog C80 - Emulator");
    GuiSetStyle(DEFAULT, TEXT_SIZE, FONT_SIZE);
    
    /* -------------------------------- GUI Items ------------------------------- */
    GuiMenuBarState menuBarState = InitGuiMenuBar((Vector2){ 0, 0 }, screenWidth);
    GuiWindowFileDialogState fileDialogState = InitGuiWindowFileDialog(GetWorkingDirectory());
    GuiTooltipTextState tooltipTextState = InitGuiToolTipText("Tooltip text", (Rectangle){ 0, 0, 100, 20 });
    GuiCpuViewState cpuViewState = InitGuiCpuView();
    GuiRamMemoryViewState ramMemoryViewState = InitGuiRamMemoryView();
    GuiRomMemoryViewState romMemoryViewState = InitGuiRomMemoryView();
    GuiPreferencesState preferencesState = InitGuiPreferences((Vector2){ screenWidth / 2, screenHeight / 2 }, 400, 300);   
    GuiToastState toastState = InitGuiToast();
    //--------------------------------------------------------------------------------------
    RenderObject renderObjects[] = 
    {
        { &cpuViewState, (void (*)(void *))renderCpuViewCallback, 0 },
        { &ramMemoryViewState, (void (*)(void *))renderRamMemoryViewCallback, 1 },
        { &romMemoryViewState, (void (*)(void *))renderRomMemoryViewCallback, 2},
        { &preferencesState, (void (*)(void *))renderPreferencesCallback, 3 },
        { &fileDialogState, (void (*)(void *))renderFileDialogCallback, 4 },
        { &toastState, (void (*)(void *))renderToastCallback, 5 }
    };
    const int renderStatesCount = sizeof(renderObjects) / sizeof(RenderObject);
    size_t renderObjectsPriority[renderStatesCount];
    for (int i = 0; i < renderStatesCount; i++) 
    {
        renderObjectsPriority[i] = i; // Store indices
    }

    EmulationState emulationState = EMULATION_PAUSED;

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
        if(menuBarState.openButtonActive == true)
        {
            fileDialogState.windowActive = !fileDialogState.windowActive;
        }

        if(menuBarState.cpuButtonActive == true)
        {
            cpuViewState.isWindowActive = !cpuViewState.isWindowActive;
        }

        if(menuBarState.ramMemoryButtonActive == true)
        {
            ramMemoryViewState.isWindowActive = !ramMemoryViewState.isWindowActive;
        }
        if(menuBarState.romMemoryButtonActive == true)
        {
            romMemoryViewState.isWindowActive = !romMemoryViewState.isWindowActive;
        }

        if(fileDialogState.SelectFilePressed == true)
        {
            char selectedFileBuffer[2048];
            createFilePath(fileDialogState.dirPathText, fileDialogState.fileNameText, selectedFileBuffer, sizeof(selectedFileBuffer));

            fileDialogState.windowActive = false;
            byte_t data[cpu->rom.memorySize];

            bool fileLoaded = loadFile(selectedFileBuffer, data, cpu->rom.memorySize);
            loadProgramToRom(&cpu->rom, data, cpu->rom.memorySize);
            if(fileLoaded == true)
            {
                zilogZ80Reset(cpu);
                GuiRamMemoryViewUpdate(&ramMemoryViewState, true);
                GuiRomMemoryViewUpdate(&romMemoryViewState, true);
                GuiToastDisplayMessage(&toastState, "File loaded.", 5000, GUI_TOAST_SUCCESS);

                isProgramLoaded = true;
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

        if(
            menuBarState.stepEmulationButtonActive == true
            || IsKeyPressed(KEY_SPACE) == true
        )
        {
            if(isProgramLoaded == false)
            {
                GuiToastDisplayMessage(&toastState, "No program is loaded into memory.", 10000, GUI_TOAST_WARNING);
            }
            else
            {
                GuiToastDisplayMessage(&toastState, "CPU step.", 2000, GUI_TOAST_MESSAGE);
            }
            zilogZ80Step(cpu); 

            GuiRamMemoryViewUpdate(&ramMemoryViewState, true);
            GuiRomMemoryViewUpdate(&romMemoryViewState, true);
            GuiCpuViewUpdate(&cpuViewState, true);
        }
        if(menuBarState.startEmulationButtonActive == true)
        {
            emulationState = EMULATION_RUNNING;
        }

        if(emulationState == EMULATION_RUNNING)
        {
            if(cpu->isHaltered == true)
            {
                emulationState = EMULATION_STOPPED;
            }
            else
            {
                if(isProgramLoaded == false)
                {
                    GuiToastDisplayMessage(&toastState, "No program is loaded into memory.", 10000, GUI_TOAST_WARNING);
                }
                else
                {
                    GuiToastDisplayMessage(&toastState, "CPU running.", 2000, GUI_TOAST_MESSAGE);
                }
                zilogZ80Step(cpu); 

                GuiRamMemoryViewUpdate(&ramMemoryViewState, true);
                GuiRomMemoryViewUpdate(&romMemoryViewState, true);
                GuiCpuViewUpdate(&cpuViewState, true);
            }
        }

        if(menuBarState.restartEmulationButtonActive == true)
        {
            zilogZ80Reset(cpu);
        }

        /* ----------------------------- CPU view update ---------------------------- */
        GuiCpuViewUpdateRegisters(&cpuViewState, cpu->A, cpu->B, cpu->C, cpu->D, cpu->E, cpu->H, cpu->L);
        GuiCpuViewUpdateFlags(&cpuViewState, cpu->F.C, cpu->F.N, cpu->F.P, cpu->F.H, cpu->F.Z, cpu->F.S);
        GuiCpuViewUpdatePointers(&cpuViewState, cpu->PC, cpu->SP);
        /* -------------------------------------------------------------------------- */

        /* --------------------------- Memory view update --------------------------- */
        GuiRamMemoryViewAddressUpdate(&ramMemoryViewState, cpu->ram.data, cpu->ram.memorySize);
        GuiRomMemoryViewAddressUpdate(&romMemoryViewState, cpu->rom.data, cpu->rom.memorySize);
        /* -------------------------------------------------------------------------- */

        /* ------------------------------ Begin Drawing ----------------------------- */
        BeginDrawing();
        {
            ClearBackground(GRAY); 

            /* -------------------------------- Draw GUI -------------------------------- */
            for(size_t i = 0; i < renderStatesCount; i++)
            {
                size_t priorityIndex = renderObjectsPriority[i];
                renderObjects[priorityIndex].renderFunction(renderObjects[priorityIndex].state);
            }
            GuiMenuBar(&menuBarState);
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

static inline void renderCpuViewCallback(void *state)
{
    GuiCpuView((GuiCpuViewState *)state);
}
static inline void renderRamMemoryViewCallback(void *state)
{
    GuiRamMemoryView((GuiRamMemoryViewState *)state);
}
static inline void renderRomMemoryViewCallback(void *state)
{
    GuiRomMemoryView((GuiRomMemoryViewState *)state);
}
static inline void renderPreferencesCallback(void *state)
{
    GuiPreferences((GuiPreferencesState *)state);
}
static inline void renderMenuBarCallback(void *state)
{
    GuiMenuBar((GuiMenuBarState *)state);
}
static inline void renderFileDialogCallback(void *state)
{
    GuiWindowFileDialog((GuiWindowFileDialogState *)state);
}
static inline void renderToolTipTextCallback(void *state)
{
    GuiToolTipText((GuiTooltipTextState *)state);
}
static inline void renderToastCallback(void *state)
{
    GuiToast((GuiToastState *)state);
}

static void checkPriority(RenderObject *renderObjects, size_t *renderObjectsPriority, const size_t renderStatesCount)
{
    if(IsMouseButtonDown(MOUSE_LEFT_BUTTON) == true)
    {
        printf("Mouse button down\n");
        for(size_t i = 0; i < renderStatesCount; i++)
        {
            if(CheckCollisionPointRec(GetMousePosition(), ((GuiMenuBarState *)renderObjects[i].state)->bounds))
            {
                // Check if the priority is already the highest
                if(renderObjectsPriority[0] == renderObjects[i].priority)
                {
                    printf("Priority already highest\n");
                    break;
                }
                else
                {
                    printf("Priority not highest\n");
                    // Move the priority to the top
                    size_t tempPriority = renderObjectsPriority[0];
                    renderObjectsPriority[0] = renderObjects[i].priority;
                    renderObjectsPriority[i] = tempPriority;
                    break;
                }
            }
        }
    }
}