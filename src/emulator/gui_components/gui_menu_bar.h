#ifndef GUI_MENU_BAR_H
#define GUI_MENU_BAR_H

#include "raylib.h"

#define BUTTON_SPACING(size, padding, count) ((size + padding) * count)

typedef struct
{
    Vector2 position;
    int iconId;
} GuiMenuButton;

typedef struct 
{
    Vector2 position;
    Rectangle bounds;
    int buttonSize;
    int buttonPadding;
    int maxWidth;
    int maxHeight;

    /* ------------------------------ Menu buttons ------------------------------ */
    GuiMenuButton openButton;
    GuiMenuButton loadToMemoryButton;

    GuiMenuButton cpuButton;
    GuiMenuButton memoryButton;
    GuiMenuButton displayButton;

    GuiMenuButton startEmulationButton;
    GuiMenuButton pauseEmulationButton;
    GuiMenuButton stepEmulationButton;
    GuiMenuButton stopEmulationButton;

    GuiMenuButton settingsButton;
    GuiMenuButton aboutButton;
    GuiMenuButton helpButton;
    /* -------------------------------------------------------------------------- */

    /* ------------------------- Menu button interaction ------------------------ */
    bool openButtonActive;
    bool loadToMemoryButtonActive;

    bool cpuButtonActive;
    bool memoryButtonActive;
    bool displayButtonActive;

    bool startEmulationButtonActive;
    bool pauseEmulationButtonActive;
    bool stepEmulationButtonActive;
    bool stopEmulationButtonActive;

    bool settingsButtonActive;
    bool aboutButtonActive;
    bool helpButtonActive;
    /* -------------------------------------------------------------------------- */

    /* -------------------------- Menu button on hover -------------------------- */
    bool openButtonHover;
    bool loadToMemoryButtonHover;
    
    bool cpuButtonHover;
    bool memoryButtonHover;
    bool displayButtonHover;

    bool startEmulationButtonHover;
    bool pauseEmulationButtonHover;
    bool stepEmulationButtonHover;
    bool stopEmulationButtonHover;


    bool settingsButtonHover;
    bool aboutButtonHover;
    bool helpButtonHover;
    /* -------------------------------------------------------------------------- */

} GuiMenuBarState;

GuiMenuBarState InitGuiMenuBar(const Vector2 position, const int maxWidth);
void GuiMenuBarResize(GuiMenuBarState *state, const int maxWidth);
void GuiMenuBar(GuiMenuBarState *state);
char *GuiMenuBarGetTooltip(GuiMenuBarState *state, bool *isAnyHovered);

#endif // GUI_MENU_BAR_H

#define GUI_MENU_BAR_IMPLEMENTATION

#ifdef GUI_MENU_BAR_IMPLEMENTATION

#include "raygui.h"

#include <stdlib.h>

GuiMenuBarState InitGuiMenuBar(const Vector2 position, const int maxWidth)
{
    GuiMenuBarState state = { 0 };

    state.position = position;
    state.maxWidth = maxWidth;
    state.maxHeight = 40;
    state.bounds = (Rectangle){ position.x, position.y, maxWidth, state.maxHeight };

    state.buttonSize = 30;

    state.buttonPadding = (state.maxHeight - state.buttonSize) / 2;

    state.openButton = (GuiMenuButton){ 0 };
    state.openButton.position = (Vector2){ position.x + state.buttonPadding, position.y + state.buttonPadding };

    return state;
}

void GuiMenuBarResize(GuiMenuBarState *state, const int maxWidth)
{
    state->maxWidth = maxWidth;
    state->bounds.width = maxWidth;
}

void GuiMenuBar(GuiMenuBarState *state)
{
    GuiPanel(state->bounds, NULL);
    DrawRectangle(state->bounds.x, state->bounds.y, state->bounds.width, state->bounds.height, BLACK);

    /* ------------------------------ Draw buttons ------------------------------ */
    state->openButtonActive = GuiButton((Rectangle)
    {
        state->openButton.position.x,
        state->openButton.position.y,
        state->buttonSize,
        state->buttonSize
    }, GuiIconText(ICON_FILE_OPEN, ""));

    state->loadToMemoryButtonActive = GuiButton((Rectangle)
    {
        state->openButton.position.x + BUTTON_SPACING(state->buttonSize, state->buttonPadding, 1),
        state->openButton.position.y,
        state->buttonSize,
        state->buttonSize
    }, GuiIconText(ICON_FILE_SAVE_CLASSIC, ""));

    state->cpuButtonActive = GuiButton((Rectangle)
    {
        state->openButton.position.x + BUTTON_SPACING(state->buttonSize, state->buttonPadding, 2),
        state->openButton.position.y,
        state->buttonSize,
        state->buttonSize
    }, GuiIconText(ICON_CPU, ""));

    state->memoryButtonActive = GuiButton((Rectangle)
    {
        state->openButton.position.x + BUTTON_SPACING(state->buttonSize, state->buttonPadding, 3),
        state->openButton.position.y,
        state->buttonSize,
        state->buttonSize
    }, GuiIconText(ICON_ROM, ""));
    
    state->startEmulationButtonActive = GuiButton((Rectangle)
    {
        state->openButton.position.x + BUTTON_SPACING(state->buttonSize, state->buttonPadding, 5),
        state->openButton.position.y,
        state->buttonSize,
        state->buttonSize
    }, GuiIconText(ICON_PLAYER_PLAY, ""));
    state->pauseEmulationButtonActive = GuiButton((Rectangle)
    {
        state->openButton.position.x + BUTTON_SPACING(state->buttonSize, state->buttonPadding, 6),
        state->openButton.position.y,
        state->buttonSize,
        state->buttonSize
    }, GuiIconText(ICON_PLAYER_PAUSE, ""));
    state->stepEmulationButtonActive = GuiButton((Rectangle)
    {
        state->openButton.position.x + BUTTON_SPACING(state->buttonSize, state->buttonPadding, 7),
        state->openButton.position.y,
        state->buttonSize,
        state->buttonSize
    }, GuiIconText(ICON_STEP_OVER, ""));
    state->stopEmulationButtonActive = GuiButton((Rectangle)
    {
        state->openButton.position.x + BUTTON_SPACING(state->buttonSize, state->buttonPadding, 8),
        state->openButton.position.y,
        state->buttonSize,
        state->buttonSize
    }, GuiIconText(ICON_PLAYER_STOP, ""));
    /* -------------------------------------------------------------------------- */

    /* ----------------------------- Check on hover ----------------------------- */
    state->openButtonHover = CheckCollisionPointRec(GetMousePosition(), (Rectangle)
    {
        state->openButton.position.x,
        state->openButton.position.y,
        state->buttonSize,
        state->buttonSize
    });
    
    state->loadToMemoryButtonHover = CheckCollisionPointRec(GetMousePosition(), (Rectangle)
    {
        state->openButton.position.x + BUTTON_SPACING(state->buttonSize, state->buttonPadding, 1),
        state->openButton.position.y,
        state->buttonSize,
        state->buttonSize
    });

    state->cpuButtonHover = CheckCollisionPointRec(GetMousePosition(), (Rectangle)
    {
        state->openButton.position.x + BUTTON_SPACING(state->buttonSize, state->buttonPadding, 2),
        state->openButton.position.y,
        state->buttonSize,
        state->buttonSize
    });

    state->memoryButtonHover = CheckCollisionPointRec(GetMousePosition(), (Rectangle)
    {
        state->openButton.position.x + BUTTON_SPACING(state->buttonSize, state->buttonPadding, 3),
        state->openButton.position.y,
        state->buttonSize,
        state->buttonSize
    });

    /* -------------------------------------------------------------------------- */
}

char *GuiMenuBarGetTooltip(GuiMenuBarState *state, bool *isAnyHovered)
{
    char *text = NULL;
    if(state->openButtonHover) text = "Open file";
    else if(state->loadToMemoryButtonHover) text = "Load to memory";
    else if(state->cpuButtonHover) text = "Open CPU window";
    else if(state->memoryButtonHover) text = "Open Memory window";

    if(state->openButtonHover || state->loadToMemoryButtonHover || state->cpuButtonHover || state->memoryButtonHover)
    {
        *isAnyHovered = true;
    }

    return text;
}

#endif // GUI_MENU_BAR_IMPLEMENTATION