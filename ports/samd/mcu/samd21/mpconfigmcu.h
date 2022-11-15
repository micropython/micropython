// Deinitions common to all SAMD21 boards
#include "samd21.h"

#define MICROPY_CONFIG_ROM_LEVEL        (MICROPY_CONFIG_ROM_LEVEL_CORE_FEATURES)

// MicroPython emitters
#define MICROPY_EMIT_THUMB              (0)
#define MICROPY_EMIT_INLINE_THUMB       (0)
#define MICROPY_MODULE_BUILTIN_INIT     (1)

#define MICROPY_FLOAT_IMPL              (MICROPY_FLOAT_IMPL_FLOAT)

#ifndef MICROPY_PY_BUILTINS_COMPLEX
#define MICROPY_PY_BUILTINS_COMPLEX     (0)
#endif

#ifndef MICROPY_PY_MATH
#define MICROPY_PY_MATH                 (1)
#endif

#ifndef MICROPY_PY_CMATH
#define MICROPY_PY_CMATH                (0)
#endif

#define VFS_BLOCK_SIZE_BYTES            (1536) // 24x 64B flash pages;

#ifndef MICROPY_HW_UART_TXBUF
#define MICROPY_HW_UART_TXBUF           (1)
#endif

#ifndef MICROPY_PY_MACHINE_RTC
#if MICROPY_HW_XOSC32K
#define MICROPY_PY_MACHINE_RTC          (1)
#endif
#endif

#define CPU_FREQ                        (48000000)
#define DFLL48M_FREQ                    (48000000)
#define MAX_CPU_FREQ                    (48000000)

#define IRQ_PRI_PENDSV                  ((1 << __NVIC_PRIO_BITS) - 1)

static inline uint32_t raise_irq_pri(uint32_t pri) {
    (void)pri;
    return 0;
}

static inline void restore_irq_pri(uint32_t basepri) {
    (void)basepri;
}
