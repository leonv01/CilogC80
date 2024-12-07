#ifndef GUI_TOAST_H
#define GUI_TOAST_H

#include "raylib.h"
#include "utils/utils.h"
#include <time.h>

typedef enum
{
    GUI_TOAST_MESSAGE = 0,
    GUI_TOAST_WARNING,
    GUI_TOAST_SUCCESS,
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

    time_t startTime;
    long long durationTime;
    bool toastRunning;

    /* -------------------------------------------------------------------------- */
} GuiToastState;

GuiToastState InitGuiToast(void);
void GuiToast(GuiToastState *state);
void GuiToastDisplayMessage(GuiToastState *state, char *text, long duration, GuiToastType toastType);
void GuiToastHideMessage(GuiToastState *state);

#endif // GUI_TOAST_H

#ifdef GUI_TOAST_IMPLEMENTATION

#include "raygui.h"
#include <stdio.h>

#include "gui_defines.h"

GuiToastState InitGuiToast(void)
{
    GuiToastState state = { 0 };

    state.padding = 12;
    state.fontSize = 10;
    state.bounds = (Rectangle){ 0, MENU_BAR_HEIGHT, GetScreenWidth(), state.fontSize + state.padding * 2 };
    state.isWindowActive = false;

    state.minHeight = 100;
    state.minWidth = 100;

    state.toastText = "Toast message";
    state.toastTextWidth = MeasureText(state.toastText, state.fontSize);
    state.toastTextHeight = state.fontSize;

    state.toastType = GUI_TOAST_MESSAGE;
    
    state.toastRunning = false;

    return state;
}

void GuiToast(GuiToastState *state)
{
    if(state->isWindowActive == true)
    {

        time_t endTime = clock();

        if((endTime - state->startTime) > state->durationTime)
        {
            GuiToastHideMessage(state);
        }

        state->bounds.x = 0;
        state->bounds.y = 40;
        state->bounds.width = GetScreenWidth();
        state->bounds.height = state->fontSize + state->padding * 2;

        Color toastBackgroundColor;
        Color outlineColor;

        switch(state->toastType)
        {
            case GUI_TOAST_MESSAGE:
            {
                toastBackgroundColor = (Color){ 230, 230, 230, 128 };
                outlineColor = BLACK;
            } break;
            case GUI_TOAST_WARNING:
            {
                toastBackgroundColor = (Color){ 227, 145, 64, 128 };
                outlineColor = ORANGE;
            } break;
            case GUI_TOAST_ERROR:
            {
                toastBackgroundColor = (Color){ 227, 64, 64, 128 };
                outlineColor = RED;
            } break;
            case GUI_TOAST_SUCCESS:
            {
                toastBackgroundColor = (Color){ 64, 227, 64, 128 };
                outlineColor = GREEN;
            } break;
            default:
            {
                toastBackgroundColor = (Color){ 230, 230, 230, 128 };
                outlineColor = BLACK;
            } break;
        }

        DrawRectangleRec(state->bounds, toastBackgroundColor);
        DrawRectangleLinesEx(state->bounds, 2, outlineColor);

        DrawText(state->toastText, state->padding , state->bounds.y + state->padding, state->fontSize, BLACK);
    }
}

void GuiToastDisplayMessage(GuiToastState *state, char *text, long duration, GuiToastType toastType)
{
    state->startTime = clock();
    state->durationTime = duration;
    state->toastType = toastType;
    state->toastText = text;
    state->isWindowActive = true;
    state->toastRunning = true;
}

void GuiToastHideMessage(GuiToastState *state)
{
    state->isWindowActive = false;
    state->toastRunning = false;
}

#endif // GUI_TOAST_IMPLEMENTATION