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
#include "defines.h"


//Global Variable Intialisations
const short int DEFAULT_FONT_SIZE = 12;
short int CURRENT_FONT_SIZE = DEFAULT_FONT_SIZE;
gboolean IS_DARK_THEME = FALSE;
gboolean buffer_has_changed = FALSE;
gboolean quit_program_after_save = FALSE;
gboolean show_text_highlighting = TRUE;
gboolean FILE_HAS_LONG_LINES = FALSE;
char * CSS_FILE_PATH = NULL;


//Functions
/****************************************************************************** */
void update_buufer_has_changed(){
    buffer_has_changed = TRUE;
}

/****************************************************************************** */