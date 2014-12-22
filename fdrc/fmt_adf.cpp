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
#include "fmt_adf.h"

#include <iostream>
using namespace std;


// ADF file loader
bool ADF_Loader::load_trk(const char *fname, struct fdrc_opts &opts, RawTrack* (&trks)[256][2])
{
    cerr << "ADF loader isn't implemented yet." << endl;
    return false;
}


// ADF file saver
bool ADF_Saver::save(struct fdrc_opts &opts, RawTrack* (&trks)[256][2])
{
    cerr << "ADF saver isn't implemented yet." << endl;
    return false;
}

