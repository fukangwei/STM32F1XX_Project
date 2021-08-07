/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h" /* FatFs lower layer API */
#include "mmc_sd.h"
#include "stdlib.h"

#define SD_CARD  0
#define EX_FLASH 1

#define FLASH_SECTOR_SIZE   512

u16     FLASH_SECTOR_COUNT = 9832;
#define FLASH_BLOCK_SIZE    8

DSTATUS disk_initialize (
    BYTE pdrv               /* Physical drive nmuber (0..) */
) {
    u8 res = 0;

    switch ( pdrv ) {
        case SD_CARD:
            res = SD_Initialize();

            if ( res ) {
                SD_SPI_SpeedLow();
                SD_SPI_ReadWriteByte ( 0xff );
                SD_SPI_SpeedHigh();
            }

            break;

        case EX_FLASH:
            break;

        default:
            res = 1;
    }

    if ( res ) {
        return  STA_NOINIT;
    } else {
        return 0;
    }
}

DSTATUS disk_status (
    BYTE pdrv       /* Physical drive nmuber (0..) */
) {
    return 0;
}

DRESULT disk_read (
    BYTE pdrv,      /* Physical drive nmuber (0..) */
    BYTE *buff,     /* Data buffer to store read data */
    DWORD sector,   /* Sector address (LBA) */
    UINT count      /* Number of sectors to read (1..128) */
) {
    u8 res = 0;

    if ( !count ) {
        return RES_PARERR;
    }

    switch ( pdrv ) {
        case SD_CARD:
            res = SD_ReadDisk ( buff, sector, count );

            if ( res ) {
                SD_SPI_SpeedLow();
                SD_SPI_ReadWriteByte ( 0xff );
                SD_SPI_SpeedHigh();
            }

            break;

        case EX_FLASH:
            break;

        default:
            res = 1;
    }

    if ( res == 0x00 ) {
        return RES_OK;
    } else {
        return RES_ERROR;
    }
}

#if _USE_WRITE
DRESULT disk_write (
    BYTE pdrv,          /* Physical drive nmuber (0..) */
    const BYTE *buff,   /* Data to be written */
    DWORD sector,       /* Sector address (LBA) */
    UINT count          /* Number of sectors to write (1..128) */
) {
    u8 res = 0;

    if ( !count ) {
        return RES_PARERR;
    }

    switch ( pdrv ) {
        case SD_CARD:
            res = SD_WriteDisk ( ( u8* ) buff, sector, count );
            break;

        case EX_FLASH:
            break;

        default:
            res = 1;
    }

    if ( res == 0x00 ) {
        return RES_OK;
    } else {
        return RES_ERROR;
    }
}
#endif

#if _USE_IOCTL
DRESULT disk_ioctl (
    BYTE pdrv,      /* Physical drive nmuber (0..) */
    BYTE cmd,       /* Control code */
    void *buff      /* Buffer to send/receive control data */
) {
    DRESULT res;

    if ( pdrv == SD_CARD ) {
        switch ( cmd ) {
            case CTRL_SYNC:
                SD_CS = 0;

                if ( SD_WaitReady() == 0 ) {
                    res = RES_OK;
                } else {
                    res = RES_ERROR;
                }

                SD_CS = 1;
                break;

            case GET_SECTOR_SIZE:
                * ( WORD* ) buff = 512;
                res = RES_OK;
                break;

            case GET_BLOCK_SIZE:
                * ( WORD* ) buff = 8;
                res = RES_OK;
                break;

            case GET_SECTOR_COUNT:
                * ( DWORD* ) buff = SD_GetSectorCount();
                res = RES_OK;
                break;

            default:
                res = RES_PARERR;
                break;
        }
    } else if ( pdrv == EX_FLASH ) {
        switch ( cmd ) {
            case CTRL_SYNC:
                res = RES_OK;
                break;

            case GET_SECTOR_SIZE:
                * ( WORD* ) buff = FLASH_SECTOR_SIZE;
                res = RES_OK;
                break;

            case GET_BLOCK_SIZE:
                * ( WORD* ) buff = FLASH_BLOCK_SIZE;
                res = RES_OK;
                break;

            case GET_SECTOR_COUNT:
                * ( DWORD* ) buff = FLASH_SECTOR_COUNT;
                res = RES_OK;
                break;

            default:
                res = RES_PARERR;
                break;
        }
    } else {
        res = RES_ERROR;
    }

    return res;
}
#endif

DWORD get_fattime ( void ) {
    return 0;
}

void *ff_memalloc ( UINT size ) {
    return ( void* ) malloc ( size );
}

void ff_memfree ( void* mf ) {
    free ( mf );
}
