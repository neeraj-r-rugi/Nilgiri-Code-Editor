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
#ifndef MAIN_H
#define MAIN_H
#include "defines.h"

//Variable Declaration
extern GtkWidget* zoom_popup;//The Label pointer to be displayed when Zooming

extern GtkWidget * file_saved_popup;
extern GtkWidget * new_file_saved_popup;
extern GtkWidget * new_file_saved_enrty;

extern GtkWidget * save_at_quit_popup;

extern GtkWidget* search_replace_box;

extern GtkSourceBuffer *buffer; 
extern GtkSourceView * text_area;

extern char file_path[1024];
extern int * no_cmd_arg;

// A structure that holds buffer data
typedef struct
{
    GtkSourceBuffer *buffer;
    GtkEntry *search_entry;
    GtkEntry *replace_entry;
    GtkTextIter last_match_end;
    gboolean has_match;
} SearchData;

typedef struct{
    GtkWidget * button_yes;
    GtkWidget * button_no;
    GtkWidget * cancel;
}SaveAtQuitButtons;


typedef struct {
    GtkSourceBuffer *buffer;
    GtkEntry        *entry;        /* the search_entry widget we attached to */
    gchar           *search_text;  /* duplicated copy */
    GtkTextIter      iter;         /* current search iterator (start from beginning) */
    GtkTextTag      *tag;
    guint            idle_id;      /* g_idle_add id, 0 if none */
    gboolean         cancelled;    /* set when canceled */
} SearchRun;

//Function Prototype
/*------------------------------------------------------------------------------*/
/*
    @brief: Intialises the GTK windows and contains declarations for all the widgets
    @param: The GTK application
    @param: Pointer Data 
*/
static void activate(GtkApplication *, gpointer);
/*
    @brief: Intialises the Zoom Overlay, to be used whenever the user Zooms
    @param: The Pointer of the GTK Label that displays the Zoom Text
    @param: The Master Overlay
*/
static void init_zoom_overlay(GtkWidget ** , GtkWidget *);
/*
    @brief: Loads the CSS data for the parsed object
    @param: The Widget whose Style is to applied
    @param: The Path where The CSS file is
*/
static void load_css_for_wideget(GtkWidget *, const char *);
/*
    @brief: Checks weather there is data and search and highlights it.
    @param: The text buffer itself
*/
GtkTextTag* ensure_search_tag(GtkTextBuffer *buffer);
/*
    @brief: Intialises the menu bar
    @param: The master menu widget. 
*/
static void innit_menu_bar(GtkWidget *, GtkWidget *, GtkApplication *);
/*The folllowing functions purpose is trivial and related to serach replace*/
/*-----------------------------------------------------------------------*/
void clear_highlights(GtkSourceBuffer *buffer);
static void on_search_changed(GtkEntry *entry, gpointer user_data);
static void on_replace_next_clicked(GtkButton *button, gpointer user_data);
static void on_replace_all_clicked(GtkButton *button, gpointer user_data);
/*-----------------------------------------------------------------------*/
/*
    @brief: Update
    @param: Update
*/
static int on_command_line(GApplication *, GApplicationCommandLine *, gpointer );
/*
    @brief: Update
    @param: Update
*/
static void init_file_saved_overlay(GtkWidget **, GtkWidget *);
/*
    @brief: Update
    @param: Update
*/
static void init_new_file_saved_overlay(GtkWidget **, GtkWidget *);
/*
    @brief: Update
    @param: Update
*/
static void init_save_at_quit_popup(GtkWidget **, GtkWidget *);
/*
    @brief: Update
    @param: Update
*/
gboolean check_to_quit(gpointer );
/*
    @brief: Update
    @param: Update
*/
void cancel_quit();
/*------------------------------------------------------------------------------*/

#endif