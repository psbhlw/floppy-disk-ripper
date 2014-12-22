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
#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <getopt.h>
#include <stdio.h>

#include "fdrc.h"
#include "raw_track.h"
#include "mfm.h"

#include "fmt_adf.h"
#include "fmt_fdr.h"
#include "fmt_img.h"
#include "fmt_mfm.h"
#include "fmt_raw.h"
#include "fmt_udi.h"
#include "fmt_auto.h"

#include "trk_analyser.h"
#include "stats.h"

using namespace std;

struct fdrc_opts opts;



// Show usage
static
void usage(char *name)
{
    cout << "Floppy Disk Ripper Converter " __FDRC_VERSION__ ". (c)2014, psb^hlw.\n\nUsage: " <<
        name << " [options] input_file [input_file [...]]\n\n"

        "Options:\n"
        "  -i fmt       input format (fdr, adf, raw, mfm)\n"
        "  -o fmt       output format (udi, adf, img)\n"
        "  -O filename  output image file name\n"
        "  -T tracks    load only specified tracks (ex.: 0,40-79)\n"
        "  -S sides     load only specified sides (0, 1 or 'both' (default))\n"
//        "  -a filename  put tracks analysis into file ('-' for stdout)\n"
//        "  -H filename  put tracks histograms into file ('-' for stdout)\n"
        "  -h           show this help\n"
//        "  -s           show image statistics\n"
        "  -r n         use revolution n (1 - default, 0 - all revolutions if possible)\n"
//        "  -m n         move IP position by n, samples\n"
        "  -w n         set bit window time, sec (0.000002 for DD, 0.000001 for HD)\n"
        "  -f n         force sample rate, Hz\n"
//        "  -l n         force track length, data bits\n"
        ;
}




// Entry point
int main(int argc, char **argv)
{
    ImgLoader *ldr = new Auto_Loader;
    ImgSaver *saver = new Auto_Saver;
    string sds, use_trks = "0-255";
    int use_sides = 2;

    // defaults
    opts.init();

    // get options
    int c;
    opterr = 0;
    while ((c = getopt (argc, argv, "i:o:O:T:S:a:hH:sr:m:w:f:l:")) != -1)
        switch (c)
        {
            case 'i':
                opts.infmt = optarg;
                if (opts.infmt == "fdr")
                    ldr = new FDR_Loader;
                else if (opts.infmt == "adf")
                    ldr = new ADF_Loader;
                else if (opts.infmt == "raw")
                    ldr = new RAW_Loader;
                else if (opts.infmt == "mfm")
                    ldr = new MFM_Loader;
                else
                {
                    cerr << "Unknown input format." << endl;
                    return 1;
                }
                break;

            case 'o':
                opts.outfmt = optarg;
                if (opts.outfmt == "udi")
                    saver = new UDI_Saver;
                else if (opts.outfmt == "adf")
                    saver = new ADF_Saver;
                else if (opts.outfmt == "img")
                    saver = new IMG_Saver;
                else
                {
                    cerr << "Unknown output format." << endl;
                    return 1;
                }
                break;

            case 'O':
                opts.outfile = optarg;
                break;

            case 'T':
                use_trks = optarg;
                break;

            case 'S':
                sds = optarg;
                if (sds == "0")
                    use_sides = 0;
                else if (sds == "1")
                    use_sides = 1;
                else if (sds == "2" || sds == "both")
                    use_sides = 2;
                else
                {
                    cerr << "Wrong side specified." << endl;
                    return 1;
                }
                break;

            case 'a':
                opts.analyse_file = optarg;
                break;

            case 'H':
                opts.hist_file = optarg;
                break;

            case 'h':
                usage(argv[0]);
                return 0;

            case 's':
                opts.show_stat = true;
                break;

            case 'r':
                opts.use_rev = atoi(optarg);
                break;

            case 'm':
                opts.move_ip_pos = atoi(optarg);
                break;

            case 'w':
                opts.bit_time = atof(optarg);
                break;

            case 'f':
                opts.fsmp_rate = atoi(optarg);
                break;

            case 'l':
                opts.ftrk_len = atoi(optarg);
                break;

            case '?':
                switch (optopt)
                {
                    case 'i':
                    case 'o':
                    case 'O':
                    case 'T':
                    case 'S':
                    case 'a':
                    case 'H':
                    case 'r':
                    case 'm':
                    case 'w':
                    case 'f':
                    case 'l':
                        cerr << "Option -" << (char)optopt << " requires an argument." << endl;
                        break;
                    default:
                        usage(argv[0]);
                }
                return 1;

            default:
                return 1;
        }

    // get file names
    for (int index = optind; index<argc; index++)
        opts.infiles.push_back(argv[index]);

    if (opts.infiles.size() == 0)
    {
        cerr << "No input files specified. Try '-h' for help." << endl;
        return 1;
    }

    opts.build_trk_map(use_trks, use_sides);

    // show opts
#ifdef DEBUG
    //opts.show();
#endif

    // loaded raw tracks
    RawTrack *trks[256][2] = {{NULL}};

    if (!ldr->load(opts, trks))
    {
        // cerr << "Couldn't load input file." << endl;
        return 1;
    }

    if (opts.analyse_file != "")
        tracks_analyse(opts, trks);
    if (opts.hist_file != "")
        tracks_hist(opts, trks);
    if (opts.show_stat)
        show_statistics(opts, trks);

    if (opts.outfile != "")
        if (!saver->save(opts, trks))
        {
            // cerr << "Couldn't save file." << endl;
            return 1;
        }



    return 0;
}


