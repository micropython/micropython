#define MICROPY_HW_BOARD_NAME "Wio Terminal D51R"
#define MICROPY_HW_MCU_NAME   "SAMD51P19A"

#define MICROPY_FLOAT_IMPL              (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_PY_BUILTINS_COMPLEX     (0)
#define MICROPY_PY_MATH                 (0)
#define MICROPY_PY_CMATH                (0)

// MicroPython configs
// samd_flash.c flash parameters
// Build a 128k Flash storage at top. 512k-128k=384k=0x60000
// 512*1024= 0x80000 minus 128*1024= 0x20000 =  0x60000
#define MICROPY_HW_FLASH_STORAGE_BASE       (0x60000)
#define MICROPY_HW_FLASH_STORAGE_BYTES      (0x1FFFF)
#define VFS_BLOCK_SIZE_BYTES                (1536) // 24x 64B flash pages;

#define CPU_FREQ (48000000) // For selecting Baud from clock.
