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
static GtkWidget    *notebook;
static GtkWidget    *grid;
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

static void openFileInNewTab(GtkNotebook *notebook, const char *filename);
static void closeFileTab(GtkNotebook *notebook, GtkWidget *button);
static void loadFileIntoTextView(GtkTextView *text_view, const char *filename);

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
    grid = gtk_grid_new();
    gtk_window_set_child(GTK_WINDOW(window), grid);

    notebook = gtk_notebook_new();

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
    createOutputView(rightPaned);

    gtk_paned_set_start_child(GTK_PANED(rightPaned), notebook);

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
static void openFileInNewTab(GtkNotebook *notebook, const char *filename)
{
    // Create a new text view for the file content
    GtkWidget *textView = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(textView), TRUE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(textView), TRUE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textView), GTK_WRAP_WORD);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(textView), TRUE);

    // Load the file content into the text view
    loadFileIntoTextView(GTK_TEXT_VIEW(textView), filename);

    // Create a new scrollable container for the text view
    GtkWidget *scrolledWindow = gtk_scrolled_window_new();
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(scrolledWindow), textView);
    gtk_widget_set_hexpand(scrolledWindow, TRUE);
    gtk_widget_set_vexpand(scrolledWindow, TRUE);

    // Add the scrollable container as a new tab in the notebook
    GtkWidget *tabHbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    GtkWidget *tabLabel = gtk_label_new(filename);
    GtkWidget *tabCloseButton = gtk_button_new_from_icon_name("window-close");

    g_signal_connect(tabCloseButton, "clicked", G_CALLBACK(closeFileTab), notebook);
    
    gtk_box_append(GTK_BOX(tabHbox), tabLabel);
    gtk_box_append(GTK_BOX(tabHbox), tabCloseButton);
    
    gtk_notebook_append_page(notebook, scrolledWindow, tabHbox);
}
static void loadFileIntoTextView(GtkTextView *textView, const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file) {
        g_warning("Could not open file: %s", filename);
        return;
    }

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(textView);
    gtk_text_buffer_set_text(buffer, "", -1); // Clear previous content

    char line[256];
    GtkTextIter iter;
    gtk_text_buffer_get_start_iter(buffer, &iter);
    while (fgets(line, sizeof(line), file)) {
        gtk_text_buffer_insert(buffer, &iter, line, -1);
    }

    fclose(file);
}
static void closeFileTab(GtkNotebook *notebook, GtkWidget *button)
{
    g_print("Close tab\n");
    GtkWidget *tabBox = gtk_widget_get_parent(button);
    gint pageNum = gtk_notebook_page_num(notebook, tabBox);
    g_print("Page number: %d\n", pageNum);
}
static void onFileChosen(GObject *object, GAsyncResult *result, gpointer userData)
{
    GFile *file = gtk_file_dialog_open_finish(GTK_FILE_DIALOG(object), result, NULL);
    if(file == NULL)
    {
        g_print("No file chosen.\n");
        return;
    }
    else
    {
        const char *filename = g_file_get_path(file);
        if(filename == NULL)
        {
            g_print("Invalid file name.\n");
            return;
        }
        else
        {
            g_print("File chosen: %s\n", filename);
            openFileInNewTab(GTK_NOTEBOOK(notebook), filename);
        }
    }
}
static void openCallback(GSimpleAction *action, GVariant *parameter, gpointer userData) 
{
    GtkFileDialog *dialog;
    GtkFileFilter *filterAsm = gtk_file_filter_new();
    GtkFileFilter *filterBin = gtk_file_filter_new();
    GtkFileFilter *filterAll = gtk_file_filter_new();

    gtk_file_filter_add_suffix(filterAsm, "asm");
    gtk_file_filter_set_name(filterAsm, "Assembly files");

    gtk_file_filter_add_suffix(filterBin, "bin");
    gtk_file_filter_set_name(filterBin, "Binary files");

    gtk_file_filter_add_pattern(filterAll, "*");
    gtk_file_filter_set_name(filterAll, "All files");

    dialog = gtk_file_dialog_new();

    GListStore *filters = g_list_store_new(GTK_TYPE_FILE_FILTER);
    g_list_store_append(filters, filterAsm);
    g_list_store_append(filters, filterBin);
    g_list_store_append(filters, filterAll);
    gtk_file_dialog_set_filters(GTK_FILE_DIALOG(dialog), G_LIST_MODEL(filters));

    gtk_file_dialog_open(
        GTK_FILE_DIALOG(dialog), 
        GTK_WINDOW(window),
        NULL,
        onFileChosen,
        NULL);

    g_object_unref(filters);
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