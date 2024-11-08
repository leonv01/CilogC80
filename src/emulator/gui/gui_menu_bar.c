#include "gui_menu_bar.h"

void guiMenuBarInit(GMenu *menu, GtkWidget *grid)
{
    // Create menubar
    menu = g_menu_new();

    // Create file menu
    GMenu *fileMenu = g_menu_new();
    g_menu_append(fileMenu, "New File", "app.new");
    g_menu_append(fileMenu, "Open", "app.open");
    g_menu_append(fileMenu, "Save", "app.save");
    g_menu_append(fileMenu, "Save As", "app.save-as");
    g_menu_append(fileMenu, "Exit", "app.quit");
    g_menu_insert_submenu(menu, 0, "File", G_MENU_MODEL(fileMenu));
    g_object_unref(fileMenu);

    // Create edit menu
    GMenu *editMenu = g_menu_new();
    g_menu_append(editMenu, "Undo", "app.undo");
    g_menu_append(editMenu, "Redo", "app.redo");
    g_menu_append(editMenu, "Cut", "app.cut");
    g_menu_append(editMenu, "Copy", "app.copy");
    g_menu_append(editMenu, "Paste", "app.paste");
    g_menu_append(editMenu, "Delete", "app.delete");
    g_menu_append(editMenu, "Select All", "app.select-all");
    g_menu_append(editMenu, "Find", "app.find");
    g_menu_append(editMenu, "Replace", "app.replace"); 
    g_menu_append(editMenu, "Preferences", "app.preferences");
    g_menu_insert_submenu(menu, 1, "Edit", G_MENU_MODEL(editMenu));
    g_object_unref(editMenu);

    // Create view menu
    GMenu *viewMenu = g_menu_new();
    g_menu_append(viewMenu, "Show RAM", "app.show-ram");
    g_menu_append(viewMenu, "Show Registers", "app.show-registers");
    g_menu_insert_submenu(menu, 2, "View", G_MENU_MODEL(viewMenu));
    g_object_unref(viewMenu);

    // Create emulation menu
    GMenu *emulationMenu = g_menu_new();
    g_menu_append(emulationMenu, "Start", "app.start");
    g_menu_append(emulationMenu, "Pause", "app.pause");
    g_menu_append(emulationMenu, "Stop", "app.stop");
    g_menu_append(emulationMenu, "Load State", "app.load-state");
    g_menu_append(emulationMenu, "Save State", "app.save-state");
    g_menu_insert_submenu(menu, 3, "Emulation", G_MENU_MODEL(emulationMenu));
    g_object_unref(emulationMenu);

    // Create help menu
    GMenu *helpMenu = g_menu_new();
    g_menu_append(helpMenu, "About", "app.about");
    g_menu_insert_submenu(menu, 4, "Help", G_MENU_MODEL(helpMenu));
    g_object_unref(helpMenu);

    GtkWidget *menubar = gtk_popover_menu_bar_new_from_model(G_MENU_MODEL(menu));
    g_object_unref(menu);
    gtk_grid_attach(GTK_GRID(grid), menubar, 0, 0, 2, 1);
    gtk_widget_set_hexpand(menubar, TRUE);
}
