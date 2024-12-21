#ifndef GUI_CPU_VIEW_H
#define GUI_CPU_VIEW_H

#include "raylib.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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

    /* ----------------------------- CPU information ---------------------------- */
    const char *emulatorStatusLabelText;
    const char *totalCyclesLabelText;
    char emulatorStatusLabelValue[32];
    char totalCyclesLabelValue[32];
    char frequencyValue[8];

    bool updateFrequency;
    bool frequencyActive;
    /* -------------------------------------------------------------------------- */

    Rectangle resizerBounds;

    int displayTypeComboBoxElement;
    bool displayTypeComboBoxEditable;

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

GuiCpuViewState InitGuiCpuView(float frequency);
void GuiCpuView(GuiCpuViewState *state);
void GuiCpuViewUpdateRegisters(GuiCpuViewState *state, const uint8_t a, const uint8_t b, const uint8_t c, const uint8_t d, const uint8_t e, const uint8_t h, const uint8_t l);
void GuiCpuViewUpdateFlags(GuiCpuViewState *state, const uint8_t c, const uint8_t n, const uint8_t p, const uint8_t h, const uint8_t z, const uint8_t s);
void GuiCpuViewUpdatePointers(GuiCpuViewState *state, const uint16_t pc, const uint16_t sp);
void GuiCpuViewUpdateEmulatorStatus(GuiCpuViewState *state, const char *status);
void GuiCpuViewUpdateCycleCount(GuiCpuViewState *state, const uint64_t cycles);
void GuiCpuViewUpdateFrequency(GuiCpuViewState *state, float *frequency);
void GuiCpuViewUpdateFontSize(GuiCpuViewState *state, const int fontSize);
void GuiCpuViewUpdate(GuiCpuViewState *state, bool update);

static void GuiCpuViewRefresh(GuiCpuViewState *state);

#endif // GUI_CPU_VIEW_H

#define GUI_CPU_VIEW_IMPLEMENTATION     
#ifdef GUI_CPU_VIEW_IMPLEMENTATION

#include "raygui.h"
#include <string.h>

typedef enum 
{
    CPU_TO_HEX = 0, 
    CPU_TO_DEC,
    CPU_TO_ASCII
} CPU_OUTPUT_TYPE;

static CPU_OUTPUT_TYPE outputType = CPU_TO_HEX;

GuiCpuViewState InitGuiCpuView(float frequency)
{
    GuiCpuViewState state = { 0 };  

    state.padding = 12;
    state.fontSize = 20;
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

    /* ----------------------------- CPU information ---------------------------- */
    state.emulatorStatusLabelText = "CPU status: ";
    state.totalCyclesLabelText = "Total cycles: ";
    strcpy(state.emulatorStatusLabelValue, "");
    strcpy(state.totalCyclesLabelValue, "0");
    sprintf(state.frequencyValue, "%.2f", frequency);
    state.updateFrequency = false;
    state.frequencyActive = false;
    /* -------------------------------------------------------------------------- */

    state.displayTypeComboBoxElement = 0;
    state.displayTypeComboBoxEditable = false;

    state.supportDrag = true;
    state.dragMode = false;
    state.panOffset = (Vector2){ 0, 0 };

    state.supportResize = false;
    state.resizeMode = false;

    state.updateCpuView = false;

    state.flagsEditable = true;
    state.registersEditable = true;
    state.pointersEditable = true;

    state.bounds.width = CPU_VIEW_SPACING(state.registerLabelWidth / 2, state.padding, 4) + (state.padding * 3);
    state.bounds.height = CPU_VIEW_SPACING(state.registerLabelHeight, state.padding, 4) + 
                            CPU_VIEW_SPACING(state.flagLabelHeight, state.padding, 2) + 
                            CPU_VIEW_SPACING(state.pointerLabelHeight, state.padding, 3) + 
                            CPU_VIEW_SPACING(state.padding, state.padding, 4) +
                            (state.padding * 7) + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT;
    state.bounds.x = GetScreenWidth() / 2 - state.bounds.width / 2;
    state.bounds.y = GetScreenHeight() / 2 - state.bounds.height / 2;

    state.position = (Vector2){ state.bounds.x, state.bounds.y };

    return state;
}

void GuiCpuView(GuiCpuViewState *state)
{
    // Only update if window is visible
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

        if(GuiDropdownBox((Rectangle)
        { 
            labelStartPos.x + CPU_VIEW_SPACING(state->registerLabelWidth, state->padding, 1), 
            labelStartPos.y + CPU_VIEW_SPACING(state->registerLabelHeight, state->padding, 0), 
            state->registerLabelWidth, 
            state->registerLabelHeight 
        }, "HEX;DEC;ASCII", &state->displayTypeComboBoxElement, state->displayTypeComboBoxEditable) == true)
        {
            state->displayTypeComboBoxEditable = !state->displayTypeComboBoxEditable;
        }
        /* -------------------------------------------------------------------------- */

        /* ----------------------------- CPU information ---------------------------- */
        const Vector2 informationStartPos = (Vector2)
        { 
            state->bounds.x + ( state->padding * 2 ), 
            pointerStartPos.y + pointerGroupBoxHeight + state->padding
        };
        const int informationGroupBoxWidth = state->bounds.width - (state->padding * 2);
        const int informationGroupBoxHeight = CPU_VIEW_SPACING(state->pointerLabelHeight, state->padding, 3);

        GuiGroupBox((Rectangle)
        {
            informationStartPos.x - state->padding,
            informationStartPos.y - state->padding,
            informationGroupBoxWidth,
            informationGroupBoxHeight
        }, "CPU information");

        GuiLabel((Rectangle)
        {
            informationStartPos.x + CPU_VIEW_SPACING(state->pointerLabelWidth / 2, state->padding, 0),
            informationStartPos.y + CPU_VIEW_SPACING(state->pointerLabelHeight, state->padding, 0),
            state->pointerLabelWidth,
            state->pointerLabelHeight
        }, "CPU frequency");

        GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
        if(GuiTextBox((Rectangle)
        {
            informationStartPos.x + CPU_VIEW_SPACING(state->pointerLabelWidth / 2, state->padding, 2) + state->padding,
            informationStartPos.y + CPU_VIEW_SPACING(state->pointerLabelHeight, state->padding, 0),
            state->pointerTextWidth,
            state->pointerLabelHeight
        }, state->frequencyValue, 1, state->frequencyActive))
        {
            state->frequencyActive = !state->frequencyActive;
        }
        GuiDropdownBox((Rectangle)
        {
            informationStartPos.x + CPU_VIEW_SPACING(state->pointerLabelWidth / 2, state->padding, 3),
            informationStartPos.y + CPU_VIEW_SPACING(state->pointerLabelHeight, state->padding, 0),
            state->pointerLabelWidth / 2,
            state->pointerLabelHeight
        }, "MHz;kHz;Hz", &state->displayTypeComboBoxElement, state->displayTypeComboBoxEditable);
        if(GuiButton((Rectangle)
        {
            informationStartPos.x + CPU_VIEW_SPACING(state->pointerLabelWidth / 2, state->padding, 2) + state->padding,
            informationStartPos.y + CPU_VIEW_SPACING(state->pointerLabelHeight, state->padding, 0) + state->pointerLabelHeight,
            state->pointerLabelWidth * 1,
            state->pointerLabelHeight
        }, "Set") == true)
        {
            state->updateFrequency = true;
        }

        GuiLabel((Rectangle)
        {
            informationStartPos.x + CPU_VIEW_SPACING(state->pointerLabelWidth / 2, state->padding, 0),
            informationStartPos.y + CPU_VIEW_SPACING(state->pointerLabelHeight, state->padding, 1) + state->padding,
            state->pointerLabelWidth,
            state->pointerLabelHeight
        }, state->emulatorStatusLabelText);
        GuiLabel((Rectangle)
        {
            informationStartPos.x + CPU_VIEW_SPACING(state->pointerLabelWidth / 2, state->padding, 2) + state->padding,
            informationStartPos.y + CPU_VIEW_SPACING(state->pointerLabelHeight, state->padding, 1) + state->padding,
            state->pointerLabelWidth,
            state->pointerLabelHeight
        }, state->emulatorStatusLabelValue);

        GuiLabel((Rectangle)
        {
            informationStartPos.x + CPU_VIEW_SPACING(state->pointerLabelWidth / 2, state->padding, 0),
            informationStartPos.y + CPU_VIEW_SPACING(state->pointerLabelHeight, state->padding, 2),
            state->pointerLabelWidth,
            state->pointerLabelHeight
        }, state->totalCyclesLabelText);
        GuiLabel((Rectangle)
        {
            informationStartPos.x + CPU_VIEW_SPACING(state->pointerLabelWidth / 2, state->padding, 2) + state->padding,
            informationStartPos.y + CPU_VIEW_SPACING(state->pointerLabelHeight, state->padding, 2),
            state->pointerLabelWidth,
            state->pointerLabelHeight
        }, state->totalCyclesLabelValue);
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

void GuiCpuViewUpdateEmulatorStatus(GuiCpuViewState *state, const char *status)
{
    if(state->updateCpuView == true)
    {
        strcpy(state->emulatorStatusLabelValue, status);
    }
}

void GuiCpuViewUpdateCycleCount(GuiCpuViewState *state, const uint64_t cycles)
{
    if(state->updateCpuView == true)
    {
        sprintf(state->totalCyclesLabelValue, "%lu", cycles);
    }
}

void GuiCpuViewUpdateFrequency(GuiCpuViewState *state, float *frequency)
{
    if(state->updateCpuView == true)
    {
        if(state->updateFrequency == true)
        {
            *frequency = atof(state->frequencyValue);
            state->updateFrequency = false;
        }
    }
}


void GuiCpuViewUpdate(GuiCpuViewState *state, bool update)
{
    state->updateCpuView = update;
}

static void GuiCpuViewRefresh(GuiCpuViewState *state)
{
    switch(state->displayTypeComboBoxElement)
    {
        case CPU_TO_HEX:
        {

        }break;
        case CPU_TO_DEC:
        {

        }break;
        case CPU_TO_ASCII:
        {

        }break;
    }
}

#endif // GUI_CPU_VIEW_IMPLEMENTATION