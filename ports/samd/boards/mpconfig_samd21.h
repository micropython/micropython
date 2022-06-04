// Deinitions common to all SAMD21 boards
#include "samd21.h"

#define MICROPY_HW_FLASH_STORAGE_BASE       (0x30000)
#define MICROPY_HW_FLASH_STORAGE_BYTES      (0xFFFF)
#define VFS_BLOCK_SIZE_BYTES                (1536) // 24x 64B flash pages;

#define CPU_FREQ            (48000000)
#define APB_FREQ            (48000000)
