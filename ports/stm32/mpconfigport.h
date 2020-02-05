/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2017 Damien P. George
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

// Options to control how MicroPython is built for this port,
// overriding defaults in py/mpconfig.h.

// board specific definitions
#include "mpconfigboard.h"
#include "mpconfigboard_common.h"

// memory allocation policies
#ifndef MICROPY_GC_STACK_ENTRY_TYPE
#if MICROPY_HW_SDRAM_SIZE
#define MICROPY_GC_STACK_ENTRY_TYPE uint32_t
#else
#define MICROPY_GC_STACK_ENTRY_TYPE uint16_t
#endif
#endif
#define MICROPY_ALLOC_PATH_MAX      (128)

// emitters
#define MICROPY_PERSISTENT_CODE_LOAD (1)
#ifndef MICROPY_EMIT_THUMB
#define MICROPY_EMIT_THUMB          (1)
#endif
#ifndef MICROPY_EMIT_INLINE_THUMB
#define MICROPY_EMIT_INLINE_THUMB   (1)
#endif

// compiler configuration
#define MICROPY_COMP_MODULE_CONST   (1)
#define MICROPY_COMP_TRIPLE_TUPLE_ASSIGN (1)
#define MICROPY_COMP_RETURN_IF_EXPR (1)

// optimisations
#define MICROPY_OPT_COMPUTED_GOTO   (1)
#define MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE (0)
#define MICROPY_OPT_MPZ_BITWISE     (1)
#define MICROPY_OPT_MATH_FACTORIAL  (1)

// Python internal features
#define MICROPY_READER_VFS          (1)
#define MICROPY_ENABLE_GC           (1)
#define MICROPY_ENABLE_FINALISER    (1)
#define MICROPY_STACK_CHECK         (1)
#define MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF (1)
#define MICROPY_EMERGENCY_EXCEPTION_BUF_SIZE (0)
#define MICROPY_KBD_EXCEPTION       (1)
#define MICROPY_HELPER_REPL         (1)
#define MICROPY_REPL_INFO           (1)
#define MICROPY_REPL_EMACS_KEYS     (1)
#define MICROPY_REPL_AUTO_INDENT    (1)
#define MICROPY_LONGINT_IMPL        (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_ENABLE_SOURCE_LINE  (1)
#ifndef MICROPY_FLOAT_IMPL // can be configured by each board via mpconfigboard.mk
#define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_FLOAT)
#endif
#define MICROPY_STREAMS_NON_BLOCK   (1)
#define MICROPY_MODULE_WEAK_LINKS   (1)
#define MICROPY_CAN_OVERRIDE_BUILTINS (1)
#define MICROPY_USE_INTERNAL_ERRNO  (1)
#define MICROPY_ENABLE_SCHEDULER    (1)
#define MICROPY_SCHEDULER_DEPTH     (8)
#define MICROPY_VFS                 (1)

// control over Python builtins
#define MICROPY_PY_FUNCTION_ATTRS   (1)
#define MICROPY_PY_DESCRIPTORS      (1)
#define MICROPY_PY_DELATTR_SETATTR  (1)
#define MICROPY_PY_BUILTINS_STR_UNICODE (1)
#define MICROPY_PY_BUILTINS_STR_CENTER (1)
#define MICROPY_PY_BUILTINS_STR_PARTITION (1)
#define MICROPY_PY_BUILTINS_STR_SPLITLINES (1)
#define MICROPY_PY_BUILTINS_MEMORYVIEW (1)
#define MICROPY_PY_BUILTINS_FROZENSET (1)
#define MICROPY_PY_BUILTINS_SLICE_ATTRS (1)
#define MICROPY_PY_BUILTINS_SLICE_INDICES (1)
#define MICROPY_PY_BUILTINS_ROUND_INT (1)
#define MICROPY_PY_ALL_SPECIAL_METHODS (1)
#define MICROPY_PY_BUILTINS_COMPILE (MICROPY_ENABLE_COMPILER)
#define MICROPY_PY_BUILTINS_EXECFILE (MICROPY_ENABLE_COMPILER)
#define MICROPY_PY_BUILTINS_NOTIMPLEMENTED (1)
#define MICROPY_PY_BUILTINS_INPUT   (1)
#define MICROPY_PY_BUILTINS_POW3    (1)
#define MICROPY_PY_BUILTINS_HELP    (1)
#define MICROPY_PY_BUILTINS_HELP_TEXT stm32_help_text
#define MICROPY_PY_BUILTINS_HELP_MODULES (1)
#define MICROPY_PY_MICROPYTHON_MEM_INFO (1)
#define MICROPY_PY_ARRAY_SLICE_ASSIGN (1)
#define MICROPY_PY_COLLECTIONS_DEQUE (1)
#define MICROPY_PY_COLLECTIONS_ORDEREDDICT (1)
#define MICROPY_PY_MATH_SPECIAL_FUNCTIONS (1)
#define MICROPY_PY_MATH_ISCLOSE     (1)
#define MICROPY_PY_MATH_FACTORIAL   (1)
#define MICROPY_PY_CMATH            (1)
#define MICROPY_PY_IO               (1)
#define MICROPY_PY_IO_IOBASE        (1)
#define MICROPY_PY_IO_FILEIO        (MICROPY_VFS_FAT) // because mp_type_fileio/textio point to fatfs impl
#define MICROPY_PY_SYS_MAXSIZE      (1)
#define MICROPY_PY_SYS_EXIT         (1)
#define MICROPY_PY_SYS_STDFILES     (1)
#define MICROPY_PY_SYS_STDIO_BUFFER (1)
#ifndef MICROPY_PY_SYS_PLATFORM     // let boards override it if they want
#define MICROPY_PY_SYS_PLATFORM     "pyboard"
#endif
#define MICROPY_PY_UERRNO           (1)
#ifndef MICROPY_PY_THREAD
#define MICROPY_PY_THREAD           (0)
#endif

// extended modules
#define MICROPY_PY_UCTYPES          (1)
#define MICROPY_PY_UZLIB            (1)
#define MICROPY_PY_UJSON            (1)
#define MICROPY_PY_URE              (1)
#define MICROPY_PY_URE_SUB          (1)
#define MICROPY_PY_UHEAPQ           (1)
#define MICROPY_PY_UHASHLIB         (1)
#define MICROPY_PY_UHASHLIB_MD5     (MICROPY_PY_USSL)
#define MICROPY_PY_UHASHLIB_SHA1    (MICROPY_PY_USSL)
#define MICROPY_PY_UCRYPTOLIB       (MICROPY_PY_USSL)
#define MICROPY_PY_UBINASCII        (1)
#define MICROPY_PY_URANDOM          (1)
#define MICROPY_PY_URANDOM_EXTRA_FUNCS (1)
#define MICROPY_PY_USELECT          (1)
#define MICROPY_PY_UTIMEQ           (1)
#define MICROPY_PY_UTIME_MP_HAL     (1)
#define MICROPY_PY_OS_DUPTERM       (3)
#define MICROPY_PY_UOS_DUPTERM_BUILTIN_STREAM (1)
#define MICROPY_PY_LWIP_SOCK_RAW    (MICROPY_PY_LWIP)
#define MICROPY_PY_MACHINE          (1)
#define MICROPY_PY_MACHINE_PULSE    (1)
#define MICROPY_PY_MACHINE_PIN_MAKE_NEW mp_pin_make_new
#define MICROPY_PY_MACHINE_I2C      (1)
#if MICROPY_HW_ENABLE_HW_I2C
#define MICROPY_PY_MACHINE_I2C_MAKE_NEW machine_hard_i2c_make_new
#endif
#define MICROPY_PY_MACHINE_SPI      (1)
#define MICROPY_PY_MACHINE_SPI_MSB  (SPI_FIRSTBIT_MSB)
#define MICROPY_PY_MACHINE_SPI_LSB  (SPI_FIRSTBIT_LSB)
#define MICROPY_PY_MACHINE_SPI_MAKE_NEW machine_hard_spi_make_new
#define MICROPY_HW_SOFTSPI_MIN_DELAY (0)
#define MICROPY_HW_SOFTSPI_MAX_BAUDRATE (HAL_RCC_GetSysClockFreq() / 48)
#define MICROPY_PY_UWEBSOCKET       (MICROPY_PY_LWIP)
#define MICROPY_PY_WEBREPL          (MICROPY_PY_LWIP)
#ifndef MICROPY_PY_FRAMEBUF
#define MICROPY_PY_FRAMEBUF         (1)
#endif
#ifndef MICROPY_PY_USOCKET
#define MICROPY_PY_USOCKET          (1)
#endif
#ifndef MICROPY_PY_NETWORK
#define MICROPY_PY_NETWORK          (1)
#endif

// fatfs configuration used in ffconf.h
#define MICROPY_FATFS_ENABLE_LFN       (1)
#define MICROPY_FATFS_LFN_CODE_PAGE    437 /* 1=SFN/ANSI 437=LFN/U.S.(OEM) */
#define MICROPY_FATFS_USE_LABEL        (1)
#define MICROPY_FATFS_RPATH            (2)
#define MICROPY_FATFS_MULTI_PARTITION  (1)

// TODO these should be generic, not bound to fatfs
#define mp_type_fileio mp_type_vfs_fat_fileio
#define mp_type_textio mp_type_vfs_fat_textio

// use vfs's functions for import stat and builtin open
#define mp_import_stat mp_vfs_import_stat
#define mp_builtin_open mp_vfs_open
#define mp_builtin_open_obj mp_vfs_open_obj

// extra built in names to add to the global namespace
#define MICROPY_PORT_BUILTINS \
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&mp_builtin_open_obj) },

// extra built in modules to add to the list of known ones
extern const struct _mp_obj_module_t machine_module;
extern const struct _mp_obj_module_t pyb_module;
extern const struct _mp_obj_module_t stm_module;
extern const struct _mp_obj_module_t mp_module_ubinascii;
extern const struct _mp_obj_module_t mp_module_ure;
extern const struct _mp_obj_module_t mp_module_uzlib;
extern const struct _mp_obj_module_t mp_module_ujson;
extern const struct _mp_obj_module_t mp_module_uheapq;
extern const struct _mp_obj_module_t mp_module_uhashlib;
extern const struct _mp_obj_module_t mp_module_uos;
extern const struct _mp_obj_module_t mp_module_utime;
extern const struct _mp_obj_module_t mp_module_usocket;
extern const struct _mp_obj_module_t mp_module_network;
extern const struct _mp_obj_module_t mp_module_onewire;

#if MICROPY_PY_STM
#define STM_BUILTIN_MODULE               { MP_ROM_QSTR(MP_QSTR_stm), MP_ROM_PTR(&stm_module) },
#else
#define STM_BUILTIN_MODULE
#endif

#if MICROPY_PY_USOCKET && MICROPY_PY_LWIP
// usocket implementation provided by lwIP
#define SOCKET_BUILTIN_MODULE               { MP_ROM_QSTR(MP_QSTR_usocket), MP_ROM_PTR(&mp_module_lwip) },
#elif MICROPY_PY_USOCKET
// usocket implementation provided by skeleton wrapper
#define SOCKET_BUILTIN_MODULE               { MP_ROM_QSTR(MP_QSTR_usocket), MP_ROM_PTR(&mp_module_usocket) },
#else
// no usocket module
#define SOCKET_BUILTIN_MODULE
#endif

#if MICROPY_PY_NETWORK
#define NETWORK_BUILTIN_MODULE              { MP_ROM_QSTR(MP_QSTR_network), MP_ROM_PTR(&mp_module_network) },
#else
#define NETWORK_BUILTIN_MODULE
#endif

#define MICROPY_PORT_BUILTIN_MODULES \
    { MP_ROM_QSTR(MP_QSTR_umachine), MP_ROM_PTR(&machine_module) }, \
    { MP_ROM_QSTR(MP_QSTR_pyb), MP_ROM_PTR(&pyb_module) }, \
    STM_BUILTIN_MODULE \
    { MP_ROM_QSTR(MP_QSTR_uos), MP_ROM_PTR(&mp_module_uos) }, \
    { MP_ROM_QSTR(MP_QSTR_utime), MP_ROM_PTR(&mp_module_utime) }, \
    SOCKET_BUILTIN_MODULE \
    NETWORK_BUILTIN_MODULE \
    { MP_ROM_QSTR(MP_QSTR__onewire), MP_ROM_PTR(&mp_module_onewire) }, \

// extra constants
#define MICROPY_PORT_CONSTANTS \
    { MP_ROM_QSTR(MP_QSTR_umachine), MP_ROM_PTR(&machine_module) }, \
    { MP_ROM_QSTR(MP_QSTR_machine), MP_ROM_PTR(&machine_module) }, \
    { MP_ROM_QSTR(MP_QSTR_pyb), MP_ROM_PTR(&pyb_module) }, \
    STM_BUILTIN_MODULE \

#define MP_STATE_PORT MP_STATE_VM

#if MICROPY_SSL_MBEDTLS
#define MICROPY_PORT_ROOT_POINTER_MBEDTLS void **mbedtls_memory;
#else
#define MICROPY_PORT_ROOT_POINTER_MBEDTLS
#endif

#if MICROPY_BLUETOOTH_NIMBLE
struct _mp_bluetooth_nimble_root_pointers_t;
#define MICROPY_PORT_ROOT_POINTER_BLUETOOTH_NIMBLE void **bluetooth_nimble_memory; struct _mp_bluetooth_nimble_root_pointers_t *bluetooth_nimble_root_pointers;
#else
#define MICROPY_PORT_ROOT_POINTER_BLUETOOTH_NIMBLE
#endif

#define MICROPY_PORT_ROOT_POINTERS \
    const char *readline_hist[8]; \
    \
    mp_obj_t pyb_hid_report_desc; \
    \
    mp_obj_t pyb_config_main; \
    \
    mp_obj_t pyb_switch_callback; \
    \
    mp_obj_t pin_class_mapper; \
    mp_obj_t pin_class_map_dict; \
    \
    mp_obj_t pyb_extint_callback[PYB_EXTI_NUM_VECTORS]; \
    \
    struct _soft_timer_entry_t *soft_timer_heap; \
    \
    /* pointers to all Timer objects (if they have been created) */ \
    struct _pyb_timer_obj_t *pyb_timer_obj_all[MICROPY_HW_MAX_TIMER]; \
    \
    /* stdio is repeated on this UART object if it's not null */ \
    struct _pyb_uart_obj_t *pyb_stdio_uart; \
    \
    /* pointers to all UART objects (if they have been created) */ \
    struct _pyb_uart_obj_t *pyb_uart_obj_all[MICROPY_HW_MAX_UART]; \
    \
    /* pointers to all CAN objects (if they have been created) */ \
    struct _pyb_can_obj_t *pyb_can_obj_all[MICROPY_HW_MAX_CAN]; \
    \
    /* list of registered NICs */ \
    mp_obj_list_t mod_network_nic_list; \
    \
    MICROPY_PORT_ROOT_POINTER_MBEDTLS \
    MICROPY_PORT_ROOT_POINTER_BLUETOOTH_NIMBLE \

// type definitions for the specific machine

#define MICROPY_MAKE_POINTER_CALLABLE(p) ((void*)((uint32_t)(p) | 1))

#define MP_SSIZE_MAX (0x7fffffff)

// Assume that if we already defined the obj repr then we also defined these items
#ifndef MICROPY_OBJ_REPR
#define UINT_FMT "%u"
#define INT_FMT "%d"
typedef int mp_int_t; // must be pointer size
typedef unsigned int mp_uint_t; // must be pointer size
#endif

typedef long mp_off_t;

#define MP_PLAT_PRINT_STRN(str, len) mp_hal_stdout_tx_strn_cooked(str, len)

// We have inlined IRQ functions for efficiency (they are generally
// 1 machine instruction).
//
// Note on IRQ state: you should not need to know the specific
// value of the state variable, but rather just pass the return
// value from disable_irq back to enable_irq.  If you really need
// to know the machine-specific values, see irq.h.

static inline void enable_irq(mp_uint_t state) {
    __set_PRIMASK(state);
}

static inline mp_uint_t disable_irq(void) {
    mp_uint_t state = __get_PRIMASK();
    __disable_irq();
    return state;
}

#define MICROPY_BEGIN_ATOMIC_SECTION()     disable_irq()
#define MICROPY_END_ATOMIC_SECTION(state)  enable_irq(state)

#if MICROPY_PY_THREAD
#define MICROPY_EVENT_POLL_HOOK \
    do { \
        extern void mp_handle_pending(bool); \
        mp_handle_pending(true); \
        if (pyb_thread_enabled) { \
            MP_THREAD_GIL_EXIT(); \
            pyb_thread_yield(); \
            MP_THREAD_GIL_ENTER(); \
        } else { \
            __WFI(); \
        } \
    } while (0);

#define MICROPY_THREAD_YIELD() pyb_thread_yield()
#else
#define MICROPY_EVENT_POLL_HOOK \
    do { \
        extern void mp_handle_pending(bool); \
        mp_handle_pending(true); \
        __WFI(); \
    } while (0);

#define MICROPY_THREAD_YIELD()
#endif

// The LwIP interface must run at a raised IRQ priority
#define MICROPY_PY_LWIP_ENTER   uint32_t irq_state = raise_irq_pri(IRQ_PRI_PENDSV);
#define MICROPY_PY_LWIP_REENTER irq_state = raise_irq_pri(IRQ_PRI_PENDSV);
#define MICROPY_PY_LWIP_EXIT    restore_irq_pri(irq_state);

// Bluetooth calls must run at a raised IRQ priority
#define MICROPY_PY_BLUETOOTH_ENTER MICROPY_PY_LWIP_ENTER
#define MICROPY_PY_BLUETOOTH_EXIT MICROPY_PY_LWIP_EXIT

// We need an implementation of the log2 function which is not a macro
#define MP_NEED_LOG2 (1)

// We need to provide a declaration/definition of alloca()
#include <alloca.h>
