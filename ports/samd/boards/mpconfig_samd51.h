// Deinitions common to all SAMD51 boards
#include "samd51.h"

#define MICROPY_FLOAT_IMPL              (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_PY_BUILTINS_COMPLEX     (0)
#define MICROPY_PY_MATH                 (0)
#define MICROPY_PY_CMATH                (0)
#define MICROPY_PY_UOS_URANDOM          (1)
#define MICROPY_PY_URANDOM_SEED_INIT_FUNC (trng_random_u32())
unsigned long trng_random_u32(void);

// Due to a limitation in the TC counter for us, the ticks period is 2**29
#define MICROPY_PY_UTIME_TICKS_PERIOD   (0x20000000)
// MicroPython configs
// samd_flash.c flash parameters
// Build a 128k Flash storage at top. 512k-128k=384k=0x60000
// 512*1024= 0x80000 minus 128*1024= 0x20000 =  0x60000
#define MICROPY_HW_FLASH_STORAGE_BASE       (0x60000)
#define MICROPY_HW_FLASH_STORAGE_BYTES      (0x1FFFF)
#define VFS_BLOCK_SIZE_BYTES                (1536) //

#define CPU_FREQ            (120000000)
#define APB_FREQ            (48000000)
#define DPLLx_REF_FREQ      (32768)
