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

#ifndef __MAIN_H
#define __MAIN_H

#include "stdint.h"
#include <Intrz80.h>


#define MAX_REVS            10  // max number of revolutions to capture
#define DATA_START_PAGE     32  // start page of captured data


// global vars
struct FDR_INFO_t
{
    char dir_name[256];
    char file_name[13];
    uint8_t single_file;
    uint8_t overwrite;
    uint8_t from_trk;
    uint8_t to_trk;
    uint8_t sides;
    uint8_t imgtype;

    uint8_t drive;
    uint8_t smprate;
    uint8_t track,side;
    uint8_t revs;
    uint32_t trklen[MAX_REVS+1];
};
extern struct FDR_INFO_t FDR_INFO;

// Proto
uint8_t make_image(char **err_msg);

#define MKI_OK          1
#define MKI_ERROR       0


// Enhanced FDD-Conf Ports
#define CPUFREQ         0x20AF
#define PAGESLT3        0x13AF
#define FDDVirt         0x29AF
#define DMADAddrL       0x1DAF
#define DMADAddrH       0x1EAF
#define DMADAddrX       0x1FAF
#define DMALen          0x26AF
#define DMANum          0x28AF
#define DMANumH         0x2CAF
#define DMACtrl         0x27AF
#define FRCtrl          0x30AF
#define FRCnt0          0x30AF
#define FRCnt1          0x31AF
#define FRCnt2          0x32AF


#define PAGE_START      (void*)0xc000
#define PAGE_SIZE       0x4000
#define SET_PAGE(p)     output(PAGESLT3, p)
#define DATA_START_PAGE 32
#define PAGE_DEFAULT    0

#define CPU14MHZ()      output(CPUFREQ, 2)
#define CPU3MHZ()       output(CPUFREQ, 0)

#define BORDER(x)       output8(0xfe,x)

#endif
