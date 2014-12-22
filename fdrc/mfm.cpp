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
#include "mfm.h"
#include <algorithm>
using namespace std;



// MFM bit-stream: Clean track data
inline void MFM::m_init()
{
    cbits=0;
    cbitval=0;
    bitlen=0;
    bit_lengths.clear();
    data.clear();
}

// MFM bit-stream: Append bits to the stream
inline void MFM::m_append(uint32_t val, uint32_t len)
{
    cbitval = (cbitval << len) | (val & ((1<<len)-1));
    cbits += len;

    while(cbits>=8)
    {
        data.push_back( (uint8_t)(cbitval >> (cbits-8)) );
        bitlen+=8;
        //cbitval &= ~ (0xff << (cbits-8));
        cbits-=8;
    }
}

// MFM bit-stream: End of track - save track length
inline void MFM::m_eot()
{
    bit_lengths.push_back(bitlen);
    bitlen=0;
}

// MFM bit-stream: Flush the last data
inline void MFM::m_flush()
{
    data.push_back( (uint8_t)(cbitval << (8-cbits)) );
    bitlen += cbits;
    cbits=0;
}




/*----------------------------------------------------------------------------*/

// Convert RawTrack to MFM bit-stream
void MFM::raw_to_mfm(RawTrack* trk, const float bit_time)
{
    // nothing to convert?
    if(!trk)
        return;

    // init mfm data
    m_init();

    // scan all revs
    // todo: more smart decoder with glitch filter/PLL
    vector<uint8_t>::iterator datab = trk->track_data.begin();
    float bits_per_smp = (1.0f / trk->smp_rate) / bit_time;
    uint32_t bits;
    for (vector<uint32_t>::iterator r = trk->rev_lengths.begin(); r != trk->rev_lengths.end(); ++r)
    {
        // decode bits from raw data
        for (uint32_t i = 0; i < *r; ++i)
        {
            bits = (uint32_t)((float)*datab * bits_per_smp + 0.5);
            m_append( (1<<(bits-1)), bits );
            datab++;
        }
        // save end-of-track point
        m_eot();
    }

    // decode bits from tail raw data
    for (uint32_t i = 0; i < trk->tail_length; ++i)
    {
        bits = (uint32_t)((float)*datab * bits_per_smp + 0.5);
        m_append( (1<<(bits-1)), bits );
        datab++;
    }
    // save end-of-tail point
    tail_bitlen = bitlen;

    m_flush();
}



// Convert MFM bit-stream to RawTrack
RawTrack* MFM::mfm_to_raw(uint32_t smp_rate, const float bit_time)
{
    // todo
    return NULL;
}




/*----------------------------------------------------------------------------*/

// Initialize IDAMs processing FSM
void MFM::idamproc_init()
{
    idamproc_state=IDAMPROC_WAIT_HDR_A1;
    idamproc_sb=0;
}

// IDAMs processing FSM.
// Function returns idam_t* of sector's hdr/body or NULL.
// If need to turn off sync (sector's body) - skip_bytes is set to amount of bytes
// to which sync should be turned off.
idam_t* MFM::idamproc_fsm(uint8_t dbyte, uint8_t syncflag, uint32_t &skip_bytes, uint32_t bit_cntr, uint32_t byte_cntr)
{
    static const uint32_t lengths[4]={128,256,512,1024}; // sector lengths
    idam_t *res=NULL;

    switch(idamproc_state)
    {
        // header search
        case IDAMPROC_WAIT_HDR_A1: //wait a1
            if (dbyte==0xa1 && syncflag)
                idamproc_state=IDAMPROC_WAIT_HDR_FE;
            break;

        case IDAMPROC_WAIT_HDR_FE: //wait fe
            if (dbyte==0xa1 && syncflag)
                idamproc_state=IDAMPROC_WAIT_HDR_FE;
            else if (dbyte==0xfe && !syncflag)
                {
                    idamproc_idam.start_bit = bit_cntr;
                    idamproc_idam.start_byte = byte_cntr;
                    idamproc_idam.data_type = dbyte;
                    idamproc_idam.data.clear();
                    idamproc_crc = crc16(0xCDB4, dbyte);
                    idamproc_state=IDAMPROC_GET_HDR;
                    idamproc_sb=skip_bytes=6;
                }
            else
                idamproc_state=IDAMPROC_WAIT_HDR_A1;
            break;

        case IDAMPROC_GET_HDR: //get hdr bytes
            switch(--idamproc_sb)
            {
                case 5:
                    idamproc_idam.track = dbyte;
                    idamproc_crc = crc16 (idamproc_crc, dbyte);
                    break;
                case 4:
                    idamproc_idam.side = dbyte;
                    idamproc_crc = crc16 (idamproc_crc, dbyte);
                    break;
                case 3:
                    idamproc_idam.sector = dbyte;
                    idamproc_crc = crc16 (idamproc_crc, dbyte);
                    break;
                case 2:
                    idamproc_idam.length = dbyte;
                    idamproc_crc = crc16 (idamproc_crc, dbyte);
                    break;
                case 1:
                    idamproc_idam.crc = dbyte;
                    break;
                case 0:
                    idamproc_idam.crc <<= 8;
                    idamproc_idam.crc |= dbyte;
                    idamproc_idam.calc_crc = idamproc_crc;
                    idamproc_idam.crc_ok = (idamproc_idam.crc == idamproc_crc);
                    res = &idamproc_idam;

                    idamproc_state = (idamproc_idam.crc_ok && idamproc_idam.length < sizeof(lengths)/sizeof(lengths[0])) ? IDAMPROC_WAIT_DATA_A1 : IDAMPROC_WAIT_HDR_A1;
                    break;
            }
            break;

        // sector's data search
        case IDAMPROC_WAIT_DATA_A1: //wait a1
            if (dbyte==0xa1 && syncflag)
                idamproc_state=IDAMPROC_WAIT_DATA_F8FB;
            break;

        case IDAMPROC_WAIT_DATA_F8FB: //wait f8/fb/fe
            if (dbyte==0xa1 && syncflag)
                idamproc_state=IDAMPROC_WAIT_DATA_F8FB;
            else if (dbyte==0xfe && !syncflag)
                {
                    idamproc_idam.start_bit = bit_cntr;
                    idamproc_idam.start_byte = byte_cntr;
                    idamproc_idam.data_type = dbyte;
                    idamproc_crc = crc16(0xCDB4, dbyte);
                    idamproc_state=IDAMPROC_GET_HDR;
                    idamproc_sb=skip_bytes=6;
                }
            else if ((dbyte==0xfb || dbyte==0xf8) && !syncflag)
                {
                    idamproc_idam.start_bit = bit_cntr;
                    idamproc_idam.start_byte = byte_cntr;
                    idamproc_idam.data_type = dbyte;
                    idamproc_crc = crc16(0xCDB4, dbyte);
                    idamproc_state=IDAMPROC_GET_DATA;
                    idamproc_sb=skip_bytes=lengths[idamproc_idam.length]+2;
                }
            else
                idamproc_state=IDAMPROC_WAIT_HDR_A1;
            break;

        case IDAMPROC_GET_DATA: //get sector data
            switch(--idamproc_sb)
            {
                case 1:
                    idamproc_idam.crc = dbyte;
                    break;
                case 0:
                    idamproc_idam.crc <<= 8;
                    idamproc_idam.crc |= dbyte;
                    idamproc_idam.calc_crc = idamproc_crc;
                    idamproc_idam.crc_ok = (idamproc_idam.crc == idamproc_crc);
                    res = &idamproc_idam;

                    idamproc_state=IDAMPROC_WAIT_HDR_A1;
                    break;
                
                default:
                    idamproc_idam.data.push_back(dbyte);
                    idamproc_crc = crc16 (idamproc_crc, dbyte);
                    break;

            }
            break;

    }

    return res;
}


// Read the whole track, 1 rev or all revs
trk_t* MFM::read_track(uint8_t rev)
{
    // init
    idamproc_init();

    // get bits per track value
    uint32_t bits_in_trk;
    if(rev==0) // all revs
    {
        bits_in_trk = tail_bitlen;
        for (vector<uint32_t>::iterator i = bit_lengths.begin(); i != bit_lengths.end(); ++i)
            bits_in_trk += *i;
    }
    else
    {
        rev--;
        if(rev>=bit_lengths.size())
            return NULL;
        bits_in_trk = bit_lengths[rev];
    }

    // get start bit number of stream
    uint32_t start_bit = 0;
    for (int i = 0; i < rev; ++i)
        start_bit += bit_lengths[i];

    // read track struct
    trk_t *trk = new trk_t;

    uint8_t mpc=0;              // mfm bit counter [0..15]
    uint16_t reg=0;             // mfm reg for sync
    uint32_t cbits = 0;         // stream bits counter
    // bit-stream pointer and byte mask
    vector<uint8_t>::iterator mp = data.begin() + (start_bit >> 3);
    uint8_t mp_mask = 1 << ((~start_bit) & 0x07);

    uint32_t nosync_counter=0;  // amount of bytes with no sync to A1/C2
    uint8_t syncflag=0;         // 'found sync byte' flag
    uint8_t dbyte=0, cbyte=0;   // data/clock bytes

    // process all bits from specified revolution
    while(cbits < bits_in_trk)
    {
        // shift new bit in
        uint32_t bitval = !!(*mp & mp_mask);
        mp_mask>>=1;
        if(mp_mask==0)
        {
            mp++;
            mp_mask=0x80;
        }
        cbits++;

        // store bit to data/clock bytes
        reg <<= 1; reg |= bitval;
        if(mpc&1)
        {
            dbyte <<= 1; dbyte |= bitval;
        }
        else
        {
            cbyte <<= 1; cbyte |= bitval;
        }
        mpc++;

        // check sync
        if((reg&0xffff) == 0x4489 && nosync_counter==0)
        {
            //$A1 $0A sync pattern
            trk->syncbytes.push_back(trk->rt_data.size());
            mpc=16;
            syncflag=1;
        }
        else
        if((reg&0xffff) == 0x5224 && nosync_counter==0)
        {
            //$C2 $14 sync pattern
            trk->syncbytes.push_back(trk->rt_data.size());
            mpc=16;
            syncflag=1;
        }

        // byte has filled - save it
        if (mpc>=16)
        {
            trk->rt_data.push_back(dbyte);
            // process IDAMs
            if(nosync_counter) nosync_counter--;
            idam_t *idam = idamproc_fsm(dbyte, syncflag, nosync_counter, cbits-16, trk->rt_data.size());
            if(idam)
                trk->idams.push_back(*idam);

            mpc=0;
            syncflag=0;
        }
    }

    return trk;
}





/*----------------------------------------------------------------------------*/

// Compare sectors numbers for sorting
static bool compare_sectors(idam_t i, idam_t j)
{
    return (i.sector < j.sector);
}

// Scan idams and return new vector with only sectors.
vector<idam_t>& MFM::get_sectors(vector<idam_t> &idams, bool sort)
{
    int state=0;
    vector<idam_t> *res = new vector<idam_t>;
    for (vector<idam_t>::iterator i = idams.begin(); i != idams.end(); ++i)
    {
        switch(state)
        {
            case 0: // wait FE
                if((*i).data_type == 0xfe && (*i).crc_ok)
                    state=1;
                break;

            case 1: // must be F8/FB
                if((*i).data_type == 0xf8 || (*i).data_type == 0xfb)
                    (*res).push_back(*i);
                else if((*i).data_type == 0xfe && (*i).crc_ok)
                    state=1; // FE again, stay here
                else
                    state=0; // wrong type
                break;
        }
        
    }

    // do sort by sector's numbers
    if(sort)
        stable_sort ((*res).begin(), (*res).end(), compare_sectors);
    return *res;
}




/*----------------------------------------------------------------------------*/

// crc16 table
uint16_t MFM::crc_ccitt[256];

// crc16 init table
void MFM::crc16_init()
{
    static bool inited = false;
    if(inited)
        return;

    for (int i = 0; i < 256; i++)
    {
        uint16_t w = i << 8;
        for (int j = 0; j < 8; j++)
            w = (w << 1) ^ ((w & 0x8000) ? 0x1021 : 0);
        crc_ccitt[i] = w;
    }
    inited = true;
}

// calc crc16 for 1 byte
inline uint16_t MFM::crc16(uint16_t crc, uint8_t b)
{
    crc = (crc << 8) ^ MFM::crc_ccitt[((crc >> 8) & 0xff) ^ b];
    return crc;
}

// calc crc16 for block
uint16_t MFM::crc16_block(uint8_t *ptr, uint16_t len, uint16_t crc)
{
    while(len--)
        crc = MFM::crc16(crc, *ptr++);
    return crc;
}

