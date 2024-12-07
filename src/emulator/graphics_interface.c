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
static CPU_t                *cpu;
static Memory_t             *memory;
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
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/*                            Forward declarations                            */
/* -------------------------------------------------------------------------- */
static inline void renderCpuViewCallback(void *state);
static inline void renderMemoryViewCallback(void *state);
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
int graphicsInit(int argc, char **argv, CPU_t *cpu, Memory_t *memory)
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
    GuiMemoryViewState memoryViewState = InitGuiMemoryView();
    GuiPreferencesState preferencesState = InitGuiPreferences((Vector2){ screenWidth / 2, screenHeight / 2 }, 400, 300);   
    GuiToastState toastState = InitGuiToast();
    //--------------------------------------------------------------------------------------
    RenderObject renderObjects[] = 
    {
        { &cpuViewState, (void (*)(void *))renderCpuViewCallback, 0 },
        { &memoryViewState, (void (*)(void *))renderMemoryViewCallback, 1 },
        { &preferencesState, (void (*)(void *))renderPreferencesCallback, 2 },
        { &fileDialogState, (void (*)(void *))renderFileDialogCallback, 3 },
        { &toastState, (void (*)(void *))renderToastCallback, 4 }
    };
    const int renderStatesCount = sizeof(renderObjects) / sizeof(RenderObject);
    size_t renderObjectsPriority[renderStatesCount];
    for (int i = 0; i < renderStatesCount; i++) {
        renderObjectsPriority[i] = i; // Store indices
    }

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
            bool fileLoaded = loadFile(selectedFileBuffer, memory->data, memory->memorySize);

            if(fileLoaded == true)
            {
                cpuReset(cpu);
                GuiMemoryViewUpdate(&memoryViewState, true);
                GuiToastDisplayMessage(&toastState, "File loaded", 5000, GUI_TOAST_SUCCESS);

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
                GuiToastDisplayMessage(&toastState, "Fugiat commodo incididunt do adipisicing excepteur quis.", 10000, GUI_TOAST_MESSAGE);
            }
            cpuStep(cpu, memory); 

            GuiMemoryViewUpdate(&memoryViewState, true);
            GuiCpuViewUpdate(&cpuViewState, true);
        }

        /* ----------------------------- CPU view update ---------------------------- */
        GuiCpuViewUpdateRegisters(&cpuViewState, cpu->A, cpu->B, cpu->C, cpu->D, cpu->E, cpu->H, cpu->L);
        GuiCpuViewUpdateFlags(&cpuViewState, cpu->F.C, cpu->F.N, cpu->F.P, cpu->F.H, cpu->F.Z, cpu->F.S);
        GuiCpuViewUpdatePointers(&cpuViewState, cpu->PC, cpu->SP);
        /* -------------------------------------------------------------------------- */

        /* --------------------------- Memory view update --------------------------- */
        GuiMemoryViewAddressUpdate(&memoryViewState, memory->data, memory->memorySize);
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
static inline void renderMemoryViewCallback(void *state)
{
    GuiMemoryView((GuiMemoryViewState *)state);
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