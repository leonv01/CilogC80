#ifndef GUI_WINDOW_PREFERENCES_H
#define GUI_WINDOW_PREFERENCES_H

#include "raylib.h"

typedef struct
{

} GuiWindowPreferencesState;

#endif // GUI_WINDOW_PREFERENCES_H

#if defined(GUI_WINDOW_PREFERENCES_IMPLEMENTATION)
#include "raygui.h"

GuiWindowPreferencesState InitGuiWindowPreferences()
{
    GuiWindowPreferencesState state = { 0 };

    return state;
}

#endif // GUI_WINDOW_PREFERENCES_IMPLEMENTATION