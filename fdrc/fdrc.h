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
#ifndef _FDRC_H
#define _FDRC_H

#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#define __FDRC_VERSION__ "v1.0 alpha"


struct fdrc_opts
{
    string infmt; vector<string> infiles;
    string outfmt; string outfile;

    string analyse_file, hist_file;
    bool show_stat;
    uint8_t use_rev;
    int32_t move_ip_pos,
            fsmp_rate,
            ftrk_len;
    bool trk_map[256][2];
    float bit_time;

    inline void show()
    {
        cout << "used options:" << endl <<
            "infmt: " << infmt << endl <<
            "outfmt: " << outfmt << endl <<
            "analyse_file: " << analyse_file << endl <<
            "hist_file: " << hist_file << endl <<
            "show_stat: " << show_stat << endl <<
            "use_rev: " << use_rev << endl <<
            "move_ip_pos: " << move_ip_pos << endl <<
            "fsmp_rate: " << fsmp_rate << endl <<
            "ftrk_len: " << ftrk_len << endl <<
            "bit_time: " << fixed << bit_time << endl;
        for(size_t i=0; i<this->infiles.size(); i++)
            cout << "infile: " << this->infiles[i] << endl;

        cout << "outfile: " << outfile << endl;
    }

    inline void init()
    {
        // default settings
        //infmt = "fdr";
        //outfmt = "udi";
        show_stat = false;
        use_rev=1;
        move_ip_pos=0;
        fsmp_rate=0;
        ftrk_len=0;
        bit_time=0.000002f;
    }

    void fill_trk_map(string trks, int sides)
    {
        uint8_t tfrom, tto;
        size_t pos;

        if((pos = trks.find("-")) == string::npos)
        {
            tfrom = tto = atoi (trks.c_str());
        }
        else
        {
            tfrom = atoi (trks.substr(0, pos).c_str());
            tto   = atoi (trks.substr(pos+1, string::npos).c_str());
        }

        for (int t = tfrom; t <= tto; ++t)
        {
            if (sides == 0 || sides == 2)
                trk_map[t][0] = true;
            if (sides == 1 || sides == 2)
                trk_map[t][1] = true;
        }
    }

    void build_trk_map(string trks, int sides)
    {
        size_t pos;
        string val;
        while ((pos = trks.find(",")) != string::npos)
        {
            val = trks.substr(0, pos);
            fill_trk_map(val, sides);
            trks.erase(0, pos + 1);
        }

        if(trks != "")
            fill_trk_map(trks, sides);
    }
    
} ;


#endif
