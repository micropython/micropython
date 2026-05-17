#ifndef SD_DISKIO_H_
#define SD_DISKIO_H_

#define SD_SECTOR_SIZE          512

//*****************************************************************************
// Disk Info Structure definition
//*****************************************************************************
typedef struct
{
    unsigned char ucCardType;
    unsigned int  ulVersion;
    unsigned int  ulCapClass;
    unsigned int ulNofBlock;
    unsigned int ulBlockSize;
    DSTATUS bStatus;
    unsigned short usRCA;
}DiskInfo_t;

extern DiskInfo_t sd_disk_info;

DSTATUS sd_disk_init (void);
void sd_disk_deinit (void);
DRESULT sd_disk_read (BYTE* pBuffer, DWORD ulSectorNumber, UINT bSectorCount);
DRESULT sd_disk_write (const BYTE* pBuffer, DWORD ulSectorNumber, UINT bSectorCount);

#endif /* SD_DISKIO_H_ */
