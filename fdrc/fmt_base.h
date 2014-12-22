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
#ifndef _FMT_BASE_H
#define _FMT_BASE_H

#include "fmt_base.h"
#include "fdrc.h"
#include "raw_track.h"

#include <string>
#include <vector>
using namespace std;


// Image loader base class
class ImgLoader
{
private:

public:
    ImgLoader() {}
    virtual ~ImgLoader() {}

    void expand_dirs(vector<string> &innames, vector<string> &outnames);
    bool Fread(FILE *f, void *buf, size_t len);

    virtual bool load(struct fdrc_opts &opts, RawTrack* (&trks)[256][2]);
    virtual bool load_trk(const char *fname, struct fdrc_opts &opts, RawTrack* (&trks)[256][2]) = 0;
};



// Image saver base class
class ImgSaver
{
public:
    ImgSaver() {}
    virtual ~ImgSaver() {}

    virtual bool save(struct fdrc_opts &opts, RawTrack* (&trks)[256][2]) = 0;
};

#endif
