#ifndef GUI_PREFERENCES_H
#define GUI_PREFERENCES_H

#include "raylib.h"

typedef struct
{
    /* ---------------------------- Window attributes --------------------------- */
    Rectangle bounds;
    Vector2 position;

    int width;
    int height;
    int minWidth;
    int minHeight;
    int padding;
    int fontSize;

    Rectangle resizerBounds;

    bool isWindowActive;
    bool isWindowMinimized;

    bool dragMode;
    bool supportDrag;

    bool resizeMode;
    bool supportResize;

    Vector2 panOffset;
    /* -------------------------------------------------------------------------- */

    /* ------------------------- Preferences GUI attributes ---------------------- */
    const char *preferencesLabelText;

    const char *uiStyleLabelText;
    /* -------------------------------------------------------------------------- */
} GuiPreferencesState;

GuiPreferencesState InitGuiPreferences();
void GuiPreferences(GuiPreferencesState *state);

#endif // GUI_PREFERENCES_H

#define GUI_PREFERENCES_IMPLEMENTATION
#ifdef GUI_PREFERENCES_IMPLEMENTATION

#include "raygui.h"

GuiPreferencesState InitGuiPreferences()
{
    GuiPreferencesState state = { 0 };

    /*

    state.bounds.width = CPU_VIEW_SPACING(state.registerLabelWidth / 2, state.padding, 4) + (state.padding * 3);
    state.bounds.height = CPU_VIEW_SPACING(state.registerLabelHeight, state.padding, 4) + 
                            CPU_VIEW_SPACING(state.flagLabelHeight, state.padding, 2) + 
                            CPU_VIEW_SPACING(state.pointerLabelHeight, state.padding, 2) + 
                            (state.padding * 7) + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT;
    state.bounds.x = GetScreenWidth() / 2 - state.bounds.width / 2;
    state.bounds.y = GetScreenHeight() / 2 - state.bounds.height / 2;

    state.position = (Vector2){ state.bounds.x, state.bounds.y };
*/
    return state;
}

void GuiPreferences(GuiPreferencesState *state)
{
    if(state->isWindowActive == true)
    {

    }
}

#endif // GUI_PREFERENCES_IMPLEMENTATION