#ifndef GUI_INTERNAL_VIEW_H
#define GUI_INTERNAL_VIEW_H

#include <gtk/gtk.h>

typedef struct GuiPanedView_t
{
    GtkWidget *paned;
    GtkWidget *frame;
    GtkWidget *view;

    struct GuiPanedView_t *nextPaned;
} GuiPanedView_t;

typedef struct GuiInternalView_t
{
    GuiPanedView_t *root;
} GuiInternalView_t;

void guiInternalViewInit(GuiInternalView_t *internalView, GtkWidget *grid);
void guiInternalViewDestroy(GuiInternalView_t *internalView);


#endif // GUI_INTERNAL_VIEW_H