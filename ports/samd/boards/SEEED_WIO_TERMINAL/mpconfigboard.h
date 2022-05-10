#define MICROPY_HW_BOARD_NAME "Wio Terminal D51R"
#define MICROPY_HW_MCU_NAME   "SAMD51P19A"

#define MICROPY_FLOAT_IMPL              (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_PY_BUILTINS_COMPLEX     (0)
#define MICROPY_PY_MATH                 (0)
#define MICROPY_PY_CMATH                (0)
// Due to a limitation in the TC counter for us, the ticks period is 2**28
#define MICROPY_PY_UTIME_TICKS_PERIOD   (0x10000000)
#define MICROPY_PY_URANDOM_SEED_INIT_FUNC (trng_random_u32())
unsigned long trng_random_u32(void);

// MicroPython configs
// samd_flash.c flash parameters
// Build a 128k Flash storage at top. 512k-128k=384k=0x60000
// 512*1024= 0x80000 minus 128*1024= 0x20000 =  0x60000
#define MICROPY_HW_FLASH_STORAGE_BASE       (0x60000)
#define MICROPY_HW_FLASH_STORAGE_BYTES      (0x1FFFF)
#define VFS_BLOCK_SIZE_BYTES                (1536) // 24x 64B flash pages;

// ASF4 MCU package specific Pin definitions
#include "samd51p19a.h"

// Please consult the SAM_D51 Datasheet, I/O Multiplexing and Considerations.
// WIO_Terminal USART pin assignments: Tx=BCM14=PB27=SERCOM2/PAD[0], Rx=BCM15=PB26=SERCOM2/PAD[1]
#define CPU_FREQ            (120000000)
#define BUS_FREQ            (48000000)
#define DPLLx_REF_FREQ      (32768)
#define MICROPY_HW_XOSC32K  (0)
