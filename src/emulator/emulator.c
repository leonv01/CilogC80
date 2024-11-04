#include "emulator.h"

#include <gtk/gtk.h>
#include <glib.h>

#include "cpu.h"
#include "memory.h"
#include "utils.h"
#include "stdbool.h"
#include "file_reader.h"
#include "error_handler.h"

typedef enum
{
    EMULATION_STATE_STOPPED = 0,
    EMULATION_STATE_RUNNING,
    EMULATION_STATE_PAUSED
} C80_EmulationState_t;

// Global variables
// -----------------------------------------------------------
static CPU_t        cpu;
static Memory_t     memory;
static C80_EmulationState_t emulationState = EMULATION_STATE_STOPPED;

static GtkWidget    *window;
static GThread      *emulationThread;
static GMutex       emulationMutex;
// -----------------------------------------------------------

// Forward declarations
// -----------------------------------------------------------
// Graphics functions
static void createMenuBar(GtkWidget *paned);
static void createRegisterView(GtkWidget *paned);
static void createRAMView(GtkWidget *paned);
static void createTextEditorView(GtkWidget *paned);
static void createOutputView(GtkWidget *paned);

// Callback functions
static void openCallback(GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void saveCallback(GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void quitCallback(GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void startEmulationCallback(GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void pauseEmulationCallback(GSimpleAction *action, GVariant *parameter, gpointer user_data);
static void stopEmulationCallback(GSimpleAction *action, GVariant *parameter, gpointer user_data);

// GTK functions
static int graphicsInit(int argc, char **argv);
static void activate(GtkApplication *app, gpointer user_data);
static gpointer emulatorThreadFunc(gpointer userData);
static gboolean updateGuiWithEmulationState(gpointer userData);

// Emulator functions
void emulatorInit(int argc, char** argv);
// -----------------------------------------------------------

// Function definitions
// -----------------------------------------------------------
void emulatorInit(int argc, char** argv)
{
    cpuInit(&cpu);
    memoryInit(&memory);
    errorStackInit();

    loadFile(&memory, "../asm/main.bin");

    graphicsInit(argc, argv);
}

static void activate(GtkApplication *app, gpointer user_data)
{
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Cilog C80");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    // Create grid layout
    GtkWidget *grid = gtk_grid_new();
    gtk_window_set_child(GTK_WINDOW(window), grid);

    // Create main paned
    GtkWidget *mainPaned = gtk_paned_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_grid_attach(GTK_GRID(grid), mainPaned, 0, 1, 3, 1);

    // Create left and right paned
    GtkWidget *leftPaned = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
    gtk_paned_set_start_child(GTK_PANED(mainPaned), leftPaned);
    gtk_widget_set_size_request(leftPaned, 100, -1);

    GtkWidget *rightPaned = gtk_paned_new(GTK_ORIENTATION_VERTICAL);
    gtk_paned_set_end_child(GTK_PANED(mainPaned), rightPaned);
    gtk_widget_set_size_request(rightPaned, 500, -1);

    createMenuBar(grid);
    createRegisterView(leftPaned);
    createRAMView(leftPaned);
    createTextEditorView(rightPaned);
    createOutputView(rightPaned);

    gtk_window_present(GTK_WINDOW(window));
}

static int graphicsInit(int argc, char **argv)
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
        {"redo", NULL},
        {"start", startEmulationCallback},
        {"pause", pauseEmulationCallback},
        {"stop", stopEmulationCallback}
    };

    g_action_map_add_action_entries(G_ACTION_MAP(app), appEntries, G_N_ELEMENTS(appEntries), app);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);

    return status;
}

static gpointer emulatorThreadFunc(gpointer userData)
{
    g_mutex_lock(&emulationMutex);
    emulationState = EMULATION_STATE_RUNNING;
    g_mutex_unlock(&emulationMutex);

    while(emulationState == EMULATION_STATE_RUNNING)
    {
        cpuStep(&cpu, &memory);
        printf("Running\n");
        g_idle_add((GSourceFunc)updateGuiWithEmulationState, NULL);
        g_usleep(1000);
    }
}
static gboolean updateGuiWithEmulationState(gpointer userData)
{
    //TODO
    return G_SOURCE_REMOVE;
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
static void createRegisterView(GtkWidget *paned)
{
    GtkWidget *registerView = gtk_frame_new("Register View");
    gtk_widget_set_hexpand(registerView, TRUE);
    gtk_widget_set_vexpand(registerView, TRUE);
    gtk_paned_set_start_child(GTK_PANED(paned), registerView);
}
static void createRAMView(GtkWidget *paned)
{
    GtkWidget *ramView = gtk_frame_new("RAM View");
    gtk_widget_set_hexpand(ramView, TRUE);
    gtk_widget_set_vexpand(ramView, TRUE);
    gtk_paned_set_end_child(GTK_PANED(paned), ramView);
}
static void createTextEditorView(GtkWidget *paned)
{
    GtkWidget *textEditorView = gtk_frame_new("Text Editor View");
    gtk_widget_set_hexpand(textEditorView, TRUE);
    gtk_widget_set_vexpand(textEditorView, TRUE);
    gtk_paned_set_start_child(GTK_PANED(paned), textEditorView);

    GtkWidget *textEditor = gtk_text_view_new();
    gtk_frame_set_child(GTK_FRAME(textEditorView), textEditor);    
    gtk_text_view_set_editable(GTK_TEXT_VIEW(textEditor), TRUE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(textEditor), TRUE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textEditor), GTK_WRAP_WORD);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(textEditor), TRUE);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(textEditor), 5);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(textEditor), 5);
    gtk_text_view_set_top_margin(GTK_TEXT_VIEW(textEditor), 5);
    gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(textEditor), 5);
}
static void createOutputView(GtkWidget *paned)
{
    GtkWidget *outputView = gtk_frame_new("Output View");
    gtk_widget_set_hexpand(outputView, TRUE);
    gtk_widget_set_vexpand(outputView, TRUE);
    gtk_paned_set_end_child(GTK_PANED(paned), outputView);

    GtkWidget *outputText = gtk_text_view_new();
    gtk_frame_set_child(GTK_FRAME(outputView), outputText);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(outputText), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(outputText), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(outputText), GTK_WRAP_WORD);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(outputText), TRUE);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(outputText), 5);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(outputText), 5);
    gtk_text_view_set_top_margin(GTK_TEXT_VIEW(outputText), 5);
    gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(outputText), 5);
}

static void onFileDialogResponse(GtkDialog *dialog, int response_id, gpointer user_data) {
    if (response_id == GTK_RESPONSE_ACCEPT) {
        GtkFileChooser *chooser = GTK_FILE_CHOOSER(dialog);
        GListModel *files = gtk_file_chooser_get_files(chooser);
        
        if (g_list_model_get_n_items(files) > 0) {
            GFile *file = G_FILE(g_list_model_get_item(files, 0)); // Get the first selected file
            char *filename = g_file_get_path(file);
            g_print("Selected file: %s\n", filename);
            
            // Free resources
            g_free(filename);
            g_object_unref(file);
        }

        g_object_unref(files); // Free the list model
    }

    // Destroy the dialog after handling the response
    gtk_window_destroy(GTK_WINDOW(dialog));
}

static void openCallback(GSimpleAction *action, GVariant *parameter, gpointer user_data) 
{
    GtkWidget *dialog;
    GtkFileFilter *filter;

    // Create a file chooser dialog
    dialog = gtk_file_chooser_dialog_new(
        "Open File",
        GTK_WINDOW(user_data),  // This should correctly reference the window passed from the button
        GTK_FILE_CHOOSER_ACTION_OPEN,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Open", GTK_RESPONSE_ACCEPT,
        NULL
    );

    // Add a filter for text files (optional)
    filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, "Text files");
    gtk_file_filter_add_mime_type(filter, "text/plain");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(dialog), filter);

    // Connect the response handler
    g_signal_connect(dialog, "response", G_CALLBACK(onFileDialogResponse), NULL);

    // Show the dialog asynchronously
    gtk_widget_show(dialog);
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
static void startEmulationCallback(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    if(emulationThread == NULL)
    {
        emulationThread = g_thread_new("emulationThread", emulatorThreadFunc, NULL);
        g_print("Emulation started.\n");
    }
}
static void pauseEmulationCallback(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    g_print("Emulation paused.\n");
    g_mutex_lock(&emulationMutex);
    emulationState = EMULATION_STATE_PAUSED;
    g_mutex_unlock(&emulationMutex);
}
static void stopEmulationCallback(GSimpleAction *action, GVariant *parameter, gpointer user_data)
{
    g_mutex_lock(&emulationMutex);
    emulationState = EMULATION_STATE_STOPPED;
    g_mutex_unlock(&emulationMutex);

    if(emulationThread != NULL)
    {
        g_thread_join(emulationThread);
        emulationThread = NULL;
        g_print("Emulation stopped.\n");
    }
}