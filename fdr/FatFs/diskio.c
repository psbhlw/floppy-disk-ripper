/*-----------------------------------------------------------------------*/
/* MMCv3/SDv1/SDv2 (in SPI mode) control module  (C)ChaN, 2010           */
/*-----------------------------------------------------------------------*/
/* Adapted to Z-Controller by psb, 2014.                                 */
/*-----------------------------------------------------------------------*/

#include "diskio.h"
#include <Intrz80.h>


// Low-level functions
// -------------------------------------------

#define ZC_SD_CFG       0x77
#define ZC_SD_DAT       0x57

#define sd_cs_off()     output8(ZC_SD_CFG, 0x03)
#define sd_cs_on()      output8(ZC_SD_CFG, 0x01)
#define sd_rd()         input8(ZC_SD_DAT)
#define sd_wr(a)        output8(ZC_SD_DAT, (a))

/* Port controls  (Platform dependent) */
#define FCLK_SLOW()     /* Set slow clock */
#define FCLK_FAST()     /* Set fast clock */



/*--------------------------------------------------------------------------

   Module Private Functions

---------------------------------------------------------------------------*/

/* Definitions for MMC/SDC command */
#define CMD0    (0)         /* GO_IDLE_STATE */
#define CMD1    (1)         /* SEND_OP_COND (MMC) */
#define ACMD41  (0x80+41)   /* SEND_OP_COND (SDC) */
#define CMD8    (8)         /* SEND_IF_COND */
#define CMD9    (9)         /* SEND_CSD */
#define CMD10   (10)        /* SEND_CID */
#define CMD12   (12)        /* STOP_TRANSMISSION */
#define ACMD13  (0x80+13)   /* SD_STATUS (SDC) */
#define CMD16   (16)        /* SET_BLOCKLEN */
#define CMD17   (17)        /* READ_SINGLE_BLOCK */
#define CMD18   (18)        /* READ_MULTIPLE_BLOCK */
#define CMD23   (23)        /* SET_BLOCK_COUNT (MMC) */
#define ACMD23  (0x80+23)   /* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24   (24)        /* WRITE_BLOCK */
#define CMD25   (25)        /* WRITE_MULTIPLE_BLOCK */
#define CMD32   (32)        /* ERASE_ER_BLK_START */
#define CMD33   (33)        /* ERASE_ER_BLK_END */
#define CMD38   (38)        /* ERASE */
#define CMD55   (55)        /* APP_CMD */
#define CMD58   (58)        /* READ_OCR */


static volatile
DSTATUS Stat = STA_NOINIT;  /* Disk status */

static
BYTE CardType;              /* Card type flags */


/*-----------------------------------------------------------------------*/
/* Power Control  (Platform dependent)                                   */
/*-----------------------------------------------------------------------*/
/* When the target system does not support socket power control, there   */
/* is nothing to do in these functions and chk_power always returns 1.   */

// static
// int power_status (void)      /* Socket power state: 0=off, 1=on */
// {
//  return 1;
// }

// static
// void power_on (void)
// {

// }

// static
// void power_off (void)
// {

// }

#define power_status() (1)
#define power_on()
#define power_off()


/*-----------------------------------------------------------------------*/
/* Transmit/Receive data from/to MMC via SPI  (Platform dependent)       */
/*-----------------------------------------------------------------------*/

/* asm module used instead; just prototypes here */
void xmit_spi_multi (
    const BYTE *p,      /* Data block to be sent */
    UINT cnt            /* Size of data block */
);
void rcvr_spi_multi (
    BYTE *p,            /* Data buffer */
    UINT cnt            /* Size of data block */
);

// /* Send a data block fast */
// static
// void xmit_spi_multi (
//  const BYTE *p,  /* Data block to be sent */
//  UINT cnt        /* Size of data block */
// )
// {
//  do {
//      sd_wr (*p++);
//      sd_wr (*p++);
//  } while (cnt -= 2);
// }

// /* Receive a data block fast */
// static
// void rcvr_spi_multi (
//  BYTE *p,    /* Data buffer */
//  UINT cnt    /* Size of data block */
// )
// {
//  do {
//      *p++ = sd_rd();
//      *p++ = sd_rd();
//  } while (cnt -= 2);
// }



/*-----------------------------------------------------------------------*/
/* Wait for card ready                                                   */
/*-----------------------------------------------------------------------*/

static
int wait_ready (    /* 1:Ready, 0:Timeout */
    UINT wt         // Timeout [ms]
)
{
    BYTE d;

    do
        d = sd_rd();
    while (d != 0xFF );

    return 1;
}


/*-----------------------------------------------------------------------*/
/* Deselect the card and release SPI bus                                 */
/*-----------------------------------------------------------------------*/

static
void deselect (void)
{
    sd_cs_off();
    sd_rd();    /* Dummy clock (force DO hi-z for multiple slave SPI) */
}



/*-----------------------------------------------------------------------*/
/* Select the card and wait for ready                                    */
/*-----------------------------------------------------------------------*/

static
int select (void)   /* 1:Successful, 0:Timeout */
{
    sd_cs_on();
    sd_rd();        /* Dummy clock (force DO enabled) */

    if (wait_ready(500)) return 1;  /* OK */
    deselect();
    return 0;       /* Timeout */
}



/*-----------------------------------------------------------------------*/
/* Receive a data packet from MMC                                        */
/*-----------------------------------------------------------------------*/

static
int rcvr_datablock (
    BYTE *buff,         /* Data buffer to store received data */
    UINT btr            /* Byte count (must be multiple of 4) */
)
{
    BYTE token;

    do {                            /* Wait for data packet in timeout of 200ms */
        token = sd_rd();
    } while ((token == 0xFF) );
    if (token != 0xFE) return 0;    /* If not valid data token, retutn with error */

    rcvr_spi_multi(buff, btr);      /* Receive the data block into buffer */
    sd_rd();                        /* Discard CRC */
    sd_rd();

    return 1;                       /* Return with success */
}



/*-----------------------------------------------------------------------*/
/* Send a data packet to MMC                                             */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
static
int xmit_datablock (
    const BYTE *buff,   /* 512 byte data block to be transmitted */
    BYTE token          /* Data/Stop token */
)
{
    BYTE resp;

    if (!wait_ready(500)) return 0;

    sd_wr(token);                       /* Xmit data token */
    if (token != 0xFD) {                /* Is data token */
        xmit_spi_multi(buff, 512);      /* Xmit the data block to the MMC */
        sd_wr(0xff);                    /* CRC (Dummy) */
        sd_wr(0xff);
        sd_rd();                        /* flush prev data for z/Contr. */
        resp = sd_rd();                 /* Reveive data response */
        if ((resp & 0x1F) != 0x05)      /* If not accepted, return with error */
            return 0;
    }

    return 1;
}
#endif



/*-----------------------------------------------------------------------*/
/* Send a command packet to MMC                                          */
/*-----------------------------------------------------------------------*/

static
BYTE send_cmd (     /* Returns R1 resp (bit7==1:Send failed) */
    BYTE cmd,       /* Command index */
    DWORD arg       /* Argument */
)
{
    BYTE n, res;

    if (cmd & 0x80) {   /* ACMD<n> is the command sequense of CMD55-CMD<n> */
        cmd &= 0x7F;
        res = send_cmd(CMD55, 0);
        if (res > 1) return res;
    }

    /* Select the card and wait for ready except to stop multiple block read */
    if (cmd != CMD12) {
        deselect();
        if (!select()) return 0xFF;
    }

    /* Send command packet */
    sd_wr(0x40 | cmd);                  /* Start + Command index */
    sd_wr((BYTE)(arg >> 24));           /* Argument[31..24] */
    sd_wr((BYTE)(arg >> 16));           /* Argument[23..16] */
    sd_wr((BYTE)(arg >> 8));            /* Argument[15..8] */
    sd_wr((BYTE)arg);                   /* Argument[7..0] */
    n = 0x01;                           /* Dummy CRC + Stop */
    if (cmd == CMD0) n = 0x95;          /* Valid CRC for CMD0(0) + Stop */
    if (cmd == CMD8) n = 0x87;          /* Valid CRC for CMD8(0x1AA) Stop */
    sd_wr(n);

    /* Receive command response */
    if (cmd == CMD12) sd_rd();          /* Skip a stuff byte when stop reading */
    n = 10;                             /* Wait for a valid response in timeout of 10 attempts */
    do
        res = sd_rd();
    while ((res & 0x80) && --n);

    return res;         /* Return with the response value */
}



/*--------------------------------------------------------------------------

   Public Functions

---------------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
    BYTE pdrv       /* Physical drive nmuber (0) */
)
{
    BYTE n, cmd, ty, ocr[4];


    if (pdrv) return STA_NOINIT;        /* Supports only single drive */
    power_off();                        /* Turn off the socket power to reset the card */
    if (Stat & STA_NODISK) return Stat; /* No card in the socket */
    power_on();                         /* Turn on the socket power */
    FCLK_SLOW();
    for (n = 10; n; n--) sd_rd();       /* 80 dummy clocks */

    ty = 0;
    if (send_cmd(CMD0, 0) == 1) {                       /* Enter Idle state */
        if (send_cmd(CMD8, 0x1AA) == 1) {               /* SDv2? */
            for (n = 0; n < 4; n++) ocr[n] = sd_rd();   /* Get trailing return value of R7 resp */
            if (ocr[2] == 0x01 && ocr[3] == 0xAA) {     /* The card can work at vdd range of 2.7-3.6V */
                while (send_cmd(ACMD41, 1UL << 30));    /* Wait for leaving idle state (ACMD41 with HCS bit) */
                if (send_cmd(CMD58, 0) == 0) {          /* Check CCS bit in the OCR */
                    for (n = 0; n < 4; n++) ocr[n] = sd_rd();
                    ty = (ocr[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;  /* SDv2 */
                }
            }
        } else {                                /* SDv1 or MMCv3 */
            if (send_cmd(ACMD41, 0) <= 1)   {
                ty = CT_SD1; cmd = ACMD41;      /* SDv1 */
            } else {
                ty = CT_MMC; cmd = CMD1;        /* MMCv3 */
            }
            while (send_cmd(cmd, 0));           /* Wait for leaving idle state */
            if (send_cmd(CMD16, 512) != 0)      /* Set R/W block length to 512 */
                ty = 0;
        }
    }
    CardType = ty;
    deselect();

    if (ty) {                       /* Initialization succeded */
        Stat &= ~STA_NOINIT;        /* Clear STA_NOINIT */
        FCLK_FAST();
    } else {                        /* Initialization failed */
        power_off();
    }

    return Stat;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    BYTE pdrv       /* Physical drive nmuber (0) */
)
{
    if (pdrv) return STA_NOINIT;    /* Supports only single drive */
    return Stat;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
    BYTE pdrv,          /* Physical drive nmuber (0) */
    BYTE *buff,         /* Pointer to the data buffer to store read data */
    DWORD sector,       /* Start sector number (LBA) */
    UINT count          /* Sector count (1..128) */
)
{
    if (pdrv || !count) return RES_PARERR;
    if (Stat & STA_NOINIT) return RES_NOTRDY;

    if (!(CardType & CT_BLOCK)) sector *= 512;  /* Convert to byte address if needed */

    if (count == 1) {   /* Single block read */
        if ((send_cmd(CMD17, sector) == 0)  /* READ_SINGLE_BLOCK */
            && rcvr_datablock(buff, 512))
            count = 0;
    }
    else {              /* Multiple block read */
        if (send_cmd(CMD18, sector) == 0) { /* READ_MULTIPLE_BLOCK */
            do {
                if (!rcvr_datablock(buff, 512)) break;
                buff += 512;
            } while (--count);
            send_cmd(CMD12, 0);             /* STOP_TRANSMISSION */
        }
    }
    deselect();

    return count ? RES_ERROR : RES_OK;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
    BYTE pdrv,          /* Physical drive nmuber (0) */
    const BYTE *buff,   /* Pointer to the data to be written */
    DWORD sector,       /* Start sector number (LBA) */
    UINT count          /* Sector count (1..128) */
)
{
    if (pdrv || !count) return RES_PARERR;
    if (Stat & STA_NOINIT) return RES_NOTRDY;
    if (Stat & STA_PROTECT) return RES_WRPRT;

    if (!(CardType & CT_BLOCK)) sector *= 512;  /* Convert to byte address if needed */

    if (count == 1) {   /* Single block write */
        if ((send_cmd(CMD24, sector) == 0)  /* WRITE_BLOCK */
            && xmit_datablock(buff, 0xFE))
            count = 0;
    }
    else {              /* Multiple block write */
        if (CardType & CT_SDC) send_cmd(ACMD23, count);
        if (send_cmd(CMD25, sector) == 0) { /* WRITE_MULTIPLE_BLOCK */
            do {
                if (!xmit_datablock(buff, 0xFC)) break;
                buff += 512;
            } while (--count);
            if (!xmit_datablock(0, 0xFD))   /* STOP_TRAN token */
                count = 1;
        }
    }
    deselect();

    return count ? RES_ERROR : RES_OK;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/
#if _USE_IOCTL
DRESULT disk_ioctl (
    BYTE pdrv,      /* Physical drive nmuber (0) */
    BYTE cmd,       /* Control code */
    void *buff      /* Buffer to send/receive control data */
)
{
    DRESULT res;
    BYTE n, csd[16], *ptr = buff;
    DWORD *dp, st, ed, csize;

    if (pdrv) return RES_PARERR;

    res = RES_ERROR;

    if (cmd == CTRL_POWER) {
        switch (ptr[0]) {
        case 0:     /* Sub control code (POWER_OFF) */
            power_off();        /* Power off */
            res = RES_OK;
            break;
        case 1:     /* Sub control code (POWER_GET) */
            ptr[1] = (BYTE)power_status();
            res = RES_OK;
            break;
        default :
            res = RES_PARERR;
        }
    }
    else {
        if (Stat & STA_NOINIT) return RES_NOTRDY;

        switch (cmd) {
        case CTRL_SYNC :        /* Make sure that no pending write process. Do not remove this or written sector might not left updated. */
            if (select()) res = RES_OK;
            break;

        case GET_SECTOR_COUNT : /* Get number of sectors on the disk (DWORD) */
            if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {
                if ((csd[0] >> 6) == 1) {   /* SDC ver 2.00 */
                    csize = csd[9] + ((WORD)csd[8] << 8) + ((DWORD)(csd[7] & 63) << 16) + 1;
                    *(DWORD*)buff = csize << 10;
                } else {                    /* SDC ver 1.XX or MMC*/
                    n = (csd[5] & 15) + ((csd[10] & 128) >> 7) + ((csd[9] & 3) << 1) + 2;
                    csize = (csd[8] >> 6) + ((WORD)csd[7] << 2) + ((WORD)(csd[6] & 3) << 10) + 1;
                    *(DWORD*)buff = csize << (n - 9);
                }
                res = RES_OK;
            }
            break;

        case GET_BLOCK_SIZE :   /* Get erase block size in unit of sector (DWORD) */
            if (CardType & CT_SD2) {    /* SDv2? */
                if (send_cmd(ACMD13, 0) == 0) { /* Read SD status */
                    sd_rd();
                    if (rcvr_datablock(csd, 16)) {              /* Read partial block */
                        for (n = 64 - 16; n; n--) sd_rd();  /* Purge trailing data */
                        *(DWORD*)buff = 16UL << (csd[10] >> 4);
                        res = RES_OK;
                    }
                }
            } else {                    /* SDv1 or MMCv3 */
                if ((send_cmd(CMD9, 0) == 0) && rcvr_datablock(csd, 16)) {  /* Read CSD */
                    if (CardType & CT_SD1) {    /* SDv1 */
                        *(DWORD*)buff = (((csd[10] & 63) << 1) + ((WORD)(csd[11] & 128) >> 7) + 1) << ((csd[13] >> 6) - 1);
                    } else {                    /* MMCv3 */
                        *(DWORD*)buff = ((WORD)((csd[10] & 124) >> 2) + 1) * (((csd[11] & 3) << 3) + ((csd[11] & 224) >> 5) + 1);
                    }
                    res = RES_OK;
                }
            }
            break;

        /* Following commands are never used by FatFs module */

        // case MMC_GET_TYPE :      /* Get card type flags (1 byte) */
        //  *ptr = CardType;
        //  res = RES_OK;
        //  break;

        // case MMC_GET_CSD :       /* Receive CSD as a data block (16 bytes) */
        //  if (send_cmd(CMD9, 0) == 0      /* READ_CSD */
        //      && rcvr_datablock(ptr, 16))
        //      res = RES_OK;
        //  break;

        // case MMC_GET_CID :       /* Receive CID as a data block (16 bytes) */
        //  if (send_cmd(CMD10, 0) == 0     /* READ_CID */
        //      && rcvr_datablock(ptr, 16))
        //      res = RES_OK;
        //  break;

        // case MMC_GET_OCR :       /* Receive OCR as an R3 resp (4 bytes) */
        //  if (send_cmd(CMD58, 0) == 0) {  /* READ_OCR */
        //      for (n = 4; n; n--) *ptr++ = sd_rd();
        //      res = RES_OK;
        //  }
        //  break;

        // case MMC_GET_SDSTAT :    /* Receive SD statsu as a data block (64 bytes) */
        //  if (send_cmd(ACMD13, 0) == 0) { /* SD_STATUS */
        //      sd_rd();
        //      if (rcvr_datablock(ptr, 64))
        //          res = RES_OK;
        //  }
        //  break;

        default:
            res = RES_PARERR;
        }

        deselect();
    }

    return res;
}
#endif


/*-----------------------------------------------------------------------*/
/* Device Timer Interrupt Procedure                                      */
/*-----------------------------------------------------------------------*/
/* This function must be called in period of 10ms                        */

// void disk_timerproc (void)
// {

// }
