#define MICROPY_HW_BOARD_NAME "Seeed Xiao"
#define MICROPY_HW_MCU_NAME   "SAMD21G18A"

// MicroPython configs
// samd_flash.c flash parameters
// Build a 64k Flash storage at top. 256k-64k=196k
// 256*1024=262144 minus 64*1024=65536 = 196608 = 0x30000
#define MICROPY_HW_FLASH_STORAGE_BASE       (0x30000)
#define MICROPY_HW_FLASH_STORAGE_BYTES      (0xFFFF)
#define VFS_BLOCK_SIZE_BYTES                (1536) // 24x 64B flash pages;

// ASF4 MCU package specific Pin definitions
#include "samd21g18a.h"

// Please consult the SAM_D21 Datasheet, I/O Multiplexing and Considerations.
// XIAO_M0 USART pin assignments: Tx=A6=PB8=SERCOM4/PAD[0], Rx=PB9=A7=SERCOM4/PAD[1]
#define CPU_FREQ (48000000)
#define BUS_FREQ (48000000)
#define MICROPY_HW_XOSC32K  (1)
