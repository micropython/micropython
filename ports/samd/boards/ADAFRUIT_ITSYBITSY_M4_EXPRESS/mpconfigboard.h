#define MICROPY_HW_BOARD_NAME "ItsyBitsy M4 Express"
#define MICROPY_HW_MCU_NAME   "SAMD51G19A"

#define MICROPY_FLOAT_IMPL              (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_PY_BUILTINS_COMPLEX     (0)
#define MICROPY_PY_MATH                 (0)
#define MICROPY_PY_CMATH                (0)
// Due to a limitation in the TC counter for us, the ticks period is 2**28
#define MICROPY_PY_UTIME_TICKS_PERIOD   (0x10000000)

// MicroPython configs
// samd_flash.c flash parameters
// Build a 128k Flash storage at top. 512k-128k=384k=0x60000
// 512*1024= 0x80000 minus 128*1024= 0x20000 =  0x60000
#define MICROPY_HW_FLASH_STORAGE_BASE       (0x60000)
#define MICROPY_HW_FLASH_STORAGE_BYTES      (0x1FFFF)
#define VFS_BLOCK_SIZE_BYTES                (1536) //

// ASF4 MCU package specific Pin definitions
#include "samd51g19a.h"

// Please consult the SAM_D51 Datasheet, I/O Multiplexing and Considerations.
// USART pin assignments: Tx=TX_D1=PA17=SERCOM3/PAD[0], Rx=RX_D0=PA16=SERCOM3/PAD[1]
#define CPU_FREQ            (120000000)
#define BUS_FREQ            (48000000)
#define DPLLx_REF_FREQ      (32768)
#define MICROPY_HW_XOSC32K  (0)
