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
#include "stdint.h"


struct menu_item_t
{
    uint8_t x,y,len,val_xoffs;
    char *text;
    void (*fmt)(struct menu_item_t*);
    void (*act)(struct menu_item_t*, uint8_t xoffs, uint8_t y, uint8_t len); // hack: xoffs,y,len are precalc'd for improve code size
};



// progress operations
#define PGS_READ    0
#define PGS_WRITE   1
#define PGS_INIT    2


void ui_init(void);
void ui_proc(void);
void ui_show_progress(uint8_t op);


