#ifndef GUI_EMULATOR_INFO_VIEW_H
#define GUI_EMULATOR_INFO_VIEW_H

#include "raylib.h"

typedef struct
{
    Rectangle bounds;
    Vector2 position;

} GuiEmulatorInfoViewState;

GuiEmulatorInfoViewState InitGuiEmulatorInfo(const Vector2 pos);
void GuiEmulatorInfoView(GuiEmulatorInfoViewState *state);

#endif //GUI_EMULATOR_INFO_VIEW_H

#ifdef GUI_EMULATOR_INFO_VIEW_IMPLEMENTATION

GuiEmulatorInfoViewState InitGuiEmulatorInfo(const Vector2 pos)
{
    GuiEmulatorInfoViewState state = { 0 };

    state.position = pos;

    return state;
}

void GuiEmulatorInfoView(GuiEmulatorInfoViewState *state)
{
    
}

#endif //GUI_EMULATOR_INFO_VIEW_IMPLEMENTATION