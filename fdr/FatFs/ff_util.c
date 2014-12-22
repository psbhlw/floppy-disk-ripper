/*
    This file is part of Floppy Disk Ripper (FDR) program.
    Copyright (C) 2014, psb^hlw.

    FDR is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    FDR is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "ff.h"     /* Declarations of FatFs API */



/* Make sequence of directories */

FRESULT f_mkdir_recursive (TCHAR* path)
{
    TCHAR *p=path,t;
    BYTE n=0;
    FRESULT res;

    while(*p)
    {
        if(*p=='\\' || *p=='/')
        {
            n=0;
            t=*p; *p=0;
            res = f_mkdir (path);
            *p=t;
            if(res!=FR_OK && res!=FR_EXIST)
                return res;
        }
        else
            n++;
        p++;
    }
    if(n)
        res = f_mkdir (path);
    return res;
}

