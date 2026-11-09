// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Dan Halbert for Adafruit Industries
//
// SPDX-License-Identifier: MIT

// This file contains settings that are common across CircuitPython ports, to make
// sure that the same feature set and settings are used, such as in atmel-samd
// and nordic.

#pragma once

#include <stdint.h>
#include <stdatomic.h>

// This is CircuitPython.
// Always 1: defined in circuitpy_mpconfig.mk
// #define CIRCUITPY (1)

#ifndef MP_SSIZE_MAX
#define MP_SSIZE_MAX (0x7fffffff)
#endif

// REPR_C encodes qstrs, 31-bit ints, and 30-bit floats in a single 32-bit word.
#ifndef MICROPY_OBJ_REPR
#define MICROPY_OBJ_REPR            (MICROPY_OBJ_REPR_C)
#endif

// options to control how MicroPython is built
// TODO(tannewt): Reduce this number if we want the REPL to function under 512
// free bytes.
// #define MICROPY_ALLOC_PARSE_RULE_INIT (64)

// These critical-section macros are used only a few places in MicroPython, but
// we need to provide actual implementations.
extern void common_hal_mcu_disable_interrupts(void);
extern void common_hal_mcu_enable_interrupts(void);
#define MICROPY_BEGIN_ATOMIC_SECTION() (common_hal_mcu_disable_interrupts(), 0)
#define MICROPY_END_ATOMIC_SECTION(state) ((void)state, common_hal_mcu_enable_interrupts())

// MicroPython-only options not used by CircuitPython, but present in various files
// inherited from MicroPython, especially in extmod/
#define MICROPY_ENABLE_DYNRUNTIME        (0)
#define MICROPY_HW_ENABLE_USB            (0)
#define MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE (0)
#define MICROPY_PY_BLUETOOTH             (0)
#define MICROPY_PY_LWIP_SLIP             (0)
#define MICROPY_PY_OS_DUPTERM            (0)
#define MICROPY_PYEXEC_COMPILE_ONLY      (0)
#define MICROPY_ROM_TEXT_COMPRESSION     (0)
#ifndef MICROPY_VFS_LFS1
#define MICROPY_VFS_LFS1                 (0)
#endif
#ifndef MICROPY_VFS_LFS2
#define MICROPY_VFS_LFS2                 (0)
#endif

// CIRCUITPY-CHANGE: Compile-time choice of the CIRCUITPY filesystem. When 1,
// the supervisor mounts littlefs instead of FAT on the block device returned
// by supervisor_flash_*. Ports set this (and MICROPY_VFS_LFS2) from their
// board configuration; the default remains FAT.
#ifndef CIRCUITPY_FILESYSTEM_LITTLEFS
#define CIRCUITPY_FILESYSTEM_LITTLEFS    (0)
#endif

// Always turn on exit code handling
#define MICROPY_PYEXEC_ENABLE_EXIT_CODE_HANDLING (1)

#ifndef MICROPY_GCREGS_SETJMP
#define MICROPY_GCREGS_SETJMP            (0)
#endif

// Sorted alphabetically for easy finding.
//
// default is 128; consider raising to reduce fragmentation.
#define MICROPY_ALLOC_PARSE_CHUNK_INIT   (16)
// default is 512. Longest path in .py bundle as of June 6th, 2023 is 73 characters.
#define MICROPY_ALLOC_PATH_MAX           (96)
#define MICROPY_CAN_OVERRIDE_BUILTINS    (1)
#define MICROPY_COMP_CONST               (1)
#define MICROPY_COMP_DOUBLE_TUPLE_ASSIGN (1)
#define MICROPY_COMP_MODULE_CONST        (1)
#define MICROPY_COMP_TRIPLE_TUPLE_ASSIGN (0)
#define MICROPY_DEBUG_PRINTERS           (0)
#define MICROPY_EMIT_INLINE_THUMB        (CIRCUITPY_ENABLE_MPY_NATIVE)
#define MICROPY_EMIT_THUMB               (CIRCUITPY_ENABLE_MPY_NATIVE)
#define MICROPY_EMIT_X64                 (0)
#define MICROPY_ENABLE_DOC_STRING        (0)
#define MICROPY_ENABLE_FINALISER         (1)
#define MICROPY_ENABLE_SELECTIVE_COLLECT (1)
#define MICROPY_ENABLE_GC                (1)
#define MICROPY_ENABLE_PYSTACK           (1)
// mbedtls allocates through m_tracked_calloc/m_tracked_free. hashlib-only ports need
// them too as of mbedtls 4.x: hashlib uses mbedtls_md_*(), which allocates on the heap.
#define MICROPY_TRACKED_ALLOC            (CIRCUITPY_SSL_MBEDTLS || CIRCUITPY_HASHLIB_MBEDTLS_ONLY)
#define MICROPY_ENABLE_SOURCE_LINE       (1)
#define MICROPY_EPOCH_IS_1970            (1)
#define MICROPY_ERROR_REPORTING          (CIRCUITPY_FULL_BUILD ? MICROPY_ERROR_REPORTING_NORMAL : MICROPY_ERROR_REPORTING_TERSE)
#define MICROPY_FLOAT_HIGH_QUALITY_HASH  (0)
#define MICROPY_FLOAT_IMPL               (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_GC_ALLOC_THRESHOLD       (0)
#define MICROPY_GC_SPLIT_HEAP            (1)
#define MICROPY_GC_SPLIT_HEAP_AUTO       (1)
#define MP_PLAT_ALLOC_HEAP(size) port_malloc(size, false)
#define MP_PLAT_FREE_HEAP(ptr) port_free(ptr)
#include "supervisor/port_heap.h"
#define MICROPY_HELPER_LEXER_UNIX        (0)
#define MICROPY_HELPER_REPL              (1)
#define MICROPY_KBD_EXCEPTION            (1)
#define MICROPY_MEM_STATS                (0)
#define MICROPY_MODULE_BUILTIN_INIT      (1)
#define MICROPY_MODULE_BUILTIN_SUBPACKAGES (1)
#define MICROPY_OPT_COMPUTED_GOTO        (1)
#define MICROPY_OPT_COMPUTED_GOTO_SAVE_SPACE (CIRCUITPY_COMPUTED_GOTO_SAVE_SPACE)
#define MICROPY_OPT_LOAD_ATTR_FAST_PATH  (CIRCUITPY_OPT_LOAD_ATTR_FAST_PATH)
#define MICROPY_OPT_MAP_LOOKUP_CACHE  (CIRCUITPY_OPT_MAP_LOOKUP_CACHE)
#define MICROPY_OPT_MPZ_BITWISE          (0)
#define MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE (CIRCUITPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE)
#define MICROPY_PERSISTENT_CODE_LOAD     (1)
#define MICROPY_PERSISTENT_CODE_LOAD_NATIVE (CIRCUITPY_LOAD_NATIVE || CIRCUITPY_ENABLE_MPY_NATIVE)

#define MICROPY_PY_ARRAY                 (CIRCUITPY_ARRAY)
#define MICROPY_PY_ARRAY_SLICE_ASSIGN    (1)
#define MICROPY_PY_ATTRTUPLE             (1)
#define MICROPY_PY_BUILTINS_BYTEARRAY    (1)
#define MICROPY_PY_BUILTINS_BYTES_HEX    (1)
#define MICROPY_PY_BUILTINS_ENUMERATE    (1)
#define MICROPY_PY_BUILTINS_FILTER       (1)
#define MICROPY_PY_BUILTINS_HELP         (1)
#define MICROPY_PY_BUILTINS_HELP_MODULES (1)
#define MICROPY_PY_BUILTINS_INPUT        (1)
#define MICROPY_PY_BUILTINS_MEMORYVIEW   (1)
#define MICROPY_PY_BUILTINS_MIN_MAX      (1)
#define MICROPY_PY_BUILTINS_PROPERTY     (1)
#define MICROPY_PY_BUILTINS_REVERSED     (1)
#define MICROPY_PY_BUILTINS_ROUND_INT    (1)
#define MICROPY_PY_BUILTINS_SET          (1)
#define MICROPY_PY_BUILTINS_SLICE        (1)
#define MICROPY_PY_BUILTINS_SLICE_ATTRS  (1)
#define MICROPY_PY_BUILTINS_SLICE_INDICES (1)
#define MICROPY_PY_BUILTINS_STR_UNICODE  (1)

#define MICROPY_PY_BINASCII             (CIRCUITPY_BINASCII)
#define MICROPY_PY_BINASCII_CRC32       (CIRCUITPY_BINASCII && CIRCUITPY_ZLIB)
#define MICROPY_PY_CMATH                 (0)
#define MICROPY_PY_COLLECTIONS           (CIRCUITPY_COLLECTIONS)
#define MICROPY_PY_DESCRIPTORS           (1)
// In extmod
#define MICROPY_PY_ERRNO                (CIRCUITPY_ERRNO)
// Uses about 80 bytes.
#define MICROPY_PY_ERRNO_ERRORCODE      (CIRCUITPY_ERRNO)
#define MICROPY_PY_GC                    (1)
// Supplanted by shared-bindings/math
#define MICROPY_PY_IO                    (CIRCUITPY_IO)
#define MICROPY_PY_IO_IOBASE             (CIRCUITPY_IO_IOBASE)
// In extmod
#define MICROPY_PY_JSON                 (CIRCUITPY_JSON)
#define MICROPY_PY_MATH                  (0)
#define MICROPY_PY_MICROPYTHON_MEM_INFO  (0)
// Supplanted by shared-bindings/random
#define MICROPY_PY_RANDOM               (0)
#define MICROPY_PY_RANDOM_EXTRA_FUNCS   (0)
#define MICROPY_PY_RE                   (CIRCUITPY_RE)
// Supplanted by shared-bindings/struct
#define MICROPY_PY_STRUCT                (0)
#define MICROPY_PY_STRUCT_UNSAFE_TYPECODES (0)
#define MICROPY_PY_SYS                   (CIRCUITPY_SYS)
#define MICROPY_PY_SYS_MAXSIZE           (1)
#define MICROPY_PY_SYS_STDFILES          (1)
#define MICROPY_PY_UCTYPES               (0)
#define MICROPY_PY___FILE__              (1)

#if CIRCUITPY_FULL_BUILD
#ifndef MICROPY_QSTR_BYTES_IN_HASH
#define MICROPY_QSTR_BYTES_IN_HASH       (1)
#endif
#else
#define MICROPY_QSTR_BYTES_IN_HASH       (0)
#endif
#define MICROPY_REPL_AUTO_INDENT         (1)
#define MICROPY_REPL_EVENT_DRIVEN        (0)
#define MICROPY_STACK_CHECK              (1)
#define MICROPY_STREAMS_NON_BLOCK        (1)
#ifndef MICROPY_USE_INTERNAL_PRINTF
#define MICROPY_USE_INTERNAL_PRINTF      (1)
#endif

// fatfs configuration used in ffconf.h
//
// 1 = SFN/ANSI 437=LFN/U.S.(OEM)
#define MICROPY_FATFS_ENABLE_LFN      (1)
// Code page is ignored because unicode is enabled.
// Don't use parens on the value below because it gets combined with a prefix in
// the preprocessor.
#define MICROPY_FATFS_LFN_CODE_PAGE   437
#define MICROPY_FATFS_USE_LABEL       (1)
#define MICROPY_FATFS_RPATH           (2)
#define MICROPY_FATFS_MULTI_PARTITION (1)
#define MICROPY_FATFS_LFN_UNICODE      2  // UTF-8

// Only enable this if you really need it. It allocates a byte cache of this size.
// #define MICROPY_FATFS_MAX_SS           (4096)

#define FILESYSTEM_BLOCK_SIZE       (512)

#define MICROPY_VFS                 (1)
// CIRCUITPY-CHANGE: CircuitPython's flash/SD block devices are native
// (supervisor/shared/flash.c sets MP_BLOCKDEV_FLAG_NATIVE).
#define MICROPY_VFS_BLOCKDEV_NATIVE (1)
#define MICROPY_VFS_FAT             (MICROPY_VFS)
#define MICROPY_READER_VFS          (MICROPY_VFS)

// type definitions for the specific machine

#define BYTES_PER_WORD (4)

// Bit 0 of a code pointer selects the Thumb instruction set.
#if defined(__thumb__)
#define MICROPY_MAKE_POINTER_CALLABLE(p) ((void *)((mp_uint_t)(p) | 1))
#else
#define MICROPY_MAKE_POINTER_CALLABLE(p) ((void *)(p))
#endif

// Track stack usage. Expose results via ustack module.
#define MICROPY_MAX_STACK_USAGE       (0)

typedef long mp_off_t;

#define MP_PLAT_PRINT_STRN(str, len) mp_hal_stdout_tx_strn_cooked(str, len)

#define mp_type_fileio mp_type_vfs_fat_fileio
#define mp_type_textio mp_type_vfs_fat_textio

#define mp_builtin_open_obj mp_vfs_open_obj


// extra built in names to add to the global namespace
// Not indented so as not to confused the editor.
#define MICROPY_PORT_BUILTINS \
    { MP_OBJ_NEW_QSTR(MP_QSTR_help), (mp_obj_t)&mp_builtin_help_obj },      \
    { MP_OBJ_NEW_QSTR(MP_QSTR_input), (mp_obj_t)&mp_builtin_input_obj }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_open), (mp_obj_t)&mp_builtin_open_obj },   \

//////////////////////////////////////////////////////////////////////////////////////////////////
// board-specific definitions, which control and may override definitions below.
#include "mpconfigboard.h"

// Turning off FULL_BUILD removes some functionality to reduce flash size on tiny SAMD21s
#define MICROPY_BUILTIN_METHOD_CHECK_SELF_ARG (CIRCUITPY_FULL_BUILD)

#ifndef MICROPY_CPYTHON_COMPAT
#define MICROPY_CPYTHON_COMPAT                (CIRCUITPY_FULL_BUILD)
#endif

#ifndef MICROPY_CPYTHON_EXCEPTION_CHAIN
#define MICROPY_CPYTHON_EXCEPTION_CHAIN       (CIRCUITPY_FULL_BUILD)
#endif

// Allow %e/%f/%g in mp_printf().
#ifndef MICROPY_PRINTF_FLOAT
#define MICROPY_PRINTF_FLOAT                  (CIRCUITPY_FULL_BUILD && MICROPY_PY_BUILTINS_FLOAT)
#endif

#define MICROPY_PY_BUILTINS_POW3              (CIRCUITPY_BUILTINS_POW3)
#define MICROPY_PY_FSTRINGS                   (1)
#define MICROPY_MODULE_WEAK_LINKS             (0)
#define MICROPY_PY_ALL_SPECIAL_METHODS        (CIRCUITPY_FULL_BUILD)

#ifndef MICROPY_PY_BUILTINS_COMPLEX
#define MICROPY_PY_BUILTINS_COMPLEX           (CIRCUITPY_FULL_BUILD)
#endif

#define MICROPY_PY_BUILTINS_FROZENSET         (CIRCUITPY_FULL_BUILD)

#ifndef MICROPY_PY_BUILTINS_NOTIMPLEMENTED
#define MICROPY_PY_BUILTINS_NOTIMPLEMENTED    (CIRCUITPY_FULL_BUILD)
#endif

#define MICROPY_PY_BUILTINS_STR_CENTER        (CIRCUITPY_FULL_BUILD)
#define MICROPY_PY_BUILTINS_STR_PARTITION     (CIRCUITPY_FULL_BUILD)
#define MICROPY_PY_BUILTINS_STR_SPLITLINES    (CIRCUITPY_FULL_BUILD)

#ifndef MICROPY_PY_COLLECTIONS_ORDEREDDICT
#define MICROPY_PY_COLLECTIONS_ORDEREDDICT    (CIRCUITPY_FULL_BUILD)
#endif

#ifndef MICROPY_PY_COLLECTIONS_DEQUE
#define MICROPY_PY_COLLECTIONS_DEQUE          (CIRCUITPY_FULL_BUILD)
#define MICROPY_PY_COLLECTIONS_DEQUE_ITER     (CIRCUITPY_FULL_BUILD)
#define MICROPY_PY_COLLECTIONS_DEQUE_SUBSCR   (CIRCUITPY_FULL_BUILD)
#endif

#ifndef MICROPY_PY_DELATTR_SETATTR
#define MICROPY_PY_DELATTR_SETATTR       (CIRCUITPY_FULL_BUILD)
#endif

#ifndef MICROPY_PY_DOUBLE_TYPECODE
#define MICROPY_PY_DOUBLE_TYPECODE       (CIRCUITPY_FULL_BUILD ? 1 : 0)
#endif

#ifndef MICROPY_PY_FUNCTION_ATTRS
#define MICROPY_PY_FUNCTION_ATTRS            (CIRCUITPY_FULL_BUILD)
#endif

#ifndef MICROPY_PY_REVERSE_SPECIAL_METHODS
#define MICROPY_PY_REVERSE_SPECIAL_METHODS   (CIRCUITPY_FULL_BUILD)
#endif

#define MICROPY_PY_RE_MATCH_GROUPS           (CIRCUITPY_RE)
#define MICROPY_PY_RE_MATCH_SPAN_START_END   (CIRCUITPY_RE)
#define MICROPY_PY_RE_SUB                    (CIRCUITPY_RE)

#define CIRCUITPY_MICROPYTHON_ADVANCED        (0)

#ifndef MICROPY_FATFS_EXFAT
#define MICROPY_FATFS_EXFAT           (CIRCUITPY_FULL_BUILD)
#endif

#ifndef MICROPY_FATFS_MKFS_FAT32
#define MICROPY_FATFS_MKFS_FAT32           (CIRCUITPY_FULL_BUILD)
#endif

// LONGINT_IMPL_xxx are defined in the Makefile.
//
#ifdef LONGINT_IMPL_NONE
#define MICROPY_LONGINT_IMPL (MICROPY_LONGINT_IMPL_NONE)
#endif

#ifdef LONGINT_IMPL_MPZ
#define MICROPY_LONGINT_IMPL (MICROPY_LONGINT_IMPL_MPZ)
#endif

#ifdef LONGINT_IMPL_LONGLONG
#define MICROPY_LONGINT_IMPL (MICROPY_LONGINT_IMPL_LONGLONG)
#endif

#ifndef MICROPY_PY_REVERSE_SPECIAL_METHODS
#define MICROPY_PY_REVERSE_SPECIAL_METHODS    (CIRCUITPY_ULAB || CIRCUITPY_FULL_BUILD)
#endif

#if !defined(__ZEPHYR__) && INTERNAL_FLASH_FILESYSTEM == 0 && QSPI_FLASH_FILESYSTEM == 0 && SPI_FLASH_FILESYSTEM == 0 && !DISABLE_FILESYSTEM
#error No *_FLASH_FILESYSTEM set!
#endif

// Default board buses.

#ifndef CIRCUITPY_MUTABLE_BOARD
#define CIRCUITPY_MUTABLE_BOARD (0)
#endif

#ifndef CIRCUITPY_BOARD_I2C
#if defined(DEFAULT_I2C_BUS_SCL) && defined(DEFAULT_I2C_BUS_SDA)
#define CIRCUITPY_BOARD_I2C     (1)
#define CIRCUITPY_BOARD_I2C_PIN {{.scl = DEFAULT_I2C_BUS_SCL, .sda = DEFAULT_I2C_BUS_SDA}}
#ifndef CIRCUITPY_BOARD_I2C_SPEED
#define CIRCUITPY_BOARD_I2C_SPEED (100000)
#endif
#else
#define CIRCUITPY_BOARD_I2C     (0)
#endif
#endif

#ifndef CIRCUITPY_BOARD_SPI
#if defined(DEFAULT_SPI_BUS_SCK) && defined(DEFAULT_SPI_BUS_MOSI) && defined(DEFAULT_SPI_BUS_MISO)
#define CIRCUITPY_BOARD_SPI     (1)
#define CIRCUITPY_BOARD_SPI_PIN {{.clock = DEFAULT_SPI_BUS_SCK, .mosi = DEFAULT_SPI_BUS_MOSI, .miso = DEFAULT_SPI_BUS_MISO}}
#else
#define CIRCUITPY_BOARD_SPI     (0)
#endif
#endif

#ifndef CIRCUITPY_BOARD_UART
#if defined(DEFAULT_UART_BUS_TX) && defined(DEFAULT_UART_BUS_RX)
#define CIRCUITPY_BOARD_UART        (1)
#define CIRCUITPY_BOARD_UART_PIN    {{.tx = DEFAULT_UART_BUS_TX, .rx = DEFAULT_UART_BUS_RX}}
#else
#define CIRCUITPY_BOARD_UART        (0)
#endif
#endif


// For easy debugging printf's.
#define PLAT_PRINTF(...) mp_printf(&mp_plat_print, __VA_ARGS__)

#if MICROPY_PY_ASYNC_AWAIT && !CIRCUITPY_TRACEBACK
#error CIRCUITPY_ASYNCIO requires CIRCUITPY_TRACEBACK
#endif

#if defined(CIRCUITPY_CONSOLE_UART_RX) || defined(CIRCUITPY_CONSOLE_UART_TX)
#if !(defined(CIRCUITPY_CONSOLE_UART_RX) && defined(CIRCUITPY_CONSOLE_UART_TX))
#error Both CIRCUITPY_CONSOLE_UART_RX and CIRCUITPY_CONSOLE_UART_TX must be defined if one is defined.
#endif
#define CIRCUITPY_CONSOLE_UART (1)
#ifndef CIRCUITPY_CONSOLE_UART_BAUDRATE
#define CIRCUITPY_CONSOLE_UART_BAUDRATE (115200)
#endif
#if !defined(CIRCUITPY_CONSOLE_UART_PRINTF)
#define CIRCUITPY_CONSOLE_UART_PRINTF(...) mp_printf(&console_uart_print, __VA_ARGS__)
#endif
#if !defined(CIRCUITPY_CONSOLE_UART_HEXDUMP)
#define CIRCUITPY_CONSOLE_UART_HEXDUMP(pfx, buf, len) print_hexdump(&console_uart_print, pfx, (const uint8_t *)buf, len)
#endif
#if !defined(CIRCUITPY_CONSOLE_UART_TIMESTAMP)
#define CIRCUITPY_CONSOLE_UART_TIMESTAMP (0)
#endif
#else
#define CIRCUITPY_CONSOLE_UART (0)
#define CIRCUITPY_CONSOLE_UART_PRINTF(...) (void)0
#define CIRCUITPY_CONSOLE_UART_HEXDUMP(...) (void)0
#endif

// Normally set from CIRCUITPY_BUSIO_SPI_ASYNC in the .mk files.
#ifndef CIRCUITPY_BUSIO_SPI_ASYNC
#define CIRCUITPY_BUSIO_SPI_ASYNC (0)
#endif

// These CIRCUITPY_xxx values should all be defined in the *.mk files as being on or off.
// So if any are not defined in *.mk, they'll throw an error here.

#if CIRCUITPY_DISPLAYIO
#ifndef CIRCUITPY_DISPLAY_LIMIT
#define CIRCUITPY_DISPLAY_LIMIT (1)
#endif

// Normally set from CIRCUITPY_DISPLAY_AREA_BUFFER_SIZE in the .mk files.
#ifndef CIRCUITPY_DISPLAY_AREA_BUFFER_SIZE
#define CIRCUITPY_DISPLAY_AREA_BUFFER_SIZE (512)
#endif

#else
#define CIRCUITPY_DISPLAY_LIMIT (0)
#endif

// This is not a top-level module; it's microcontroller.nvm.
#if CIRCUITPY_NVM
extern const struct _mp_obj_module_t nvm_module;
#endif

#ifndef ULAB_SUPPORTS_COMPLEX
#define ULAB_SUPPORTS_COMPLEX (0)
#endif

// The random module is fairly large.
#ifndef ULAB_NUMPY_HAS_RANDOM_MODULE
#define ULAB_NUMPY_HAS_RANDOM_MODULE (0)
#endif

// The scipy.integrate module is about 6 kB.
#ifndef ULAB_SCIPY_HAS_INTEGRATE_MODULE
#define ULAB_SCIPY_HAS_INTEGRATE_MODULE (0)
#endif

// The ndarray modulo operators are about 4 kB.
#ifndef NDARRAY_HAS_BINARY_OP_MODULO
#define NDARRAY_HAS_BINARY_OP_MODULO (0)
#endif
#ifndef NDARRAY_HAS_INPLACE_MODULO
#define NDARRAY_HAS_INPLACE_MODULO (0)
#endif

#if CIRCUITPY_ULAB
// ulab requires reverse special methods
#if defined(MICROPY_PY_REVERSE_SPECIAL_METHODS) && !MICROPY_PY_REVERSE_SPECIAL_METHODS
#error "ulab requires MICROPY_PY_REVERSE_SPECIAL_METHODS"
#endif
#endif

// Define certain native modules with weak links so they can be replaced with Python
// implementations. This list may grow over time.

#define MICROPY_PORT_BUILTIN_MODULE_WEAK_LINKS

// Native modules that are weak links can be accessed directly
// by prepending their name with an underscore. This list should correspond to
// MICROPY_PORT_BUILTIN_MODULE_WEAK_LINKS, assuming you want the native modules
// to be accessible when overridden.

#define MICROPY_PORT_BUILTIN_MODULE_ALT_NAMES

// This is an inclusive list that should correspond to the CIRCUITPY_XXX list above,
// including dependencies.
// Some of these definitions will be blank depending on what is turned on and off.
// Some are omitted because they're in MICROPY_PORT_BUILTIN_MODULE_WEAK_LINKS above.

#define MICROPY_PORT_BUILTIN_MODULES_STRONG_LINKS

// If weak links are enabled, just include strong links in the main list of modules,
// and also include the underscore alternate names.
#if MICROPY_MODULE_WEAK_LINKS
#define MICROPY_PORT_BUILTIN_MODULES \
    MICROPY_PORT_BUILTIN_MODULES_STRONG_LINKS \
    MICROPY_PORT_BUILTIN_MODULE_ALT_NAMES
#else
// If weak links are disabled, included both strong and potentially weak lines
#define MICROPY_PORT_BUILTIN_MODULES \
    MICROPY_PORT_BUILTIN_MODULES_STRONG_LINKS \
    MICROPY_PORT_BUILTIN_MODULE_WEAK_LINKS
#endif

// We need to provide a declaration/definition of alloca()
#include <alloca.h>

#define MP_STATE_PORT MP_STATE_VM

void background_callback_run_all(void);
#define RUN_BACKGROUND_TASKS (background_callback_run_all())

#define MICROPY_VM_HOOK_LOOP RUN_BACKGROUND_TASKS;
#define MICROPY_VM_HOOK_RETURN RUN_BACKGROUND_TASKS;
#define MICROPY_INTERNAL_EVENT_HOOK (RUN_BACKGROUND_TASKS)

// CIRCUITPY_AUTORELOAD_DELAY_MS = 0 will completely disable autoreload.
#ifndef CIRCUITPY_AUTORELOAD_DELAY_MS
#define CIRCUITPY_AUTORELOAD_DELAY_MS 750
#endif

#ifndef CIRCUITPY_FILESYSTEM_FLUSH_INTERVAL_MS
#define CIRCUITPY_FILESYSTEM_FLUSH_INTERVAL_MS 1000
#endif

#ifndef CIRCUITPY_PYSTACK_SIZE
#define CIRCUITPY_PYSTACK_SIZE 2048
#endif

// The VM heap starts at this size and doubles in size as needed until it runs
// out of memory in the outer heap. Once it can't double, it'll then grow into
// the largest contiguous free area.
#ifndef CIRCUITPY_HEAP_START_SIZE
#define CIRCUITPY_HEAP_START_SIZE (8 * 1024)
#endif

// How much of the c stack we leave to ensure we can process exceptions.
#ifndef CIRCUITPY_EXCEPTION_STACK_SIZE
#define CIRCUITPY_EXCEPTION_STACK_SIZE 1024
#endif

// Wait this long before sleeping immediately after startup, to see if we are connected via USB or BLE.
#ifndef CIRCUITPY_WORKFLOW_CONNECTION_SLEEP_DELAY
#define CIRCUITPY_WORKFLOW_CONNECTION_SLEEP_DELAY 5
#endif

#ifndef CIRCUITPY_PROCESSOR_COUNT
#define CIRCUITPY_PROCESSOR_COUNT (1)
#endif

#ifndef CIRCUITPY_STATUS_LED_POWER_INVERTED
#define CIRCUITPY_STATUS_LED_POWER_INVERTED (0)
#endif

// Byte order of the status NeoPixel. Most WS2812s take GRB. Boards whose LED takes RGB set
// this to STATUS_NEOPIXEL_ORDER_RGB, from supervisor/shared/status_leds.h.
#ifndef MICROPY_HW_NEOPIXEL_ORDER
#define MICROPY_HW_NEOPIXEL_ORDER (STATUS_NEOPIXEL_ORDER_GRB)
#endif

#define CIRCUITPY_BOOT_OUTPUT_FILE "/boot_out.txt"

#ifndef CIRCUITPY_BOOT_COUNTER
#define CIRCUITPY_BOOT_COUNTER 0
#endif

#if !defined(CIRCUITPY_INTERNAL_NVM_SIZE) && CIRCUITPY_BOOT_COUNTER != 0
#error "boot counter requires CIRCUITPY_NVM enabled"
#endif

#ifndef CIRCUITPY_VERBOSE_BLE
#define CIRCUITPY_VERBOSE_BLE 0
#endif

// Display the Blinka logo in the REPL on displayio displays.
#ifndef CIRCUITPY_REPL_LOGO
#define CIRCUITPY_REPL_LOGO (1)
#endif

// USB settings

#ifndef CIRCUITPY_SDCARD_USB
#if CIRCUITPY_USB_DEVICE
#define CIRCUITPY_SDCARD_USB (CIRCUITPY_SDCARDIO && CIRCUITPY_USB_MSC)
// Default value of CIRCUITPY_SDCARD_USB in settings.toml.
#ifndef CIRCUITPY_SDCARD_USB_DEFAULT
// True for most boards.
#define CIRCUITPY_SDCARD_USB_DEFAULT (true)
#endif
#else
#define CIRCUITPY_SDCARD_USB (0)
#endif
#endif

#if CIRCUITPY_SDCARD_USB && !(CIRCUITPY_SDCARDIO)
#error CIRCUITPY_SDCARD_USB requires CIRCUITPY_SDCARDIO
#endif

// Debug level for TinyUSB. Only outputs over debug UART so it doesn't cause
// additional USB logging.
#ifndef CIRCUITPY_DEBUG_TINYUSB
#define CIRCUITPY_DEBUG_TINYUSB 0
#endif

#ifndef CIRCUITPY_USB_DEVICE_INSTANCE
#define CIRCUITPY_USB_DEVICE_INSTANCE 0
#endif

#ifndef CIRCUITPY_USB_DEVICE_HIGH_SPEED
#define CIRCUITPY_USB_DEVICE_HIGH_SPEED 0
#endif

#ifndef CIRCUITPY_USB_HOST_INSTANCE
#define CIRCUITPY_USB_HOST_INSTANCE -1
#endif

#ifndef CIRCUITPY_USB_HOST_HIGH_SPEED
#define CIRCUITPY_USB_HOST_HIGH_SPEED 0
#endif

// usb_host_bulk takes a usb.core.Device and uses TinyUSB host calls.
#if CIRCUITPY_USB_HOST_BULK && !(CIRCUITPY_USB_HOST && CIRCUITPY_PYUSB)
#error CIRCUITPY_USB_HOST_BULK requires CIRCUITPY_USB_HOST and CIRCUITPY_PYUSB
#endif

// If the port requires certain USB endpoint numbers, define these in mpconfigport.h.

#ifndef USB_CDC_EP_NUM_NOTIFICATION
#define USB_CDC_EP_NUM_NOTIFICATION (0)
#endif

#ifndef USB_CDC_EP_NUM_DATA_OUT
#define USB_CDC_EP_NUM_DATA_OUT (0)
#endif

#ifndef USB_CDC_EP_NUM_DATA_IN
#define USB_CDC_EP_NUM_DATA_IN (0)
#endif

#ifndef USB_CDC2_EP_NUM_NOTIFICATION
#define USB_CDC2_EP_NUM_NOTIFICATION (0)
#endif

#ifndef USB_CDC2_EP_NUM_DATA_OUT
#define USB_CDC2_EP_NUM_DATA_OUT (0)
#endif

#ifndef USB_CDC2_EP_NUM_DATA_IN
#define USB_CDC2_EP_NUM_DATA_IN (0)
#endif

#ifndef USB_MSC_EP_NUM_OUT
#define USB_MSC_EP_NUM_OUT (0)
#endif

#ifndef USB_MSC_EP_NUM_IN
#define USB_MSC_EP_NUM_IN (0)
#endif

#ifndef USB_HID_EP_NUM_OUT
#define USB_HID_EP_NUM_OUT (0)
#endif

#ifndef USB_HID_EP_NUM_IN
#define USB_HID_EP_NUM_IN (0)
#endif

// The most complicated device currently known of is the head and eye tracker, which requires 5
// report ids.
// https://usb.org/sites/default/files/hutrr74_-_usage_page_for_head_and_eye_trackers_0.pdf
// The default descriptors only use 1, so that is the minimum.
#ifndef CIRCUITPY_USB_HID_MAX_REPORT_IDS_PER_DESCRIPTOR
#define CIRCUITPY_USB_HID_MAX_REPORT_IDS_PER_DESCRIPTOR (6)
#elif CIRCUITPY_USB_HID_MAX_REPORT_IDS_PER_DESCRIPTOR < 1
#error "CIRCUITPY_USB_HID_MAX_REPORT_IDS_PER_DESCRIPTOR must be at least 1"
#endif

#ifndef USB_MIDI_EP_NUM_OUT
#define USB_MIDI_EP_NUM_OUT (0)
#endif

#ifndef USB_MIDI_EP_NUM_IN
#define USB_MIDI_EP_NUM_IN (0)
#endif

#ifndef MICROPY_WRAP_MP_MAP_LOOKUP
#define MICROPY_WRAP_MP_MAP_LOOKUP PLACE_IN_ITCM
#endif

#ifndef MICROPY_WRAP_MP_BINARY_OP
#define MICROPY_WRAP_MP_BINARY_OP PLACE_IN_ITCM
#endif

#ifndef MICROPY_WRAP_MP_EXECUTE_BYTECODE
#define MICROPY_WRAP_MP_EXECUTE_BYTECODE PLACE_IN_ITCM
#endif

#ifndef MICROPY_WRAP_MP_LOAD_GLOBAL
#define MICROPY_WRAP_MP_LOAD_GLOBAL PLACE_IN_ITCM
#endif

#ifndef MICROPY_WRAP_MP_LOAD_NAME
#define MICROPY_WRAP_MP_LOAD_NAME PLACE_IN_ITCM
#endif

#ifndef MICROPY_WRAP_MP_OBJ_GET_TYPE
#define MICROPY_WRAP_MP_OBJ_GET_TYPE PLACE_IN_ITCM
#endif

#ifndef CIRCUITPY_DIGITALIO_HAVE_INPUT_ONLY
#define CIRCUITPY_DIGITALIO_HAVE_INPUT_ONLY (0)
#endif

#ifndef CIRCUITPY_DIGITALIO_HAVE_INVALID_PULL
#define CIRCUITPY_DIGITALIO_HAVE_INVALID_PULL (0)
#endif

#ifndef CIRCUITPY_DIGITALIO_HAVE_INVALID_DRIVE_MODE
#define CIRCUITPY_DIGITALIO_HAVE_INVALID_DRIVE_MODE (0)
#endif

// Align the internal sector buffer. Useful when it is passed into TinyUSB for
// loads.
#ifndef MICROPY_FATFS_WINDOW_ALIGNMENT
#define MICROPY_FATFS_WINDOW_ALIGNMENT 64 // Espressif is strictest
#endif

#define FF_FS_CASE_INSENSITIVE_COMPARISON_ASCII_ONLY (1)

#define FF_FS_MAKE_VOLID (1)

#define MICROPY_PY_OPTIMIZE_PROPERTY_FLASH_SIZE (CIRCUITPY_OPTIMIZE_PROPERTY_FLASH_SIZE)

// Enable compiler functionality.
#define MICROPY_ENABLE_COMPILER (1)
#define MICROPY_PY_BUILTINS_COMPILE (1)

#ifndef CIRCUITPY_MIN_GCC_VERSION
#define CIRCUITPY_MIN_GCC_VERSION 14
#endif

#ifndef CIRCUITPY_SAVES_PARTITION_SIZE
#define CIRCUITPY_SAVES_PARTITION_SIZE 0
#endif

// Boards that have a boot button connected to a GPIO pin should set
// CIRCUITPY_BOOT_BUTTON_NO_GPIO to 1.
#ifndef CIRCUITPY_BOOT_BUTTON_NO_GPIO
#define CIRCUITPY_BOOT_BUTTON_NO_GPIO (0)
#endif
#if defined(CIRCUITPY_BOOT_BUTTON) && CIRCUITPY_BOOT_BUTTON_NO_GPIO
#error "CIRCUITPY_BOOT_BUTTON and CIRCUITPY_BOOT_BUTTON_NO_GPIO are mutually exclusive"
#endif

// Boards that define CIRCUITPY_BOOT_BUTTON can set
// CIRCUITPY_BOOT_BUTTON_ACTIVE_HIGH to 1 when the button pin reads high while
// pressed. The default is active-low (the pin reads low while pressed).
#ifndef CIRCUITPY_BOOT_BUTTON_ACTIVE_HIGH
#define CIRCUITPY_BOOT_BUTTON_ACTIVE_HIGH (0)
#endif

#if defined(__GNUC__) && !defined(__ZEPHYR__)
#if __GNUC__ < CIRCUITPY_MIN_GCC_VERSION
// (the 3 level scheme here is required to get expansion & stringization
// correct)
#define DO_PRAGMA(x) _Pragma(#x)
#define DO_ERROR_HELPER(x) DO_PRAGMA(GCC error #x)
#define DO_ERROR(x) DO_ERROR_HELPER(Minimum GCC version x \
    -- older versions are known to miscompile CircuitPython)
DO_ERROR(CIRCUITPY_MIN_GCC_VERSION);
#endif
#endif
