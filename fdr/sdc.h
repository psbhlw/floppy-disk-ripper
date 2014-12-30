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


uint8_t sdc_init(void);
uint8_t sdc_prepare(void);
uint8_t sdc_save_track(void);
uint8_t sdc_finalize(void);

#define SDCI_OK         0
#define SDCI_MOUNT_ERR  1

#define SDCP_OK         0
#define SDCP_MKDIR_ERR  1
#define SDCP_CHDIR_ERR  2
#define SDCP_EXIST      3
#define SDCP_OPEN_ERR   4

#define SDCS_OK         0
#define SDCS_EXIST      1
#define SDCS_OPEN_ERR   2
#define SDCS_WRITE_ERR  3
#define SDCS_BREAK      4

#define SDCF_OK         0
#define SDCF_ERROR      1

