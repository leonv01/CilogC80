#ifndef GUI_TOAST_H
#define GUI_TOAST_H

#include "raylib.h"

typedef enum
{
    GUI_TOAST_MESSAGE = 0,
    GUI_TOAST_WARNING,
    GUI_TOAST_ERROR
} GuiToastType;

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

    bool dragMode;
    bool supportDrag;

    Vector2 panOffset;

    GuiToastType toastType;

    qword_t toastDuration;
    qword_t toastStartTime;
    /* -------------------------------------------------------------------------- */

    /* ------------------------- Toast GUI attributes ---------------------------- */
    const char *toastText;

    int toastTextWidth;
    int toastTextHeight;
    /* -------------------------------------------------------------------------- */
} GuiToastState;

GuiToastState InitGuiToast(const Vector2 position, const int width, const int height);
void GuiToast(GuiToastState *state);
void GuiToastShowMessage(GuiToastState *state, const char *message, GuiToastType type);

#endif // GUI_TOAST_H

#ifdef GUI_TOAST_IMPLEMENTATION

#include "raygui.h"

#include <time.h>

GuiToastState InitGuiToast(const Vector2 position, const int width, const int height)
{
    GuiToastState state = { 0 };

    state.position.x = position.x - (width / 2);
    state.position.y = position.y - (height / 2);
    state.width = width;
    state.height = height;
    state.padding = 12;
    state.fontSize = 20;
    state.bounds = (Rectangle){ position.x + state.padding, position.y + state.padding, width, height };
    state.isWindowActive = false;

    state.minHeight = 100;
    state.minWidth = 100;

    state.toastText = "Toast message";
    state.toastTextWidth = MeasureText(state.toastText, state.fontSize);
    state.toastTextHeight = state.fontSize;

    state.toastType = GUI_TOAST_MESSAGE;

    return state;
}

void GuiToast(GuiToastState *state)
{
    if(state->isWindowActive == true)
    {
        state->bounds.x = state->position.x;
        state->bounds.y = state->position.y;

        Color toastBackgroundColor;
        Color outlineColor;

        switch(state->toastType)
        {
            case GUI_TOAST_MESSAGE:
            {
                toastBackgroundColor = (Color){ 230, 230, 230, 255 };
                outlineColor = BLACK;
            } break;
            case GUI_TOAST_WARNING:
            {
                toastBackgroundColor = (Color){ 227, 145, 64, 255 };
                outlineColor = ORANGE;
            } break;
            case GUI_TOAST_ERROR:
            {
                toastBackgroundColor = (Color){ 227, 64, 64, 255 };
                outlineColor = RED;
            } break;
            default:
            {
                toastBackgroundColor = (Color){ 230, 230, 230, 255 };
                outlineColor = BLACK;
            } break;
        }

        DrawRectangleRec(state->bounds, toastBackgroundColor);
        DrawRectangleLinesEx(state->bounds, 2, outlineColor);
        
        GuiLabel((Rectangle){ state->bounds.x + state->padding, state->bounds.y + state->padding, state->toastTextWidth, state->toastTextHeight }, state->toastText);
    }
}

void GuiToastShowMessage(GuiToastState *state, const char *message, GuiToastType type)
{

}



#endif // GUI_TOAST_IMPLEMENTATION