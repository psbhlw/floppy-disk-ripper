/*
    This file is part of Floppy Disk Ripper (FDR) program.
    Copyright (C) 2014, psb^hlw.

    FDR is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FDR is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define kbNONE  	0   	// no keys
#define kbTRUEV 	1       // true video
#define kbINVV  	2       // inv video
#define kbEDIT  	3       // edit
#define kbEMODE 	4       // ext mode(unimplemented)
#define kbCAPS  	5       // caps lock
#define kbGRAPH 	6       // graph
#define kbSSSP  	7       // ss+space
#define kbLEFT  	8       // cursor left
#define kbRIGHT 	9       // cursor right
#define kbDOWN  	10      // cursor down
#define kbUP    	11      // cursor up
#define kbDEL   	12      // delete
#define kbENTER 	13      // enter
#define kbBREAK 	27      // break

char keyboard(void);
char is_key_pressed(void);
char is_break_pressed(void);
