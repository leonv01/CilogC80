#ifndef GUI_CPU_VIEW_H
#define GUI_CPU_VIEW_H

#include "raylib.h"

#define CPU_VIEW_SPACING(size, padding, count) ((size + padding) * count)

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
    /* -------------------------------------------------------------------------- */

    /* ------------------------- Register GUI attributes ------------------------ */
    const char *registerALabelText;
    const char *registerBLabelText;
    const char *registerCLabelText;
    const char *registerDLabelText;
    const char *registerELabelText;
    const char *registerHLabelText;
    const char *registerLLabelText;

    char registerAValue[5];
    char registerBValue[5];
    char registerCValue[5];
    char registerDValue[5];
    char registerEValue[5];
    char registerHValue[5];
    char registerLValue[5];

    int registerLabelWidth;
    int registerLabelHeight;

    int registerTextWidth;

    bool registerATextActive;
    bool registerBTextActive;
    bool registerCTextActive;
    bool registerDTextActive;
    bool registerETextActive;
    bool registerHTextActive;
    bool registerLTextActive;
    /* -------------------------------------------------------------------------- */

    /* --------------------------- Flag GUI attributes -------------------------- */
    const char *flagCLabelText;
    const char *flagNLabelText;
    const char *flagPLabelText;
    const char *flagHLabelText;
    const char *flagZLabelText;
    const char *flagSLabelText;

    char flagCValue[2];    
    char flagNValue[2];
    char flagPValue[2];
    char flagHValue[2];
    char flagZValue[2];
    char flagSValue[2];

    int flagLabelWidth;
    int flagLabelHeight;

    int flagTextWidth;

    bool flagCTextActive;
    bool flagNTextActive;
    bool flagPTextActive;
    bool flagHTextActive;
    bool flagZTextActive;
    bool flagSTextActive;
    /* -------------------------------------------------------------------------- */

    /* ------------------------- Pointer GUI attributes ------------------------- */
    const char *programCounterLabelText;
    const char *stackPointerLabelText;

    char programCounterValue[7];
    char stackPointerValue[7];

    int pointerLabelWidth;
    int pointerLabelHeight;

    int pointerTextWidth;

    bool pointerPcTextActive;
    bool pointerSpTextActive;
    /* -------------------------------------------------------------------------- */

    Rectangle resizerBounds;

    bool isWindowActive;
    bool isWindowMinimized;

    bool dragMode;
    bool supportDrag;

    bool resizeMode;
    bool supportResize;
    Vector2 panOffset;

    bool registersEditable;
    bool flagsEditable;
    bool pointersEditable;

    bool updateCpuView;
} GuiCpuViewState;

GuiCpuViewState InitGuiCpuView(const Vector2 position, const int width, const int height);
void GuiCpuView(GuiCpuViewState *state);
void GuiCpuViewUpdateRegisters(GuiCpuViewState *state, const uint8_t a, const uint8_t b, const uint8_t c, const uint8_t d, const uint8_t e, const uint8_t h, const uint8_t l);
void GuiCpuViewUpdateFlags(GuiCpuViewState *state, const uint8_t c, const uint8_t n, const uint8_t p, const uint8_t h, const uint8_t z, const uint8_t s);
void GuiCpuViewUpdatePointers(GuiCpuViewState *state, const uint16_t pc, const uint16_t sp);
void GuiCpuViewUpdateFontSize(GuiCpuViewState *state, const int fontSize);
void GuiCpuViewUpdate(GuiCpuViewState *state, bool update);

#endif // GUI_CPU_VIEW_H

#ifdef GUI_CPU_VIEW_IMPLEMENTATION

#include "raygui.h"

GuiCpuViewState InitGuiCpuView(const Vector2 position, const int width, const int height)
{
    GuiCpuViewState state = { 0 };  

    state.position = (Vector2){ position.x - (width / 2), position.y - (height / 2) };
    state.width = width;
    state.height = height;
    state.padding = 12;
    state.fontSize = 20;
    state.bounds = (Rectangle){ position.x, position.y, width, height }; 
    state.isWindowActive = false;
    state.isWindowMinimized = false;

    state.minHeight = 100;
    state.minWidth = 100;

    state.registerLabelWidth = 100;
    state.registerLabelHeight = 20;

    /* -------------------------- Register GUI elements ------------------------- */
    state.registerALabelText = "Register A";
    state.registerBLabelText = "Register B";
    state.registerCLabelText = "Register C";
    state.registerDLabelText = "Register D";
    state.registerELabelText = "Register E";
    state.registerHLabelText = "Register H";
    state.registerLLabelText = "Register L";

    char *initText = "0x00";
    strcpy(state.registerAValue, initText);
    strcpy(state.registerBValue, initText);
    strcpy(state.registerCValue, initText);
    strcpy(state.registerDValue, initText);
    strcpy(state.registerEValue, initText);
    strcpy(state.registerHValue, initText);
    strcpy(state.registerLValue, initText);

    state.registerTextWidth = 50;

    state.registerATextActive = false;
    state.registerBTextActive = false;
    state.registerCTextActive = false;
    state.registerDTextActive = false;
    state.registerETextActive = false;
    state.registerHTextActive = false;
    state.registerLTextActive = false;
    /* -------------------------------------------------------------------------- */

    /* --------------------------- Flag GUI elements ---------------------------- */
    state.flagCLabelText = "C";
    state.flagNLabelText = "N";
    state.flagPLabelText = "P";
    state.flagHLabelText = "H";
    state.flagZLabelText = "Z";
    state.flagSLabelText = "S";

    strcpy(state.flagCValue, "0");
    strcpy(state.flagNValue, "0");
    strcpy(state.flagPValue, "0");
    strcpy(state.flagHValue, "0");
    strcpy(state.flagZValue, "0");
    strcpy(state.flagSValue, "0");

    state.flagLabelWidth = 20;
    state.flagLabelHeight = 20;

    state.flagCTextActive = false;
    state.flagNTextActive = false;
    state.flagPTextActive = false;
    state.flagHTextActive = false;
    state.flagZTextActive = false;
    state.flagSTextActive = false;
    /* -------------------------------------------------------------------------- */

    /* ------------------------- Pointer GUI elements --------------------------- */
    state.programCounterLabelText = "Program counter";
    state.stackPointerLabelText = "Stack pointer";

    strcpy(state.programCounterValue, "0x0000");
    strcpy(state.stackPointerValue, "0x0000");

    state.pointerLabelWidth = 100;
    state.pointerLabelHeight = 20;

    state.pointerTextWidth = 50;

    state.pointerPcTextActive = false;
    state.pointerSpTextActive = false;
    /* -------------------------------------------------------------------------- */

    state.resizerBounds = (Rectangle){ position.x + width - 16, position.y + height - 16, 16, 16 };

    state.supportDrag = true;
    state.dragMode = false;
    state.panOffset = (Vector2){ 0, 0 };

    state.supportResize = false;
    state.resizeMode = false;

    state.updateCpuView = false;

    state.flagsEditable = true;
    state.registersEditable = true;
    state.pointersEditable = true;

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
        state->bounds.width = CPU_VIEW_SPACING(state->registerLabelWidth / 2, state->padding, 4) + (state->padding * 3);
        state->bounds.height = CPU_VIEW_SPACING(state->registerLabelHeight, state->padding, 4) + 
                                CPU_VIEW_SPACING(state->flagLabelHeight, state->padding, 2) + 
                                CPU_VIEW_SPACING(state->pointerLabelHeight, state->padding, 2) + 
                                (state->padding * 7) + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT;
        /* --------------------------- Render GUI elements -------------------------- */
        if(GuiWindowBox(state->bounds, "CPU view") == true)
        {
            state->isWindowMinimized = !state->isWindowMinimized;
            state->isWindowActive = false;
        }

        if(state->supportResize == true)
        {
            GuiDrawIcon(72, state->resizerBounds.x, state->resizerBounds.y, 1, BLACK);
        }
        
        /* ---------------------------- Register elements --------------------------- */
        const Vector2 labelStartPos = (Vector2)
        { 
            state->bounds.x + ( state->padding * 2 ), 
            state->bounds.y + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT + (state->padding * 2) 
        };
        const int registerGroupBoxWidth = state->bounds.width - (state->padding * 2);
        const int registerGroupBoxHeight = CPU_VIEW_SPACING(state->registerLabelHeight, state->padding, 4) + state->padding;
        GuiGroupBox((Rectangle)
        { 
            state->bounds.x + state->padding, 
            state->bounds.y + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT + state->padding, 
            registerGroupBoxWidth, 
            registerGroupBoxHeight 
        }, "Registers");

        GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
        // Register A
        GuiLabel((Rectangle)
        { 
            labelStartPos.x + CPU_VIEW_SPACING(state->registerLabelWidth, state->padding, 0), 
            labelStartPos.y + CPU_VIEW_SPACING(state->registerLabelHeight, state->padding, 0), 
            state->registerLabelWidth, 
            state->registerLabelHeight 
        }, state->registerALabelText);
        if(state->registerATextActive == true)
        {
            GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
        }
        if(GuiTextBox((Rectangle)
        { 
            labelStartPos.x + CPU_VIEW_SPACING(state->registerLabelWidth / 2, state->padding, 1), 
            labelStartPos.y + CPU_VIEW_SPACING(state->registerLabelHeight, state->padding, 0), 
            state->registerTextWidth, 
            state->registerLabelHeight 
        }, state->registerAValue, 1, state->registerATextActive))
        {
            state->registerATextActive = !state->registerATextActive;
        }
        GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

        // Register B
        GuiLabel((Rectangle)
        { 
            labelStartPos.x + CPU_VIEW_SPACING(state->registerLabelWidth, state->padding, 0), 
            labelStartPos.y + CPU_VIEW_SPACING(state->registerLabelHeight, state->padding, 1), 
            state->registerLabelWidth, 
            state->registerLabelHeight 
        }, state->registerBLabelText);
        if(state->registerBTextActive == true)
        {
            GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
        }
        if(GuiTextBox((Rectangle)
        { 
            labelStartPos.x + CPU_VIEW_SPACING(state->registerLabelWidth / 2, state->padding, 1), 
            labelStartPos.y + CPU_VIEW_SPACING(state->registerLabelHeight, state->padding, 1), 
            state->registerTextWidth, 
            state->registerLabelHeight 
        }, state->registerBValue, 1, state->registerBTextActive))
        {
            state->registerBTextActive = !state->registerBTextActive;
        }
        GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

        // Register C
        GuiLabel((Rectangle)
        { 
            labelStartPos.x + CPU_VIEW_SPACING(state->registerLabelWidth / 2, state->padding, 2), 
            labelStartPos.y + CPU_VIEW_SPACING(state->registerLabelHeight, state->padding, 1), 
            state->registerLabelWidth, 
            state->registerLabelHeight 
        }, state->registerCLabelText);
        if(state->registerCTextActive == true)
        {
            GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
        }
        if(GuiTextBox((Rectangle)
        { 
            labelStartPos.x + CPU_VIEW_SPACING(state->registerLabelWidth / 2, state->padding, 3), 
            labelStartPos.y + CPU_VIEW_SPACING(state->registerLabelHeight, state->padding, 1), 
            state->registerTextWidth, 
            state->registerLabelHeight 
        }, state->registerCValue, 1, state->registerCTextActive))
        {
            state->registerCTextActive = !state->registerCTextActive;
        }
        GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

        // Register D
        GuiLabel((Rectangle)
        { 
            labelStartPos.x + CPU_VIEW_SPACING(state->registerLabelWidth, state->padding, 0), 
            labelStartPos.y + CPU_VIEW_SPACING(state->registerLabelHeight, state->padding, 2), 
            state->registerLabelWidth, 
            state->registerLabelHeight 
        }, state->registerDLabelText);
        if(state->registerDTextActive == true)
        {
            GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
        }
        if(GuiTextBox((Rectangle)
        { 
            labelStartPos.x + CPU_VIEW_SPACING(state->registerLabelWidth / 2, state->padding, 1), 
            labelStartPos.y + CPU_VIEW_SPACING(state->registerLabelHeight, state->padding, 2), 
            state->registerTextWidth, 
            state->registerLabelHeight 
        }, state->registerDValue, 1, state->registerDTextActive))
        {
            state->registerDTextActive = !state->registerDTextActive;
        }
        GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

        // Register E
        GuiLabel((Rectangle)
        { 
            labelStartPos.x + CPU_VIEW_SPACING(state->registerLabelWidth / 2, state->padding, 2), 
            labelStartPos.y + CPU_VIEW_SPACING(state->registerLabelHeight, state->padding, 2), 
            state->registerLabelWidth, 
            state->registerLabelHeight 
        }, state->registerELabelText);
        if(state->registerETextActive == true)
        {
            GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
        }
        if(GuiTextBox((Rectangle)
        { 
            labelStartPos.x + CPU_VIEW_SPACING(state->registerLabelWidth / 2, state->padding, 3), 
            labelStartPos.y + CPU_VIEW_SPACING(state->registerLabelHeight, state->padding, 2), 
            state->registerTextWidth, 
            state->registerLabelHeight 
        }, state->registerEValue, 1, state->registerETextActive))
        {
            state->registerETextActive = !state->registerETextActive;
        }
        GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

        // Register H
        GuiLabel((Rectangle)
        { 
            labelStartPos.x + CPU_VIEW_SPACING(state->registerLabelWidth, state->padding, 0), 
            labelStartPos.y + CPU_VIEW_SPACING(state->registerLabelHeight, state->padding, 3), 
            state->registerLabelWidth, 
            state->registerLabelHeight 
        }, state->registerHLabelText);
        if(state->registerHTextActive == true)
        {
            GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
        }
        if(GuiTextBox((Rectangle)
        { 
            labelStartPos.x + CPU_VIEW_SPACING(state->registerLabelWidth / 2, state->padding, 1), 
            labelStartPos.y + CPU_VIEW_SPACING(state->registerLabelHeight, state->padding, 3), 
            state->registerTextWidth, 
            state->registerLabelHeight 
        }, state->registerHValue, 1, state->registerHTextActive))
        {
            state->registerHTextActive = !state->registerHTextActive;
        }
        GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

        // Register L
        GuiLabel((Rectangle)
        { 
            labelStartPos.x + CPU_VIEW_SPACING(state->registerLabelWidth / 2, state->padding, 2), 
            labelStartPos.y + CPU_VIEW_SPACING(state->registerLabelHeight, state->padding, 3), 
            state->registerLabelWidth, 
            state->registerLabelHeight 
        }, state->registerLLabelText);
        if(state->registerLTextActive == true)
        {
            GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
        }
        if(GuiTextBox((Rectangle)
        { 
            labelStartPos.x + CPU_VIEW_SPACING(state->registerLabelWidth / 2, state->padding, 3), 
            labelStartPos.y + CPU_VIEW_SPACING(state->registerLabelHeight, state->padding, 3), 
            state->registerTextWidth, 
            state->registerLabelHeight 
        }, state->registerLValue, 1, state->registerLTextActive))
        {
            state->registerLTextActive = !state->registerLTextActive;
        }
        GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
        /* -------------------------------------------------------------------------- */

        /* ------------------------------ Flag elements ----------------------------- */
        const Vector2 flagsStartPos = (Vector2)
        { 
            state->bounds.x + ( state->padding * 2 ), 
            state->bounds.y + registerGroupBoxHeight + (state->padding * 3) + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT
        };        
        const int flagsGroupBoxWidth = state->bounds.width - (state->padding * 2);
        const int flagsGroupBoxHeight = CPU_VIEW_SPACING(state->registerLabelHeight, state->padding, 2) + state->padding;

        GuiGroupBox((Rectangle)
        { 
            flagsStartPos.x - state->padding, 
            flagsStartPos.y - state->padding,
            flagsGroupBoxWidth, 
            flagsGroupBoxHeight
        }, "Flags");

        // Flag C
        GuiLabel((Rectangle)
        { 
            flagsStartPos.x + CPU_VIEW_SPACING(state->flagLabelWidth / 2, state->padding, 0), 
            flagsStartPos.y + CPU_VIEW_SPACING(state->flagLabelHeight, state->padding, 0), 
            state->flagLabelWidth, 
            state->flagLabelHeight 
        }, state->flagCLabelText);
        if(state->flagCTextActive == true)
        {
            GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
        }
        if(GuiTextBox((Rectangle)
        { 
            flagsStartPos.x + CPU_VIEW_SPACING(state->flagLabelWidth / 2, state->padding, 0) + state->padding, 
            flagsStartPos.y + CPU_VIEW_SPACING(state->flagLabelHeight, state->padding, 0), 
            state->flagLabelWidth, 
            state->flagLabelHeight 
        }, state->flagCValue, 1, state->flagCTextActive))
        {
            state->flagCTextActive = !state->flagCTextActive;
        }
        GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

        // Flag N
        GuiLabel((Rectangle)
        { 
            flagsStartPos.x + CPU_VIEW_SPACING(state->flagLabelWidth / 2, state->padding, 4), 
            flagsStartPos.y + CPU_VIEW_SPACING(state->flagLabelHeight, state->padding, 0), 
            state->flagLabelWidth, 
            state->flagLabelHeight 
        }, state->flagNLabelText);
        if(state->flagNTextActive == true)
        {
            GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
        }
        if(GuiTextBox((Rectangle)
        { 
            flagsStartPos.x + CPU_VIEW_SPACING(state->flagLabelWidth / 2, state->padding, 4) + state->padding, 
            flagsStartPos.y + CPU_VIEW_SPACING(state->flagLabelHeight, state->padding, 0), 
            state->flagLabelWidth, 
            state->flagLabelHeight 
        }, state->flagNValue, 1, state->flagNTextActive))
        {
            state->flagNTextActive = !state->flagNTextActive;
        }
        GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

        // Flag P
        GuiLabel((Rectangle)
        { 
            flagsStartPos.x + CPU_VIEW_SPACING(state->flagLabelWidth / 2, state->padding, 8), 
            flagsStartPos.y + CPU_VIEW_SPACING(state->flagLabelHeight, state->padding, 0), 
            state->flagLabelWidth, 
            state->flagLabelHeight 
        }, state->flagPLabelText);
        if(state->flagPTextActive == true)
        {
            GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
        }
        if(GuiTextBox((Rectangle)
        { 
            flagsStartPos.x + CPU_VIEW_SPACING(state->flagLabelWidth / 2, state->padding, 8) + state->padding, 
            flagsStartPos.y + CPU_VIEW_SPACING(state->flagLabelHeight, state->padding, 0), 
            state->flagLabelWidth, 
            state->flagLabelHeight 
        }, state->flagPValue, 1, state->flagPTextActive))
        {
            state->flagPTextActive = !state->flagPTextActive;
        }
        GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

        // Flag H
        GuiLabel((Rectangle)
        { 
            flagsStartPos.x + CPU_VIEW_SPACING(state->flagLabelWidth / 2, state->padding, 0), 
            flagsStartPos.y + CPU_VIEW_SPACING(state->flagLabelHeight, state->padding, 1), 
            state->flagLabelWidth, 
            state->flagLabelHeight 
        }, state->flagHLabelText);
        if(state->flagHTextActive == true)
        {
            GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
        }
        if(GuiTextBox((Rectangle)
        { 
            flagsStartPos.x + CPU_VIEW_SPACING(state->flagLabelWidth / 2, state->padding, 0) + state->padding, 
            flagsStartPos.y + CPU_VIEW_SPACING(state->flagLabelHeight, state->padding, 1), 
            state->flagLabelWidth, 
            state->flagLabelHeight 
        }, state->flagHValue, 1, state->flagHTextActive))
        {
            state->flagHTextActive = !state->flagHTextActive;
        }
        GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

        // Flag Z
        GuiLabel((Rectangle)
        { 
            flagsStartPos.x + CPU_VIEW_SPACING(state->flagLabelWidth / 2, state->padding, 4), 
            flagsStartPos.y + CPU_VIEW_SPACING(state->flagLabelHeight, state->padding, 1), 
            state->flagLabelWidth, 
            state->flagLabelHeight 
        }, state->flagZLabelText);
        if(state->flagZTextActive == true)
        {
            GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
        }
        if(GuiTextBox((Rectangle)
        { 
            flagsStartPos.x + CPU_VIEW_SPACING(state->flagLabelWidth / 2, state->padding, 4) + state->padding, 
            flagsStartPos.y + CPU_VIEW_SPACING(state->flagLabelHeight, state->padding, 1), 
            state->flagLabelWidth, 
            state->flagLabelHeight 
        }, state->flagZValue, 1, state->flagZTextActive))
        {
            state->flagZTextActive = !state->flagZTextActive;
        }
        GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

        // Flag S
        GuiLabel((Rectangle)
        { 
            flagsStartPos.x + CPU_VIEW_SPACING(state->flagLabelWidth / 2, state->padding, 8), 
            flagsStartPos.y + CPU_VIEW_SPACING(state->flagLabelHeight, state->padding, 1), 
            state->flagLabelWidth, 
            state->flagLabelHeight 
        }, state->flagSLabelText);
        if(state->flagSTextActive == true)
        {
            GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
        }
        if(GuiTextBox((Rectangle)
        { 
            flagsStartPos.x + CPU_VIEW_SPACING(state->flagLabelWidth / 2, state->padding, 8) + state->padding, 
            flagsStartPos.y + CPU_VIEW_SPACING(state->flagLabelHeight, state->padding, 1), 
            state->flagLabelWidth, 
            state->flagLabelHeight 
        }, state->flagSValue, 1, state->flagSTextActive))
        {
            state->flagSTextActive = !state->flagSTextActive;
        }
        GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
        /* -------------------------------------------------------------------------- */

        /* ----------------------------- Pointer elements --------------------------- */    
        const Vector2 pointerStartPos = (Vector2)
        { 
            state->bounds.x + ( state->padding * 2 ), 
            state->bounds.y + registerGroupBoxHeight + (state->padding * 4) + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT + flagsGroupBoxHeight
        };
        const int pointerGroupBoxWidth = state->bounds.width - (state->padding * 2);
        const int pointerGroupBoxHeight = CPU_VIEW_SPACING(state->pointerLabelHeight, state->padding, 2) + state->padding;

        GuiGroupBox((Rectangle)
        {
            pointerStartPos.x - state->padding,
            pointerStartPos.y - state->padding,
            pointerGroupBoxWidth,
            pointerGroupBoxHeight
        }, "Pointers");

        // Program counter
        GuiLabel((Rectangle)
        {
            pointerStartPos.x + CPU_VIEW_SPACING(state->pointerTextWidth / 2, state->padding, 0),
            pointerStartPos.y + CPU_VIEW_SPACING(state->pointerLabelHeight, state->padding, 0),
            state->pointerLabelWidth,
            state->pointerLabelHeight
        }, state->programCounterLabelText);
        if(state->pointerPcTextActive == true)
        {
            GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
        }
        if(GuiTextBox((Rectangle)
        {
            pointerStartPos.x + CPU_VIEW_SPACING(state->pointerLabelWidth / 2, state->padding, 2) + state->padding,
            pointerStartPos.y + CPU_VIEW_SPACING(state->pointerLabelHeight, state->padding, 0),
            state->pointerTextWidth,
            state->pointerLabelHeight
        }, state->programCounterValue, 1, state->pointerPcTextActive))
        {
            state->pointerPcTextActive = !state->pointerPcTextActive;
        }
        GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

        // Stack pointer
        GuiLabel((Rectangle)
        {
            pointerStartPos.x + CPU_VIEW_SPACING(state->pointerTextWidth / 2, state->padding, 0),
            pointerStartPos.y + CPU_VIEW_SPACING(state->pointerLabelHeight, state->padding, 1),
            state->pointerLabelWidth,
            state->pointerLabelHeight
        }, state->stackPointerLabelText);
        if(state->pointerSpTextActive == true)
        {
            GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
        }
        if(GuiTextBox((Rectangle)
        {
            pointerStartPos.x + CPU_VIEW_SPACING(state->pointerLabelWidth / 2, state->padding, 2) + state->padding,
            pointerStartPos.y + CPU_VIEW_SPACING(state->pointerLabelHeight, state->padding, 1),
            state->pointerTextWidth,
            state->pointerLabelHeight
        }, state->stackPointerValue, 1, state->pointerSpTextActive))
        {
            state->pointerSpTextActive = !state->pointerSpTextActive;
        }
        GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
        /* -------------------------------------------------------------------------- */

        /* -------------------------------------------------------------------------- */
    }
}

void GuiCpuViewUpdateRegisters(GuiCpuViewState *state, const uint8_t a, const uint8_t b, const uint8_t c, const uint8_t d, const uint8_t e, const uint8_t h, const uint8_t l)
{
    if(state->updateCpuView == true)
    {
        sprintf(state->registerAValue, "0x%02X", a);
        sprintf(state->registerBValue, "0x%02X", b);
        sprintf(state->registerCValue, "0x%02X", c);
        sprintf(state->registerDValue, "0x%02X", d);
        sprintf(state->registerEValue, "0x%02X", e);
        sprintf(state->registerHValue, "0x%02X", h);
        sprintf(state->registerLValue, "0x%02X", l);
    }
}

void GuiCpuViewUpdateFlags(GuiCpuViewState *state, const uint8_t c, const uint8_t n, const uint8_t p, const uint8_t h, const uint8_t z, const uint8_t s)
{
    if(state->updateCpuView == true)
    {
        sprintf(state->flagCValue, "%d", c);
        sprintf(state->flagNValue, "%d", n);
        sprintf(state->flagPValue, "%d", p);
        sprintf(state->flagHValue, "%d", h);
        sprintf(state->flagZValue, "%d", z);
        sprintf(state->flagSValue, "%d", s);
    }
}

void GuiCpuViewUpdatePointers(GuiCpuViewState *state, const uint16_t pc, const uint16_t sp)
{
    if(state->updateCpuView == true)
    {
        sprintf(state->programCounterValue, "0x%04X", pc);
        sprintf(state->stackPointerValue, "0x%04X", sp);
    }
}

void GuiCpuViewUpdateFontSize(GuiCpuViewState *state, const int fontSize)
{
    state->fontSize = fontSize;

    state->registerLabelHeight = fontSize + 10;
    state->flagLabelHeight = fontSize + 10;
    state->pointerLabelHeight = fontSize + 10;

    state->registerTextWidth = 50 + (fontSize * 2);
    state->flagLabelWidth = 25 + (fontSize);
    state->pointerTextWidth = 50 + (fontSize * 2);

    state->registerLabelWidth = 100 + (fontSize * 2);
    state->flagLabelWidth = 25 + (fontSize * 2);
    state->pointerLabelWidth = 100 + (fontSize * 2);

    state->padding = 12 + (.5 * fontSize);
}

void GuiCpuViewUpdate(GuiCpuViewState *state, bool update)
{
    state->updateCpuView = update;
}

#endif // GUI_CPU_VIEW_IMPLEMENTATION