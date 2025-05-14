// Definitions common to all SAMD21 boards
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

// Selected extensions beyond the basic features set.
#define MICROPY_ENABLE_FINALISER        (1)
#define MICROPY_STACK_CHECK             (1)
#define MICROPY_KBD_EXCEPTION           (1)
#define MICROPY_HELPER_REPL             (1)
#define MICROPY_REPL_AUTO_INDENT        (1)
#define MICROPY_ENABLE_SOURCE_LINE      (1)
#define MICROPY_STREAMS_NON_BLOCK       (1)
#define MICROPY_PY_BUILTINS_HELP        (1)
#define MICROPY_PY_BUILTINS_HELP_MODULES (1)
#define MICROPY_ENABLE_SCHEDULER        (1)
#define MICROPY_PY_BUILTINS_BYTES_HEX   (1)
#define MICROPY_PY_BUILTINS_MEMORYVIEW  (1)
#define MICROPY_PY_BUILTINS_INPUT       (1)
#define MICROPY_PY_MICROPYTHON_MEM_INFO (1)
#define MICROPY_PY_ARRAY_SLICE_ASSIGN   (1)
#define MICROPY_PY_SYS_STDFILES         (1)
#define MICROPY_PY_SYS_MAXSIZE          (1)
#define MICROPY_PY_IO_IOBASE            (1)
#define MICROPY_PY_OS                   (1)
#define MICROPY_PY_JSON                 (1)
#define MICROPY_PY_RE                   (1)
#define MICROPY_PY_BINASCII             (1)
#define MICROPY_PY_UCTYPES              (1)
#define MICROPY_PY_HEAPQ                (1)
#define MICROPY_PY_RANDOM               (1)
#define MICROPY_PY_PLATFORM             (1)

#define MICROPY_PY_RANDOM_SEED_INIT_FUNC (trng_random_u32(300))
unsigned long trng_random_u32(int delay);

#ifndef MICROPY_HW_UART_TXBUF
#define MICROPY_HW_UART_TXBUF           (1)
#endif
#ifndef MICROPY_HW_UART_RTSCTS
#define MICROPY_HW_UART_RTSCTS          (SAMD21_EXTRA_FEATURES)
#endif
#define MICROPY_PY_MACHINE_UART_IRQ     (SAMD21_EXTRA_FEATURES)

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
#ifndef MICROPY_PY_FRAMEBUF
#define MICROPY_PY_FRAMEBUF             (SAMD21_EXTRA_FEATURES)
#endif
#ifndef MICROPY_PY_ASYNCIO
#define MICROPY_PY_ASYNCIO              (SAMD21_EXTRA_FEATURES)
#endif
#ifndef MICROPY_PY_SELECT
#define MICROPY_PY_SELECT               (SAMD21_EXTRA_FEATURES)
#endif
#define MICROPY_PY_ERRNO                (SAMD21_EXTRA_FEATURES)
#define MICROPY_PY_DEFLATE              (SAMD21_EXTRA_FEATURES)
#ifndef MICROPY_PY_ONEWIRE
#define MICROPY_PY_ONEWIRE              (SAMD21_EXTRA_FEATURES)
#endif

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
