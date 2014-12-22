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
#ifndef _MFM_H
#define _MFM_H

#include "raw_track.h"

#include <stdint.h>
#include <vector>
using namespace std;


// get_idams()/get_sectors() result data structure
struct idam_t
{
    uint32_t start_bit;             // start bit in mfm-stream
    uint32_t start_byte;            // byte offset in trk_t.rt_data
    uint8_t data_type;              // FE/FB/F8

    uint8_t track;                  // sector header
    uint8_t side;                   // sector header
    uint8_t sector;                 // sector header
    uint8_t length;                 // sector header

    vector<uint8_t> data;           // sector data
    uint16_t crc;                   // read crc value
    uint16_t calc_crc;              // calc'd crc value
    bool crc_ok;                    // is crc good
};


// read_track() result data structure
struct trk_t
{
    vector<uint8_t> rt_data;        // read bytes
    vector<uint16_t> syncbytes;     // offsets to sync bytes
    vector<idam_t> idams;           // found idams
};


// Class for represent MFM data of one track
class MFM
{
private:
    inline void m_init();
    inline void m_append(uint32_t val, uint32_t len);
    inline void m_eot();
    inline void m_flush();

    enum idamproc_st_t
    {
        IDAMPROC_WAIT_HDR_A1,
        IDAMPROC_WAIT_HDR_FE,
        IDAMPROC_GET_HDR,
        IDAMPROC_WAIT_DATA_A1,
        IDAMPROC_WAIT_DATA_F8FB,
        IDAMPROC_GET_DATA
    };
    void idamproc_init();
    idam_t* idamproc_fsm(uint8_t dbyte, uint8_t syncflag, uint32_t &skip_bytes, uint32_t bit_cntr, uint32_t byte_cntr);

    uint32_t cbits,cbitval;
    uint32_t bitlen;

    idamproc_st_t idamproc_state;
    uint32_t idamproc_sb;
    uint16_t idamproc_crc;
    idam_t idamproc_idam;
    uint8_t hdr_data[6];

    static uint16_t crc_ccitt[256];
    static void crc16_init();
    static inline uint16_t crc16(uint16_t crc, uint8_t b);
    static uint16_t crc16_block(uint8_t *ptr, uint16_t len, uint16_t crc=0xffff);

public:
    MFM() {crc16_init();}
    MFM(RawTrack* trk, float bit_time) {crc16_init(); raw_to_mfm(trk, bit_time);}
    ~MFM() {}

    void raw_to_mfm(RawTrack* trk, float bit_time);
    RawTrack* mfm_to_raw(uint32_t smp_rate, const float bit_time);

    trk_t* read_track(uint8_t rev);
    vector<idam_t>& get_idams(uint8_t rev);
    vector<idam_t>& get_sectors(uint8_t rev, bool sort=false);
    static vector<idam_t>& get_sectors(vector<idam_t> &idams, bool sort=false);


    vector<uint8_t> data;
    vector<uint32_t> bit_lengths;
    uint32_t tail_bitlen;
};

#endif
