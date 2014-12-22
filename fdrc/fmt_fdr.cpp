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
#include "fmt_fdr.h"

#include <stdio.h>
#include <iostream>
using namespace std;


// Load FDR file
bool FDR_Loader::load_trk(const char *fname, struct fdrc_opts &opts, RawTrack* (&trks)[256][2])
{
    struct
    {
        uint32_t sig;           // signature FDR0
        uint32_t smp_rate;      // dword sample rate
        uint8_t track,side;     // track, side
        uint8_t revs;           // captured revolutions
    } __attribute__((__packed__)) hdr;
    bool res = false;
    RawTrack *ldtrks[256][2] = {{NULL}}; // loaded tracks, temp array

    // open file
    FILE *f;
    f = fopen(fname, "rb");
    if(!f)
        return res;

    while(1)
    {
        // read header
        if (!Fread(f, &hdr, sizeof(hdr)))
            break;

        // check signature
        res = false;
        if (hdr.sig != 0x30524446) //'FDR0'
        {
            cerr << "FDR: signature not found in file " << fname << endl;
            break;
        }
        if (hdr.side > 1)
        {
            cerr << "FDR: side is greater than 2 in file " << fname << endl;
            break;
        }

        // create RawTrack
        RawTrack* trk = new RawTrack;
        if(opts.fsmp_rate)
            trk->smp_rate = opts.fsmp_rate; // force smp rate
        else
            trk->smp_rate = hdr.smp_rate;

        // get track lengths
        trk->rev_lengths.resize(hdr.revs);
        res = Fread(f, &trk->rev_lengths[0], sizeof(uint32_t) * (hdr.revs));
        res = Fread(f, &trk->tail_length, sizeof(uint32_t)) && res;

        // calc total data length
        uint32_t ts=trk->tail_length;
        for(uint32_t i=0; i<hdr.revs; ts+=trk->rev_lengths[i++]);

        // get track data
        trk->track_data.resize(ts);
        res = Fread(f, &trk->track_data[0], ts) && res;
        if (!res)
        {
            delete trk;
            break;
        }

        ldtrks[hdr.track][hdr.side] = trk;
        // res = true;
    }

    fclose(f);

    // if loaded ok - move loaded tracks into trks[][]
    if(res)
    {
        for (int t = 0; t < 256; ++t)
        for (int s = 0; s < 2; ++s)
        {
            if(ldtrks[t][s])
            {
                if(opts.trk_map[t][s])
                {
                    if(trks[t][s]) delete trks[t][s];
                    trks[t][s] = ldtrks[t][s];
                }
                else
                    delete ldtrks[t][s];
            }
        }
    }

    return res;
}

