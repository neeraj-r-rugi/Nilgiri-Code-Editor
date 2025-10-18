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
/*
    @author: Neeraj R Rugi
    @category: Code/Text Editor
    @brief: A simple GUI based Text Editor using the GNU TOOL KIT
    @details:
        -> Frameworks used:
            -> GTK-3.0
            -> GTKSourceView - 3.0
        -> Developed Primarily for Linux Operating Systems using GNOME
    @attention:
        -> Comments bearing "!External Requirement" indicate header files not present in the standard C Library

*/
#include "defines.h"


// Global Variables
GtkWidget * save_at_quit_popup = NULL;
SaveAtQuitButtons save_at_quit_buttons;
GtkWidget * new_file_saved_popup = NULL;
GtkWidget * new_file_saved_enrty = NULL;
int * no_cmd_arg = NULL;
GtkWidget * zoom_popup = NULL;
GtkWidget * file_saved_popup = NULL;
GtkWidget *search_replace_box = 0;
GtkSourceView * text_area = NULL;                                         // *From GtkSourceView, Implenets a Screen wide textfield (Where all the text can be editied)
GtkSourceBuffer *buffer = NULL;
char file_path[1024];  





/*********************************************************************************************** */
static int on_command_line(GApplication *app, GApplicationCommandLine *command_line, gpointer user_data) {
    gchar ** argv;
    int argc;

    argv = g_application_command_line_get_arguments(command_line, &argc);

    if (argc > 1) {
        strcpy(file_path, argv[1]);
    }else{
        strcpy(file_path, "");
    }

    g_application_activate(app);

    return 0; // success
}

// Definitions of search and replace elements
GtkTextTag * ensure_search_tag(GtkTextBuffer *buffer)
{
    GtkTextTagTable *tag_table = gtk_text_buffer_get_tag_table(buffer);
    GtkTextTag *tag = gtk_text_tag_table_lookup(tag_table, "search-tag");
    if (!tag)
    {
        tag = gtk_text_tag_new("search-tag");
        g_object_set(tag, "background", "#bbdefb","foreground", "#000000", NULL);
        gtk_text_tag_table_add(tag_table, tag);
    }
    return tag;
}

void clear_highlights(GtkSourceBuffer *buffer)
{
    GtkTextIter start, end;
    gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer), &start);
    gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(buffer), &end);
    gtk_text_buffer_remove_tag_by_name(GTK_TEXT_BUFFER(buffer), "search-tag", &start, &end);
}

static void on_search_changed(GtkEntry *entry, gpointer user_data)
{
    SearchData *data = (SearchData *)user_data;
    start_incremental_search(data);
    gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(data->buffer), &data->last_match_end);
    data->has_match = FALSE;
}

static void on_replace_next_clicked(GtkButton *button, gpointer user_data)
{
    SearchData *data = user_data;
    GtkSourceBuffer *buffer = data->buffer;
    const gchar *search_text = gtk_entry_get_text(data->search_entry);
    const gchar *replace_text = gtk_entry_get_text(data->replace_entry);

    if (strlen(search_text) == 0)
        return;

    GtkTextIter match_start, match_end;
    if (!gtk_text_iter_forward_search(&data->last_match_end, search_text,
                                      GTK_TEXT_SEARCH_CASE_INSENSITIVE, &match_start, &match_end, NULL))
    {
        // Restart from beginning
        gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer), &match_start);
        if (!gtk_text_iter_forward_search(&match_start, search_text,
                                          GTK_TEXT_SEARCH_CASE_INSENSITIVE, &match_start, &match_end, NULL))
            return; // no match
    }

    gtk_text_buffer_delete(GTK_TEXT_BUFFER(buffer), &match_start, &match_end);
    gtk_text_buffer_insert(GTK_TEXT_BUFFER(buffer), &match_start, replace_text, -1);

    // Update search position
    data->last_match_end = match_start;
    data->has_match = TRUE;

    // Reapply highlights
    on_search_changed(GTK_ENTRY(data->search_entry), data);
}

static void on_replace_all_clicked(GtkButton *button, gpointer user_data)
{
    SearchData *data = user_data;
    GtkSourceBuffer *buffer = data->buffer;
    const gchar *search_text = gtk_entry_get_text(data->search_entry);
    const gchar *replace_text = gtk_entry_get_text(data->replace_entry);

    if (strlen(search_text) == 0)
        return;

    GtkTextIter start, match_start, match_end;
    gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer), &start);

    while (gtk_text_iter_forward_search(&start, search_text,
                                        GTK_TEXT_SEARCH_CASE_INSENSITIVE, &match_start, &match_end, NULL))
    {
        gtk_text_buffer_delete(GTK_TEXT_BUFFER(buffer), &match_start, &match_end);
        gtk_text_buffer_insert(GTK_TEXT_BUFFER(buffer), &match_start, replace_text, -1);

        // Continue searching after replaced text
        start = match_start;
    }

    // Reapply highlights
    on_search_changed(GTK_ENTRY(data->search_entry), data);
}
/*********************************************************************************************** */

static void innit_menu_bar(GtkWidget * menu_bar, GtkWidget * window, GtkApplication * app){
    //File menu
    GtkWidget *file_menu = gtk_menu_new();
    GtkWidget * file_items = gtk_menu_item_new_with_label("File");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(file_items), file_menu);//Set File items as submenu of menu bar
    GtkWidget *save_item = gtk_menu_item_new_with_label("Save");
    GtkWidget *quit_item = gtk_menu_item_new_with_label("Quit");
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), save_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), quit_item);

    //Tools menu
    GtkWidget *tool_menu = gtk_menu_new();
    GtkWidget * tool_items = gtk_menu_item_new_with_label("Tools");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(tool_items), tool_menu);
    GtkWidget *search_replace_item = gtk_menu_item_new_with_label("Search-Replace");
    gtk_menu_shell_append(GTK_MENU_SHELL(tool_menu), search_replace_item);
    //View Menu
    GtkWidget *view_menu = gtk_menu_new();
    GtkWidget * view_items = gtk_menu_item_new_with_label("View");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(view_items), view_menu);
    GtkWidget * zoom_in_item = gtk_menu_item_new_with_label("Zoom-In");
    GtkWidget * zoom_out_item = gtk_menu_item_new_with_label("Zoom-Out");
    gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), zoom_in_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), zoom_out_item);


    //Add Submenu to Menu Bar
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file_items);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), tool_items);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), view_items);

    //Implementation of sub menu
    g_signal_connect(search_replace_item, "activate", G_CALLBACK(menu_show_search_replace), NULL);
    g_signal_connect(quit_item, "activate", G_CALLBACK(menu_application_quit), app);
    g_signal_connect(zoom_in_item, "activate", G_CALLBACK(menu_zoom_in), window);
    g_signal_connect(zoom_out_item, "activate", G_CALLBACK(menu_zoom_out), window);
    g_signal_connect(save_item, "activate", G_CALLBACK(save_buffer_to_file), window);
    
}


static void load_css_for_wideget(GtkWidget *widget, const char *file_path)
{
    GtkCssProvider *provider = gtk_css_provider_new();
    GError *error = NULL;
    if (!gtk_css_provider_load_from_path(provider, file_path, &error))
    {
        g_warning("Failed to load CSS file: %s", error->message);
        g_clear_error(&error);
    }
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(context,
                                   GTK_STYLE_PROVIDER(provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    g_object_unref(provider);
}

static void init_zoom_overlay(GtkWidget **zoom, GtkWidget *overlay)
{
    GtkCssProvider *provider = gtk_css_provider_new();
    *zoom = gtk_label_new("");
    gtk_widget_set_halign(*zoom, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(*zoom, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_bottom(*zoom, 10);
    gtk_widget_set_margin_top(*zoom, 10);
    gtk_widget_set_margin_end(*zoom, 10);
    gtk_widget_set_name(*zoom, "zoom-popup");   // Set Name For CSS
    load_css_for_wideget(*zoom, CSS_FILE_PATH); // Call CSS Styling for widget
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), *zoom);
}

static void init_file_saved_overlay(GtkWidget ** popup, GtkWidget * overlay){
    GtkCssProvider *provider = gtk_css_provider_new();
    *popup = gtk_label_new("");
    gtk_widget_set_halign(*popup, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(*popup, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_bottom(*popup, 10);
    gtk_widget_set_margin_top(*popup, 10);
    gtk_widget_set_margin_end(*popup, 10);
    gtk_widget_set_name(*popup, "file-saved-popup");   // Set Name For CSS
    load_css_for_wideget(*popup, CSS_FILE_PATH); // Call CSS Styling for widget
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), *popup);

}

static void init_new_file_saved_overlay(GtkWidget ** popup, GtkWidget * overlay){
    GtkCssProvider *provider = gtk_css_provider_new();
    *popup = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    GtkWidget *label = gtk_label_new("Enter The Filename with valid file extension to save\nEnter `~` blank to cancel save");
    new_file_saved_enrty= gtk_entry_new();

    gtk_box_pack_start(GTK_BOX(*popup), label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(*popup), new_file_saved_enrty, FALSE, FALSE, 0);

    gtk_widget_set_halign(*popup, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(*popup, GTK_ALIGN_START);
    gtk_widget_set_margin_bottom(*popup, 10);
    gtk_widget_set_margin_top(*popup, 10);
    gtk_widget_set_margin_end(*popup, 10);
    gtk_widget_set_name(*popup, "new-file-saved-popup");   // Set Name For CSS
    load_css_for_wideget(*popup, CSS_FILE_PATH); // Call CSS Styling for widget
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), *popup);

}

static void init_save_at_quit_popup(GtkWidget ** popup, GtkWidget * overlay){
    GtkCssProvider *provider = gtk_css_provider_new();
    *popup = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    GtkWidget * hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    GtkWidget *label = gtk_label_new("Your File is Not Saved\nDo you want to save?");
    save_at_quit_buttons.button_yes = gtk_button_new_with_label("YES");
    save_at_quit_buttons.button_no = gtk_button_new_with_label("NO");
    save_at_quit_buttons.cancel = gtk_button_new_with_label("CANCEL");
    

    gtk_box_pack_start(GTK_BOX(*popup), label, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), save_at_quit_buttons.button_yes, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), save_at_quit_buttons.button_no, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), save_at_quit_buttons.cancel, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(*popup), hbox, TRUE, TRUE, 0);

    gtk_widget_set_halign(*popup, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(*popup, GTK_ALIGN_START);
    gtk_widget_set_margin_bottom(*popup, 10);
    gtk_widget_set_margin_top(*popup, 10);
    gtk_widget_set_margin_end(*popup, 10);
    gtk_widget_set_name(*popup, "quit-file-saved-popup");   // Set Name For CSS
    load_css_for_wideget(*popup, CSS_FILE_PATH); // Call CSS Styling for widget
    gtk_overlay_add_overlay(GTK_OVERLAY(overlay), *popup);

}

gboolean check_to_quit(gpointer user_data){
    if(quit_program_after_save && !buffer_has_changed){
        g_application_quit(G_APPLICATION(user_data));
    }
    return TRUE;
}

void cancel_quit(){
    gtk_widget_hide(save_at_quit_popup);
    gtk_widget_set_sensitive(GTK_WIDGET(text_area), TRUE);
}

static void activate(GtkApplication * app, gpointer user_data)
{
    // Window and Overlay Declarations
    GtkWidget *window; //*Declaring the Editor window
    GtkWidget *replace_next_btn, *replace_all_btn, *scrolled_window,
        *text_view, *hbox, *entry_box, *search_entry, *replace_entry; // *Widgets Pertaining to search and replace
    GtkWidget *scrollable_window;                                     // *Makes the current window in context scrollable
    GtkWidget *master_overlay;                                        // *Declaration for the overlay when the user Zooms the font;
    
    GdkEventKey *key_event;                                           //! This hasn't been used anywhere, check validity of this declarartion.
    GtkWidget * master_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);// *Master box which holds the data for everything being rendered.
    GtkWidget * menu_bar;

    
    
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Nilgiri Text Editor");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);

    master_overlay = gtk_overlay_new();

    //Menu Ribbon Intialisation
    menu_bar = gtk_menu_bar_new();
    innit_menu_bar(menu_bar, window, app);


    // Search and Replace Box and widgets
    search_replace_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    entry_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    search_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(search_entry), "Search...");
    replace_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(replace_entry), "Replace with...");
    replace_next_btn = gtk_button_new_with_label("Replace Next");
    replace_all_btn = gtk_button_new_with_label("Replace All");
    gtk_box_pack_start(GTK_BOX(entry_box), search_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(entry_box), replace_entry, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(entry_box), replace_next_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(entry_box), replace_all_btn, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(search_replace_box), entry_box, FALSE, FALSE, 0);

    scrollable_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollable_window), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    // Add Text Editor(GtkSourceView) to scrolable window
    buffer = gtk_source_buffer_new(NULL);
    text_area = GTK_SOURCE_VIEW(gtk_source_view_new_with_buffer(buffer));
    gtk_source_view_set_show_line_numbers(text_area, TRUE); // Adds Line Number to Text Editor
    if(!FILE_HAS_LONG_LINES){
        gtk_source_view_set_auto_indent(text_area, TRUE);    // Adds Auto Indent in the text Editor
        gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(text_area), GTK_WRAP_WORD_CHAR);
        gtk_source_view_set_highlight_current_line(text_area, FALSE);
    }
    

    // Text Data for Search and Replace
    SearchData *data = g_new0(SearchData, 1);
    data->buffer = buffer;
    data->search_entry = GTK_ENTRY(search_entry);
    data->replace_entry = GTK_ENTRY(replace_entry);
    gtk_text_buffer_get_start_iter(GTK_TEXT_BUFFER(buffer), &data->last_match_end);
    data->has_match = FALSE;

    //Load Files
    if(*no_cmd_arg >1){

        load_file_into_buffer();
    }

    
    
    // Adding Containers
    gtk_container_add(GTK_CONTAINER(scrollable_window), GTK_WIDGET(text_area));
    gtk_container_add(GTK_CONTAINER(master_overlay), scrollable_window);
    gtk_box_pack_start(GTK_BOX(master_box), menu_bar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(master_box), master_overlay, TRUE, TRUE, 0);
    gtk_container_add(GTK_CONTAINER(window), master_box);
    
    init_zoom_overlay(&zoom_popup, master_overlay);
    init_file_saved_overlay(&file_saved_popup, master_overlay);
    init_new_file_saved_overlay(&new_file_saved_popup, master_overlay);
    init_save_at_quit_popup(&save_at_quit_popup, master_overlay);
    
    // Adding Search Replace Overlay
    gtk_overlay_add_overlay(GTK_OVERLAY(master_overlay), search_replace_box);
    gtk_widget_set_halign(search_replace_box, GTK_ALIGN_END);
    gtk_widget_set_valign(search_replace_box, GTK_ALIGN_START);
    gtk_widget_set_name(search_replace_box, "search-replace"); 
    load_css_for_wideget(search_replace_box, CSS_FILE_PATH);
    gtk_widget_set_margin_start(search_replace_box, 10);
    gtk_widget_set_margin_top(search_replace_box, 10);
    gtk_widget_set_can_focus(search_entry, TRUE);
    gtk_widget_set_can_focus(replace_entry, TRUE);
    gtk_widget_set_can_focus(GTK_WIDGET(text_area), TRUE);

    // Checking for Key Events
    g_signal_connect(window, "key-press-event", G_CALLBACK(zoom_key_pressed), NULL);
    g_signal_connect(window, "key-press-event", G_CALLBACK(quit_key_pressed), app);
    g_signal_connect(window, "key-press-event", G_CALLBACK(toggle_dark_theme), NULL);
    g_signal_connect(window, "key-press-event", G_CALLBACK(show_search_replace_box), buffer);
    g_signal_connect(window, "key-press-event", G_CALLBACK(save_key_pressed), NULL);
    g_signal_connect(search_entry, "changed", G_CALLBACK(on_search_changed), data);
    g_signal_connect(replace_next_btn, "clicked", G_CALLBACK(on_replace_next_clicked), data);
    g_signal_connect(replace_all_btn, "clicked", G_CALLBACK(on_replace_all_clicked), data);
    g_signal_connect(new_file_saved_enrty, "activate", G_CALLBACK(save_buffer_to_file), NULL);
    g_signal_connect(buffer, "changed", update_buufer_has_changed, NULL);
    g_signal_connect(save_at_quit_buttons.button_yes, "clicked", G_CALLBACK(save_at_quit_yes), app);
    g_signal_connect(save_at_quit_buttons.button_no, "clicked", G_CALLBACK(save_at_quit_no), app);
    g_signal_connect(save_at_quit_buttons.cancel, "clicked", G_CALLBACK(cancel_quit), NULL);
    g_signal_connect(window, "key-press-event", G_CALLBACK(highlight_key_pressed), NULL);


    //Quits the application after user saves/not saves
    g_timeout_add(500, check_to_quit, app);
    

    // Dispaly All Required UI Initially
    gtk_widget_show_all(window);

    // UI that does not need to be shown at start
    gtk_widget_hide(zoom_popup);
    gtk_widget_hide(search_replace_box);
    gtk_widget_hide(file_saved_popup);
    gtk_widget_hide(new_file_saved_popup);
    gtk_widget_hide(save_at_quit_popup);
}

int main(int argc, char ** argv)
{
    GtkApplication *app;
    int status;
    CSS_FILE_PATH = get_css_file_path();

    app = gtk_application_new("com.nilgiri.texteditor", G_APPLICATION_HANDLES_COMMAND_LINE);
    no_cmd_arg = &argc;


    g_signal_connect(app, "command-line", G_CALLBACK(on_command_line), NULL);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}