// Board and hardware specific configuration
#define MICROPY_HW_MCU_NAME                     "PSoC62"    
#define MICROPY_HW_BOARD_NAME                   "CY8CKIT-062S2-AI"

#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT     "CY8C-062S2-AI"

#define MICROPY_GC_HEAP_SIZE                    (256 * 1024) // 256 KB

#define MICROPY_PY_HASHLIB                      (1)
#define MICROPY_PY_HASHLIB_MD5                  (1)
#define MICROPY_PY_HASHLIB_SHA1                 (1)
#define MICROPY_PY_HASHLIB_SHA256               (1)

// Flash type enablement for board
#define MICROPY_PY_EXT_FLASH (1)
#if (MICROPY_PY_EXT_FLASH)
#define EXT_FLASH_BASE                              (0x00)          /** 0x00 */
#define EXT_FLASH_SIZE                              (0x4000000)     /** 64MB */
#define EXT_FLASH_SECTOR_SIZE                       (0x40000)       /** 256KB*/
#define EXT_FLASH_BLOCK_SIZE_BYTES                  (EXT_FLASH_SECTOR_SIZE) 
#define EXT_FLASH_PAGE_SIZE                         (0x200)         /** 512 bytes*/
#endif

// Board specific configurations
#define MAX_UART             2 // TODO: Change as per bsp?
#define MAX_TIMER (32)


