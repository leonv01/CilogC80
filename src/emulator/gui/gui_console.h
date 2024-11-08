#ifndef GUI_CONSOLE_H
#define GUI_CONSOLE_H

#include <gtk/gtk.h>

typedef struct GuiConsole_t
{
    GtkWidget *frame;
    GtkWidget *textOutput;
} GuiConsole_t;

void guiConsoleInit(GuiConsole_t *console, GtkWidget *paned);

void guiConsoleAddText(GMenu *menu, const char* text);

#endif // GUI_CONSOLE_H