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

#ifndef DEFINES_H
#define DEFINES_H
/********************************************************************* */
/*
    @about: Consists of header declarations for the programs
*/

//External and System Header Inclusion
#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include<stdint.h>
#include<stdbool.h>
#include<gtk/gtk.h> //!External Requirement
#include<gtksourceview/gtksource.h>//!!External Requirement

#if defined(_WIN32)
    #include <windows.h>
    #include <wchar.h>
    #include <shlwapi.h>
#elif defined(__APPLE__)
    #include <mach-o/dyld.h>
    #include <libgen.h>
    #include <limits.h>
#else
    #include <unistd.h>
    #include <libgen.h>
    #include <limits.h>
#endif


#define PATH_MAX 4096

//Internal Header Inclusion
#include "file_handling.h"
#include "main.h"
#include "fonts.h"
#include "process.h"
#include "overlays.h"
#include "menu_bar.h"
#include "search_replace.h"
/********************************************************************* */

/********************************************************************* */
//MACROS FOR THE PROGRAM

extern char * CSS_FILE_PATH;

/********************************************************************* */
/*
    @about: All global Variable Declarations done here 

*/
extern const short int DEFAULT_FONT_SIZE;//Declares the intial font size to be set
extern short int CURRENT_FONT_SIZE;//Updated Font Size
extern gboolean IS_DARK_THEME;//Toggles weather Dark Theme Should be enabled or Not.
extern gboolean buffer_has_changed;
extern gboolean quit_program_after_save;
extern gboolean show_text_highlighting;
extern gboolean FILE_HAS_LONG_LINES;


//Functions
void update_buufer_has_changed();
/********************************************************************* */
#endif