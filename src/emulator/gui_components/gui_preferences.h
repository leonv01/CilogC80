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

GuiPreferencesState InitGuiPreferences(const Vector2 position, const int width, const int height);
void GuiPreferences(GuiPreferencesState *state);

#endif // GUI_PREFERENCES_H

#ifdef GUI_PREFERENCES_IMPLEMENTATION

#include "raygui.h"

GuiPreferencesState InitGuiPreferences(const Vector2 position, const int width, const int height)
{
    GuiPreferencesState state = { 0 };

    state.position.x = position.x - (width / 2);
    state.position.y = position.y - (height / 2);
    state.width = width;
    state.height = height;
    state.padding = 12;
    state.fontSize = 20;
    state.bounds = (Rectangle){ position.x, position.y, width, height };
    state.isWindowActive = false;
    state.isWindowMinimized = false;

    state.minHeight = 100;
    state.minWidth = 100;

    state.resizerBounds = (Rectangle){ state.bounds.x + state.bounds.width - 16, state.bounds.y + state.bounds.height - 16, 16, 16 };

    state.supportDrag = false;
    state.dragMode = false;

    state.supportResize = false;
    state.resizeMode = false;

    state.panOffset = (Vector2){ 0, 0 };

    return state;
}

void GuiPreferences(GuiPreferencesState *state)
{
    if(state->isWindowActive == true)
    {

    }
}

#endif // GUI_PREFERENCES_IMPLEMENTATION