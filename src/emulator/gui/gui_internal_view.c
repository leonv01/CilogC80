#include "gui_internal_view.h"

static void guiPanedViewInit(GuiPanedView_t *paned);
static void addGuiPanedView(GuiInternalView_t *view, GuiPanedView_t *paned);

void guiInternalViewInit(GuiInternalView_t *internalView, GtkWidget *grid)
{
    if(internalView == NULL)
    {
        internalView = (GuiInternalView_t*) malloc(sizeof(internalView));
    }

    GuiPanedView_t *registerPaned;
    GuiPanedView_t *cpuPaned;
    GuiPanedView_t *memoryPaned;
    GuiPanedView_t *interruptPaned;

    guiPanedViewInit(registerPaned);
    guiPanedViewInit(cpuPaned);
    guiPanedViewInit(memoryPaned);
    guiPanedViewInit(interruptPaned);
}

static void guiPanedViewInit(GuiPanedView_t *paned)
{
    if(paned == NULL)
    {
        paned = (GuiPanedView_t*) malloc(sizeof(GuiPanedView_t));
    }
}

static void addGuiPanedView(GuiInternalView_t *view, GuiPanedView_t *paned)
{
    GuiPanedView_t *root;

    while(true)
    {
        if(root == NULL)
        {
            root = (GuiPanedView_t*) malloc(sizeof(GuiPanedView_t));

            // TODO:
        }
    }
}