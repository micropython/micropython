#ifndef SFLASH_DISKIO_H_
#define SFLASH_DISKIO_H_

#define SFLASH_BLOCK_SIZE           2048
#define SFLASH_BLOCK_COUNT          MICROPY_PORT_SFLASH_BLOCK_COUNT
#define SFLASH_SECTOR_SIZE          512
#define SFLASH_SECTOR_COUNT         ((SFLASH_BLOCK_SIZE * SFLASH_BLOCK_COUNT) / SFLASH_SECTOR_SIZE)
#define SFLASH_SECTORS_PER_BLOCK    (SFLASH_BLOCK_SIZE / SFLASH_SECTOR_SIZE)

DRESULT sflash_disk_init(void);
DRESULT sflash_disk_status(void);
DRESULT sflash_disk_read(BYTE *buff, DWORD sector, UINT count);
DRESULT sflash_disk_write(const BYTE *buff, DWORD sector, UINT count);
DRESULT sflash_disk_flush(void);

#endif /* SFLASH_DISKIO_H_ */
