// Deinitions common to all SAMD21 boards
#include "samd21.h"

#define MICROPY_CONFIG_ROM_LEVEL        (MICROPY_CONFIG_ROM_LEVEL_BASIC_FEATURES)
#if MICROPY_HW_CODESIZE == 248
#define SAMD21_EXTRA_FEATURES           1
#else
#define SAMD21_EXTRA_FEATURES           0
#endif

// MicroPython emitters
#define MICROPY_EMIT_THUMB              (SAMD21_EXTRA_FEATURES)
#define MICROPY_EMIT_INLINE_THUMB       (SAMD21_EXTRA_FEATURES)
#define MICROPY_EMIT_THUMB_ARMV7M       (0)
#define MICROPY_MODULE_BUILTIN_INIT     (1)

#define MICROPY_FLOAT_IMPL              (MICROPY_FLOAT_IMPL_FLOAT)

#ifndef MICROPY_PY_MATH
#define MICROPY_PY_MATH                 (1)
#define MP_NEED_LOG2                    (1)
#endif

#ifndef MICROPY_PY_BUILTINS_COMPLEX
#define MICROPY_PY_BUILTINS_COMPLEX     (0)
#endif

#ifndef MICROPY_PY_CMATH
#define MICROPY_PY_CMATH                (0)
#endif

#define MICROPY_PY_RANDOM_SEED_INIT_FUNC (trng_random_u32(300))
unsigned long trng_random_u32(int delay);

#ifndef MICROPY_HW_UART_TXBUF
#define MICROPY_HW_UART_TXBUF           (1)
#endif
#ifndef MICROPY_HW_UART_RTSCTS
#define MICROPY_HW_UART_RTSCTS          (SAMD21_EXTRA_FEATURES)
#endif

// selected extensions of the extra features set
#define MICROPY_PY_OS_URANDOM           (1)
#define MICROPY_COMP_TRIPLE_TUPLE_ASSIGN (SAMD21_EXTRA_FEATURES)
#define MICROPY_COMP_RETURN_IF_EXPR     (SAMD21_EXTRA_FEATURES)
#define MICROPY_OPT_MPZ_BITWISE         (SAMD21_EXTRA_FEATURES)
#define MICROPY_PY_BUILTINS_STR_CENTER  (SAMD21_EXTRA_FEATURES)
#define MICROPY_PY_BUILTINS_STR_PARTITION (SAMD21_EXTRA_FEATURES)
#define MICROPY_PY_BUILTINS_STR_SPLITLINES (SAMD21_EXTRA_FEATURES)
#define MICROPY_PY_BUILTINS_ROUND_INT   (SAMD21_EXTRA_FEATURES)
#define MICROPY_CAN_OVERRIDE_BUILTINS   (SAMD21_EXTRA_FEATURES)
#define MICROPY_PY_SYS_STDIO_BUFFER     (SAMD21_EXTRA_FEATURES)
#define MICROPY_PY_FRAMEBUF             (SAMD21_EXTRA_FEATURES)
#define MICROPY_PY_ASYNCIO              (SAMD21_EXTRA_FEATURES)
#define MICROPY_PY_SELECT               (SAMD21_EXTRA_FEATURES)
#define MICROPY_PY_ERRNO                (SAMD21_EXTRA_FEATURES)
#define MICROPY_PY_DEFLATE              (SAMD21_EXTRA_FEATURES)
#define MICROPY_PY_ONEWIRE              (SAMD21_EXTRA_FEATURES)

#ifndef MICROPY_PY_MACHINE_PIN_BOARD_CPU
#define MICROPY_PY_MACHINE_PIN_BOARD_CPU (1)
#endif

#define VFS_BLOCK_SIZE_BYTES            (1536) // 24x 64B flash pages;

#define CPU_FREQ                        (48000000)
#define DFLL48M_FREQ                    (48000000)
#define MAX_CPU_FREQ                    (54000000)
#define FDPLL_REF_FREQ                  (32768)

#define IRQ_PRI_PENDSV                  ((1 << __NVIC_PRIO_BITS) - 1)

static inline uint32_t raise_irq_pri(uint32_t pri) {
    (void)pri;
    return 0;
}

static inline void restore_irq_pri(uint32_t basepri) {
    (void)basepri;
}
