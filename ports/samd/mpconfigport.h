/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

// Options controlling how MicroPython is built, overriding defaults in py/mpconfig.h

// Board specific definitions
#include "mpconfigboard.h"


// Memory allocation policies
#define MICROPY_GC_STACK_ENTRY_TYPE         uint16_t
#define MICROPY_GC_ALLOC_THRESHOLD          (0)
#define MICROPY_ALLOC_PARSE_CHUNK_INIT      (32)
#define MICROPY_ALLOC_PATH_MAX              (256)
#define MICROPY_QSTR_BYTES_IN_HASH          (1)

// Compiler configuration
#define MICROPY_COMP_CONST                  (1)

// Python internal features
#define MICROPY_ENABLE_GC                   (1)
#define MICROPY_KBD_EXCEPTION               (1)
#define MICROPY_HELPER_REPL                 (1)
#define MICROPY_REPL_AUTO_INDENT            (1)
#define MICROPY_LONGINT_IMPL                (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_ENABLE_SOURCE_LINE          (1)
#define MICROPY_STREAMS_NON_BLOCK           (1)
#define MICROPY_ERROR_REPORTING             (MICROPY_ERROR_REPORTING_TERSE)
#define MICROPY_CPYTHON_COMPAT              (0)
#define MICROPY_CAN_OVERRIDE_BUILTINS       (1)
#define MICROPY_PY_BUILTINS_HELP            (1)
#define MICROPY_PY_BUILTINS_HELP_TEXT       samd_help_text
#define MICROPY_PY_BUILTINS_HELP_MODULES    (1)
#define MICROPY_ENABLE_SCHEDULER            (1)
#define MICROPY_MODULE_WEAK_LINKS           (1)

// Control over Python builtins
#define MICROPY_PY_ASYNC_AWAIT              (1)
#define MICROPY_PY_BUILTINS_STR_COUNT       (0)
#define MICROPY_PY_BUILTINS_MEMORYVIEW      (1)
#define MICROPY_PY_BUILTINS_SET             (0)
#define MICROPY_PY_BUILTINS_FROZENSET       (0)
#define MICROPY_PY_BUILTINS_PROPERTY        (0)
#define MICROPY_PY_BUILTINS_ENUMERATE       (1)
#define MICROPY_PY_BUILTINS_FILTER          (0)
#define MICROPY_PY_BUILTINS_REVERSED        (0)
#define MICROPY_PY_BUILTINS_NOTIMPLEMENTED  (1)
#define MICROPY_PY_BUILTINS_MIN_MAX         (1)
#define MICROPY_PY___FILE__                 (0)
#define MICROPY_PY_MICROPYTHON_MEM_INFO     (1)
#define MICROPY_PY_ARRAY_SLICE_ASSIGN       (1)
#define MICROPY_PY_ATTRTUPLE                (1)
#define MICROPY_PY_COLLECTIONS              (0)
#define MICROPY_PY_SYS                      (1)
#define MICROPY_PY_SYS_PLATFORM             "samd"
#define MICROPY_PY_SYS_EXIT                 (1)
#define MICROPY_PY_SYS_STDFILES             (1)
#define MICROPY_PY_SYS_MAXSIZE              (1)
#define MICROPY_PY_IO_FILEIO                (1)
#define MICROPY_PY_IO                       (1)
#define MICROPY_PY_IO_IOBASE                (1)

// Extended modules
#define MICROPY_PY_UTIME_MP_HAL             (1)
#define MICROPY_PY_MACHINE                  (1)
#define MICROPY_PY_UOS                      (1)
#define MICROPY_PY_UOS_INCLUDEFILE          "ports/samd/moduos.c"
#define MICROPY_READER_VFS                  (1)
#define MICROPY_VFS                         (1)
#define MICROPY_PY_UJSON                    (1)
#define MICROPY_PY_URE                      (1)
#define MICROPY_PY_UBINASCII                (1)
#define MICROPY_PY_UCTYPES                  (1)
#define MICROPY_PY_UHEAPQ                   (1)
#define MICROPY_PY_URANDOM                  (1)
#define MICROPY_PY_UZLIB                    (1)
#define MICROPY_PY_UASYNCIO                 (1)
#define MICROPY_PY_MACHINE_SOFTI2C          (1)
#define MICROPY_PY_MACHINE_SOFTSPI          (1)
#define MICROPY_PY_OS_DUPTERM               (3)

// Use VfsLfs's types for fileio/textio
#define mp_type_fileio mp_type_vfs_lfs1_fileio
#define mp_type_textio mp_type_vfs_lfs1_textio

#define MICROPY_PORT_ROOT_POINTERS \
    const char *readline_hist[8]; \
    void *samd_uart_rx_buffer[SERCOM_INST_NUM]; \
    /*
    void *samd_uart_tx_buffer[SERCOM_INST_NUM]; \
    */

#define MP_STATE_PORT MP_STATE_VM

// Miscellaneous settings
__attribute__((always_inline)) static inline void enable_irq(uint32_t state) {
    __set_PRIMASK(state);
}

__attribute__((always_inline)) static inline uint32_t disable_irq(void) {
    uint32_t state = __get_PRIMASK();
    __disable_irq();
    return state;
}

#if defined(MCU_SAMD51)
#define NVIC_PRIORITYGROUP_4    ((uint32_t)0x00000003)
#define IRQ_PRI_PENDSV          NVIC_EncodePriority(NVIC_PRIORITYGROUP_4, 7, 0)

static inline uint32_t raise_irq_pri(uint32_t pri) {
    uint32_t basepri = __get_BASEPRI();
    // If non-zero, the processor does not process any exception with a
    // priority value greater than or equal to BASEPRI.
    // When writing to BASEPRI_MAX the write goes to BASEPRI only if either:
    //   - Rn is non-zero and the current BASEPRI value is 0
    //   - Rn is non-zero and less than the current BASEPRI value
    pri <<= (8 - __NVIC_PRIO_BITS);
    __ASM volatile ("msr basepri_max, %0" : : "r" (pri) : "memory");
    return basepri;
}

// "basepri" should be the value returned from raise_irq_pri
static inline void restore_irq_pri(uint32_t basepri) {
    __set_BASEPRI(basepri);
}
#else
#define IRQ_PRI_PENDSV          ((1 << __NVIC_PRIO_BITS) - 1)

static inline uint32_t raise_irq_pri(uint32_t pri) {
    (void) pri;
    return 0;
}

static inline void restore_irq_pri(uint32_t basepri) {
    (void) basepri;
}
#endif

#define MICROPY_BEGIN_ATOMIC_SECTION()     disable_irq()
#define MICROPY_END_ATOMIC_SECTION(state)  enable_irq(state)

#define MICROPY_EVENT_POLL_HOOK \
    do { \
        extern void mp_handle_pending(bool); \
        mp_handle_pending(true); \
        __WFI(); \
    } while (0);

#define MICROPY_MAKE_POINTER_CALLABLE(p) ((void *)((mp_uint_t)(p) | 1))

#define MP_SSIZE_MAX (0x7fffffff)
typedef int mp_int_t; // must be pointer size
typedef unsigned mp_uint_t; // must be pointer size
typedef long mp_off_t;

// Need to provide a declaration/definition of alloca()
#include <alloca.h>
