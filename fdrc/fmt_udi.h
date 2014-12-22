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
#ifndef _FMT_UDI_H
#define _FMT_UDI_H

#include "fmt_base.h"
#include "fdrc.h"
#include "raw_track.h"
#include "trk_analyser.h"

#include <stdint.h>
#include <vector>
using namespace std;


// UDI file saver
class UDI_Saver : public ImgSaver
{
private:
    bool add_udi_track(RawTrack* (&trks)[256][2], uint8_t t, uint8_t s, uint8_t rev, float bit_time, vector<uint8_t> &udi_data, TracksAnalyser *ta);
	inline uint32_t udi_CRC32(uint32_t CRC, unsigned char Symbol);
	
public:
    UDI_Saver() {}
    virtual ~UDI_Saver() {}

    virtual bool save(struct fdrc_opts &opts, RawTrack* (&trks)[256][2]);
};

#endif
