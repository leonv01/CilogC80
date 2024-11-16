#ifndef GUI_TOOLTIPTTEXT_H
#define GUI_TOOLTIPTTEXT_H

#include "raylib.h"

typedef struct
{
    char *text;
    Rectangle bounds;
    bool isActive;
} GuiTooltipTextState;

#endif // GUI_TOOLTIPTTEXT_H

#if defined(GUI_TOOLTIPTTEXT_IMPLEMENTATION)
#include "raygui.h"

GuiTooltipTextState InitGuiToolTipText(char *text, Rectangle bounds)
{
    GuiTooltipTextState state = { 0 };
    state.text = text;
    state.bounds = bounds;
    state.isActive = false;

    return state;
}

void GuiToolTipText(GuiTooltipTextState *state)
{
    if (state->isActive)
    {
        Vector2 mousePosition = GetMousePosition();

        // Adjust tooltip position near mouse
        float tooltipX = mousePosition.x + 10;
        float tooltipY = mousePosition.y + 10;

        // Ensure tooltip doesn't go off-screen
        if (tooltipX + state->bounds.width > GetScreenWidth())
            tooltipX = GetScreenWidth() - state->bounds.width - 10;
        if (tooltipY + state->bounds.height > GetScreenHeight())
            tooltipY = GetScreenHeight() - state->bounds.height - 10;

        // Draw tooltip background
        DrawRectangle(
            tooltipX,
            tooltipY,
            state->bounds.width,
            state->bounds.height,
            Fade(BLACK, 0.8f));

        // Draw tooltip text
        DrawText(
            state->text,
            tooltipX + 5,
            tooltipY + 5,
            10,
            RAYWHITE); // Use a contrasting text color
    }
}


#endif // GUI_TOOLTIPTTEXT_IMPLEMENTATION