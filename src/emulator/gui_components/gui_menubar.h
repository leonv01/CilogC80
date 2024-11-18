/*******************************************************************************************
*
*   Menubar v1.0.0 - Tool Description
*
*   MODULE USAGE:
*       #define GUI_MENUBAR_IMPLEMENTATION
*       #include "gui_menubar.h"
*
*       INIT: GuiMenubarState state = InitGuiMenubar();
*       DRAW: GuiMenubar(&state);
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

#ifndef GUI_MENUBAR_H
#define GUI_MENUBAR_H

typedef struct {
    Vector2 anchor01;
    Vector2 anchor02;
    Vector2 anchor03;
    Vector2 anchor04;
    Vector2 anchor05;
    
    bool fileNewFilePressed;
    bool fileOpenFilePressed;
    bool fileSaveFilePressed;
    bool fileCloseFilePressed;
    bool viewCpuPressed;
    bool viewMemoryPressed;
    bool viewInterruptPressed;
    bool viewAssemblyPressed;
    bool controlPlayPressed;
    bool controlPausePressed;
    bool controlStopPressed;
    bool controlStepPressed;
    bool controlStepIntoPressed;
    bool controlStepToBreakpointPressed;
    bool controlStepToCursorPressed;
    bool viewBytecodePressed;
    bool viewConsolePressed;
    bool preferencesButtonPressed;
    bool aboutButtonPressed;
    bool helpButtonPressed;

    Rectangle layoutRecs[28];

    // Custom state variables (depend on development software)
    // NOTE: This variables should be added manually if required

} GuiMenubarState;

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
GuiMenubarState InitGuiMenubar(void);
void GuiMenubar(GuiMenubarState *state);

#ifdef __cplusplus
}
#endif

#endif // GUI_MENUBAR_H

/***********************************************************************************
*
*   GUI_MENUBAR IMPLEMENTATION
*
************************************************************************************/
#if defined(GUI_MENUBAR_IMPLEMENTATION)

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
GuiMenubarState InitGuiMenubar(void)
{
    GuiMenubarState state = { 0 };

    state.anchor01 = (Vector2){ 0, 24 };
    state.anchor02 = (Vector2){ 448, 24 };
    state.anchor03 = (Vector2){ 192, 24 };
    state.anchor04 = (Vector2){ 760, 24 };
    state.anchor05 = (Vector2){ 1160, 24 };
    
    state.fileNewFilePressed = false;
    state.fileOpenFilePressed = false;
    state.fileSaveFilePressed = false;
    state.fileCloseFilePressed = false;
    state.viewCpuPressed = false;
    state.viewMemoryPressed = false;
    state.viewInterruptPressed = false;
    state.viewAssemblyPressed = false;
    state.controlPlayPressed = false;
    state.controlPausePressed = false;
    state.controlStopPressed = false;
    state.controlStepPressed = false;
    state.controlStepIntoPressed = false;
    state.controlStepToBreakpointPressed = false;
    state.controlStepToCursorPressed = false;
    state.viewBytecodePressed = false;
    state.viewConsolePressed = false;
    state.preferencesButtonPressed = false;
    state.aboutButtonPressed = false;
    state.helpButtonPressed = false;

    state.layoutRecs[0] = (Rectangle){ state.anchor01.x + 0, state.anchor01.y + 0, 192, 40 };
    state.layoutRecs[1] = (Rectangle){ state.anchor01.x + 56, state.anchor01.y + 8, 24, 24 };
    state.layoutRecs[2] = (Rectangle){ state.anchor01.x + 88, state.anchor01.y + 8, 24, 24 };
    state.layoutRecs[3] = (Rectangle){ state.anchor01.x + 120, state.anchor01.y + 8, 24, 24 };
    state.layoutRecs[4] = (Rectangle){ state.anchor01.x + 152, state.anchor01.y + 8, 24, 24 };
    state.layoutRecs[5] = (Rectangle){ state.anchor03.x + 0, state.anchor03.y + 0, 256, 40 };
    state.layoutRecs[6] = (Rectangle){ state.anchor03.x + 56, state.anchor03.y + 8, 24, 24 };
    state.layoutRecs[7] = (Rectangle){ state.anchor03.x + 88, state.anchor03.y + 8, 24, 24 };
    state.layoutRecs[8] = (Rectangle){ state.anchor03.x + 120, state.anchor03.y + 8, 24, 24 };
    state.layoutRecs[9] = (Rectangle){ state.anchor03.x + 152, state.anchor03.y + 8, 24, 24 };
    state.layoutRecs[10] = (Rectangle){ state.anchor02.x + 0, state.anchor02.y + 0, 312, 40 };
    state.layoutRecs[11] = (Rectangle){ state.anchor02.x + 16, state.anchor02.y + 8, 56, 24 };
    state.layoutRecs[12] = (Rectangle){ state.anchor02.x + 80, state.anchor02.y + 8, 24, 24 };
    state.layoutRecs[13] = (Rectangle){ state.anchor02.x + 112, state.anchor02.y + 8, 24, 24 };
    state.layoutRecs[14] = (Rectangle){ state.anchor02.x + 144, state.anchor02.y + 8, 24, 24 };
    state.layoutRecs[15] = (Rectangle){ state.anchor02.x + 176, state.anchor02.y + 8, 24, 24 };
    state.layoutRecs[16] = (Rectangle){ state.anchor02.x + 208, state.anchor02.y + 8, 24, 24 };
    state.layoutRecs[17] = (Rectangle){ state.anchor02.x + 240, state.anchor02.y + 8, 24, 24 };
    state.layoutRecs[18] = (Rectangle){ state.anchor02.x + 272, state.anchor02.y + 8, 24, 24 };
    state.layoutRecs[19] = (Rectangle){ state.anchor03.x + 184, state.anchor03.y + 8, 24, 24 };
    state.layoutRecs[20] = (Rectangle){ state.anchor03.x + 216, state.anchor03.y + 8, 24, 24 };
    state.layoutRecs[21] = (Rectangle){ state.anchor04.x + 0, state.anchor04.y + 0, 400, 40 };
    state.layoutRecs[22] = (Rectangle){ state.anchor05.x + 0, state.anchor05.y + 0, 120, 40 };
    state.layoutRecs[23] = (Rectangle){ state.anchor03.x + 16, state.anchor03.y + 8, 32, 24 };
    state.layoutRecs[24] = (Rectangle){ state.anchor01.x + 16, state.anchor01.y + 8, 32, 24 };
    state.layoutRecs[25] = (Rectangle){ state.anchor05.x + 16, state.anchor05.y + 8, 24, 24 };
    state.layoutRecs[26] = (Rectangle){ state.anchor05.x + 48, state.anchor05.y + 8, 24, 24 };
    state.layoutRecs[27] = (Rectangle){ state.anchor05.x + 80, state.anchor05.y + 8, 24, 24 };

    // Custom variables initialization

    return state;
}

void GuiMenubar(GuiMenubarState *state)
{
    GuiPanel(state->layoutRecs[0], NULL);
    state->fileNewFilePressed = GuiButton(state->layoutRecs[1], GuiIconText(ICON_FILE_ADD, "")); 
    state->fileOpenFilePressed = GuiButton(state->layoutRecs[2], GuiIconText(ICON_FILE_OPEN, "")); 
    state->fileSaveFilePressed = GuiButton(state->layoutRecs[3], GuiIconText(ICON_FILE_SAVE_CLASSIC, "")); 
    state->fileCloseFilePressed = GuiButton(state->layoutRecs[4], GuiIconText(ICON_FILE_DELETE, "")); 
    GuiPanel(state->layoutRecs[5], NULL);
    state->viewCpuPressed = GuiButton(state->layoutRecs[6], GuiIconText(ICON_CPU, "")); 
    state->viewMemoryPressed = GuiButton(state->layoutRecs[7], GuiIconText(ICON_ROM, "")); 
    state->viewInterruptPressed = GuiButton(state->layoutRecs[8], GuiIconText(ICON_CONSOLE_VIEW, "")); 
    state->viewAssemblyPressed = GuiButton(state->layoutRecs[9], NULL); 
    GuiPanel(state->layoutRecs[10], NULL);
    GuiLabel(state->layoutRecs[11], "Controls");
    state->controlPlayPressed = GuiButton(state->layoutRecs[12], GuiIconText(ICON_PLAYER_PLAY, "")); 
    state->controlPausePressed = GuiButton(state->layoutRecs[13], GuiIconText(ICON_PLAYER_PAUSE, "")); 
    state->controlStopPressed = GuiButton(state->layoutRecs[14], GuiIconText(ICON_PLAYER_STOP, "")); 
    state->controlStepPressed = GuiButton(state->layoutRecs[15], GuiIconText(ICON_STEP_OVER, "")); 
    state->controlStepIntoPressed = GuiButton(state->layoutRecs[16], GuiIconText(ICON_STEP_INTO, "")); 
    state->controlStepToBreakpointPressed = GuiButton(state->layoutRecs[17], NULL); 
    state->controlStepToCursorPressed = GuiButton(state->layoutRecs[18], NULL); 
    state->viewBytecodePressed = GuiButton(state->layoutRecs[19], GuiIconText(ICON_FILETYPE_BINARY, "")); 
    state->viewConsolePressed = GuiButton(state->layoutRecs[20], GuiIconText(ICON_CONSOLE_VIEW, "")); 
    GuiPanel(state->layoutRecs[21], NULL);
    GuiPanel(state->layoutRecs[22], NULL);
    GuiLabel(state->layoutRecs[23], "View");
    GuiLabel(state->layoutRecs[24], "File");
    state->preferencesButtonPressed = GuiButton(state->layoutRecs[25], NULL); 
    state->aboutButtonPressed = GuiButton(state->layoutRecs[26], NULL); 
    state->helpButtonPressed = GuiButton(state->layoutRecs[27], NULL); 
}

#endif // GUI_MENUBAR_IMPLEMENTATION
