/*******************************************************************************************
*
*   FooterBar v1.0.0 - Tool Description
*
*   MODULE USAGE:
*       #define GUI_FOOTER_BAR_IMPLEMENTATION
*       #include "gui_footer_bar.h"
*
*       INIT: GuiFooterBarState state = InitGuiFooterBar();
*       DRAW: GuiFooterBar(&state);
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

#ifndef GUI_FOOTER_BAR_H
#define GUI_FOOTER_BAR_H

typedef struct {

    // Custom state variables (depend on development software)
    // NOTE: This variables should be added manually if required

} GuiFooterBarState;

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
GuiFooterBarState InitGuiFooterBar(void);
void GuiFooterBar(GuiFooterBarState *state);

#ifdef __cplusplus
}
#endif

#endif // GUI_FOOTER_BAR_H

/***********************************************************************************
*
*   GUI_FOOTER_BAR IMPLEMENTATION
*
************************************************************************************/
#if defined(GUI_FOOTER_BAR_IMPLEMENTATION)

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
GuiFooterBarState InitGuiFooterBar(void)
{
    GuiFooterBarState state = { 0 };


    // Custom variables initialization

    return state;
}

void GuiFooterBar(GuiFooterBarState *state)
{
    GuiPanel((Rectangle){ 0, 696, 1280, 24 }, NULL);
}

#endif // GUI_FOOTER_BAR_IMPLEMENTATION
