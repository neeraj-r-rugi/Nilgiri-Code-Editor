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
#include "process.h"

gboolean zoom_key_pressed(GtkWidget* window, GdkEventKey* event){
    if((event->state & GDK_CONTROL_MASK) && (event->keyval == GDK_KEY_equal|| event->keyval == GDK_KEY_KP_Add)){
        editor_zoom_in(window);
        return TRUE;
    }else if((event->state & GDK_CONTROL_MASK) && (event->keyval == GDK_KEY_minus || event->keyval == GDK_KEY_KP_Subtract)){
        editor_zoom_out(window);
        return TRUE;
    }else{
        return FALSE;
    }
}

gboolean save_key_pressed(GtkWidget* window, GdkEventKey* event){
    if((event->state & GDK_CONTROL_MASK) && (event->keyval == GDK_KEY_s)){
        save_buffer_to_file();
        return TRUE;
    }else{
        return FALSE;
    }
}

gboolean quit_key_pressed(GtkWidget * window, GdkEventKey * event, gpointer user_data){
    if(((event->state && GDK_CONTROL_MASK) && (event->keyval == GDK_KEY_Escape))){
        if(buffer_has_changed){
            to_save_at_quit();
        }
        if(!buffer_has_changed){
            g_application_quit(user_data);
        }
        return TRUE;
    }
    return FALSE;

}

gboolean show_search_replace_box(GtkWidget * window, GdkEventKey * event, gpointer user_data){
    if((event->state & GDK_CONTROL_MASK) && (event->keyval == GDK_KEY_F || event->keyval == GDK_KEY_f)){
        if(gtk_widget_get_visible(search_replace_box) == FALSE){
            gtk_widget_show(search_replace_box);
        }else{
            gtk_widget_hide(search_replace_box);
            clear_highlights(user_data);
        }
        return TRUE;
    }
    return FALSE;

}

/******************************************************************************************* */
//Toggles Dark Theme
gboolean toggle_dark_theme(GtkWidget *window, GdkEventKey *event) {
    if ((event->state & GDK_MOD1_MASK) && (event->keyval == GDK_KEY_D || event->keyval == GDK_KEY_d)) {
        IS_DARK_THEME = !IS_DARK_THEME; 
        set_theme(IS_DARK_THEME);
        return TRUE;
    }
    return FALSE;
}

gboolean highlight_key_pressed(GtkWidget * window, GdkEventKey * event, gpointer user_data){
    if((event->state & GDK_CONTROL_MASK) && (event->keyval == GDK_KEY_H || event->keyval == GDK_KEY_h)){
        show_text_highlighting = !show_text_highlighting;
        if(show_text_highlighting && !FILE_HAS_LONG_LINES){
            set_file_language();
        }else{
            unset_file_language();
        }
        return TRUE;
    }
    return FALSE;

}

void set_theme(gboolean enable) {
    GtkSettings *theme_settings = gtk_settings_get_default();
    g_object_set(theme_settings, "gtk-application-prefer-dark-theme", enable, NULL);
}

void save_at_quit_no(GtkWidget * window, gpointer user_data){
    g_application_quit(user_data);
}

void save_at_quit_yes(GtkWidget * window, gpointer user_data){
    quit_program_after_save = TRUE;
    save_buffer_to_file();
}

void set_file_language(){
    //Set language for text buffer
    GtkSourceLanguageManager * language_manager = gtk_source_language_manager_get_default();
    GtkSourceLanguage * file_language = gtk_source_language_manager_guess_language(language_manager, file_path, NULL);
    if(file_language){
        gtk_source_buffer_set_language(buffer, file_language);
        gtk_source_buffer_set_highlight_syntax(buffer, TRUE);
    }else{
        file_language = gtk_source_language_manager_get_language(language_manager, "txt");
        gtk_source_buffer_set_language(buffer, file_language);

    }
}
void unset_file_language(){
    GtkSourceLanguageManager * language_manager = gtk_source_language_manager_get_default();
    GtkSourceLanguage * file_language = gtk_source_language_manager_get_language(language_manager, "txt");
    gtk_source_buffer_set_language(buffer, file_language);
    gtk_source_buffer_set_highlight_syntax(buffer, FALSE);

}

char * get_css_file_path() {
    static char css_path[PATH_MAX];
    char exe_path[PATH_MAX];

#if defined(_WIN32)
    wchar_t wpath[MAX_PATH];
    if (!GetModuleFileNameW(NULL, wpath, MAX_PATH)) {
        fprintf(stderr, "GetModuleFileNameW failed\n");
        exit(EXIT_FAILURE);
    }
    // Convert wide char to UTF-8
    WideCharToMultiByte(CP_UTF8, 0, wpath, -1, exe_path, PATH_MAX, NULL, NULL);
    PathRemoveFileSpecA(exe_path);
    snprintf(css_path, sizeof(css_path), "%s\\..\\Style.css", exe_path);

#elif defined(__APPLE__)
    uint32_t size = sizeof(exe_path);
    if (_NSGetExecutablePath(exe_path, &size) != 0) {
        fprintf(stderr, "_NSGetExecutablePath buffer too small\n");
        exit(EXIT_FAILURE);
    }
    char exe_copy[PATH_MAX];
    strncpy(exe_copy, exe_path, PATH_MAX);
    snprintf(css_path, sizeof(css_path), "%s/../Style.css", dirname(exe_copy));

#else // Linux
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (len == -1) {
        perror("readlink failed");
        exit(EXIT_FAILURE);
    }
    exe_path[len] = '\0';
    char exe_copy[PATH_MAX];
    strncpy(exe_copy, exe_path, PATH_MAX);
    snprintf(css_path, sizeof(css_path), "%s/support/Nilgiri/Style.css", dirname(exe_copy));
#endif

    return css_path;
}
/******************************************************************************************* */
