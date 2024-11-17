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

    /* ------------------------- Menu Button definition ------------------------- */
    const int buttonSize = 40;
    const int buttonPadding = 10;
    const int menuHeightOffset = 24;

    GuiMenuButton newFileButton = { (Vector2){ buttonPadding, menuHeightOffset + buttonPadding}, "New File", false, ICON_FILE_ADD };
    GuiMenuButton openFileButton = { (Vector2){ (buttonPadding * 2) + buttonSize, menuHeightOffset + buttonPadding}, "Open File", false, ICON_FILE_OPEN };
    GuiMenuButton saveFileButton = { (Vector2){ (buttonSize * 2) + (buttonPadding * 3), menuHeightOffset + buttonPadding}, "Save File", false, ICON_FILE_SAVE_CLASSIC };
    
    GuiMenuButton cpuViewButton = { (Vector2){ (buttonSize * 3) + (buttonPadding * 4), menuHeightOffset + buttonPadding}, "Open CPU View", false, ICON_CPU};
    GuiMenuButton memoryViewButton = { (Vector2){ (buttonSize * 4) + (buttonPadding * 5), menuHeightOffset + buttonPadding}, "Open Memory View", false, ICON_FILE_OPEN };
    GuiMenuButton interruptViewButton = { (Vector2){ (buttonSize * 5) + (buttonPadding * 6), menuHeightOffset + buttonPadding}, "Open Interrupt View", false, ICON_FILE_OPEN };
    GuiMenuButton preferencesButton = { (Vector2){ (buttonSize * 6) + (buttonPadding * 7), menuHeightOffset + buttonPadding}, "Open Preferences", false, ICON_FILE_OPEN };
    GuiMenuButton aboutButton = { (Vector2){ (buttonSize * 7) + (buttonPadding * 8), menuHeightOffset + buttonPadding}, "About", false, ICON_FILE_OPEN };
    GuiMenuButton helpButton = { (Vector2){ (buttonSize * 8) + (buttonPadding * 9), menuHeightOffset + buttonPadding}, "Help", false, ICON_FILE_OPEN };

    GuiMenuButton menuButtons[] = { newFileButton, openFileButton, saveFileButton, cpuViewButton, memoryViewButton, interruptViewButton, preferencesButton, aboutButton, helpButton };
    const int menuButtonsCount = sizeof(menuButtons) / sizeof(menuButtons[0]);
    /* -------------------------------------------------------------------------- */
    GuiTooltipTextState tooltip = InitGuiToolTipText("Create new file", (Rectangle){ 0, 0, 150, 30 });

    // Variables for file dialog
    Texture texture = { 0 };
    char fileNameToLoad[512] = { 0 };
    GuiWindowFileDialogState fileDialogState = InitGuiWindowFileDialog(GetWorkingDirectory());

    GuiCpuViewState cpuViewState = InitGuiCpuView();

    // Variables for the portable window
    Vector2 mousePosition = { 0 };
    Vector2 windowPosition = { 500, 200 };
    Vector2 panOffset = mousePosition;
    bool dragWindow = false;
    bool exitWindow = false;
    
    SetWindowPosition(windowPosition.x, windowPosition.y);
    
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

        bool isMouseOverButton = false;
        for (int i = 0; i < menuButtonsCount; i++)
        {
            if(CheckCollisionPointRec(mousePosition, (Rectangle){ menuButtons[i].position.x, menuButtons[i].position.y, buttonSize, buttonSize }))
            {
                isMouseOverButton = true;
                tooltip.isActive = true;
                tooltip.text = menuButtons[i].tooltip;
            }
        }

        if(isMouseOverButton == false)
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
            const int menuHeightOffset = 24;
            const int menuButtonOffset = 10;
            DrawRectangle(0, menuHeightOffset, GetScreenWidth(), buttonSize + (2 * buttonPadding), Fade(GRAY, 0.8f));
            GuiGroupBox((Rectangle){ 0, menuHeightOffset, GetScreenWidth(), buttonSize + (2 * buttonPadding) }, "");

            /* -------------------------------------------------------------------------- */
            // New file button
            if(GuiButton((Rectangle){ newFileButton.position.x, newFileButton.position.y, buttonSize, buttonSize }, GuiIconText(newFileButton.iconId, "")))
            {
                fileDialogState.windowActive = true;
            }
            // Open file button
            if(GuiButton((Rectangle){ openFileButton.position.x, openFileButton.position.y, buttonSize, buttonSize }, GuiIconText(openFileButton.iconId, "")))
            {
                fileDialogState.windowActive = true;
            }       
            // Save file button
            if(GuiButton((Rectangle){ saveFileButton.position.x, saveFileButton.position.y, buttonSize, buttonSize }, GuiIconText(saveFileButton.iconId, "")))
            {
                fileDialogState.windowActive = true;
                // TOOD: Set save file mode
                fileDialogState.saveFileMode = true;
            }
            /* -------------------------------------------------------------------------- */
            // CPU view button
            if(GuiButton((Rectangle){ cpuViewButton.position.x, cpuViewButton.position.y, buttonSize, buttonSize }, GuiIconText(cpuViewButton.iconId, "")))
            {
                cpuViewState.cpuAndRegisterWindowActive = !cpuViewState.cpuAndRegisterWindowActive;
            }
            // Memory view button
            if(GuiButton((Rectangle){ memoryViewButton.position.x, memoryViewButton.position.y, buttonSize, buttonSize }, GuiIconText(memoryViewButton.iconId, "")))
            {
                fileDialogState.windowActive = true;
            }

            GuiUnlock();
            
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
// -----------------------------------------------------------

// GUI creation functions
// -----------------------------------------------------------

// -----------------------------------------------------------

// Callback functions
// -----------------------------------------------------------

// -----------------------------------------------------------