/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Armink (armink.ztl@gmail.com)
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
#include <rthw.h>
#include <rtthread.h>
#include <stdint.h>
#include <stdio.h>

// options to control how MicroPython is built

// You can disable the built-in MicroPython compiler by setting the following
// config option to 0.  If you do this then you won't get a REPL prompt, but you
// will still be able to execute pre-compiled scripts, compiled with mpy-cross.
#define MICROPY_ENABLE_COMPILER     (1)

#if defined(PKG_MICROPYTHON_HEAP_SIZE)
    #define MICROPY_HEAP_SIZE PKG_MICROPYTHON_HEAP_SIZE
#else
    #define MICROPY_HEAP_SIZE (8 * 1024)
#endif

#define MP_ENDIANNESS_LITTLE        (1)
#define MICROPY_STACK_CHECK         (1)
#define MICROPY_PY_MICROPYTHON_STACK_USE (1)
#define MICROPY_QSTR_BYTES_IN_HASH  (1)
#define MICROPY_QSTR_EXTRA_POOL     mp_qstr_frozen_const_pool
#define MICROPY_ALLOC_PATH_MAX      (256)
#define MICROPY_ALLOC_PARSE_CHUNK_INIT (16)
#define MICROPY_EMIT_X64            (0)
#define MICROPY_EMIT_THUMB          (0)
#define MICROPY_EMIT_INLINE_THUMB   (0)
#define MICROPY_COMP_MODULE_CONST   (0)
#define MICROPY_COMP_CONST          (0)
#define MICROPY_COMP_DOUBLE_TUPLE_ASSIGN (0)
#define MICROPY_COMP_TRIPLE_TUPLE_ASSIGN (0)
#define MICROPY_MEM_STATS           (0)
#define MICROPY_DEBUG_PRINTERS      (0)
#define MICROPY_ENABLE_GC           (1)
#define MICROPY_ENABLE_FINALISER    (1)
#define MICROPY_GC_ALLOC_THRESHOLD  (0)
#define MICROPY_REPL_EVENT_DRIVEN   (0)
#define MICROPY_REPL_AUTO_INDENT    (1)
#define MICROPY_KBD_EXCEPTION       (1)
#define MICROPY_HELPER_REPL         (1)
#define MICROPY_HELPER_LEXER_UNIX   (0)
#define MICROPY_ENABLE_SOURCE_LINE  (1)
#define MICROPY_ENABLE_DOC_STRING   (0)
#define MICROPY_ENABLE_SCHEDULER    (1)
#define MICROPY_ERROR_REPORTING     (MICROPY_ERROR_REPORTING_TERSE)
#define MICROPY_BUILTIN_METHOD_CHECK_SELF_ARG (0)
#define MICROPY_PY_ASYNC_AWAIT      (0)
// control over Python builtins
#define MICROPY_PY_FUNCTION_ATTRS   (1)
#define MICROPY_PY_BUILTINS_STR_UNICODE (1)
#define MICROPY_PY_BUILTINS_STR_CENTER (1)
#define MICROPY_PY_BUILTINS_STR_PARTITION (1)
#define MICROPY_PY_BUILTINS_STR_SPLITLINES (1)
#define MICROPY_PY_BUILTINS_BYTEARRAY (1)
#define MICROPY_PY_BUILTINS_MEMORYVIEW (1)
#define MICROPY_PY_BUILTINS_SLICE_ATTRS (1)
#define MICROPY_PY_ALL_SPECIAL_METHODS (1)
#define MICROPY_PY_BUILTINS_INPUT (1)
#define MICROPY_PY_BUILTINS_POW3 (1)
#define MICROPY_PY_BUILTINS_ENUMERATE (1)
#define MICROPY_PY_BUILTINS_FILTER  (1)
#define MICROPY_PY_BUILTINS_FROZENSET (1)
#define MICROPY_PY_BUILTINS_REVERSED (1)
#define MICROPY_PY_BUILTINS_SET     (1)
#define MICROPY_PY_BUILTINS_HELP    (1)
#define MICROPY_PY_BUILTINS_HELP_TEXT rtthread_help_text
#define MICROPY_PY_BUILTINS_HELP_MODULES (1)
#define MICROPY_PY_BUILTINS_SLICE   (1)
#define MICROPY_PY_BUILTINS_PROPERTY (1)
#define MICROPY_PY_BUILTINS_MIN_MAX (1)
#define MICROPY_PY___FILE__         (1)
#define MICROPY_PY_GC               (1)
#define MICROPY_PY_ARRAY            (1)
#define MICROPY_PY_ARRAY_SLICE_ASSIGN (1)
#define MICROPY_PY_ATTRTUPLE        (1)
#define MICROPY_PY_COLLECTIONS      (1)
#define MICROPY_PY_COLLECTIONS_ORDEREDDICT (1)
#define MICROPY_PY_MATH             (1)
#define MICROPY_PY_MATH_SPECIAL_FUNCTIONS (1)
#define MICROPY_PY_MICROPYTHON_MEM_INFO (1)
#define MICROPY_STREAMS_NON_BLOCK   (1)
#define MICROPY_MODULE_WEAK_LINKS   (1)
#define MICROPY_CAN_OVERRIDE_BUILTINS (1)
#define MICROPY_USE_INTERNAL_ERRNO  (1)
#define MICROPY_USE_INTERNAL_PRINTF (0)
#define MICROPY_PY_STRUCT           (1)
#define MICROPY_PY_SYS              (1)
#define MICROPY_MODULE_FROZEN_MPY   (1)
#define MICROPY_MODULE_FROZEN_STR   (0)
#define MICROPY_CPYTHON_COMPAT      (1)
#define MICROPY_LONGINT_IMPL        (MICROPY_LONGINT_IMPL_MPZ)
#ifdef MICROPYTHON_USING_FLOAT_IMPL_FLOAT
    #define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_FLOAT)
#else
    #define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_DOUBLE)
#endif
#define MICROPY_READER_VFS          (0)
#define MICROPY_PY_PIN              (1)
#define MICROPY_PY_OS_DUPTERM       (1)
#define MICROPY_VFS                 (0)
#define MICROPY_VFS_FAT             (0)
#define MICROPY_PY_UTIME            (1)
#define MICROPY_PY_MACHINE          (1)
#define MICROPY_PY_MACHINE_PIN_MAKE_NEW mp_pin_make_new
#define MICROPY_PY_UTIME_MP_HAL     (1)
#define MICROPY_PY_UTIMEQ           (1)
#define MICROPY_PY_RTTHREAD         (1)

/*****************************************************************************/
/* Hardware Module                                                           */

#ifdef MICROPYTHON_USING_MACHINE_I2C
    #define MICROPY_PY_MACHINE_I2C      (1)
    #define MICROPY_PY_MACHINE_I2C_MAKE_NEW machine_hard_i2c_make_new
#endif

#ifdef MICROPYTHON_USING_MACHINE_SPI
    #define MICROPY_PY_MACHINE_SPI      (1)
    #define MICROPY_PY_MACHINE_SPI_MAKE_NEW machine_hard_spi_make_new
#endif

#ifdef MICROPYTHON_USING_MACHINE_UART
    #define MICROPY_PY_MACHINE_UART      (1)
#endif

#ifdef MICROPYTHON_USING_MACHINE_ADC
    #define MICROPY_PY_MACHINE_ADC       (1)
#endif

#ifdef MICROPYTHON_USING_MACHINE_PWM
    #define MICROPY_PY_MACHINE_PWM       (1)
#endif

#ifdef MICROPYTHON_USING_MACHINE_LCD
    #define MICROPY_PY_MACHINE_LCD       (1)
#endif

#ifdef MICROPYTHON_USING_MACHINE_RTC
    #define MICROPY_PY_MACHINE_RTC       (1)
#endif

#ifdef MICROPYTHON_USING_MACHINE_WDT
    #define MICROPY_PY_MACHINE_WDT       (1)
#endif

#ifdef MICROPYTHON_USING_MACHINE_TIMER
    #define MICROPY_PY_MACHINE_TIMER     (1)
#endif

/*****************************************************************************/
/* System Module                                                             */

#ifdef MICROPYTHON_USING_UOS
    #define MICROPY_PY_IO               (1)
    #define MICROPY_PY_IO_FILEIO        (1)
    #define MICROPY_PY_MODUOS           (1)
    #define MICROPY_PY_MODUOS_FILE      (1)
    #define MICROPY_PY_SYS_STDFILES     (1)
    #define MICROPY_READER_POSIX        (1)
    #define MICROPY_PY_BUILTINS_COMPILE (1)
    #define MICROPY_PY_BUILTINS_EXECFILE (1)
    #define MICROPY_PERSISTENT_CODE_LOAD (1)
#else
    #define MICROPY_PY_IO               (0)
    #define MICROPY_PY_MODUOS           (0)
#endif /* MICROPYTHON_USING_UOS */

#ifdef MICROPYTHON_USING_THREAD
    #define MICROPY_PY_THREAD           (1)
    #define MICROPY_SCHEDULER_DEPTH     (8)
#endif /* MICROPYTHON_USING_THREAD */

#ifdef MICROPYTHON_USING_USELECT
    #define MICROPY_PY_USELECT          (1)
#endif

#ifdef MICROPYTHON_USING_UCTYPES
    #define MICROPY_PY_UCTYPES          (1)
#endif

#ifdef MICROPYTHON_USING_UERRNO
    #define MICROPY_PY_UERRNO           (1)
#endif

#ifdef MICROPYTHON_USING_FFI
    #define MICROPY_PY_FFI              (1)
#endif

/*****************************************************************************/
/* Tools Module                                                              */

#ifdef MICROPYTHON_USING_CMATH
    #define MICROPY_PY_CMATH            (1)
#endif

#ifdef MICROPYTHON_USING_UBINASCII
    #define MICROPY_PY_UBINASCII        (1)
#endif

#ifdef MICROPYTHON_USING_UHASHLIB
    #define MICROPY_PY_UHASHLIB         (1)
#endif

#ifdef MICROPYTHON_USING_UHEAPQ
    #define MICROPY_PY_UHEAPQ           (1)
#endif

#ifdef MICROPYTHON_USING_UJSON
    #define MICROPY_PY_UJSON            (1)
#endif

#ifdef MICROPYTHON_USING_URE
    #define MICROPY_PY_URE              (1)
#endif

#ifdef MICROPYTHON_USING_UZLIB
    #define MICROPY_PY_UZLIB            (1)
#endif

#ifdef MICROPYTHON_USING_URANDOM
    #define MICROPY_PY_URANDOM             (1)
    #define MICROPY_PY_URANDOM_EXTRA_FUNCS (1)
#endif

/*****************************************************************************/
/* Network Module                                                            */

#ifdef MICROPYTHON_USING_USOCKET
    #define MICROPY_PY_USOCKET          (1)
#endif

#ifdef MICROPYTHON_USING_USSL
    #define MICROPY_PY_USSL             (1)
    #define MICROPY_SSL_MBEDTLS         (1)
#endif

#ifdef MICROPYTHON_USING_NETWORK
    #define MICROPY_PY_NETWORK          (1)
#endif

#ifdef MICROPYTHON_USING_WLAN
    #define MICROPY_PY_WLAN             (1)
#endif

#if MICROPY_PY_THREAD
#define MICROPY_EVENT_POLL_HOOK \
    do { \
        extern void mp_handle_pending(bool); \
        mp_handle_pending(true); \
        MP_THREAD_GIL_EXIT(); \
        MP_THREAD_GIL_ENTER(); \
    } while (0);
#else
#define MICROPY_EVENT_POLL_HOOK \
    do { \
        extern void mp_handle_pending(bool); \
        mp_handle_pending(true); \
        rt_thread_delay(1); \
    } while (0);
#endif

/*****************************************************************************/
/* Modules define in your project
   You must provide 'moddefs.user.extmods.h'
   and 'qstrdefs.user.extmods.h'
*/

#ifdef MICROPYTHON_USING_USEREXTMODS
    #define MICROPY_USER_EXTMODS     (1)
    #include <moddefs.user.extmods.h>
#else
    #define MICROPY_USER_EXTMODS     (0)
#endif
#ifndef MICROPY_USER_MODULES
    #define MICROPY_USER_MODULES
#endif

#if defined(__CC_ARM)
    #include <sys/types.h>
    #define MICROPY_NO_ALLOCA           1
    #define MP_WEAK                     RT_WEAK
    #define MP_NOINLINE
    #define MP_ALWAYSINLINE
    #define MP_LIKELY(x)               x
    #define MP_UNLIKELY(x)             x
    #undef __arm__
    #undef __thumb__
    #undef __thumb2__
#elif defined(__ICCARM__)
    #include <libc/libc_errno.h>
    #include <sys/types.h>
    #define MICROPY_NO_ALLOCA           1
    #define NORETURN                    __noreturn
    #define MP_WEAK                     RT_WEAK
    #define MP_NOINLINE
    #define MP_ALWAYSINLINE
    #define MP_LIKELY(x)               x
    #define MP_UNLIKELY(x)             x
#elif defined(__GNUC__)
    // We need to provide a declaration/definition of alloca()
    #include <alloca.h>
#else
    #error "not supported compiler"
#endif /* defined(__CC_ARM) */

// type definitions for the specific machine

#define MICROPY_MAKE_POINTER_CALLABLE(p) ((void*)((mp_uint_t)(p) | 1))

// This port is intended to be 32-bit, but unfortunately, int32_t for
// different targets may be defined in different ways - either as int
// or as long. This requires different printf formatting specifiers
// to print such value. So, we avoid int32_t and use int directly.
#define UINT_FMT "%u"
#define INT_FMT "%d"
typedef intptr_t mp_int_t; // must be pointer size
typedef uintptr_t mp_uint_t; // must be pointer size

typedef long mp_off_t;

#define MP_PLAT_PRINT_STRN(str, len)   mp_hal_stdout_tx_strn_stream(str, len)

#define MICROPY_PY_SYS_PLATFORM        "rt-thread"
#define MICROPY_HW_BOARD_NAME          "Universal python platform"
#define MICROPY_HW_MCU_NAME            "RT-Thread"
#define MICROPY_PY_PATH_FIRST          "/libs/mpy/"
#define MICROPY_PY_PATH_SECOND         "/scripts/"
#define MICROPY_MAIN_PY_PATH           "/main.py"

#define MICROPY_BEGIN_ATOMIC_SECTION()     rt_hw_interrupt_disable()
#define MICROPY_END_ATOMIC_SECTION(state)  rt_hw_interrupt_enable(state)

#ifdef __linux__
    #define MICROPY_MIN_USE_STDOUT (1)
#endif

#define MP_STATE_PORT                  MP_STATE_VM

#define MICROPY_PORT_ROOT_POINTERS     const char *readline_hist[8];

extern const struct _mp_obj_module_t pyb_module;
extern const struct _mp_obj_module_t mp_module_rtthread;
extern const struct _mp_obj_module_t mp_module_time;
extern const struct _mp_obj_module_t mp_module_machine;
extern const struct _mp_obj_module_t mp_module_uos;
extern const struct _mp_obj_module_t mp_module_uselect;
extern const struct _mp_obj_module_t mp_module_usocket;
extern const struct _mp_obj_module_t mp_module_io;
extern const struct _mp_obj_fun_builtin_fixed_t machine_soft_reset_obj;
extern const struct _mp_obj_module_t mp_module_ffi;
extern const struct _mp_obj_module_t mp_module_network;
extern const struct _mp_obj_module_t mp_module_userfunc;

#if MICROPY_PY_RTTHREAD
    #define RTTHREAD_PORT_BUILTIN_MODULES { MP_ROM_QSTR(MP_QSTR_rtthread), MP_ROM_PTR(&mp_module_rtthread) },
#else
    #define RTTHREAD_PORT_BUILTIN_MODULES
#endif /* MICROPY_PY_RTTHREAD */

#if MICROPY_PY_MODUOS
    #define MODUOS_PORT_BUILTINS                     { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&mp_builtin_open_obj) },
    #define MODUOS_PORT_BUILTIN_MODULES              { MP_ROM_QSTR(MP_QSTR_uos), MP_ROM_PTR(&mp_module_uos) },
    #define MODUOS_PORT_BUILTIN_MODULE_WEAK_LINKS    { MP_ROM_QSTR(MP_QSTR_os), MP_ROM_PTR(&mp_module_uos) },
    #define mp_import_stat(x)                        mp_posix_import_stat(x)
#else
    #define MODUOS_PORT_BUILTINS
    #define MODUOS_PORT_BUILTIN_MODULES
    #define MODUOS_PORT_BUILTIN_MODULE_WEAK_LINKS
#endif /* MICROPY_PY_MODUOS */

#if MICROPY_PY_USOCKET
    #define SOCKET_PORT_BUILTIN_MODULES              { MP_ROM_QSTR(MP_QSTR_usocket), MP_ROM_PTR(&mp_module_usocket) },
    #define SOCKET_PORT_BUILTIN_MODULE_WEAK_LINKS    { MP_ROM_QSTR(MP_QSTR_socket), MP_ROM_PTR(&mp_module_usocket) },
#else
    #define SOCKET_PORT_BUILTIN_MODULES
    #define SOCKET_PORT_BUILTIN_MODULE_WEAK_LINKS
#endif /* MICROPY_PY_USOCKET */

#ifdef MICROPYTHON_USING_UBINASCII
    #define MODUBINASCII_PORT_BUILTIN_MODULE_WEAK_LINKS        { MP_ROM_QSTR(MP_QSTR_binascii), MP_ROM_PTR(&mp_module_ubinascii) },
#else
    #define MODUBINASCII_PORT_BUILTIN_MODULE_WEAK_LINKS
#endif /* MICROPY_PY_UBINASCII */

#ifdef MICROPY_PY_COLLECTIONS
    #define MODUCOLLECTIONS_PORT_BUILTIN_MODULE_WEAK_LINKS     { MP_ROM_QSTR(MP_QSTR_collections), MP_ROM_PTR(&mp_module_collections ) },
#else
    #define MODUCOLLECTIONS_PORT_BUILTIN_MODULE_WEAK_LINKS
#endif /* MICROPY_PY_COLLECTIONS */

#ifdef MICROPYTHON_USING_UERRNO
    #define MODUERRNO_PORT_BUILTIN_MODULE_WEAK_LINKS           { MP_ROM_QSTR(MP_QSTR_errno), MP_ROM_PTR(&mp_module_uerrno ) },
#else
    #define MODUERRNO_PORT_BUILTIN_MODULE_WEAK_LINKS
#endif /* MICROPY_PY_UERRNO */

#ifdef MICROPYTHON_USING_UHASHLIB
    #define MODUHASHLIB_PORT_BUILTIN_MODULE_WEAK_LINKS         { MP_ROM_QSTR(MP_QSTR_hashlib), MP_ROM_PTR(&mp_module_uhashlib ) },
#else
    #define MODUHASHLIB_PORT_BUILTIN_MODULE_WEAK_LINKS
#endif /* MICROPY_PY_UHASHLIB */

#ifdef MICROPYTHON_USING_UHEAPQ
    #define MODUHEAPQ_PORT_BUILTIN_MODULE_WEAK_LINKS           { MP_ROM_QSTR(MP_QSTR_heapq), MP_ROM_PTR(&mp_module_uheapq ) },
#else
    #define MODUHEAPQ_PORT_BUILTIN_MODULE_WEAK_LINKS
#endif /* MICROPY_PY_UHEAPQ */

#if MICROPY_PY_IO
    #define MODUIO_PORT_BUILTIN_MODULE_WEAK_LINKS              { MP_ROM_QSTR(MP_QSTR_io), MP_ROM_PTR(&mp_module_io ) },
#else
    #define MODUIO_PORT_BUILTIN_MODULE_WEAK_LINKS
#endif /* MICROPY_PY_UIO */

#ifdef MICROPYTHON_USING_UJSON
    #define MODUJSON_PORT_BUILTIN_MODULE_WEAK_LINKS            { MP_ROM_QSTR(MP_QSTR_json), MP_ROM_PTR(&mp_module_ujson ) },
#else
    #define MODUJSON_PORT_BUILTIN_MODULE_WEAK_LINKS
#endif /* MICROPY_PY_UJSON */

#ifdef MICROPYTHON_USING_URANDOM
    #define MODURANDOM_PORT_BUILTIN_MODULE_WEAK_LINKS          { MP_ROM_QSTR(MP_QSTR_random), MP_ROM_PTR(&mp_module_urandom ) },
#else
    #define MODURANDOM_PORT_BUILTIN_MODULE_WEAK_LINKS
#endif /* MICROPY_PY_URANDOM */

#ifdef MICROPYTHON_USING_URE
    #define MODURE_PORT_BUILTIN_MODULE_WEAK_LINKS              { MP_ROM_QSTR(MP_QSTR_re), MP_ROM_PTR(&mp_module_ure ) },
#else
    #define MODURE_PORT_BUILTIN_MODULE_WEAK_LINKS
#endif /* MICROPY_PY_URE */

#ifdef MICROPYTHON_USING_USELECT
    #define MODUSELECT_PORT_BUILTIN_MODULE_WEAK_LINKS          { MP_ROM_QSTR(MP_QSTR_select), MP_ROM_PTR(&mp_module_uselect ) },
#else
    #define MODUSELECT_PORT_BUILTIN_MODULE_WEAK_LINKS
#endif /* MICROPY_PY_USELECT */

#if MICROPY_PY_USSL
    #define MODUSSL_PORT_BUILTIN_MODULE_WEAK_LINKS             { MP_ROM_QSTR(MP_QSTR_ssl), MP_ROM_PTR(&mp_module_ussl ) },
#else
    #define MODUSSL_PORT_BUILTIN_MODULE_WEAK_LINKS
#endif /* MICROPY_PY_USSL */

#if MICROPY_PY_STRUCT
    #define MODUSTRUCT_PORT_BUILTIN_MODULE_WEAK_LINKS          { MP_ROM_QSTR(MP_QSTR_struct), MP_ROM_PTR(&mp_module_ustruct ) },
#else
    #define MODUSTRUCT_PORT_BUILTIN_MODULE_WEAK_LINKS
#endif /* MICROPY_PY_USTRUCT */

#ifdef MICROPY_PY_UTIME
    #define MODUTIME_PORT_BUILTIN_MODULES                      { MP_ROM_QSTR(MP_QSTR_utime), MP_ROM_PTR(&mp_module_time) },
    #define MODUTIME_PORT_BUILTIN_MODULE_WEAK_LINKS            { MP_ROM_QSTR(MP_QSTR_time), MP_ROM_PTR(&mp_module_time ) },
#else
    #define SOCKET_PORT_BUILTIN_MODULES
    #define MODUTIME_PORT_BUILTIN_MODULE_WEAK_LINKS
#endif /* MICROPY_PY_UTIME */

#ifdef MICROPYTHON_USING_UZLIB
    #define MODUZLIB_PORT_BUILTIN_MODULE_WEAK_LINKS            { MP_ROM_QSTR(MP_QSTR_zlib), MP_ROM_PTR(&mp_module_uzlib ) },
#else
    #define MODUZLIB_PORT_BUILTIN_MODULE_WEAK_LINKS
#endif /* MICROPY_PY_UZLIB */

#if MICROPY_PY_FFI
    #define MODFFI_PORT_BUILTIN_MODULES                        { MP_ROM_QSTR(MP_QSTR_ffi), MP_ROM_PTR(&mp_module_ffi) },
#else
    #define MODFFI_PORT_BUILTIN_MODULES
#endif

#if MICROPY_PY_NETWORK
    #define MODNETWORK_PORT_BUILTIN_MODULES                    { MP_ROM_QSTR(MP_QSTR_network), MP_ROM_PTR(&mp_module_network) },
#else
    #define MODNETWORK_PORT_BUILTIN_MODULES
#endif

#define USERFUNC_PORT_BUILTIN_MODULES { MP_ROM_QSTR(MP_QSTR_userfunc), MP_ROM_PTR(&mp_module_userfunc) },

// extra built in names to add to the global namespace
#define MICROPY_PORT_BUILTINS \
    { MP_ROM_QSTR(MP_QSTR_exit), MP_ROM_PTR(&machine_soft_reset_obj) }, \
    { MP_ROM_QSTR(MP_QSTR_quit), MP_ROM_PTR(&machine_soft_reset_obj) }, \
    MODUOS_PORT_BUILTINS \

#define MICROPY_PORT_BUILTIN_MODULES \
    { MP_ROM_QSTR(MP_QSTR_machine), MP_ROM_PTR(&mp_module_machine) }, \
    { MP_ROM_QSTR(MP_QSTR_pyb), MP_ROM_PTR(&pyb_module) }, \
    RTTHREAD_PORT_BUILTIN_MODULES \
    MODUOS_PORT_BUILTIN_MODULES \
    SOCKET_PORT_BUILTIN_MODULES \
    MODUTIME_PORT_BUILTIN_MODULES \
    MODFFI_PORT_BUILTIN_MODULES \
    MODNETWORK_PORT_BUILTIN_MODULES \
    USERFUNC_PORT_BUILTIN_MODULES \
    MICROPY_USER_MODULES \

#define MICROPY_PORT_BUILTIN_MODULE_WEAK_LINKS \
    MODUTIME_PORT_BUILTIN_MODULE_WEAK_LINKS \
    MODUOS_PORT_BUILTIN_MODULE_WEAK_LINKS \
    SOCKET_PORT_BUILTIN_MODULE_WEAK_LINKS \
    MODUBINASCII_PORT_BUILTIN_MODULE_WEAK_LINKS \
    MODUCOLLECTIONS_PORT_BUILTIN_MODULE_WEAK_LINKS \
    MODUERRNO_PORT_BUILTIN_MODULE_WEAK_LINKS \
    MODUHASHLIB_PORT_BUILTIN_MODULE_WEAK_LINKS \
    MODUHEAPQ_PORT_BUILTIN_MODULE_WEAK_LINKS \
    MODUIO_PORT_BUILTIN_MODULE_WEAK_LINKS \
    MODUJSON_PORT_BUILTIN_MODULE_WEAK_LINKS \
    MODURANDOM_PORT_BUILTIN_MODULE_WEAK_LINKS \
    MODURE_PORT_BUILTIN_MODULE_WEAK_LINKS \
    MODUSELECT_PORT_BUILTIN_MODULE_WEAK_LINKS \
    MODUSSL_PORT_BUILTIN_MODULE_WEAK_LINKS \
    MODUSTRUCT_PORT_BUILTIN_MODULE_WEAK_LINKS \
    MODUTIME_PORT_BUILTIN_MODULE_WEAK_LINKS \
    MODUZLIB_PORT_BUILTIN_MODULE_WEAK_LINKS \
    MODUSTRUCT_PORT_BUILTIN_MODULE_WEAK_LINKS \

#define MP_RTT_NOT_IMPL_PRINT mp_printf(&mp_plat_print, "Not implement on %s:%ld, Please add for your board!\n", __FILE__, __LINE__)

