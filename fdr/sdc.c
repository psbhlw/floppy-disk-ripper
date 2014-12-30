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
#include <string.h>

#include "ff.h"         /* Declarations of FatFs API */
#include "ff_util.h"
#include "main.h"
#include "fdd.h"
#include "sdc.h"

// Be sure these values are the same as in ui module.
static
long const smp_rates[] = {28000000, 14000000, 7000000};


// FDR file header
static
struct FDR0_HDR_t
{
    char sig[4];            // signature FDR0
    long smprate;           // dword sample rate
    uint8_t track,side;     // track, side
    uint8_t revs;           // captured revolutions
} FDR_HDR;
// Lengths of captured tracks (revs + tail)
long trklen[MAX_REVS+1];
// Number of captured images
uint16_t image_number;
// File descriptor
static
FIL fd;


static
FATFS FatFs;                /* FatFs work area needed for each volume */


// Convert uint16_t to 3 chars
static void bin2dec3(uint16_t val, char *dst)
{
    *dst++ = val/100 + '0'; val %= 100;
    *dst++= val/10 + '0';
    *dst  = val%10 + '0';
}


// Make filename from track/side
static void mk_filename(char *fn, uint8_t track, uint8_t side)
{
    if(FDR_INFO.single_file)
    {
        memcpy(fn, "FDRIMAGE.xxx", 13);
        bin2dec3(image_number, &fn[9]);
    }
    else
    {
        memcpy(fn, "TRKxxx_y.FDR", 13);
        bin2dec3(track, &fn[3]);
        fn[7]  = side + '0';
    }
}


// Open image file for write
static FRESULT open_file(void)
{
    FRESULT res;
    BYTE mode;

    // make file name
    mk_filename(FDR_INFO.file_name, FDR_INFO.track, FDR_INFO.side);

    // open file
    if (FDR_INFO.overwrite)
        mode = FA_WRITE|FA_CREATE_ALWAYS;
    else
        mode = FA_WRITE|FA_CREATE_NEW;
    res=f_open(&fd, FDR_INFO.file_name, mode);
    return res;
}



// Init FS
uint8_t sdc_init(void)
{
    return (f_mount(&FatFs,"",0) == FR_OK) ? SDCI_OK : SDCI_MOUNT_ERR;
}


// Prepare to capturing:
// + create folder
// + chdir to it
// + check if file already exist
uint8_t sdc_prepare(void)
{
    FRESULT res;

    res=f_mkdir_recursive(FDR_INFO.dir_name);
    if (res!=FR_OK && res!=FR_EXIST)
        return SDCP_MKDIR_ERR;

    f_chdir("/");
    res=f_chdir(FDR_INFO.dir_name);
    if (res!=FR_OK)
        return SDCP_CHDIR_ERR;

    // check single file mode
    if (FDR_INFO.single_file)
    {
        res=open_file();
        if (res==FR_EXIST)
            return SDCP_EXIST;
        else if (res!=FR_OK)
            return SDCP_OPEN_ERR;
    }

    return SDCP_OK;
}


// Save captured track
uint8_t sdc_save_track(void)
{
    FRESULT res;
    BYTE page, i, r = SDCS_OK;
    UINT bw, tbw;
    long bytesleft, trklen_old;


    // fill header struct
    FDR_HDR.sig[0] = 'F';
    FDR_HDR.sig[1] = 'D';
    FDR_HDR.sig[2] = 'R';
    FDR_HDR.sig[3] = '0';
    FDR_HDR.smprate = smp_rates[FDR_INFO.smprate];
    FDR_HDR.track = FDR_INFO.track;
    FDR_HDR.side = FDR_INFO.side;
    FDR_HDR.revs = FDR_INFO.revs;

    trklen_old = 0;
    for(i=0; i<=FDR_INFO.revs; i++) //revs+1 for tail
    {
        trklen[i] = FDR_INFO.trklen[i] - trklen_old;
        trklen_old = FDR_INFO.trklen[i];
    }
    bytesleft = FDR_INFO.trklen[FDR_INFO.revs];

    // open file
    if (!FDR_INFO.single_file)
    {
        res=open_file();
        if (res==FR_EXIST)
            return SDCS_EXIST;
        else if (res!=FR_OK)
            return SDCS_OPEN_ERR;
    }

    // write header
    res=f_write(&fd, &FDR_HDR, sizeof(FDR_HDR), &bw);
    if (res!=FR_OK)
    {
        r = SDCS_WRITE_ERR;
        goto exit;
    }
    res=f_write(&fd, trklen, sizeof(trklen[0])*(FDR_INFO.revs+1), &bw);
    if (res!=FR_OK)
    {
        r = SDCS_WRITE_ERR;
        goto exit;
    }

    // save data
    page=DATA_START_PAGE;
    while(bytesleft)
    {
        if (bytesleft > PAGE_SIZE)
            tbw = PAGE_SIZE;
        else
            tbw = bytesleft;

        SET_PAGE(page);
        res=f_write(&fd, PAGE_START, tbw, &bw);
        if (res!=FR_OK || bw!=tbw)
        {
            r = SDCS_WRITE_ERR;
            break;
        }

        // exit at break
        if(is_break_pressed())
        {
            r = SDCS_BREAK;
            break;
        }

        page++;
        bytesleft -= tbw;
        fdd_keep_alive(); // keep motor on
    }

exit:
    // close file
    if (!FDR_INFO.single_file)
        f_close(&fd);

    // restore default page
    SET_PAGE(PAGE_DEFAULT);
    return r;
}


// Reserved
uint8_t sdc_finalize(void)
{
    if (FDR_INFO.single_file)
        f_close(&fd);

    image_number++;
    return SDCF_OK;
}


