// Deinitions common to all SAMD21 boards

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

#define CPU_FREQ            (48000000)
#define BUS_FREQ            (48000000)

#define IRQ_PRI_PENDSV          ((1 << __NVIC_PRIO_BITS) - 1)

static inline uint32_t raise_irq_pri(uint32_t pri) {
    (void)pri;
    return 0;
}

static inline void restore_irq_pri(uint32_t basepri) {
    (void)basepri;
}
