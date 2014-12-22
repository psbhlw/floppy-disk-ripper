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
#ifndef _TRK_AN_H
#define _TRK_AN_H

#include <stdint.h>
#include <vector>
#include "fdrc.h"
#include "raw_track.h"
#include "mfm.h"
using namespace std;

void tracks_analyse(struct fdrc_opts &opts, RawTrack* (&trks)[256][2]);
void tracks_hist(struct fdrc_opts &opts, RawTrack* (&trks)[256][2]);


//
class TracksAnalyser
{
private:
    vector<idam_t> prev_sectors0;
    // vector<idam_t> prev_sectors1;
public:
    TracksAnalyser() {}
    ~TracksAnalyser() {}

    void analyse(uint8_t track, uint8_t side, vector<idam_t> &idams);
};

#endif
