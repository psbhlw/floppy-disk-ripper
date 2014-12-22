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
#ifndef _FMT_AUTO_H
#define _FMT_AUTO_H

#include "fmt_base.h"
#include "fdrc.h"
#include "raw_track.h"


// Image auto-loader (detect format and call appropriate loader).
class Auto_Loader : public ImgLoader
{
public:
    Auto_Loader() {}
    virtual ~Auto_Loader() {}

    virtual bool load_trk(const char *fname, struct fdrc_opts &opts, RawTrack* (&trks)[256][2]);
};


// Image auto-saver (call appropriate saver).
class Auto_Saver : public ImgSaver
{
public:
    Auto_Saver() {}
    virtual ~Auto_Saver() {}

    virtual bool save(struct fdrc_opts &opts, RawTrack* (&trks)[256][2]);
};

#endif
