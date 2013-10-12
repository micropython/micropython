/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include <stdint.h>
#include <stdio.h>
#include "ff.h"        /* FatFs lower layer API */
#include "diskio.h"        /* FatFs lower layer API */

PARTITION VolToPart[] = {
    {0, 1},     // Logical drive 0 ==> Physical drive 0, 1st partition
    /*
    {0, 2},     // Logical drive 1 ==> Physical drive 0, 2nd partition
    {0, 3},     // Logical drive 2 ==> Physical drive 0, 3rd partition
    {1, 0},     // Logical drive 3 ==> Physical drive 1 (auto detection)
    */
};

#define PD_FLASH_SECTOR_SIZE (512)
#define PD_FLASH_PART1_START_SECTOR (0x100)
#define PD_FLASH_PART1_NUM_SECTORS (128) // 64k
#define PD_FLASH_MEM_START_ADDR (0x08020000) // 128k above start, first 128k block

#define PD_FLASH_RAM_BUF (0x10000000) // CCM data RAM, 64k

static void pd_flash_init() {
    printf("IN\n");
    // fill RAM buffer
    uint32_t *src = (uint32_t*)PD_FLASH_MEM_START_ADDR;
    uint32_t *dest = (uint32_t*)PD_FLASH_RAM_BUF;
    for (int i = 0; i < PD_FLASH_PART1_NUM_SECTORS * PD_FLASH_SECTOR_SIZE / 4; i++) {
        *dest++ = *src++;
    }
}

extern void flash_write(uint32_t flash_dest, const uint32_t *src, uint32_t num_word32);

static void pd_flash_flush() {
    printf("FL\n");
    // sync the RAM buffer by writing it to the flash page
    flash_write(PD_FLASH_MEM_START_ADDR, (const uint32_t*)PD_FLASH_RAM_BUF, PD_FLASH_PART1_NUM_SECTORS * PD_FLASH_SECTOR_SIZE / 4);
}

static void build_partition(uint8_t *buf, int boot, int type, uint32_t start_sector, uint32_t num_sectors) {
    buf[0] = boot;

    if (num_sectors == 0) {
        buf[1] = 0;
        buf[2] = 0;
        buf[3] = 0;
    } else {
        buf[1] = 0xff;
        buf[2] = 0xff;
        buf[3] = 0xff;
    }

    buf[4] = type;

    if (num_sectors == 0) {
        buf[5] = 0;
        buf[6] = 0;
        buf[7] = 0;
    } else {
        buf[5] = 0xff;
        buf[6] = 0xff;
        buf[7] = 0xff;
    }

    buf[8] = start_sector;
    buf[9] = start_sector >> 8;
    buf[10] = start_sector >> 16;
    buf[11] = start_sector >> 24;

    buf[12] = num_sectors;
    buf[13] = num_sectors >> 8;
    buf[14] = num_sectors >> 16;
    buf[15] = num_sectors >> 24;
}

static DRESULT pd_flash_read_sector(uint8_t *dest, uint32_t sector) {
    //printf("RD %u\n", sector);
    if (sector == 0) {
        // fake the MBR so we can decide on our own partition table

        for (int i = 0; i < 446; i++) {
            dest[i] = 0;
        }

        build_partition(dest + 446, 0, 0x01 /* FAT12 */, PD_FLASH_PART1_START_SECTOR, PD_FLASH_PART1_NUM_SECTORS);
        build_partition(dest + 462, 0, 0, 0, 0);
        build_partition(dest + 478, 0, 0, 0, 0);
        build_partition(dest + 494, 0, 0, 0, 0);

        dest[510] = 0x55;
        dest[511] = 0xaa;

        return RES_OK;

    } else if (PD_FLASH_PART1_START_SECTOR <= sector && sector < PD_FLASH_PART1_START_SECTOR + PD_FLASH_PART1_NUM_SECTORS) {
        // non-MBR sector(s), just copy straight from flash
        uint8_t *src = (uint8_t*)PD_FLASH_RAM_BUF + (sector - PD_FLASH_PART1_START_SECTOR) * PD_FLASH_SECTOR_SIZE;
        for (int i = PD_FLASH_SECTOR_SIZE; i > 0; i--) {
            *dest++ = *src++;
        }
        return RES_OK;

    } else {
        // bad sector number
        return RES_ERROR;
    }
}

static DRESULT pd_flash_write_sector(const uint8_t *src, uint32_t sector) {
    printf("WR %u\n", sector);
    if (sector == 0) {
        // can't write MBR, but pretend we did

        return RES_OK;

    } else if (PD_FLASH_PART1_START_SECTOR <= sector && sector < PD_FLASH_PART1_START_SECTOR + PD_FLASH_PART1_NUM_SECTORS) {
        // non-MBR sector(s), copy to RAM buffer
        uint8_t *dest = (uint8_t*)PD_FLASH_RAM_BUF + (sector - PD_FLASH_PART1_START_SECTOR) * PD_FLASH_SECTOR_SIZE;
        for (int i = PD_FLASH_SECTOR_SIZE; i > 0; i--) {
            *dest++ = *src++;
        }
        return RES_OK;

    } else {
        // bad sector number
        return RES_ERROR;
    }
}

/* Definitions of physical drive number for each media */
#define PD_FLASH (0)
#define PD_SD    (1)

/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
    BYTE pdrv                /* Physical drive nmuber (0..) */
)
{
    switch (pdrv) {
        case PD_FLASH :
            pd_flash_init();
            return 0;
    }

    return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
    BYTE pdrv        /* Physical drive nmuber (0..) */
)
{
    switch (pdrv) {
        case PD_FLASH :
            // flash is ready
            return 0;

        case PD_SD:
            return STA_NOINIT;
    }

    return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
    BYTE pdrv,        /* Physical drive nmuber (0..) */
    BYTE *buff,        /* Data buffer to store read data */
    DWORD sector,    /* Sector address (LBA) */
    UINT count        /* Number of sectors to read (1..128) */
)
{
    DRESULT res;
    switch (pdrv) {
        case PD_FLASH:
            for (int i = 0; i < count; i++) {
                if ((res = pd_flash_read_sector(buff + i * PD_FLASH_SECTOR_SIZE, sector + i)) != RES_OK) {
                    return res;
                }
            }
            return RES_OK;
    }

    return RES_PARERR;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
    BYTE pdrv,            /* Physical drive nmuber (0..) */
    const BYTE *buff,    /* Data to be written */
    DWORD sector,        /* Sector address (LBA) */
    UINT count            /* Number of sectors to write (1..128) */
)
{
    DRESULT res;
    switch (pdrv) {
        case PD_FLASH:
            for (int i = 0; i < count; i++) {
                if ((res = pd_flash_write_sector(buff + i * PD_FLASH_SECTOR_SIZE, sector + i)) != RES_OK) {
                    return res;
                }
            }
            return RES_OK;
    }

    return RES_PARERR;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
    BYTE pdrv,        /* Physical drive nmuber (0..) */
    BYTE cmd,        /* Control code */
    void *buff        /* Buffer to send/receive control data */
)
{
    switch (pdrv) {
        case PD_FLASH:
            switch (cmd) {
                case CTRL_SYNC:
                    pd_flash_flush();
                    return RES_OK;

                case GET_BLOCK_SIZE:
                    *((DWORD*)buff) = 1; // block erase size in units of the sector size
                    return RES_OK;
            }
    }

    return RES_PARERR;
}
#endif

DWORD get_fattime (
    void
)
{
    int year = 2013;
    int month = 10;
    int day = 12;
    int hour = 21;
    int minute = 42;
    int second = 13;
    return ((year - 1980) << 25) | ((month) << 21) | ((day) << 16) | ((hour) << 11) | ((minute) << 5) | (second / 2);
}
