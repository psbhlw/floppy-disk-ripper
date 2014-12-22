/*
    This file is part of Floppy Disk Ripper Converter (FDRC) program.
    Copyright (C) 2014, psb^hlw.

    FDRC is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FDRC is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "fmt_auto.h"
#include "fmt_fdr.h"
#include "fmt_udi.h"
#include "fmt_adf.h"
#include "fmt_img.h"

#include <iostream>
#include <stdio.h>
using namespace std;


// Detect format and call appropriate load_trk()
bool Auto_Loader::load_trk(const char *fname, struct fdrc_opts &opts, RawTrack* (&trks)[256][2])
{
    ImgLoader *ldr = NULL;
    char sig[5] = {0};
    bool res = false;

    // open file
    FILE *f = fopen(fname, "rb");
    if (!f)
        return false;

    // check file type
    if(Fread(f, sig, 4))
    {
        if(string(sig) == "FDR0")
        {
            ldr = new FDR_Loader;
        }

    }

    fclose(f);

    // go load track
    if(!ldr)
    {
        cerr << "Auto loader: Unknown file format " << fname << endl;
        return false;
    }
    else
    {
        res = ldr->load_trk(fname, opts, trks);
        delete ldr;
    }
    return res;
}



// Call appropriate saver (based on file ext.)
bool Auto_Saver::save(struct fdrc_opts &opts, RawTrack* (&trks)[256][2])
{
    ImgSaver *saver = new UDI_Saver; // default

    // check ext.
    size_t found = opts.outfile.rfind(".");
    if(found != string::npos)
    {
        string ext = opts.outfile.substr(found+1);
        if(ext == "udi")
            saver = new UDI_Saver;
        else if(ext == "adf")
            saver = new ADF_Saver;
        else if(ext == "img")
            saver = new IMG_Saver;
    }

    // save
    bool res = saver->save(opts, trks);
    delete saver;
    return res;
}


