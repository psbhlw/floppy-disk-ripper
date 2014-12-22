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
#ifndef _RAW_TRACK_H
#define _RAW_TRACK_H

#include <stdint.h>
#include <vector>
using namespace std;


// Class for represent RAW track (RLE of sampled data)
class RawTrack
{
public:
    RawTrack() {}
    ~RawTrack() {}

	void get_rev_info(uint8_t rev, vector<uint8_t>::iterator &begin, uint32_t &rev_len);
	void get_tail_info(vector<uint8_t>::iterator &begin, uint32_t &tail_len);
	uint32_t count_samples(vector<uint8_t>::iterator begin, uint32_t len);
	double get_track_time(uint8_t rev);
	double get_ip_time();

    uint32_t smp_rate;
    vector<uint8_t> track_data;
    vector<uint32_t> rev_lengths;
    uint32_t tail_length;
};

#endif
