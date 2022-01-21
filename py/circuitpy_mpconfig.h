/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Dan Halbert for Adafruit Industries
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

// This file contains settings that are common across CircuitPython ports, to make
// sure that the same feature set and settings are used, such as in atmel-samd
// and nrf.

#ifndef __INCLUDED_MPCONFIG_CIRCUITPY_H
#define __INCLUDED_MPCONFIG_CIRCUITPY_H

#include <stdint.h>
#include <stdatomic.h>

// This is CircuitPython.
#define CIRCUITPY 1

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

// Sorted alphabetically for easy finding.
//
// default is 128; consider raising to reduce fragmentation.
#define MICROPY_ALLOC_PARSE_CHUNK_INIT   (16)
// default is 512.
#define MICROPY_ALLOC_PATH_MAX           (256)
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
#define MICROPY_ENABLE_GC                (1)
#define MICROPY_ENABLE_SOURCE_LINE       (1)
#define MICROPY_EPOCH_IS_1970            (1)
#define MICROPY_ERROR_REPORTING          (MICROPY_ERROR_REPORTING_NORMAL)
#define MICROPY_FLOAT_HIGH_QUALITY_HASH  (0)
#define MICROPY_FLOAT_IMPL               (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_GC_ALLOC_THRESHOLD       (0)
#define MICROPY_HELPER_LEXER_UNIX        (0)
#define MICROPY_HELPER_REPL              (1)
#define MICROPY_KBD_EXCEPTION            (1)
#define MICROPY_MEM_STATS                (0)
#define MICROPY_MODULE_BUILTIN_INIT      (1)
#define MICROPY_NONSTANDARD_TYPECODES    (0)
#define MICROPY_OPT_COMPUTED_GOTO        (1)
#define MICROPY_OPT_COMPUTED_GOTO_SAVE_SPACE (CIRCUITPY_COMPUTED_GOTO_SAVE_SPACE)
#define MICROPY_OPT_LOAD_ATTR_FAST_PATH  (CIRCUITPY_OPT_LOAD_ATTR_FAST_PATH)
#define MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE (CIRCUITPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE)
#define MICROPY_PERSISTENT_CODE_LOAD     (1)

#define MICROPY_PY_ARRAY                 (1)
#define MICROPY_PY_ARRAY_SLICE_ASSIGN    (1)
#define MICROPY_PY_ATTRTUPLE             (1)

#define MICROPY_PY_BUILTINS_BYTEARRAY    (1)
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

#define MICROPY_PY_CMATH                 (0)
#define MICROPY_PY_COLLECTIONS           (1)
#define MICROPY_PY_DESCRIPTORS           (1)
#define MICROPY_PY_IO_FILEIO             (1)
#define MICROPY_PY_GC                    (1)
// Supplanted by shared-bindings/math
#define MICROPY_PY_MATH                  (0)
#define MICROPY_PY_MICROPYTHON_MEM_INFO  (0)
// Supplanted by shared-bindings/struct
#define MICROPY_PY_STRUCT                (0)
#define MICROPY_PY_SYS                   (1)
#define MICROPY_PY_SYS_MAXSIZE           (1)
#define MICROPY_PY_SYS_STDFILES          (1)
// Supplanted by shared-bindings/random
#define MICROPY_PY_URANDOM               (0)
#define MICROPY_PY_URANDOM_EXTRA_FUNCS   (0)
#define MICROPY_PY___FILE__              (1)

#define MICROPY_QSTR_BYTES_IN_HASH       (1)
#define MICROPY_REPL_AUTO_INDENT         (1)
#define MICROPY_REPL_EVENT_DRIVEN        (0)
#define MICROPY_ENABLE_PYSTACK           (1)
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
#define MICROPY_VFS_FAT             (MICROPY_VFS)
#define MICROPY_READER_VFS          (MICROPY_VFS)

// type definitions for the specific machine

#define BYTES_PER_WORD (4)

#define MICROPY_MAKE_POINTER_CALLABLE(p) ((void *)((mp_uint_t)(p) | 1))

// Track stack usage. Expose results via ustack module.
#define MICROPY_MAX_STACK_USAGE       (0)

#define UINT_FMT "%u"
#define INT_FMT "%d"
#ifdef __LP64__
typedef long mp_int_t; // must be pointer size
typedef unsigned long mp_uint_t; // must be pointer size
#else
// These are definitions for machines where sizeof(int) == sizeof(void*),
// regardless of actual size.
typedef int mp_int_t; // must be pointer size
typedef unsigned int mp_uint_t; // must be pointer size
#endif
#if __GNUC__ >= 10 // on recent gcc versions we can check that this is so
_Static_assert(sizeof(mp_int_t) == sizeof(void *));
_Static_assert(sizeof(mp_uint_t) == sizeof(void *));
#endif
typedef long mp_off_t;

#define MP_PLAT_PRINT_STRN(str, len) mp_hal_stdout_tx_strn_cooked(str, len)

#define mp_type_fileio mp_type_vfs_fat_fileio
#define mp_type_textio mp_type_vfs_fat_textio

#define mp_import_stat mp_vfs_import_stat
#define mp_builtin_open_obj mp_vfs_open_obj


// extra built in names to add to the global namespace
#define MICROPY_PORT_BUILTINS \
    { MP_OBJ_NEW_QSTR(MP_QSTR_help), (mp_obj_t)&mp_builtin_help_obj }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_input), (mp_obj_t)&mp_builtin_input_obj }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR_open), (mp_obj_t)&mp_builtin_open_obj },

//////////////////////////////////////////////////////////////////////////////////////////////////
// board-specific definitions, which control and may override definitions below.
#include "mpconfigboard.h"

// Turning off FULL_BUILD removes some functionality to reduce flash size on tiny SAMD21s
#define MICROPY_BUILTIN_METHOD_CHECK_SELF_ARG (CIRCUITPY_FULL_BUILD)
#ifndef MICROPY_CPYTHON_COMPAT
#define MICROPY_CPYTHON_COMPAT                (CIRCUITPY_FULL_BUILD)
#endif
#define MICROPY_PY_BUILTINS_POW3              (CIRCUITPY_BUILTINS_POW3)
#define MICROPY_PY_FSTRINGS                   (MICROPY_CPYTHON_COMPAT)
#define MICROPY_MODULE_WEAK_LINKS             (0)
#define MICROPY_PY_ALL_SPECIAL_METHODS        (CIRCUITPY_FULL_BUILD)
#ifndef MICROPY_PY_BUILTINS_COMPLEX
#define MICROPY_PY_BUILTINS_COMPLEX           (CIRCUITPY_FULL_BUILD)
#endif
#define MICROPY_PY_BUILTINS_FROZENSET         (CIRCUITPY_FULL_BUILD)
#define MICROPY_PY_BUILTINS_STR_CENTER        (CIRCUITPY_FULL_BUILD)
#define MICROPY_PY_BUILTINS_STR_PARTITION     (CIRCUITPY_FULL_BUILD)
#define MICROPY_PY_BUILTINS_STR_SPLITLINES    (CIRCUITPY_FULL_BUILD)
#ifndef MICROPY_PY_COLLECTIONS_ORDEREDDICT
#define MICROPY_PY_COLLECTIONS_ORDEREDDICT    (CIRCUITPY_FULL_BUILD)
#endif
#define MICROPY_PY_URE_MATCH_GROUPS           (CIRCUITPY_RE)
#define MICROPY_PY_URE_MATCH_SPAN_START_END   (CIRCUITPY_RE)
#define MICROPY_PY_URE_SUB                    (CIRCUITPY_RE)

#define CIRCUITPY_MICROPYTHON_ADVANCED        (CIRCUITPY_FULL_BUILD)

#ifndef MICROPY_FATFS_EXFAT
#define MICROPY_FATFS_EXFAT           (CIRCUITPY_FULL_BUILD)
#endif

// LONGINT_IMPL_xxx are defined in the Makefile.
//
#ifdef LONGINT_IMPL_NONE
#define MICROPY_LONGINT_IMPL (MICROPY_LONGINT_IMPL_NONE)
#endif

#ifdef LONGINT_IMPL_MPZ
#define MICROPY_LONGINT_IMPL (MICROPY_LONGINT_IMPL_MPZ)
#define MP_SSIZE_MAX (0x7fffffff)
#endif

#ifdef LONGINT_IMPL_LONGLONG
#define MICROPY_LONGINT_IMPL (MICROPY_LONGINT_IMPL_LONGLONG)
#define MP_SSIZE_MAX (0x7fffffff)
#endif

#ifndef MICROPY_PY_REVERSE_SPECIAL_METHODS
#define MICROPY_PY_REVERSE_SPECIAL_METHODS    (CIRCUITPY_ULAB || CIRCUITPY_FULL_BUILD)
#endif

#if INTERNAL_FLASH_FILESYSTEM == 0 && QSPI_FLASH_FILESYSTEM == 0 && SPI_FLASH_FILESYSTEM == 0 && !DISABLE_FILESYSTEM
#error No *_FLASH_FILESYSTEM set!
#endif

// Default board buses.

#ifndef CIRCUITPY_BOARD_I2C
#if defined(DEFAULT_I2C_BUS_SCL) && defined(DEFAULT_I2C_BUS_SDA)
#define CIRCUITPY_BOARD_I2C     (1)
#define CIRCUITPY_BOARD_I2C_PIN {{.scl = DEFAULT_I2C_BUS_SCL, .sda = DEFAULT_I2C_BUS_SDA}}
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
#define BOARD_UART_ROOT_POINTER     mp_obj_t board_uart_bus;
#else
#define CIRCUITPY_BOARD_UART        (0)
#define BOARD_UART_ROOT_POINTER
#endif
#else
#define BOARD_UART_ROOT_POINTER     mp_obj_t board_uart_bus;
#endif

// These CIRCUITPY_xxx values should all be defined in the *.mk files as being on or off.
// So if any are not defined in *.mk, they'll throw an error here.

#if CIRCUITPY_DISPLAYIO
#ifndef CIRCUITPY_DISPLAY_LIMIT
#define CIRCUITPY_DISPLAY_LIMIT (1)
#endif

// Framebuffer area size in bytes. Rounded down to power of four for alignment.
#ifndef CIRCUITPY_DISPLAY_AREA_BUFFER_SIZE
#define CIRCUITPY_DISPLAY_AREA_BUFFER_SIZE (128)
#endif

#else
#define CIRCUITPY_DISPLAY_LIMIT (0)
#define CIRCUITPY_DISPLAY_AREA_BUFFER_SIZE (0)
#endif

#if CIRCUITPY_GAMEPADSHIFT
// Scan gamepad every 32ms
#define CIRCUITPY_GAMEPAD_TICKS 0x1f
#define GAMEPAD_ROOT_POINTERS mp_obj_t gamepad_singleton;
#else
#define GAMEPAD_ROOT_POINTERS
#endif

#if CIRCUITPY_KEYPAD
#define KEYPAD_ROOT_POINTERS mp_obj_t keypad_scanners_linked_list;
#else
#define KEYPAD_ROOT_POINTERS
#endif

#if CIRCUITPY_MEMORYMONITOR
#define MEMORYMONITOR_ROOT_POINTERS mp_obj_t active_allocationsizes; \
    mp_obj_t active_allocationalarms;
#else
#define MEMORYMONITOR_ROOT_POINTERS
#endif

// This is not a top-level module; it's microcontroller.nvm.
#if CIRCUITPY_NVM
extern const struct _mp_obj_module_t nvm_module;
#endif

// Following modules are implemented in either extmod or py directory.

#define MICROPY_PY_UBINASCII CIRCUITPY_BINASCII

#define MICROPY_PY_UERRNO CIRCUITPY_ERRNO
// Uses about 80 bytes.
#define MICROPY_PY_UERRNO_ERRORCODE CIRCUITPY_ERRNO

#define MICROPY_PY_URE CIRCUITPY_RE

#if CIRCUITPY_JSON
#define MICROPY_PY_UJSON (1)
#define MICROPY_PY_IO (1)
#else
#ifndef MICROPY_PY_IO
// We don't need MICROPY_PY_IO unless someone else wants it.
#define MICROPY_PY_IO (0)
#endif
#endif

#ifndef ULAB_SUPPORTS_COMPLEX
#define ULAB_SUPPORTS_COMPLEX (0)
#endif

#if CIRCUITPY_ULAB
// ulab requires reverse special methods
#if defined(MICROPY_PY_REVERSE_SPECIAL_METHODS) && !MICROPY_PY_REVERSE_SPECIAL_METHODS
#error "ulab requires MICROPY_PY_REVERSE_SPECIAL_METHODS"
#endif
#endif

#if CIRCUITPY_WIFI
#define WIFI_MONITOR_ROOT_POINTERS mp_obj_t wifi_monitor_singleton;
#else
#define WIFI_MONITOR_ROOT_POINTERS
#endif

// Define certain native modules with weak links so they can be replaced with Python
// implementations. This list may grow over time.

#define MICROPY_PORT_BUILTIN_MODULE_WEAK_LINKS

// Native modules that are weak links can be accessed directly
// by prepending their name with an underscore. This list should correspond to
// MICROPY_PORT_BUILTIN_MODULE_WEAK_LINKS, assuming you want the native modules
// to be accessible when overriden.

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

#include "supervisor/flash_root_pointers.h"

// From supervisor/memory.c
struct _supervisor_allocation_node;

#define CIRCUITPY_COMMON_ROOT_POINTERS \
    FLASH_ROOT_POINTERS \
    KEYPAD_ROOT_POINTERS \
    GAMEPAD_ROOT_POINTERS \
    BOARD_UART_ROOT_POINTER \
    WIFI_MONITOR_ROOT_POINTERS \
    MEMORYMONITOR_ROOT_POINTERS \
    vstr_t *repl_line; \
    mp_obj_t pew_singleton; \
    mp_obj_t rtc_time_source; \
    const char *readline_hist[8]; \
    struct _supervisor_allocation_node *first_embedded_allocation; \

void supervisor_run_background_tasks_if_tick(void);
#define RUN_BACKGROUND_TASKS (supervisor_run_background_tasks_if_tick())

#define MICROPY_VM_HOOK_LOOP RUN_BACKGROUND_TASKS;
#define MICROPY_VM_HOOK_RETURN RUN_BACKGROUND_TASKS;

// CIRCUITPY_AUTORELOAD_DELAY_MS = 0 will completely disable autoreload.
#ifndef CIRCUITPY_AUTORELOAD_DELAY_MS
#define CIRCUITPY_AUTORELOAD_DELAY_MS 500
#endif

#ifndef CIRCUITPY_FILESYSTEM_FLUSH_INTERVAL_MS
#define CIRCUITPY_FILESYSTEM_FLUSH_INTERVAL_MS 1000
#endif

#ifndef CIRCUITPY_PYSTACK_SIZE
#define CIRCUITPY_PYSTACK_SIZE 1536
#endif

// Wait this long imediately after startup to see if we are connected to USB.
#ifndef CIRCUITPY_USB_CONNECTED_SLEEP_DELAY
#define CIRCUITPY_USB_CONNECTED_SLEEP_DELAY 5
#endif

#ifndef CIRCUITPY_PROCESSOR_COUNT
#define CIRCUITPY_PROCESSOR_COUNT (1)
#endif

#ifndef CIRCUITPY_STATUS_LED_POWER_INVERTED
#define CIRCUITPY_STATUS_LED_POWER_INVERTED (0)
#endif

#define CIRCUITPY_BOOT_OUTPUT_FILE "/boot_out.txt"

#ifndef CIRCUITPY_BOOT_COUNTER
#define CIRCUITPY_BOOT_COUNTER 0
#endif

#if !defined(CIRCUITPY_INTERNAL_NVM_SIZE) && CIRCUITPY_BOOT_COUNTER != 0
#error "boot counter requires CIRCUITPY_NVM enabled"
#endif

#define CIRCUITPY_VERBOSE_BLE 0

// This trades ~1k flash space (1) for that much in RAM plus the cost to compute
// the values once on init (0). Only turn it off, when you really need the flash
// space and are willing to trade the RAM.
#ifndef CIRCUITPY_PRECOMPUTE_QSTR_ATTR
#define CIRCUITPY_PRECOMPUTE_QSTR_ATTR (1)
#endif

// USB settings

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


#endif  // __INCLUDED_MPCONFIG_CIRCUITPY_H
