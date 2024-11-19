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
    bool loadFileToMemoryPressed;
    bool fileToExecuteEditMode;
    int fileToExecuteActive;
    bool clearFileFromMemoryPressed;
    bool viewOverviewPressed;

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
    state.loadFileToMemoryPressed = false;
    state.fileToExecuteEditMode = false;
    state.fileToExecuteActive = 0;
    state.clearFileFromMemoryPressed = false;
    state.viewOverviewPressed = false;

    // Custom variables initialization

    return state;
}

void GuiMenubar(GuiMenubarState *state)
{
    if (state->fileToExecuteEditMode) GuiLock();

    GuiPanel((Rectangle){ 0, 24, 192, 40 }, NULL);
    state->fileNewFilePressed = GuiButton((Rectangle){ 56, 32, 24, 24 }, NULL); 
    state->fileOpenFilePressed = GuiButton((Rectangle){ 88, 32, 24, 24 }, NULL); 
    state->fileSaveFilePressed = GuiButton((Rectangle){ 120, 32, 24, 24 }, NULL); 
    state->fileCloseFilePressed = GuiButton((Rectangle){ 152, 32, 24, 24 }, NULL); 
    GuiPanel((Rectangle){ 192, 24, 296, 40 }, NULL);
    state->viewCpuPressed = GuiButton((Rectangle){ 248, 32, 24, 24 }, NULL); 
    state->viewMemoryPressed = GuiButton((Rectangle){ 280, 32, 24, 24 }, NULL); 
    state->viewInterruptPressed = GuiButton((Rectangle){ 312, 32, 24, 24 }, NULL); 
    state->viewAssemblyPressed = GuiButton((Rectangle){ 344, 32, 24, 24 }, NULL); 
    GuiPanel((Rectangle){ 488, 24, 312, 40 }, NULL);
    GuiLabel((Rectangle){ 504, 32, 56, 24 }, "Controls");
    state->controlPlayPressed = GuiButton((Rectangle){ 568, 32, 24, 24 }, NULL); 
    state->controlPausePressed = GuiButton((Rectangle){ 600, 32, 24, 24 }, NULL); 
    state->controlStopPressed = GuiButton((Rectangle){ 632, 32, 24, 24 }, NULL); 
    state->controlStepPressed = GuiButton((Rectangle){ 664, 32, 24, 24 }, NULL); 
    state->controlStepIntoPressed = GuiButton((Rectangle){ 696, 32, 24, 24 }, NULL); 
    state->controlStepToBreakpointPressed = GuiButton((Rectangle){ 728, 32, 24, 24 }, NULL); 
    state->controlStepToCursorPressed = GuiButton((Rectangle){ 760, 32, 24, 24 }, NULL); 
    state->viewBytecodePressed = GuiButton((Rectangle){ 384, 32, 24, 24 }, NULL); 
    state->viewConsolePressed = GuiButton((Rectangle){ 416, 32, 24, 24 }, NULL); 
    GuiPanel((Rectangle){ 800, 24, 232, 40 }, NULL);
    GuiPanel((Rectangle){ 1160, 24, 120, 40 }, NULL);
    GuiLabel((Rectangle){ 208, 32, 32, 24 }, "View");
    GuiLabel((Rectangle){ 16, 32, 32, 24 }, "File");
    state->preferencesButtonPressed = GuiButton((Rectangle){ 1176, 32, 24, 24 }, NULL); 
    state->aboutButtonPressed = GuiButton((Rectangle){ 1208, 32, 24, 24 }, NULL); 
    state->helpButtonPressed = GuiButton((Rectangle){ 1240, 32, 24, 24 }, NULL); 
    state->loadFileToMemoryPressed = GuiButton((Rectangle){ 816, 32, 24, 24 }, NULL); 
    state->clearFileFromMemoryPressed = GuiButton((Rectangle){ 848, 32, 24, 24 }, NULL); 
    state->viewOverviewPressed = GuiButton((Rectangle){ 448, 32, 24, 24 }, NULL); 
    GuiPanel((Rectangle){ 1032, 24, 128, 40 }, NULL);
    if (GuiDropdownBox((Rectangle){ 880, 32, 136, 24 }, "Choose Exec", &state->fileToExecuteActive, state->fileToExecuteEditMode)) state->fileToExecuteEditMode = !state->fileToExecuteEditMode;
    
    GuiUnlock();
}

#endif // GUI_MENUBAR_IMPLEMENTATION
