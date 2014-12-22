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
#include "fmt_udi.h"
#include "mfm.h"

#include <stdio.h>
#include <iostream>
using namespace std;


// Save UDI image
bool UDI_Saver::save(struct fdrc_opts &opts, RawTrack* (&trks)[256][2])
{
    // cannot use all revs for UDI
    if(opts.use_rev == 0)
    {
        cerr << "UDI: Cannot use all revolutions for this format." << endl;
        return false;
    }
    // must be at least 1st track
    if(!trks[0][0])
    {
        cerr << "UDI: Track 0 side 0 absent." << endl;
        return false;
    }
    // requested revolution must also exist
    if(opts.use_rev > trks[0][0]->rev_lengths.size())
    {
        cerr << "UDI: Requested revolution does not exist for Track 0 side 0." << endl;
        return false;
    }

    // max track/side numbers
    uint8_t maxtrk=0, 
            maxside=(trks[0][1])?1:0;

    // array for UDI image
    vector<uint8_t> UDI;

    // put UDI header
    // char sig[4];
    UDI.push_back('U');
    UDI.push_back('D');
    UDI.push_back('I');
    UDI.push_back('!');
    // uint32_t size;
    UDI.insert(UDI.end(), 4, 0);
    // uint8_t vers,mtrk,mside,res;
    // uint32_t exthdr;
    UDI.insert(UDI.end(), 8, 0);

    TracksAnalyser ta;
    // put tracks
    for (int t = 0; t < 256; ++t)
    {
        // no more tracks?
        if(!trks[t][0])
            break;

        // add side 0 track
        if(!add_udi_track(trks,t,0, opts.use_rev, opts.bit_time, UDI, &ta))
        {
            cerr << "UDI: Requested revolution does not exist for Track " << t << " side 0." << endl;
            return false;
        }

        // add side 1 track if double side disk
        if(maxside)
        {
            // always must be 2nd side here
            if(!trks[t][1])
            {
                cerr << "UDI: Track " << t << " side 1 absent." << endl;
                return false;
            }
            // add side 1 track
            if(!add_udi_track(trks,t,1, opts.use_rev, opts.bit_time, UDI, &ta))
            {
                cerr << "UDI: Requested revolution does not exist for Track " << t << " side 1." << endl;
                return false;
            }
        }

        maxtrk = t;
    }

    // update UDI header
    uint32_t sz = UDI.size();
    UDI[4] = sz & 0xff;
    UDI[5] = (sz >> 8) & 0xff;
    UDI[6] = (sz >> 16) & 0xff;
    UDI[7] = (sz >> 24) & 0xff;
    UDI[9] = maxtrk;
    UDI[10] = maxside;

    // calc and append CRC
    uint32_t crc = 0xffffffff;
    for (vector<uint8_t>::iterator b = UDI.begin(); b != UDI.end(); ++b)
    {
        crc = udi_CRC32(crc, *b);
    }
    UDI.push_back(crc & 0xff);
    UDI.push_back((crc >> 8) & 0xff);
    UDI.push_back((crc >> 16) & 0xff);
    UDI.push_back((crc >> 24) & 0xff);

    // save image file
    FILE *f = fopen(opts.outfile.c_str(), "wb");
    if(!f)
    {
        cerr << "UDI: Cannot create output file " << opts.outfile << endl;
        return false;
    }
    fwrite(&UDI[0], UDI.size(), 1, f);
    fclose(f);

    cout << "UDI: Image saved, " << maxtrk+1 << " track(s), " << maxside+1 << " side(s)." << endl;

    return true;
}



// Add track's data to UDI byte array
bool UDI_Saver::add_udi_track(RawTrack* (&trks)[256][2], uint8_t t, uint8_t s, uint8_t rev, float bit_time, vector<uint8_t> &udi_data, TracksAnalyser *ta)
{
    // convert track to MFM and do read_track()
    MFM *m = new MFM(trks[t][s], bit_time);
    trk_t *rdtrk = m->read_track(rev);
    vector<uint8_t> sb;

    // something has read?
    if(!rdtrk)
    {
        delete m;
        return false;
    }

    // make sync bytes map
    sb.resize((rdtrk->rt_data.size() + 7) / 8);
    for (vector<uint16_t>::iterator i = rdtrk->syncbytes.begin(); i != rdtrk->syncbytes.end(); ++i)
    {
        sb[(*i) >> 3] |= 1<<((*i) & 7);
    }

    // append track
    udi_data.push_back(0);
    udi_data.push_back(rdtrk->rt_data.size() & 0xff);
    udi_data.push_back(rdtrk->rt_data.size() >> 8);
    udi_data.insert( udi_data.end(), rdtrk->rt_data.begin(), rdtrk->rt_data.end() );
    udi_data.insert( udi_data.end(), sb.begin(), sb.end() );

    // analyse track and show errors
    if(ta) ta->analyse(t,s,rdtrk->idams);

    delete rdtrk;
    delete m;
    return true;
}



// UDI custom CRC32
inline uint32_t UDI_Saver::udi_CRC32(uint32_t CRC, unsigned char Symbol)
{
   uint32_t temp;
   int k;
   CRC ^= -1l ^ Symbol;
   for(k = 8; k--;)
      { temp = -(CRC & 1), CRC >>= 1, CRC ^= 0xEDB88320ul & temp; }
   CRC ^= -1l;
   return CRC;
}

