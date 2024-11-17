/*******************************************************************************************
*
*   CpuView v1.0.0 - Tool Description
*
*   MODULE USAGE:
*       #define GUI_CPU_VIEW_IMPLEMENTATION
*       #include "gui_cpu_view.h"
*
*       INIT: GuiCpuViewState state = InitGuiCpuView();
*       DRAW: GuiCpuView(&state);
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

#ifndef GUI_CPU_VIEW_H
#define GUI_CPU_VIEW_H

typedef struct {
    Vector2 rootAnchor;
    
    bool cpuAndRegisterWindowActive;
    bool registerBTextEditMode;
    char registerBTextText[128];
    bool registerCTextEditMode;
    char registerCTextText[128];
    bool registerDTextEditMode;
    char registerDTextText[128];
    bool registerHTextEditMode;
    char registerHTextText[128];
    bool registerETextEditMode;
    char registerETextText[128];
    bool TextBox020EditMode;
    char TextBox020Text[128];
    bool registerATextEditMode;
    char registerATextText[128];
    bool flagCTextEditMode;
    char flagCTextText[128];
    bool flagNTextEditMode;
    char flagNTextText[128];
    bool flagPTextEditMode;
    char flagPTextText[128];
    bool flagHTextEditMode;
    char flagHTextText[128];
    bool flagZTextEditMode;
    char flagZTextText[128];
    bool flagSTextEditMode;
    char flagSTextText[128];
    bool programCounterTextEditMode;
    char programCounterTextText[128];
    bool stackPointerTextEditMode;
    char stackPointerTextText[128];

    Rectangle layoutRecs[34];

    // Custom state variables (depend on development software)
    // NOTE: This variables should be added manually if required
    bool supportDrag;
    bool dragMode;
    Vector2 panOffset;
} GuiCpuViewState;

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
GuiCpuViewState InitGuiCpuView(void);
void GuiCpuView(GuiCpuViewState *state);

#ifdef __cplusplus
}
#endif

#endif // GUI_CPU_VIEW_H

/***********************************************************************************
*
*   GUI_CPU_VIEW IMPLEMENTATION
*
************************************************************************************/
#if defined(GUI_CPU_VIEW_IMPLEMENTATION)

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
GuiCpuViewState InitGuiCpuView(void)
{
    GuiCpuViewState state = { 0 };

    state.rootAnchor = (Vector2){ 288, 96 };
    
    state.cpuAndRegisterWindowActive = true;
    state.registerBTextEditMode = false;
    strcpy(state.registerBTextText, "0");
    state.registerCTextEditMode = false;
    strcpy(state.registerCTextText, "0");
    state.registerDTextEditMode = false;
    strcpy(state.registerDTextText, "0");
    state.registerHTextEditMode = false;
    strcpy(state.registerHTextText, "0");
    state.registerETextEditMode = false;
    strcpy(state.registerETextText, "0");
    state.TextBox020EditMode = false;
    strcpy(state.TextBox020Text, "0");
    state.registerATextEditMode = false;
    strcpy(state.registerATextText, "0");
    state.flagCTextEditMode = false;
    strcpy(state.flagCTextText, "0");
    state.flagNTextEditMode = false;
    strcpy(state.flagNTextText, "0");
    state.flagPTextEditMode = false;
    strcpy(state.flagPTextText, "0");
    state.flagHTextEditMode = false;
    strcpy(state.flagHTextText, "0");
    state.flagZTextEditMode = false;
    strcpy(state.flagZTextText, "0");
    state.flagSTextEditMode = false;
    strcpy(state.flagSTextText, "0");
    state.programCounterTextEditMode = false;
    strcpy(state.programCounterTextText, "0");
    state.stackPointerTextEditMode = false;
    strcpy(state.stackPointerTextText, "0");

    state.layoutRecs[0] = (Rectangle){ state.rootAnchor.x + 0, state.rootAnchor.y + 0, 360, 528 };
    state.layoutRecs[1] = (Rectangle){ state.rootAnchor.x + 24, state.rootAnchor.y + 48, 312, 216 };
    state.layoutRecs[2] = (Rectangle){ state.rootAnchor.x + 24, state.rootAnchor.y + 288, 312, 120 };
    state.layoutRecs[3] = (Rectangle){ state.rootAnchor.x + 24, state.rootAnchor.y + 432, 312, 72 };
    state.layoutRecs[4] = (Rectangle){ state.rootAnchor.x + 104, state.rootAnchor.y + 120, 64, 24 };
    state.layoutRecs[5] = (Rectangle){ state.rootAnchor.x + 248, state.rootAnchor.y + 120, 64, 24 };
    state.layoutRecs[6] = (Rectangle){ state.rootAnchor.x + 104, state.rootAnchor.y + 168, 64, 24 };
    state.layoutRecs[7] = (Rectangle){ state.rootAnchor.x + 104, state.rootAnchor.y + 216, 64, 24 };
    state.layoutRecs[8] = (Rectangle){ state.rootAnchor.x + 248, state.rootAnchor.y + 168, 64, 24 };
    state.layoutRecs[9] = (Rectangle){ state.rootAnchor.x + 248, state.rootAnchor.y + 216, 64, 24 };
    state.layoutRecs[10] = (Rectangle){ state.rootAnchor.x + 104, state.rootAnchor.y + 72, 64, 24 };
    state.layoutRecs[11] = (Rectangle){ state.rootAnchor.x + 32, state.rootAnchor.y + 72, 72, 24 };
    state.layoutRecs[12] = (Rectangle){ state.rootAnchor.x + 32, state.rootAnchor.y + 120, 72, 24 };
    state.layoutRecs[13] = (Rectangle){ state.rootAnchor.x + 32, state.rootAnchor.y + 168, 72, 24 };
    state.layoutRecs[14] = (Rectangle){ state.rootAnchor.x + 32, state.rootAnchor.y + 216, 72, 24 };
    state.layoutRecs[15] = (Rectangle){ state.rootAnchor.x + 176, state.rootAnchor.y + 120, 72, 24 };
    state.layoutRecs[16] = (Rectangle){ state.rootAnchor.x + 176, state.rootAnchor.y + 168, 72, 24 };
    state.layoutRecs[17] = (Rectangle){ state.rootAnchor.x + 176, state.rootAnchor.y + 216, 72, 24 };
    state.layoutRecs[18] = (Rectangle){ state.rootAnchor.x + 96, state.rootAnchor.y + 312, 24, 24 };
    state.layoutRecs[19] = (Rectangle){ state.rootAnchor.x + 48, state.rootAnchor.y + 312, 48, 24 };
    state.layoutRecs[20] = (Rectangle){ state.rootAnchor.x + 144, state.rootAnchor.y + 312, 48, 24 };
    state.layoutRecs[21] = (Rectangle){ state.rootAnchor.x + 192, state.rootAnchor.y + 312, 24, 24 };
    state.layoutRecs[22] = (Rectangle){ state.rootAnchor.x + 240, state.rootAnchor.y + 312, 48, 24 };
    state.layoutRecs[23] = (Rectangle){ state.rootAnchor.x + 288, state.rootAnchor.y + 312, 24, 24 };
    state.layoutRecs[24] = (Rectangle){ state.rootAnchor.x + 48, state.rootAnchor.y + 360, 48, 24 };
    state.layoutRecs[25] = (Rectangle){ state.rootAnchor.x + 96, state.rootAnchor.y + 360, 24, 24 };
    state.layoutRecs[26] = (Rectangle){ state.rootAnchor.x + 144, state.rootAnchor.y + 360, 48, 24 };
    state.layoutRecs[27] = (Rectangle){ state.rootAnchor.x + 192, state.rootAnchor.y + 360, 24, 24 };
    state.layoutRecs[28] = (Rectangle){ state.rootAnchor.x + 240, state.rootAnchor.y + 360, 48, 24 };
    state.layoutRecs[29] = (Rectangle){ state.rootAnchor.x + 288, state.rootAnchor.y + 360, 24, 24 };
    state.layoutRecs[30] = (Rectangle){ state.rootAnchor.x + 48, state.rootAnchor.y + 456, 24, 24 };
    state.layoutRecs[31] = (Rectangle){ state.rootAnchor.x + 72, state.rootAnchor.y + 456, 72, 24 };
    state.layoutRecs[32] = (Rectangle){ state.rootAnchor.x + 240, state.rootAnchor.y + 456, 72, 24 };
    state.layoutRecs[33] = (Rectangle){ state.rootAnchor.x + 216, state.rootAnchor.y + 456, 24, 24 };

    // Custom variables initialization
    state.supportDrag = true;
    state.dragMode = false;
    state.panOffset = (Vector2){ 0, 0 };
    return state;
}

void GuiCpuView(GuiCpuViewState *state)
{
    if (state->cpuAndRegisterWindowActive)
    {
        if(state->supportDrag)
        {
            Vector2 mousePosition = GetMousePosition();

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                // Window can be dragged from the top window bar
                if (CheckCollisionPointRec(mousePosition, (Rectangle){ state->layoutRecs[0].x, state->layoutRecs[0].y, (float)state->layoutRecs[0].width, RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT }))
                {
                    state->dragMode = true;
                    state->panOffset.x = mousePosition.x - state->rootAnchor.x;
                    state->panOffset.y = mousePosition.y - state->rootAnchor.y;
                }
            }

            if(state->dragMode)
            {
                state->rootAnchor.x = (mousePosition.x - state->panOffset.x);
                state->rootAnchor.y = (mousePosition.y - state->panOffset.y);
                if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) state->dragMode = false;

                state->layoutRecs[0] = (Rectangle){ state->rootAnchor.x + 0, state->rootAnchor.y + 0, 360, 528 };
                state->layoutRecs[1] = (Rectangle){ state->rootAnchor.x + 24, state->rootAnchor.y + 48, 312, 216 };
                state->layoutRecs[2] = (Rectangle){ state->rootAnchor.x + 24, state->rootAnchor.y + 288, 312, 120 };
                state->layoutRecs[3] = (Rectangle){ state->rootAnchor.x + 24, state->rootAnchor.y + 432, 312, 72 };
                state->layoutRecs[4] = (Rectangle){ state->rootAnchor.x + 104, state->rootAnchor.y + 120, 64, 24 };
                state->layoutRecs[5] = (Rectangle){ state->rootAnchor.x + 248, state->rootAnchor.y + 120, 64, 24 };
                state->layoutRecs[6] = (Rectangle){ state->rootAnchor.x + 104, state->rootAnchor.y + 168, 64, 24 };
                state->layoutRecs[7] = (Rectangle){ state->rootAnchor.x + 104, state->rootAnchor.y + 216, 64, 24 };
                state->layoutRecs[8] = (Rectangle){ state->rootAnchor.x + 248, state->rootAnchor.y + 168, 64, 24 };
                state->layoutRecs[9] = (Rectangle){ state->rootAnchor.x + 248, state->rootAnchor.y + 216, 64, 24 };
                state->layoutRecs[10] = (Rectangle){ state->rootAnchor.x + 104, state->rootAnchor.y + 72, 64, 24 };
                state->layoutRecs[11] = (Rectangle){ state->rootAnchor.x + 32, state->rootAnchor.y + 72, 72, 24 };
                state->layoutRecs[12] = (Rectangle){ state->rootAnchor.x + 32, state->rootAnchor.y + 120, 72, 24 };
                state->layoutRecs[13] = (Rectangle){ state->rootAnchor.x + 32, state->rootAnchor.y + 168, 72, 24 };
                state->layoutRecs[14] = (Rectangle){ state->rootAnchor.x + 32, state->rootAnchor.y + 216, 72, 24 };
                state->layoutRecs[15] = (Rectangle){ state->rootAnchor.x + 176, state->rootAnchor.y + 120, 72, 24 };
                state->layoutRecs[16] = (Rectangle){ state->rootAnchor.x + 176, state->rootAnchor.y + 168, 72, 24 };
                state->layoutRecs[17] = (Rectangle){ state->rootAnchor.x + 176, state->rootAnchor.y + 216, 72, 24 };
                state->layoutRecs[18] = (Rectangle){ state->rootAnchor.x + 96, state->rootAnchor.y + 312, 24, 24 };
                state->layoutRecs[19] = (Rectangle){ state->rootAnchor.x + 48, state->rootAnchor.y + 312, 48, 24 };
                state->layoutRecs[20] = (Rectangle){ state->rootAnchor.x + 144, state->rootAnchor.y + 312, 48, 24 };
                state->layoutRecs[21] = (Rectangle){ state->rootAnchor.x + 192, state->rootAnchor.y + 312, 24, 24 };
                state->layoutRecs[22] = (Rectangle){ state->rootAnchor.x + 240, state->rootAnchor.y + 312, 48, 24 };
                state->layoutRecs[23] = (Rectangle){ state->rootAnchor.x + 288, state->rootAnchor.y + 312, 24, 24 };
                state->layoutRecs[24] = (Rectangle){ state->rootAnchor.x + 48, state->rootAnchor.y + 360, 48, 24 };
                state->layoutRecs[25] = (Rectangle){ state->rootAnchor.x + 96, state->rootAnchor.y + 360, 24, 24 };
                state->layoutRecs[26] = (Rectangle){ state->rootAnchor.x + 144, state->rootAnchor.y + 360, 48, 24 };
                state->layoutRecs[27] = (Rectangle){ state->rootAnchor.x + 192, state->rootAnchor.y + 360, 24, 24 };
                state->layoutRecs[28] = (Rectangle){ state->rootAnchor.x + 240, state->rootAnchor.y + 360, 48, 24 };
                state->layoutRecs[29] = (Rectangle){ state->rootAnchor.x + 288, state->rootAnchor.y + 360, 24, 24 };
                state->layoutRecs[30] = (Rectangle){ state->rootAnchor.x + 48, state->rootAnchor.y + 456, 24, 24 };
                state->layoutRecs[31] = (Rectangle){ state->rootAnchor.x + 72, state->rootAnchor.y + 456, 72, 24 };
                state->layoutRecs[32] = (Rectangle){ state->rootAnchor.x + 240, state->rootAnchor.y + 456, 72, 24 };
                state->layoutRecs[33] = (Rectangle){ state->rootAnchor.x + 216, state->rootAnchor.y + 456, 24, 24 };
            }
        }

        state->cpuAndRegisterWindowActive = !GuiWindowBox(state->layoutRecs[0], "CPU and Register View");
        GuiGroupBox(state->layoutRecs[1], "Registers");
        GuiGroupBox(state->layoutRecs[2], "Flags");
        GuiGroupBox(state->layoutRecs[3], NULL);
        if (GuiTextBox(state->layoutRecs[4], state->registerBTextText, 128, state->registerBTextEditMode)) state->registerBTextEditMode = !state->registerBTextEditMode;
        if (GuiTextBox(state->layoutRecs[5], state->registerCTextText, 128, state->registerCTextEditMode)) state->registerCTextEditMode = !state->registerCTextEditMode;
        if (GuiTextBox(state->layoutRecs[6], state->registerDTextText, 128, state->registerDTextEditMode)) state->registerDTextEditMode = !state->registerDTextEditMode;
        if (GuiTextBox(state->layoutRecs[7], state->registerHTextText, 128, state->registerHTextEditMode)) state->registerHTextEditMode = !state->registerHTextEditMode;
        if (GuiTextBox(state->layoutRecs[8], state->registerETextText, 128, state->registerETextEditMode)) state->registerETextEditMode = !state->registerETextEditMode;
        if (GuiTextBox(state->layoutRecs[9], state->TextBox020Text, 128, state->TextBox020EditMode)) state->TextBox020EditMode = !state->TextBox020EditMode;
        if (GuiTextBox(state->layoutRecs[10], state->registerATextText, 128, state->registerATextEditMode)) state->registerATextEditMode = !state->registerATextEditMode;
        GuiLabel(state->layoutRecs[11], "Register A");
        GuiLabel(state->layoutRecs[12], "Register B");
        GuiLabel(state->layoutRecs[13], "Register D");
        GuiLabel(state->layoutRecs[14], "Register H");
        GuiLabel(state->layoutRecs[15], "Register C");
        GuiLabel(state->layoutRecs[16], "Register E");
        GuiLabel(state->layoutRecs[17], "Register L");
        if (GuiTextBox(state->layoutRecs[18], state->flagCTextText, 128, state->flagCTextEditMode)) state->flagCTextEditMode = !state->flagCTextEditMode;
        GuiLabel(state->layoutRecs[19], "Flag C");
        GuiLabel(state->layoutRecs[20], "Flag N");
        if (GuiTextBox(state->layoutRecs[21], state->flagNTextText, 128, state->flagNTextEditMode)) state->flagNTextEditMode = !state->flagNTextEditMode;
        GuiLabel(state->layoutRecs[22], "Flag P");
        if (GuiTextBox(state->layoutRecs[23], state->flagPTextText, 128, state->flagPTextEditMode)) state->flagPTextEditMode = !state->flagPTextEditMode;
        GuiLabel(state->layoutRecs[24], "Flag H");
        if (GuiTextBox(state->layoutRecs[25], state->flagHTextText, 128, state->flagHTextEditMode)) state->flagHTextEditMode = !state->flagHTextEditMode;
        GuiLabel(state->layoutRecs[26], "Flag Z");
        if (GuiTextBox(state->layoutRecs[27], state->flagZTextText, 128, state->flagZTextEditMode)) state->flagZTextEditMode = !state->flagZTextEditMode;
        GuiLabel(state->layoutRecs[28], "Flag S");
        if (GuiTextBox(state->layoutRecs[29], state->flagSTextText, 128, state->flagSTextEditMode)) state->flagSTextEditMode = !state->flagSTextEditMode;
        GuiLabel(state->layoutRecs[30], "PC");
        if (GuiTextBox(state->layoutRecs[31], state->programCounterTextText, 128, state->programCounterTextEditMode)) state->programCounterTextEditMode = !state->programCounterTextEditMode;
        if (GuiTextBox(state->layoutRecs[32], state->stackPointerTextText, 128, state->stackPointerTextEditMode)) state->stackPointerTextEditMode = !state->stackPointerTextEditMode;
        GuiLabel(state->layoutRecs[33], "SP");
    }
}

#endif // GUI_CPU_VIEW_IMPLEMENTATION
