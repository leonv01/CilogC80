#ifndef GUI_MEMORY_VIEW_H
#define GUI_MEMORY_VIEW_H

#include "raylib.h"

#define MEMORY_VIEW_SPACING(size, padding, count) ((size + padding) * count)

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
    bool isWindowMinimized;

    bool dragMode;
    bool supportDrag;

    bool resizeMode;
    bool supportResize;

    Vector2 panOffset;
    /* -------------------------------------------------------------------------- */

    /* ------------------------- Memory view GUI attributes ---------------------- */
    const char *memoryAddressLabelText;

    char memoryAddressPos_X[16][3];
    char memoryAddressPosX_[16][5];
    char memoryEntry[16][16][3];

    int memoryAddressLabelWidth;
    int memoryAddressLabelHeight;

    int memoryAddressPosLabelWidth;

    int memoryAddressTextWidth;

    Rectangle memoryPanelBounds;

    int memoryAddressSpinnerValue;
    bool memoryAddressSpinnerActive;
    /* -------------------------------------------------------------------------- */

} GuiMemoryViewState;

GuiMemoryViewState InitGuiMemoryView(const Vector2 position, const int width, const int height);
void GuiMemoryView(GuiMemoryViewState *state);
void GuiMemoryViewAddressUpdate(GuiMemoryViewState *state, const int address, const uint8_t *memory, const size_t memorySize);

#endif // GUI_MEMORY_VIEW_H

#ifdef GUI_MEMORY_VIEW_IMPLEMENTATION

#include "raygui.h"

GuiMemoryViewState InitGuiMemoryView(const Vector2 position, const int width, const int height)
{
    GuiMemoryViewState state = { 0 };

    /* ---------------------------- Window attributes --------------------------- */
    state.position = position;
    state.width = width;
    state.height = height;
    state.padding = 12;
    state.fontSize = 20;
    state.bounds = (Rectangle){ position.x, position.y, width, height };

    state.minHeight = 100;
    state.minWidth = 100;
    
    state.resizerBounds = (Rectangle){ state.bounds.x + state.bounds.width - 16, state.bounds.y + state.bounds.height - 16, 16, 16 };

    state.isWindowActive = false;
    state.isWindowMinimized = false;

    state.supportDrag = true;
    state.dragMode = false;

    state.supportResize = true;
    state.resizeMode = false;

    state.panOffset = (Vector2){ 0, 0 };
    /* -------------------------------------------------------------------------- */

    /* ------------------------- Memory view GUI attributes ---------------------- */
    state.memoryAddressLabelText = "Address";

    for(size_t i = 0; i < 0x10; i++)
    {
        sprintf(state.memoryAddressPos_X[i], "%X", i);
    }
    for(size_t i = 0; i < 0x10; i++)
    {
        sprintf(state.memoryAddressPosX_[i], "%03X0", i);
    }
    for(size_t x = 0; x < 0x10; x++)
    {
        for (size_t y = 0; y < 0x10; y++)
        {
            sprintf(state.memoryEntry[x][y], "00");
        }
        
    }

    state.memoryAddressSpinnerValue = 0;
    state.memoryAddressSpinnerActive = false;

    state.memoryAddressLabelWidth = 100;
    state.memoryAddressLabelHeight = 20;

    state.memoryAddressPosLabelWidth = 10;

    state.memoryAddressTextWidth = 50;
    /* -------------------------------------------------------------------------- */
    return state;
}
void GuiMemoryView(GuiMemoryViewState *state)
{
    if(state->isWindowActive == true)
    {
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
            }
        }

        if(state->supportResize == true)
        {
            // TODO: Resize with content
        }

        state->resizerBounds = (Rectangle){ state->bounds.x + state->bounds.width - 16, state->bounds.y + state->bounds.height - 16, 16, 16 };

        /* --------------------------- Render GUI elements -------------------------- */
        if(GuiWindowBox(state->bounds, "Memory view") == true)
        {
            state->isWindowMinimized = !state->isWindowMinimized;
            state->isWindowActive = false;
        }

        GuiDrawIcon(72, state->resizerBounds.x, state->resizerBounds.y, 1, BLACK);

        /* ---------------------------- Address elements ---------------------------- */
        const Vector2 labelStartPos = (Vector2)
        {
            state->bounds.x + (state->padding * 2),
            state->bounds.y + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT + (state->padding * 2)
        };
        const int addressPosStartX = MEMORY_VIEW_SPACING(state->memoryAddressLabelWidth / 2, state->padding, 1);
        const int addressGroupBoxWidth = addressPosStartX + MEMORY_VIEW_SPACING(state->memoryAddressLabelHeight / 2, state->padding, 16) + state->padding;
        const int addressGroupBoxHeight = MEMORY_VIEW_SPACING(state->memoryAddressLabelHeight / 2, state->padding, 19);

        GuiGroupBox((Rectangle)
        {
            state->bounds.x + state->padding,
            state->bounds.y + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT + state->padding,
            addressGroupBoxWidth,
            addressGroupBoxHeight
        }, "Memory");
        
        // Address label
        GuiLabel((Rectangle)
        {
            labelStartPos.x + MEMORY_VIEW_SPACING(state->memoryAddressLabelWidth / 2, state->padding, 0),
            labelStartPos.y + MEMORY_VIEW_SPACING(state->memoryAddressLabelHeight / 2, state->padding, 0),
            state->memoryAddressLabelWidth,
            state->memoryAddressLabelHeight
        }, state->memoryAddressLabelText);

        for(size_t i = 0; i < 0x10; i++)
        {
            GuiLabel((Rectangle)
            {
                labelStartPos.x + addressPosStartX + MEMORY_VIEW_SPACING(state->memoryAddressLabelHeight / 2, state->padding, i),
                labelStartPos.y + MEMORY_VIEW_SPACING(state->memoryAddressLabelHeight / 2, state->padding, 0),
                state->memoryAddressLabelWidth,
                state->memoryAddressLabelHeight
            }, state->memoryAddressPos_X[i]);
        }

        for(size_t i = 0; i < 0x10; i++)
        {
            const char *labelText = state->memoryAddressPosX_[i];
            GuiLabel((Rectangle)
            {
                labelStartPos.x + MEMORY_VIEW_SPACING(state->memoryAddressLabelWidth / 2, state->padding, 0),
                labelStartPos.y + MEMORY_VIEW_SPACING(state->memoryAddressLabelHeight / 2, state->padding, 1) + MEMORY_VIEW_SPACING(state->memoryAddressLabelHeight / 2, state->padding, i),
                state->memoryAddressLabelWidth,
                state->memoryAddressLabelHeight
            }, labelText);
        }

        for(size_t x = 0; x < 0x10; x++)
        {
            for(size_t y = 0; y < 0x10; y++)
            {
                GuiLabel((Rectangle)
                {
                    labelStartPos.x + addressPosStartX + MEMORY_VIEW_SPACING(state->memoryAddressLabelHeight / 2, state->padding, y),
                    labelStartPos.y + MEMORY_VIEW_SPACING(state->memoryAddressLabelHeight / 2, state->padding, 1) + MEMORY_VIEW_SPACING(state->memoryAddressLabelHeight / 2, state->padding, x),
                    state->memoryAddressLabelWidth,
                    state->memoryAddressLabelHeight
                }, state->memoryEntry[x][y]);
            }
        }
        
        if(GuiSpinner((Rectangle)
        {
            labelStartPos.x + MEMORY_VIEW_SPACING(state->memoryAddressLabelWidth / 2, state->padding, 2),
            labelStartPos.y + MEMORY_VIEW_SPACING(state->memoryAddressLabelHeight / 2, state->padding, 17),
            state->memoryAddressLabelWidth + MEMORY_VIEW_SPACING(state->memoryAddressLabelWidth / 2, state->padding, 2),
            state->memoryAddressLabelHeight
        }, "Memory address page", &state->memoryAddressSpinnerValue, 0, 0xFF, state->memoryAddressSpinnerActive))
        {
            state->memoryAddressSpinnerActive = !state->memoryAddressSpinnerActive;
        }
        /* -------------------------------------------------------------------------- */
        /* -------------------------------------------------------------------------- */

    }
}

void GuiMemoryViewAddressUpdate(GuiMemoryViewState *state, const int address, const uint8_t *memory, const size_t memorySize)
{
    if(state->isWindowActive == true)
    {
        if(state->memoryAddressSpinnerValue * 0x0100 <= memorySize)
        {
            for(size_t x = 0; x < 0x10; x++)
            {
                sprintf(state->memoryAddressPosX_[x], "%03X0", x + (state->memoryAddressSpinnerValue * 0x10));
                for (size_t y = 0; y < 0x10; y++)
                {
                    sprintf(state->memoryEntry[x][y], "%02X", memory[(state->memoryAddressSpinnerValue * 0x0100) + (x * 0x10) + y]);
                }
                
            }
        }
    }
}

#endif // GUI_MEMORY_VIEW_IMPLEMENTATION