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
#include <string.h>

#include "main.h"
#include "ui.h"
#include "keyb.h"
#include "textmod.h"


// Menu item handlers protos
static void fmt_drive(struct menu_item_t *m);
static void fmt_dir(struct menu_item_t *m);
static void fmt_sings(struct menu_item_t *m);
static void fmt_overw(struct menu_item_t *m);
static void fmt_ftrk(struct menu_item_t *m);
static void fmt_ltrk(struct menu_item_t *m);
static void fmt_side(struct menu_item_t *m);
static void fmt_revs(struct menu_item_t *m);
static void fmt_imgtype(struct menu_item_t *m);
static void fmt_smprate(struct menu_item_t *m);
static void act_drive(struct menu_item_t *m, uint8_t xoffs, uint8_t y, uint8_t len);
static void act_dir(struct menu_item_t *m, uint8_t xoffs, uint8_t y, uint8_t len);
static void act_sings(struct menu_item_t *m, uint8_t xoffs, uint8_t y, uint8_t len);
static void act_overw(struct menu_item_t *m, uint8_t xoffs, uint8_t y, uint8_t len);
static void act_ftrk(struct menu_item_t *m, uint8_t xoffs, uint8_t y, uint8_t len);
static void act_ltrk(struct menu_item_t *m, uint8_t xoffs, uint8_t y, uint8_t len);
static void act_side(struct menu_item_t *m, uint8_t xoffs, uint8_t y, uint8_t len);
static void act_revs(struct menu_item_t *m, uint8_t xoffs, uint8_t y, uint8_t len);
static void act_imgtype(struct menu_item_t *m, uint8_t xoffs, uint8_t y, uint8_t len);
static void act_smprate(struct menu_item_t *m, uint8_t xoffs, uint8_t y, uint8_t len);
static void act_start(struct menu_item_t *m, uint8_t xoffs, uint8_t y, uint8_t len);


// Colors
#define MENU_CURSOR_CLR     0x40
#define EDIT_CURSOR_CLR     0x60
#define MENU_CLR            0x1F
#define PBAR_ON_CLR         0xFF
#define PBAR_OFF_CLR        0x10
#define PBAR_SIZE           74

// Main menu
static const
struct menu_item_t menu[] =
{
    {10,5,60,0,  "Drive: ",         fmt_drive,      act_drive},
    {10,6,60,0,  "SD Card dir: ",   fmt_dir,        act_dir},
    {10,7,60,0,  "Single file: ",   fmt_sings,      act_sings},
    {10,8,60,0,  "Overwrite: ",     fmt_overw,      act_overw},
    {10,9,60,0,  "First track: ",   fmt_ftrk,       act_ftrk},
    {10,10,60,0, "Last track: ",    fmt_ltrk,       act_ltrk},
    {10,11,60,0, "Sides: ",         fmt_side,       act_side},
    {10,12,60,0, "Revolutions: ",   fmt_revs,       act_revs},
    {10,13,60,0, "Image Type: ",    fmt_imgtype,    act_imgtype},
//    {10,13,60,0, "Sample rate: ",   fmt_smprate,    act_smprate},

    {10,15,60,0, "Start",           0,              act_start}
};
#define MENU_ITEMS (sizeof(menu)/sizeof(menu[0]))


// Be sure these values are the same as in sdc module.
static char * const smp_rates_txt[] = {"28 MHz", "14 MHz", "7 MHz"};
#define SMP_RATES_SIZE (sizeof(smp_rates_txt)/sizeof(smp_rates_txt[0]))


/*--------------------------------------------------------------------------*/


// Check if char is num
static
uint8_t check_num(uint8_t k)
{
    return (k>='0' && k<='9');
}

// Check if char is alfanum / allowable for path
static
uint8_t check_fname(uint8_t k)
{
    return (k>='0' && k<='9') ||
           (k>='A' && k<='Z') ||
           (k>='a' && k<='z') ||
           (k=='/' || k=='\\' || k=='.' || k=='_' || k=='-' || k=='+')
           ;
}


// Edit string in a buffer/screen
static
void edit_str(char *str, uint8_t maxlen, uint8_t x, uint8_t y, uint8_t arealen, uint8_t check_type)
{
    uint8_t l, curpos, k, estart, need_upd;

    l=strlen(str);

    curpos=l;       // current cursor pos in buffer
    estart=0;       // offset in buffer for printing
    need_upd=1;

    while(1)
    {
        halt();

        if(need_upd)
        {
            // correct estart
            if(curpos < estart)
            {
                estart=curpos;
            }
            else if(curpos >= estart+arealen)
            {
                estart=curpos-arealen;
            }

            // print buf
            gotoxy(x,y);
            if(estart+arealen<l)
            { // only a part of buf visible
                k=str[estart+arealen];
                str[estart+arealen]=0;
                print_str(&str[estart]);
                str[estart+arealen]=k;
            }
            else
            { // whole buf visible
                print_str(&str[estart]);
                print_pad(x+arealen);
            }
            // put cursor on screen
            fill_attr(x+(curpos-estart),y, 1, EDIT_CURSOR_CLR);
            need_upd=0;
        }

        // keyboard poll
        k=keyboard();
        if(
              (check_type && check_num(k)) ||
              (!check_type && check_fname(k))
           )
        {
            // insert char into buf
            if(l<maxlen)
            {
                memmove(&str[curpos+1], &str[curpos], l-curpos+1);
                str[curpos++]=k;
                l++;
            }
        }
        else if (k==kbENTER)
        {
            // stop edit
            break;
        }
        else if (k==kbDEL)
        {
            // del 1 char
            if(curpos)
            {
                memmove(&str[curpos-1], &str[curpos], l-curpos+1);
                curpos--;
                l--;
            }

        }
        else if (k==kbLEFT)
        {
            // move cursor left
            if(curpos)
                curpos--;
        }
        else if (k==kbRIGHT)
        {
            // move cursor right
            if(curpos<l)
                curpos++;
        }
        else
            // nothing useful pressed
            continue;

        // need to update screen
        need_upd=1;

    }
}



// Byte to string
static
void num2str(unsigned char val, char *dst)
{
    uint8_t t,f=0;
    t=val/100;
    if (t) {*dst++ = t+'0'; f=1;}
    val %= 100;
    t=val/10;
    if (t || f==1) *dst++ = t+'0';
    *dst++ = (val %10)+'0';
    *dst=0;
}

// String to byte
static
uint8_t str2num(char *dst)
{
    uint16_t t=0;

    while(*dst)
    {
        t=t*10 + (*dst-'0');
        dst++;
    }

    return (t>255) ? 255 : t;
}

// Print byte
static
void print_num(unsigned char val)
{
    char buf[4];
    num2str(val, buf);
    print_str(buf);
}



// Edit a number
static
uint8_t get_num(uint8_t val, uint8_t xoffs, uint8_t y, uint8_t areasize)
{
    char editbuf[4];

    num2str(val, editbuf);
    edit_str(editbuf, 3, xoffs,y,areasize, 1);
    return str2num(editbuf);
}


// Open window
static
void win_open(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t clr)
{
    store_win(x,y,w,h);
    draw_win(x,y,w,h,clr);
}

// Close last window
static
void win_close(void)
{
    restore_win();
}

// Show msg box with message
static
void msg_box(char *msg, uint8_t clr)
{
    uint8_t l;

    l=strlen(msg);
    win_open((TEXTMOD_WIDTH-l-3-3)/2, (TEXTMOD_HEIGHT-1-2-2)/2, (l+3+3), (1+2+2), clr);
    gotoxy(3,2);
    print_str(msg);
}




/*--------------------------------------------------------------------------*/

static void fmt_drive(struct menu_item_t *m)
{
    print_char(FDR_INFO.drive + 'A');
}

static void fmt_dir(struct menu_item_t *m)
{
    uint8_t l,n;

    l=strlen(FDR_INFO.dir_name);
    n=m->len-(m->val_xoffs-m->x);   // value length on screen
    n = (l > n) ? (l - n + 3) : 0;
    if (n)
    {
        print_str("...");
    }
    print_str(&FDR_INFO.dir_name[n]);
}

static void fmt_sings(struct menu_item_t *m)
{
    static char * const sings[] = {"No", "Yes"};
    print_str(sings[FDR_INFO.single_file]);
}

static void fmt_overw(struct menu_item_t *m)
{
    static char * const sides[] = {"No", "Yes"};
    print_str(sides[FDR_INFO.overwrite]);
}

static void fmt_ftrk(struct menu_item_t *m)
{
    print_num(FDR_INFO.from_trk);
}

static void fmt_ltrk(struct menu_item_t *m)
{
    print_num(FDR_INFO.to_trk);
}

static void fmt_side(struct menu_item_t *m)
{
    static char * const sides[] = {"0 (down)", "1 (up)", "Both"};
    print_str(sides[FDR_INFO.sides]);
}

static void fmt_revs(struct menu_item_t *m)
{
    print_num(FDR_INFO.revs);
}

static void fmt_imgtype(struct menu_item_t *m)
{
    static char * const types[] = {"Raw sampling"};
    print_str(types[FDR_INFO.imgtype]);
}

static void fmt_smprate(struct menu_item_t *m)
{
    print_str(smp_rates_txt[FDR_INFO.smprate]);
}



static void act_drive(struct menu_item_t *m, uint8_t xoffs, uint8_t y, uint8_t len)
{
    FDR_INFO.drive = (FDR_INFO.drive+1) & 0x3;
}

static void act_dir(struct menu_item_t *m, uint8_t xoffs, uint8_t y, uint8_t len)
{
    edit_str(FDR_INFO.dir_name, sizeof(FDR_INFO.dir_name)-1, xoffs, y, len, 0);
}

static void act_sings(struct menu_item_t *m, uint8_t xoffs, uint8_t y, uint8_t len)
{
    FDR_INFO.single_file ^= 1;
}

static void act_overw(struct menu_item_t *m, uint8_t xoffs, uint8_t y, uint8_t len)
{
    FDR_INFO.overwrite ^= 1;
}

static void act_ftrk(struct menu_item_t *m, uint8_t xoffs, uint8_t y, uint8_t len)
{
    FDR_INFO.from_trk = get_num(FDR_INFO.from_trk, xoffs, y, len);
}

static void act_ltrk(struct menu_item_t *m, uint8_t xoffs, uint8_t y, uint8_t len)
{
    FDR_INFO.to_trk = get_num(FDR_INFO.to_trk, xoffs, y, len);
}

static void act_side(struct menu_item_t *m, uint8_t xoffs, uint8_t y, uint8_t len)
{
    FDR_INFO.sides++;
    if(FDR_INFO.sides>2)
        FDR_INFO.sides=0;
}

static void act_revs(struct menu_item_t *m, uint8_t xoffs, uint8_t y, uint8_t len)
{
    uint8_t t = get_num(FDR_INFO.revs, xoffs, y, len);
    if(t==0)
        t=1;
    else if(t>MAX_REVS)
        t=MAX_REVS;
    FDR_INFO.revs = t;
}

static void act_imgtype(struct menu_item_t *m, uint8_t xoffs, uint8_t y, uint8_t len)
{

}

static void act_smprate(struct menu_item_t *m, uint8_t xoffs, uint8_t y, uint8_t len)
{
    FDR_INFO.smprate++;
    if(FDR_INFO.smprate>=SMP_RATES_SIZE)
        FDR_INFO.smprate=0;
}

static void act_start(struct menu_item_t *m, uint8_t xoffs, uint8_t y, uint8_t len)
{
    char *msg;
    uint8_t res;

/*
------------------------------------------------------------------------------

                                 Dumping disk

                  Drive: n   Track: xxx   Side: x   Revs: nn
                              File: 12345678.123

 [..........................................................................]

                                 [ Reading ]

------------------------------------------------------------------------------
*/

    win_open(1, 10, 78, 10, 0x4f);
    print_str(
        "\x16\x21\x01" "Dumping disk"
        "\x16\x12\x03" "Drive:     Track:       Side:     Revs:   "
        "\x16\x1e\x04" "File:"
        "\x16\x01\x06" "["
        "\x16\x4c\x06" "]"
        "\x16\x21\x08" "["
        "\x16\x2b\x08" "]"
        );
    gotoxy(25,3); print_char(FDR_INFO.drive + 'A');
    gotoxy(58,3); print_num(FDR_INFO.revs);
    FDR_INFO.track=FDR_INFO.from_trk;

    res=make_image(&msg);
    if(res!=MKI_OK)
    {
        msg_box(msg,0x27);
    }
    else
    {
        msg_box("Well done!",0x6f);
    }

    while(is_key_pressed());
    while(!is_key_pressed());

    restore_win();
    restore_win();
}




/*--------------------------------------------------------------------------*/

// Init UI
void ui_init(void)
{
    textmod_init();

    print_str("\x16\x05\x00" "Floppy Disk Ripper v0.1a" "\x16\x05\x01" "(c)2014, psb, tsl.");
}


// Menu
void ui_proc(void)
{
    uint8_t n=0,i,v,k;

    enable_interrupt();
    while(1)
    {
        // Print menu items
        set_clr(MENU_CLR);
        for(i=0; i<MENU_ITEMS; i++)
        {
            if(n==i) halt();                // anti-flicker hack
            gotoxy(menu[i].x,menu[i].y);
            print_str(menu[i].text);
            getxy(&menu[i].val_xoffs,&v);   // save x offset of value
            if(menu[i].fmt)
                menu[i].fmt(&menu[i]);      // format value if possible
            print_pad(menu[i].x + menu[i].len); // print padding
            if(n==i)                        // choosen item?
                fill_attr(menu[i].x,menu[i].y,menu[i].len,MENU_CURSOR_CLR);
        }

        // Keyboard loop
        while(1)
        {
            k=keyboard();
            if((k==kbUP || k=='q') && n>0)
            {
                v=-1;
                break;
            }
            else if((k==kbDOWN || k=='a') && n<MENU_ITEMS-1)
            {
                v=1;
                break;
            }
            else if(k==kbENTER && menu[n].act)
            {
                halt();     // anti-flicker hack
                // clear cursor
                fill_attr(menu[n].x,menu[n].y,menu[n].len,MENU_CLR);
                // activate item's proc
                menu[n].act(&menu[n], menu[n].val_xoffs, menu[n].y, menu[n].len-(menu[n].val_xoffs-menu[n].x));
                v=0;
                break;
            }
            halt();
        }
        n+=v;
    }
}



// Show dump progress
void ui_show_progress(uint8_t op)
{
    uint16_t progr,pnow;
    uint8_t t1,t2;

    // Current operation
    gotoxy(35,8);
    if(op==PGS_READ)
        print_str("Reading");
    else if(op==PGS_WRITE)
        print_str("Writing");
    else
        print_str("Initing");

    // Track/side
    gotoxy(36,3); print_num(FDR_INFO.track); print_str("  ");
    gotoxy(48,3); print_num(FDR_INFO.side);
    // Filename
    gotoxy(36,4); print_str(FDR_INFO.file_name);

    // Progress bar calc
    t1=FDR_INFO.from_trk;
    t2=FDR_INFO.to_trk;
    if (t1 < t2)
    {
        progr = t2 - t1;
        pnow = FDR_INFO.track - t1;
    }
    else
    {
        progr = t1 - t2;
        pnow = t1 - FDR_INFO.track;
    }
    if (FDR_INFO.sides==2)
    {
        progr=progr*2+1;
        pnow*=2;
        if (FDR_INFO.side==1)
        {
            pnow+=1;
        }
    }

    // Progress bar show
    progr = pnow*PBAR_SIZE/progr;
    if (progr)
        fill_attr(3,5+11,progr,PBAR_ON_CLR);
    if (progr<PBAR_SIZE)
        fill_attr(3+progr,5+11,PBAR_SIZE-progr,PBAR_OFF_CLR);

}



