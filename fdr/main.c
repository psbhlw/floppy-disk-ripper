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

#include "main.h"
#include "ui.h"
#include "keyb.h"
#include "fdd.h"
#include "sdc.h"


//
struct FDR_INFO_t FDR_INFO;




int main(void)
{
    CPU14MHZ();

    // Some default values here
    memcpy(FDR_INFO.dir_name, "IMAGES/DISK1", sizeof ("IMAGES/DISK1"));
    FDR_INFO.single_file = 1;
    FDR_INFO.overwrite = 0;
    FDR_INFO.from_trk = 0;
    FDR_INFO.to_trk = 79;
    FDR_INFO.drive = 0;
    FDR_INFO.smprate = 1;
    FDR_INFO.sides = 2;
    FDR_INFO.revs = 1;

    // UI init
    ui_init();

    // Start UI
    while(1)
        ui_proc();
}




// Make image routine, called from UI.
uint8_t make_image(char **err_msg)
{
    uint8_t trk, side, res, rc, s1,s2, sdcpp=0;
    int8_t sdir;

    res = MKI_ERROR;
    //memset(FDR_INFO.file_name, 0, 12);
    FDR_INFO.file_name[0] = 0;
    ui_show_progress(PGS_INIT);

    // Init ZC
    if (sdc_init()!=SDCI_OK)
    {
        *err_msg="FatFs mount error.";
        goto exit;
    }

    // Init FDC
    rc=fdd_init();
    if (rc!=FDDI_OK)
    {
        if(rc==FDDI_NO_DRIVE)
            *err_msg="No drive.";
        else
            *err_msg="No disk in drive.";
        goto exit;
    }

    // Check sd card
    if (sdc_prepare()!=SDCP_OK)
    {
        *err_msg="SD Card (prepare) failed.";
        goto exit;
    }
    sdcpp=1; // sd card prepared, finalize needed

    s1=FDR_INFO.sides & 1;      // side from
    s2=1 + (!!FDR_INFO.sides);  // side to
    sdir=(FDR_INFO.from_trk >= FDR_INFO.to_trk) ? -1 : 1;   // step dir

    // Capture loop
    for (trk=FDR_INFO.from_trk; trk!=(FDR_INFO.to_trk+sdir); trk+=sdir)
        for (side=s1; side<s2; side++)
        {
            FDR_INFO.track = trk;
            FDR_INFO.side = side;
            ui_show_progress(PGS_READ);
            if (fdd_capture()!=FDDC_OK)
            {
                *err_msg="Floppy read timeout.";
                goto exit;
            }

            ui_show_progress(PGS_WRITE);
            if (sdc_save_track()!=SDCS_OK)
            {
                *err_msg="Save track failed.";
                goto exit;
            }

            if(is_break_pressed())
            {
                *err_msg="Break pressed.";
                goto exit;
            }
        }

    res=MKI_OK;

exit:
    if(sdcpp!=0 && sdc_finalize()!=SDCF_OK)
    {
        *err_msg="SD Card (finalize) failed.";
    }
    return res;
}

