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
#include "raw_track.h"


// Get rev start offset and rev length
void RawTrack::get_rev_info(uint8_t rev, vector<uint8_t>::iterator &begin, uint32_t &rev_len)
{
	begin = track_data.begin();

    if(rev==0) // all revs
    {
        rev_len = track_data.size();
        return;
    }

    rev--;
    if(rev>=rev_lengths.size())
    {
		rev_len = 0;
        return;
    }

	rev_len = rev_lengths[rev];
	while(rev--)
		begin += rev_lengths[rev];
}


// Get tail start offset and length
void RawTrack::get_tail_info(vector<uint8_t>::iterator &begin, uint32_t &tail_len)
{
	begin = track_data.end() + 1 - tail_length;
	tail_len = tail_length;
}


// Get number of samples
uint32_t RawTrack::count_samples(vector<uint8_t>::iterator begin, uint32_t len)
{
	uint32_t sum=0;
	while(len--)
		sum += *begin++;
	return sum;
}


// Get track time in seconds (for 1 revolution)
double RawTrack::get_track_time(uint8_t rev)
{
	vector<uint8_t>::iterator revdata;
	uint32_t rev_len;
	get_rev_info(rev, revdata, rev_len);

	uint32_t smps = count_samples(revdata, rev_len);
	return (double)smps / (double)smp_rate;
}


// Get index pointer time in seconds
double RawTrack::get_ip_time()
{
	vector<uint8_t>::iterator revdata;
	uint32_t rev_len;
	get_tail_info(revdata, rev_len);

	uint32_t smps = count_samples(revdata, rev_len);
	return (double)smps / (double)smp_rate;
}

