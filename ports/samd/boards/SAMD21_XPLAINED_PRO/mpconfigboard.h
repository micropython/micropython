#define MICROPY_HW_BOARD_NAME "SAMD21-XPLAINED-PRO"
#define MICROPY_HW_MCU_NAME   "SAMD21J18A"

// MicroPython configs
// samd_flash.c flash parameters
// Build a 64k Flash storage at top. 256k-64k=196k
// 256*1024=262144 minus 64*1024=65536 = 196608 = 0x30000
#define MICROPY_HW_FLASH_STORAGE_BASE       (0x30000)
#define MICROPY_HW_FLASH_STORAGE_BYTES      (0xFFFF)
#define VFS_BLOCK_SIZE_BYTES                (1536) // 24x 64B flash pages;

// ASF4 MCU package specific Pin definitions
#include "samd21j18a.h"

// Please consult the SAM_D21 Datasheet, I/O Multiplexing and Considerations.
// USART pin assignments: (This board has 3 USARTS brought out to the pins. See https://docs.zephyrproject.org/1.14.1/boards/arm/atsamd21_xpro/doc/index.html#serial-port )
// Tx=PA10=SERCOM0/PAD[2], Rx=PA11=SERCOM0/PAD[3]
#define CPU_FREQ            (48000000)
#define BUS_FREQ            (48000000)
#define MICROPY_HW_XOSC32K  (0)
