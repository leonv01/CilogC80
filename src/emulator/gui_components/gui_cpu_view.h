#ifndef GUI_CPU_VIEW_H
#define GUI_CPU_VIEW_H

#include "raylib.h"

typedef struct
{
    const char *registerALabelText;
    const char *registerBLabelText;
    const char *registerCLabelText;
    const char *registerDLabelText;
    const char *registerELabelText;
    const char *registerHLabelText;
    const char *registerLLabelText;

    Rectangle bounds;
    Vector2 position;
    int width;
    int height;
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
    state.padding = 8;
    state.fontSize = 20;
    state.bounds = (Rectangle){ position.x, position.y, width, height }; 
    state.isWindowActive = false;
    state.isWindowMinimized = false;

    state.resizerBounds = (Rectangle){ position.x + width - 16, position.y + height - 16, 16, 16 };

    state.registerALabelText = "Register A";
    state.registerBLabelText = "Register B";
    state.registerCLabelText = "Register C";
    state.registerDLabelText = "Register D";
    state.registerELabelText = "Register E";
    state.registerHLabelText = "Register H";
    state.registerLLabelText = "Register L";

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

                if(state->bounds.y < 0)
                {
                    state->bounds.y = 0;
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

                if(state->bounds.width < 100)
                {
                    state->bounds.width = 100;
                }
                else if(state->bounds.width > GetScreenWidth())
                {
                    state->bounds.width = GetScreenWidth();
                }

                if(state->bounds.height < 100)
                {
                    state->bounds.height = 100;
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
        /* -------------------------------------------------------------------------- */
    }


}

#endif // GUI_CPU_VIEW_IMPLEMENTATION