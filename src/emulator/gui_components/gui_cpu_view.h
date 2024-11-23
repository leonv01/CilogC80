#ifndef GUI_CPU_VIEW_H
#define GUI_CPU_VIEW_H

#include "raylib.h"

#define CPU_VIEW_SPACING(size, padding, count) ((size + padding) * count)

typedef struct
{

    Rectangle bounds;
    Vector2 position;
    int width;
    int height;
    int minWidth;
    int minHeight;
    int padding;
    int fontSize;


    /* ------------------------- Register GUI attributes ------------------------ */
    const char *registerALabelText;
    const char *registerBLabelText;
    const char *registerCLabelText;
    const char *registerDLabelText;
    const char *registerELabelText;
    const char *registerHLabelText;
    const char *registerLLabelText;

    char *registerAValue;
    char *registerBValue;
    char *registerCValue;
    char *registerDValue;
    char *registerEValue;
    char *registerHValue;
    char *registerLValue;

    int labelWidth;
    int labelHeight;

    int registerTextWidth;
    /* -------------------------------------------------------------------------- */

    Rectangle resizerBounds;

    bool isWindowActive;
    bool isWindowMinimized;

    bool dragMode;
    bool supportDrag;

    bool resizeMode;
    bool supportResize;
    Vector2 panOffset;

} GuiCpuViewState;

GuiCpuViewState InitGuiCpuView(const Vector2 position, const int width, const int height);
void GuiCpuView(GuiCpuViewState *state);

#endif // GUI_CPU_VIEW_H

#ifdef GUI_CPU_VIEW_IMPLEMENTATION

#include "raygui.h"

GuiCpuViewState InitGuiCpuView(const Vector2 position, const int width, const int height)
{
    GuiCpuViewState state = { 0 };  

    state.position = position;
    state.width = width;
    state.height = height;
    state.padding = 12;
    state.fontSize = 20;
    state.bounds = (Rectangle){ position.x, position.y, width, height }; 
    state.isWindowActive = false;
    state.isWindowMinimized = false;

    state.minHeight = 100;
    state.minWidth = 100;

    state.labelWidth = 100;
    state.labelHeight = 20;

    /* -------------------------- Register GUI elements ------------------------- */
    state.registerALabelText = "Register A";
    state.registerBLabelText = "Register B";
    state.registerCLabelText = "Register C";
    state.registerDLabelText = "Register D";
    state.registerELabelText = "Register E";
    state.registerHLabelText = "Register H";
    state.registerLLabelText = "Register L";

    state.registerAValue = "0x00";
    state.registerBValue = "0x00";
    state.registerCValue = "0x00";
    state.registerDValue = "0x00";
    state.registerEValue = "0x00";
    state.registerHValue = "0x00";
    state.registerLValue = "0x00";

    state.registerTextWidth = 50;
    /* -------------------------------------------------------------------------- */

    state.resizerBounds = (Rectangle){ position.x + width - 16, position.y + height - 16, 16, 16 };

    state.supportDrag = true;
    state.dragMode = false;
    state.panOffset = (Vector2){ 0, 0 };

    state.supportResize = true;
    state.resizeMode = false;

    return state;
}

void GuiCpuView(GuiCpuViewState *state)
{
    // Only update if window is visible
    // TODO: Minimize logic idk
    if(state->isWindowActive == true)
    {
        // Only update position if window supports dragging
        if(state->supportDrag == true)
        {
            Vector2 mousePosition = GetMousePosition();

            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                if(CheckCollisionPointRec(mousePosition, (Rectangle){ state->bounds.x, state->bounds.y, (float)state->bounds.width, RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT }))
                {
                    state->dragMode = true;
                    state->panOffset.x = mousePosition.x - state->bounds.x;
                    state->panOffset.y = mousePosition.y - state->bounds.y;
                }
            }

            if(state->dragMode == true)
            {
                state->bounds.x = (mousePosition.x - state->panOffset.x);
                state->bounds.y = (mousePosition.y - state->panOffset.y);

                if(state->bounds.x < 0)
                {
                    state->bounds.x = 0;
                }
                else if(state->bounds.x > (GetScreenWidth() - state->bounds.width))
                {
                    state->bounds.x = GetScreenWidth() - state->bounds.width;
                }

                // TODO: Parameterize this
                if(state->bounds.y < 40)
                {
                    state->bounds.y = 40;
                }
                else if(state->bounds.y > (GetScreenHeight() - state->bounds.height))
                {
                    state->bounds.y = GetScreenHeight() - state->bounds.height;
                }

                if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
                {
                    state->dragMode = false;
                }

                state->resizerBounds = (Rectangle){ state->bounds.x + state->bounds.width - 16, state->bounds.y + state->bounds.height - 16, 16, 16 };
            }
        }

        // TODO: Resize with content
        if(state->supportResize == true)
        {
            Vector2 mousePosition = GetMousePosition();

            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                if(CheckCollisionPointRec(mousePosition, state->resizerBounds))
                {
                    state->resizeMode = true;
                }
            }

            if(state->resizeMode == true)
            {
                state->bounds.width = mousePosition.x - state->bounds.x;
                state->bounds.height = mousePosition.y - state->bounds.y;

                if(state->bounds.width < state->minWidth)
                {
                    state->bounds.width = state->minWidth;
                }
                else if(state->bounds.width > GetScreenWidth())
                {
                    state->bounds.width = GetScreenWidth();
                }

                if(state->bounds.height < state->minHeight)
                {
                    state->bounds.height = state->minHeight;
                }
                else if(state->bounds.height > GetScreenHeight())
                {
                    state->bounds.height = GetScreenHeight();
                }

                if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
                {
                    state->resizeMode = false;
                }
            }
        }

        state->resizerBounds = (Rectangle){ state->bounds.x + state->bounds.width - 16, state->bounds.y + state->bounds.height - 16, 16, 16 };

        /* --------------------------- Render GUI elements -------------------------- */
        if(GuiWindowBox(state->bounds, "CPU view") == true)
        {
            state->isWindowMinimized = !state->isWindowMinimized;
            state->isWindowActive = false;
        }

        GuiDrawIcon(72, state->resizerBounds.x, state->resizerBounds.y, 1, BLACK);

        const Vector2 labelStartPos = (Vector2){ state->bounds.x + ( state->padding * 2 ), state->bounds.y + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT + (state->padding * 2) };

        GuiGroupBox((Rectangle)
        { 
            state->bounds.x + state->padding, 
            state->bounds.y + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT + state->padding, 
            CPU_VIEW_SPACING(state->labelWidth / 2, state->padding, 4) + state->padding, 
            CPU_VIEW_SPACING(state->labelHeight, state->padding, 4) + state->padding 
        }, "Registers");

        GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
        // Register A
        GuiLabel((Rectangle)
        { 
            labelStartPos.x + CPU_VIEW_SPACING(state->labelWidth, state->padding, 0), 
            labelStartPos.y + CPU_VIEW_SPACING(state->labelHeight, state->padding, 0), 
            state->labelWidth, 
            state->labelHeight 
        }, state->registerALabelText);
        GuiTextBox((Rectangle)
        { 
            labelStartPos.x + CPU_VIEW_SPACING(state->labelWidth / 2, state->padding, 1), 
            labelStartPos.y + CPU_VIEW_SPACING(state->labelHeight, state->padding, 0), 
            state->registerTextWidth, 
            state->labelHeight 
        }, state->registerAValue, 1, false);

        // Register B
        GuiLabel((Rectangle)
        { 
            labelStartPos.x + CPU_VIEW_SPACING(state->labelWidth, state->padding, 0), 
            labelStartPos.y + CPU_VIEW_SPACING(state->labelHeight, state->padding, 1), 
            state->labelWidth, 
            state->labelHeight 
        }, state->registerBLabelText);
        GuiTextBox((Rectangle)
        { 
            labelStartPos.x + CPU_VIEW_SPACING(state->labelWidth / 2, state->padding, 1), 
            labelStartPos.y + CPU_VIEW_SPACING(state->labelHeight, state->padding, 1), 
            state->registerTextWidth, 
            state->labelHeight 
        }, state->registerBValue, 1, false);

        // Register C
        GuiLabel((Rectangle)
        { 
            labelStartPos.x + CPU_VIEW_SPACING(state->labelWidth / 2, state->padding, 2), 
            labelStartPos.y + CPU_VIEW_SPACING(state->labelHeight, state->padding, 1), 
            state->labelWidth, 
            state->labelHeight 
        }, state->registerCLabelText);
        GuiTextBox((Rectangle)
        { 
            labelStartPos.x + CPU_VIEW_SPACING(state->labelWidth / 2, state->padding, 3), 
            labelStartPos.y + CPU_VIEW_SPACING(state->labelHeight, state->padding, 1), 
            state->registerTextWidth, 
            state->labelHeight 
        }, state->registerCValue, 1, false);

        // Register D
        GuiLabel((Rectangle)
        { 
            labelStartPos.x + CPU_VIEW_SPACING(state->labelWidth, state->padding, 0), 
            labelStartPos.y + CPU_VIEW_SPACING(state->labelHeight, state->padding, 2), 
            state->labelWidth, 
            state->labelHeight 
        }, state->registerDLabelText);
        GuiTextBox((Rectangle)
        { 
            labelStartPos.x + CPU_VIEW_SPACING(state->labelWidth / 2, state->padding, 1), 
            labelStartPos.y + CPU_VIEW_SPACING(state->labelHeight, state->padding, 2), 
            state->registerTextWidth, 
            state->labelHeight 
        }, state->registerDValue, 1, false);

        // Register E
        GuiLabel((Rectangle)
        { 
            labelStartPos.x + CPU_VIEW_SPACING(state->labelWidth / 2, state->padding, 2), 
            labelStartPos.y + CPU_VIEW_SPACING(state->labelHeight, state->padding, 2), 
            state->labelWidth, 
            state->labelHeight 
        }, state->registerELabelText);
        GuiTextBox((Rectangle)
        { 
            labelStartPos.x + CPU_VIEW_SPACING(state->labelWidth / 2, state->padding, 3), 
            labelStartPos.y + CPU_VIEW_SPACING(state->labelHeight, state->padding, 2), 
            state->registerTextWidth, 
            state->labelHeight 
        }, state->registerEValue, 1, false);

        // Register H
        GuiLabel((Rectangle)
        { 
            labelStartPos.x + CPU_VIEW_SPACING(state->labelWidth, state->padding, 0), 
            labelStartPos.y + CPU_VIEW_SPACING(state->labelHeight, state->padding, 3), 
            state->labelWidth, 
            state->labelHeight 
        }, state->registerHLabelText);
        GuiTextBox((Rectangle)
        { 
            labelStartPos.x + CPU_VIEW_SPACING(state->labelWidth / 2, state->padding, 1), 
            labelStartPos.y + CPU_VIEW_SPACING(state->labelHeight, state->padding, 3), 
            state->registerTextWidth, 
            state->labelHeight 
        }, state->registerHValue, 1, false);

        // Register L
        GuiLabel((Rectangle)
        { 
            labelStartPos.x + CPU_VIEW_SPACING(state->labelWidth / 2, state->padding, 2), 
            labelStartPos.y + CPU_VIEW_SPACING(state->labelHeight, state->padding, 3), 
            state->labelWidth, 
            state->labelHeight 
        }, state->registerLLabelText);
        GuiTextBox((Rectangle)
        { 
            labelStartPos.x + CPU_VIEW_SPACING(state->labelWidth / 2, state->padding, 3), 
            labelStartPos.y + CPU_VIEW_SPACING(state->labelHeight, state->padding, 3), 
            state->registerTextWidth, 
            state->labelHeight 
        }, state->registerLValue, 1, false);

        /* -------------------------------------------------------------------------- */
    }
}

#endif // GUI_CPU_VIEW_IMPLEMENTATION