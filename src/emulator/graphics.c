#include "graphics.h"

#include <gtk/gtk.h>

static GtkWidget *window;

static void createMenuBar(GtkWidget *grid);
static void createRegisterView(GtkWidget *grid);
static void createRAMView(GtkWidget *grid);

static void openCallback(GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void saveCallback(GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void quitCallback(GSimpleAction *action, GVariant *parameter, gpointer user_data);

static void activate(GtkApplication *app, gpointer user_data)
{
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Cilog C80");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    // Create grid layout
    GtkWidget *grid = gtk_grid_new();
    gtk_window_set_child(GTK_WINDOW(window), grid);

    createMenuBar(grid);
    createRegisterView(grid);
    createRAMView(grid);

    gtk_window_present(GTK_WINDOW(window));
}

int graphicsInit(int argc, char *argv[])
{
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.gtk.example", G_APPLICATION_DEFAULT_FLAGS);

    GActionEntry appEntries[] = 
    {
        {"open", openCallback},
        {"save", saveCallback},
        {"quit", quitCallback},
        {"undo", NULL},
        {"redo", NULL}
    };

    g_action_map_add_action_entries(G_ACTION_MAP(app), appEntries, G_N_ELEMENTS(appEntries), app);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);


    return status;
}

static void createMenuBar(GtkWidget *grid)
{
    // Create menubar
    GMenu *menu = g_menu_new();

    // Create file menu
    GMenu *fileMenu = g_menu_new();
    g_menu_append(fileMenu, "Open", "app.open");
    g_menu_append(fileMenu, "Save", "app.save");
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

    GtkWidget *menubar = gtk_popover_menu_bar_new_from_model(G_MENU_MODEL(menu));
    g_object_unref(menu);
    gtk_grid_attach(GTK_GRID(grid), menubar, 0, 0, 2, 1);
}

static void createRegisterView(GtkWidget *grid)
{
    GtkWidget *registerView = gtk_frame_new("Register View");
    gtk_widget_set_hexpand(registerView, TRUE);
    gtk_widget_set_vexpand(registerView, TRUE);
    gtk_grid_attach(GTK_GRID(grid), registerView, 0, 1, 1, 1);
}

static void createRAMView(GtkWidget *grid)
{
    GtkWidget *ramView = gtk_frame_new("RAM View");
    gtk_widget_set_hexpand(ramView, TRUE);
    gtk_widget_set_vexpand(ramView, TRUE);
    gtk_grid_attach(GTK_GRID(grid), ramView, 1, 1, 1, 1);
}


int graphicsDestroy()
{
    
    return 0;
}

static void openCallback(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    g_print("Open\n");
}

static void saveCallback(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    g_print("Save\n");
}
static void quitCallback(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    g_print("Quit\n");
    gtk_window_close(GTK_WINDOW(window));
}