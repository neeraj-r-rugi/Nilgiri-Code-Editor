/*
Nilgiri Text Editor: A simple lightweight text/code editor developed as a student project!  
Copyright (C) 2024 NEERAJ R RUGI

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "file_handling.h"
void load_file_into_buffer()
{
    gchar *content = NULL;
    gsize length = 0;
    GError *error = NULL;
    
    if (access(file_path, F_OK) != 0) {
        FILE *fp = fopen(file_path, "w");
        fclose(fp);
    }
    
    if (!g_file_get_contents(file_path, &content, &length, &error)) {
        g_printerr("Error reading file: %s\n", error->message);
        g_error_free(error);
        return;
    }
    
    // Checking for extremely long lines
    gchar *line_start = content;
    gchar *line_end;
    
    while ((line_end = strchr(line_start, '\n')) != NULL && !FILE_HAS_LONG_LINES) {//No need to check other line if atleast one line is long.
        if (line_end - line_start > NO_OF_CHAR_PER_LINE) { // 5k chars per line
            FILE_HAS_LONG_LINES = TRUE;
            break;
        }
        line_start = line_end + 1;
    }
    
    gtk_text_buffer_set_text(GTK_TEXT_BUFFER(buffer), content, length);
    
    // Only enable syntax highlighting for files without extremely long lines
    if (!FILE_HAS_LONG_LINES) {
        set_file_language();
    }else{
        g_print("The Current File is too big for syntax highlighting. Syntax Highlighting \
            Disabled.\n");
    }
    
    g_free(content);
}

void save_buffer_to_file()
{
    const gchar *entry = NULL;
    if (strcmp(file_path, "") != 0)
    {
        GtkTextIter start, end;
        gchar *text;
        GError *error = NULL;

        // Get the full range of the buffer
        gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer), &start);
        gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer), &end);

        // Get the text in the buffer
        text = gtk_text_buffer_get_text(GTK_TEXT_BUFFER(buffer), &start, &end, FALSE);

        // Write to file
        if (!g_file_set_contents(file_path, text, -1, &error))
        {
            g_printerr("Error saving file: %s\n", error->message);
            g_error_free(error);
        }
        else
        {
            // Nothing For Future
        }
        g_free(text);
        buffer_has_changed = FALSE;
        show_file_saved_overlay();
    }
    else
    {
        gtk_widget_hide(save_at_quit_popup);
        gtk_widget_show(new_file_saved_popup);
        gtk_widget_set_sensitive(GTK_WIDGET(text_area), FALSE);
        gtk_widget_grab_focus(new_file_saved_enrty);
        entry = gtk_entry_get_text(GTK_ENTRY(new_file_saved_enrty));
        if (g_strcmp0(entry, "~") == 0)
        {
            gtk_widget_hide(new_file_saved_popup);
            gtk_widget_set_sensitive(GTK_WIDGET(text_area), TRUE);
            gtk_entry_set_text(GTK_ENTRY(new_file_saved_enrty), "");
            buffer_has_changed = FALSE;
        }else if(g_strcmp0(entry, "~") != 0 && g_strcmp0(entry, "") != 0){
            strncpy(file_path, entry, sizeof(file_path)-1);
            file_path[sizeof(file_path) - 1] = '\0';
            FILE * fp = fopen(file_path, "w");
            fclose(fp);
            gtk_widget_hide(new_file_saved_popup);
            gtk_widget_set_sensitive(GTK_WIDGET(text_area), TRUE);
            gtk_entry_set_text(GTK_ENTRY(new_file_saved_enrty), "");
            save_buffer_to_file();
        }
    }
}

void to_save_at_quit(){
    gtk_widget_show(save_at_quit_popup);
    gtk_widget_set_sensitive(GTK_WIDGET(text_area), FALSE);
}
