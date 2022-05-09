#define MICROPY_HW_BOARD_NAME "ItsyBitsy M0 Express"
#define MICROPY_HW_MCU_NAME   "SAMD21G18A"

// #define MICROPY_FLOAT_IMPL              (MICROPY_FLOAT_IMPL_FLOAT)
// #define MICROPY_PY_BUILTINS_COMPLEX     (0)
// #define MICROPY_PY_MATH                 (0)
// #define MICROPY_PY_CMATH                (0)
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
// USART pin assignments: Tx=D1=PA10=SERCOM0/PAD[2], Rx=D0=PA11=SERCOM0/PAD[3]
#define CPU_FREQ            (48000000)
#define BUS_FREQ            (48000000)
#define MICROPY_HW_XOSC32K  (0)
