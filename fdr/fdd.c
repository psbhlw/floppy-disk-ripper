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
#include "fdd.h"
#include "main.h"

// This struct pass a data into fdd low level asm functions.
// Be sure fdd_asm.a80 use correct offsets.
static
struct FDD_t
{
    uint8_t drive;
    uint8_t smprate;
    uint8_t track,side;
    uint8_t revs;
    uint32_t *trklen;
} FDD_parms;

// Low level asm functions. Dummy parm used to prevent the need to save BC pair.
uint8_t fdd_init_lowl(struct FDD_t*, uint8_t dummy);
uint8_t fdd_capture_lowl(struct FDD_t*, uint8_t dummy);
void fdd_keep_alive_lowl(void);



static
void fdd_parms_init(void)
{
    FDD_parms.drive = FDR_INFO.drive;
    FDD_parms.smprate = FDR_INFO.smprate;
    FDD_parms.track = FDR_INFO.track;
    FDD_parms.side = FDR_INFO.side;
    FDD_parms.revs = FDR_INFO.revs;
    FDD_parms.trklen = FDR_INFO.trklen;
}


// Init drive.
uint8_t fdd_init(void)
{
    fdd_parms_init();
    return fdd_init_lowl(&FDD_parms, 0);
}


// Do a capture. This func can do different type of captures if needed.
uint8_t fdd_capture(void)
{
    fdd_parms_init();
    return fdd_capture_lowl(&FDD_parms, 0);
}


// Keep FDD motor rotating.
void fdd_keep_alive(void)
{
    fdd_keep_alive_lowl();
}

