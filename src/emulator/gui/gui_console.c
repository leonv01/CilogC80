#include "gui_console.h"

void guiConsoleInit(GuiConsole_t *console, GtkWidget *paned)
{
    if(console == NULL)
    {
        console = (GuiConsole_t*) malloc(sizeof(console));
    }

    GtkWidget *frame = gtk_frame_new("Console");
    gtk_widget_set_hexpand(frame, TRUE);
    gtk_widget_set_vexpand(frame, TRUE);
    gtk_paned_set_end_child(GTK_PANED(paned), frame);

    GtkWidget *textOutput = gtk_text_view_new();
    gtk_frame_set_child(GTK_FRAME(frame), textOutput);
    gtk_text_view_set_editable(GTK_TEXT_VIEW(textOutput), FALSE);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(textOutput), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textOutput), GTK_WRAP_WORD);
    gtk_text_view_set_monospace(GTK_TEXT_VIEW(textOutput), TRUE);
    gtk_text_view_set_left_margin(GTK_TEXT_VIEW(textOutput), 5);
    gtk_text_view_set_right_margin(GTK_TEXT_VIEW(textOutput), 5);
    gtk_text_view_set_top_margin(GTK_TEXT_VIEW(textOutput), 5);
    gtk_text_view_set_bottom_margin(GTK_TEXT_VIEW(textOutput), 5);

    console->frame = frame;
    console->textOutput = textOutput;
}

void guiConsoleAddText(GMenu *menu, const char* text)
{

}