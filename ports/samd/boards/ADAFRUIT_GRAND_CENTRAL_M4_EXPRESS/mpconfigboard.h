#define MICROPY_HW_BOARD_NAME "Grand Central M4 Express"
#define MICROPY_HW_MCU_NAME   "ATSAMD51P20A"

#define MICROPY_HW_XOSC32K          (1)

// 8MB QSPI chip
#define MICROPY_HW_QSPIFLASH GD25Q64C

// fatfs configuration used in ffconf.h
#define MICROPY_FATFS_ENABLE_LFN         (1)
#define MICROPY_FATFS_RPATH              (2)
#define MICROPY_FATFS_MAX_SS             (4096)
#define MICROPY_FATFS_LFN_CODE_PAGE      437 /* 1=SFN/ANSI 437=LFN/U.S.(OEM) */
