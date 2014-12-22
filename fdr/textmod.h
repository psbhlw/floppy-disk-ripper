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

#define TEXTMOD_WIDTH   80
#define TEXTMOD_HEIGHT  30

void textmod_init(void);
void cls(void);
void gotoxy(uint8_t x, uint8_t y);
void getxy(uint8_t *x, uint8_t *y);
void set_clr(uint8_t clr);
void print_char(char c);
void print_str(char *str);
void print_pad(uint8_t until_x);
void fill_attr(uint8_t x, uint8_t y, uint8_t len, uint8_t clr);
void store_win(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
void restore_win(void);
void draw_win(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t clr);


