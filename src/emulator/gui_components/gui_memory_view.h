/*******************************************************************************************
*
*   MemoryView v1.0.0 - Tool Description
*
*   MODULE USAGE:
*       #define GUI_MEMORY_VIEW_IMPLEMENTATION
*       #include "gui_memory_view.h"
*
*       INIT: GuiMemoryViewState state = InitGuiMemoryView();
*       DRAW: GuiMemoryView(&state);
*
*   LICENSE: Propietary License
*
*   Copyright (c) 2022 raylib technologies. All Rights Reserved.
*
*   Unauthorized copying of this file, via any medium is strictly prohibited
*   This project is proprietary and confidential unless the owner allows
*   usage in any other form by expresely written permission.
*
**********************************************************************************************/

#include "raylib.h"

// WARNING: raygui implementation is expected to be defined before including this header
#undef RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include <string.h>     // Required for: strcpy()

#ifndef GUI_MEMORY_VIEW_H
#define GUI_MEMORY_VIEW_H

typedef struct {
    Vector2 rootAnchor;
    
    bool memoryWindowActive;
    bool TextBox020EditMode;
    char TextBox020Text[128];
    bool TextBox004EditMode;
    char TextBox004Text[128];

    // Custom state variables (depend on development software)
    // NOTE: This variables should be added manually if required

} GuiMemoryViewState;

#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
GuiMemoryViewState InitGuiMemoryView(void);
void GuiMemoryView(GuiMemoryViewState *state);

#ifdef __cplusplus
}
#endif

#endif // GUI_MEMORY_VIEW_H

/***********************************************************************************
*
*   GUI_MEMORY_VIEW IMPLEMENTATION
*
************************************************************************************/
#if defined(GUI_MEMORY_VIEW_IMPLEMENTATION)

#include "raygui.h"

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Internal Module Functions Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
GuiMemoryViewState InitGuiMemoryView(void)
{
    GuiMemoryViewState state = { 0 };

    state.rootAnchor = (Vector2){ 288, 96 };
    
    state.memoryWindowActive = true;
    state.TextBox020EditMode = false;
    strcpy(state.TextBox020Text, "a");
    state.TextBox004EditMode = false;
    strcpy(state.TextBox004Text, "a");

    // Custom variables initialization

    return state;
}

void GuiMemoryView(GuiMemoryViewState *state)
{
    if (state->memoryWindowActive)
    {
        state->memoryWindowActive = !GuiWindowBox((Rectangle){ state->rootAnchor.x + 0, state->rootAnchor.y + 0, 360, 528 }, "Memory VIew");
        GuiGroupBox((Rectangle){ state->rootAnchor.x + 24, state->rootAnchor.y + 48, 312, 216 }, "ROM");
        if (GuiTextBox((Rectangle){ state->rootAnchor.x + 96, state->rootAnchor.y + 56, 232, 200 }, state->TextBox020Text, 128, state->TextBox020EditMode)) state->TextBox020EditMode = !state->TextBox020EditMode;
        GuiGroupBox((Rectangle){ state->rootAnchor.x + 24, state->rootAnchor.y + 288, 312, 216 }, "RAM");
        if (GuiTextBox((Rectangle){ state->rootAnchor.x + 96, state->rootAnchor.y + 296, 232, 200 }, state->TextBox004Text, 128, state->TextBox004EditMode)) state->TextBox004EditMode = !state->TextBox004EditMode;
    }
}

#endif // GUI_MEMORY_VIEW_IMPLEMENTATION
