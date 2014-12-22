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
#include "trk_analyser.h"

#include <iostream>
using namespace std;


void tracks_analyse(struct fdrc_opts &opts, RawTrack* (&trks)[256][2]){}
void tracks_hist(struct fdrc_opts &opts, RawTrack* (&trks)[256][2]){}


void TracksAnalyser::analyse(uint8_t track, uint8_t side, vector<idam_t> &idams)
{
    vector<idam_t> &sectors = MFM::get_sectors(idams, true);

    // empty track
    if (sectors.size() == 0)
    {
        cerr << "TracksAnalyser: No data found at track " << (uint32_t)track << " side " << (uint32_t)side << endl;
        return;
    }

    for (vector<idam_t>::iterator s = sectors.begin(); s != sectors.end(); ++s)
    {
        // crc check
        if( !(*s).crc_ok )
        {
            cerr << "TracksAnalyser: CRC error at track " << (uint32_t)track << "(" << (uint32_t)(*s).track << ") side " <<
                    (uint32_t)side << "(" << (uint32_t)(*s).side << ") sector " << (uint32_t)(*s).sector << endl;
        }

        // track num mismatch
        if( (*s).track != track )
        {
            cerr << "TracksAnalyser: Track number mismatch: physical trk" << (uint32_t)track << ", read " << (uint32_t)(*s).track << endl;
        }
    }

    // missing / new sectors
    // vector<idam_t> &ps = (side) ? prev_sectors1 : prev_sectors0;
    vector<idam_t> &ps = prev_sectors0;
    if(!ps.empty())
    {
        vector<idam_t>::iterator snew=sectors.begin();
        vector<idam_t>::iterator sold=ps.begin();
        vector<uint8_t> missing, added;
        while (snew!=sectors.end() && sold!=ps.end())
        {
            if ((*snew).sector<(*sold).sector)
                { added.push_back((*snew).sector); ++snew; }
            else if ((*sold).sector<(*snew).sector)
                { missing.push_back((*sold).sector); ++sold; }
            else
                { ++snew; ++sold; }
        }

        if(!missing.empty())
        {
            cerr << "TracksAnalyser: Missing sectors at track " << (uint32_t)track << " side " << (uint32_t)side << ": ";
            for (vector<uint8_t>::iterator i = missing.begin(); i != missing.end(); ++i)
            {
                cerr << (uint32_t)*i << " ";
            }
            cerr << endl;
        }
        if(!added.empty())
        {
            cerr << "TracksAnalyser: New sectors at track " << (uint32_t)track << " side " << (uint32_t)side << ": ";
            for (vector<uint8_t>::iterator i = added.begin(); i != added.end(); ++i)
            {
                cerr << (uint32_t)*i << " ";
            }
            cerr << endl;
        }

    }
    ps.assign(sectors.begin(), sectors.end());

    delete &sectors;
}



