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
#include "fmt_base.h"
#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>


// Expand innames list into outnames list: directory items in innames -> files list.
void ImgLoader::expand_dirs(vector<string> &innames, vector<string> &outnames)
{
    struct dirent *dp;
    struct stat statbuf;
    DIR* dir;

    for (vector<string>::iterator i = innames.begin(); i != innames.end(); ++i)
    {
        // file/dir exists?
        if(stat((*i).c_str(), &statbuf) != 0)
        {
            cerr << "Image loader: File/dir " << *i << " not found." << endl;
            continue;
        }

        // is it a dir?
        if (S_ISDIR(statbuf.st_mode))
        {
            dir = opendir((*i).c_str());
            if(!dir)
                continue;

            while ((dp = readdir(dir)) != NULL)
            {

                // get entry's information
                string pathfile = (*i) + "/" + dp->d_name;
                if (stat(pathfile.c_str(), &statbuf) != 0)
                    continue;

                if (S_ISREG(statbuf.st_mode))
                {
                    outnames.push_back(pathfile);
                }

            }
            closedir(dir);

        }
        // is it a regular file?
        else if (S_ISREG(statbuf.st_mode))
        {
            outnames.push_back((*i));
        }
    }
}


// fread() wrapper
bool ImgLoader::Fread(FILE *f, void *buf, size_t len)
{
    size_t r = fread(buf, 1, len, f);
    return (r == len) ? true : false;
}


// Load tracks from input files (calls appropriate track loader)
bool ImgLoader::load(struct fdrc_opts &opts, RawTrack* (&trks)[256][2])
{
    vector<string> fnames;

    expand_dirs(opts.infiles, fnames);

    if (fnames.size() == 0)
    {
        cerr << "Image loader: No input files found." << endl;
        return false;
    }

    bool loaded = false;
    for (vector<string>::iterator i = fnames.begin(); i != fnames.end(); ++i)
    {
        loaded = this->load_trk((*i).c_str(), opts, trks) || loaded;
    }

    if (!loaded)
    {
        cerr << "Image loader: No tracks loaded." << endl;
        return false;
    }

    return true;
}

