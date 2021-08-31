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
#define MICROPY_OBJ_REPR            (MICROPY_OBJ_REPR_C)

// options to control how MicroPython is built
// TODO(tannewt): Reduce this number if we want the REPL to function under 512
// free bytes.
// #define MICROPY_ALLOC_PARSE_RULE_INIT (64)

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

// This port is intended to be 32-bit, but unfortunately, int32_t for
// different targets may be defined in different ways - either as int
// or as long. This requires different printf formatting specifiers
// to print such value. So, we avoid int32_t and use int directly.
#define UINT_FMT "%u"
#define INT_FMT "%d"
typedef int mp_int_t; // must be pointer size
typedef unsigned mp_uint_t; // must be pointer size
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
#define MICROPY_COMP_FSTRING_LITERAL          (MICROPY_CPYTHON_COMPAT)
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

// These CIRCUITPY_xxx values should all be defined in the *.mk files as being on or off.
// So if any are not defined in *.mk, they'll throw an error here.

#if CIRCUITPY_AESIO
extern const struct _mp_obj_module_t aesio_module;
#define AESIO_MODULE       { MP_OBJ_NEW_QSTR(MP_QSTR_aesio), (mp_obj_t)&aesio_module },
#else
#define AESIO_MODULE
#endif

#if CIRCUITPY_ALARM
extern const struct _mp_obj_module_t alarm_module;
#define ALARM_MODULE           { MP_OBJ_NEW_QSTR(MP_QSTR_alarm), (mp_obj_t)&alarm_module },
#else
#define ALARM_MODULE
#endif

// CIRCUITPY_ANALOGIO uses MP_REGISTER_MODULE
// CIRCUITPY_ATEXIT uses MP_REGISTER_MODULE
// CIRCUITPY_AUDIOBUSIO uses MP_REGISTER_MODULE
// CIRCUITPY_AUDIOCORE uses MP_REGISTER_MODULE
// CIRCUITPY_AUDIOIO uses MP_REGISTER_MODULE
// CIRCUITPY_AUDIOMIXER uses MP_REGISTER_MODULE
// CIRCUITPY_AUDIOMP3 uses MP_REGISTER_MODULE

#if CIRCUITPY_AUDIOPWMIO
#define AUDIOPWMIO_MODULE         { MP_OBJ_NEW_QSTR(MP_QSTR_audiopwmio), (mp_obj_t)&audiopwmio_module },
extern const struct _mp_obj_module_t audiopwmio_module;
#else
#define AUDIOPWMIO_MODULE
#endif

#if CIRCUITPY_BINASCII
#define MICROPY_PY_UBINASCII CIRCUITPY_BINASCII
#define BINASCII_MODULE        { MP_ROM_QSTR(MP_QSTR_binascii), MP_ROM_PTR(&mp_module_ubinascii) },
#else
#define BINASCII_MODULE
#endif

// CIRCUITPY_BITBANGIO uses MP_REGISTER_MODULE
// CIRCUITPY_BITMAPTOOLS uses MP_REGISTER_MODULE

#if CIRCUITPY_BITOPS
extern const struct _mp_obj_module_t bitops_module;
#define BITOPS_MODULE        { MP_OBJ_NEW_QSTR(MP_QSTR_bitops),(mp_obj_t)&bitops_module },
#else
#define BITOPS_MODULE
#endif

// CIRCUITPY_BLEIO uses MP_REGISTER_MODULE
// CIRCUITPY_BOARD uses MP_REGISTER_MODULE

#if CIRCUITPY_BOARD
#define BOARD_I2C (defined(DEFAULT_I2C_BUS_SDA) && defined(DEFAULT_I2C_BUS_SCL))
#define BOARD_SPI (defined(DEFAULT_SPI_BUS_SCK) && defined(DEFAULT_SPI_BUS_MISO) && defined(DEFAULT_SPI_BUS_MOSI))
#define BOARD_UART (defined(DEFAULT_UART_BUS_RX) && defined(DEFAULT_UART_BUS_TX))

// I2C and SPI are always allocated off the heap.

#if BOARD_UART
#define BOARD_UART_ROOT_POINTER mp_obj_t shared_uart_bus;
#else
#define BOARD_UART_ROOT_POINTER
#endif

#else
#define BOARD_UART_ROOT_POINTER
#endif

// CIRCUITPY_BUSDEVICE (adafruit_bus_device_module) uses MP_REGISTER_MODULE
// CIRCUITPY_BUSIO uses MP_REGISTER_MODULE

#if CIRCUITPY_CAMERA
extern const struct _mp_obj_module_t camera_module;
#define CAMERA_MODULE        { MP_OBJ_NEW_QSTR(MP_QSTR_camera), (mp_obj_t)&camera_module },
#else
#define CAMERA_MODULE
#endif

#if CIRCUITPY_CANIO
extern const struct _mp_obj_module_t canio_module;
#define CANIO_MODULE           { MP_OBJ_NEW_QSTR(MP_QSTR_canio), (mp_obj_t)&canio_module },
#else
#define CANIO_MODULE
#endif

// CIRCUITPY_COUNTIO uses MP_REGISTER_MODULE
// CIRCUITPY_DIGITALIO uses MP_REGISTER_MODULE
// CIRCUITPY_DISPLAYIO uses MP_REGISTER_MODULE
// CIRCUITPY_PARALLELDISPLAY uses MP_REGISTER_MODULE
// CIRCUITPY_TERMINALIO uses MP_REGISTER_MODULE
// CIRCUITPY_FONTIO uses MP_REGISTER_MODULE

#if CIRCUITPY_DISPLAYIO
#ifndef CIRCUITPY_DISPLAY_LIMIT
#define CIRCUITPY_DISPLAY_LIMIT (1)
#endif
#else
#define CIRCUITPY_DISPLAY_LIMIT (0)
#endif

#if CIRCUITPY_DUALBANK
extern const struct _mp_obj_module_t dualbank_module;
#define DUALBANK_MODULE           { MP_OBJ_NEW_QSTR(MP_QSTR_dualbank), (mp_obj_t)&dualbank_module },
#else
#define DUALBANK_MODULE
#endif

#if CIRCUITPY_ERRNO
#define MICROPY_PY_UERRNO (1)
// Uses about 80 bytes.
#define MICROPY_PY_UERRNO_ERRORCODE (1)
#define ERRNO_MODULE           { MP_ROM_QSTR(MP_QSTR_errno), MP_ROM_PTR(&mp_module_uerrno) },
#else
#define ERRNO_MODULE
#
 #endif

#if CIRCUITPY_ESPIDF
extern const struct _mp_obj_module_t espidf_module;
#define ESPIDF_MODULE            { MP_OBJ_NEW_QSTR(MP_QSTR_espidf),(mp_obj_t)&espidf_module },
#else
#define ESPIDF_MODULE
#endif

#if CIRCUITPY__EVE
extern const struct _mp_obj_module_t _eve_module;
#define _EVE_MODULE            { MP_OBJ_NEW_QSTR(MP_QSTR__eve), (mp_obj_t)&_eve_module },
#else
#define _EVE_MODULE
#endif

// CIRCUITPY_FRAMEBUFFERIO uses MP_REGISTER_MODULE
// CIRCUITPY_VECTORIO uses MP_REGISTER_MODULE

// CIRCUITPY_FREQUENCYIO uses MP_REGISTER_MODULE
// CIRCUITPY_GAMEPADSHIFT uses MP_REGISTER_MODULE

#if CIRCUITPY_GAMEPADSHIFT
// Scan gamepad every 32ms
#define CIRCUITPY_GAMEPAD_TICKS 0x1f
#define GAMEPAD_ROOT_POINTERS mp_obj_t gamepad_singleton;
#else
#define GAMEPAD_ROOT_POINTERS
#endif

// CIRCUITPY_GETPASS uses MP_REGISTER_MODULE

#if CIRCUITPY_GNSS
extern const struct _mp_obj_module_t gnss_module;
#define GNSS_MODULE        { MP_OBJ_NEW_QSTR(MP_QSTR_gnss), (mp_obj_t)&gnss_module },
#else
#define GNSS_MODULE
#endif

#if CIRCUITPY_I2CPERIPHERAL
extern const struct _mp_obj_module_t i2cperipheral_module;
#define I2CPERIPHERAL_MODULE        { MP_OBJ_NEW_QSTR(MP_QSTR_i2cperipheral), (mp_obj_t)&i2cperipheral_module },
#else
#define I2CPERIPHERAL_MODULE
#endif

#if CIRCUITPY_IMAGECAPTURE
extern const struct _mp_obj_module_t imagecapture_module;
#define IMAGECAPTURE_MODULE           { MP_OBJ_NEW_QSTR(MP_QSTR_imagecapture), (mp_obj_t)&imagecapture_module },
#else
#define IMAGECAPTURE_MODULE
#endif

#if CIRCUITPY_IPADDRESS
extern const struct _mp_obj_module_t ipaddress_module;
#define IPADDRESS_MODULE        { MP_OBJ_NEW_QSTR(MP_QSTR_ipaddress), (mp_obj_t)&ipaddress_module },
#else
#define IPADDRESS_MODULE
#endif

#if CIRCUITPY_JSON
#define MICROPY_PY_UJSON (1)
#define MICROPY_PY_IO (1)
#define JSON_MODULE            { MP_ROM_QSTR(MP_QSTR_json), MP_ROM_PTR(&mp_module_ujson) },
#else
#ifndef MICROPY_PY_IO
// We don't need MICROPY_PY_IO unless someone else wants it.
#define MICROPY_PY_IO (0)
#endif
#define JSON_MODULE
#endif

// CIRCUITPY_KEYPAD uses MP_REGISTER_MODULE

#if CIRCUITPY_KEYPAD
#define KEYPAD_ROOT_POINTERS mp_obj_t keypad_scanners_linked_list;
#else
#define KEYPAD_ROOT_POINTERS
#endif

// CIRCUITPY_MATH uses MP_REGISTER_MODULE

#if CIRCUITPY_MEMORYMONITOR
extern const struct _mp_obj_module_t memorymonitor_module;
#define MEMORYMONITOR_MODULE { MP_OBJ_NEW_QSTR(MP_QSTR_memorymonitor), (mp_obj_t)&memorymonitor_module },
#define MEMORYMONITOR_ROOT_POINTERS mp_obj_t active_allocationsizes; \
    mp_obj_t active_allocationalarms;
#else
#define MEMORYMONITOR_MODULE
#define MEMORYMONITOR_ROOT_POINTERS
#endif

// CIRCUITPY_MICROCONTROLLER uses MP_REGISTER_MODULE
// CIRCUITPY_NEOPIXEL_WRITE uses MP_REGISTER_MODULE

// This is not a top-level module; it's microcontroller.nvm.
#if CIRCUITPY_NVM
extern const struct _mp_obj_module_t nvm_module;
#endif

// CIRCUITPY_ONEWIREIO_WRITE uses MP_REGISTER_MODULE

#if CIRCUITPY_OS
extern const struct _mp_obj_module_t os_module;
#define OS_MODULE              { MP_OBJ_NEW_QSTR(MP_QSTR_os), (mp_obj_t)&os_module },
#define OS_MODULE_ALT_NAME     { MP_OBJ_NEW_QSTR(MP_QSTR__os), (mp_obj_t)&os_module },
#else
#define OS_MODULE
#define OS_MODULE_ALT_NAME
#endif

#if CIRCUITPY_PEW
extern const struct _mp_obj_module_t pew_module;
#define PEW_MODULE          { MP_OBJ_NEW_QSTR(MP_QSTR__pew),(mp_obj_t)&pew_module },
#else
#define PEW_MODULE
#endif

// CIRCUITPY_PIXELBUF (pixelbuf_module) uses MP_REGISTER_MODULE
// CIRCUITPY_PS2IO uses MP_REGISTER_MODULE
// CIRCUITPY_PULSEIO uses MP_REGISTER_MODULE
// CIRCUITPY_PWMIO uses MP_REGISTER_MODULE
// CIRCUITPY_QRIO uses MP_REGISTER_MODULE
// CIRCUITPY_RAINBOWIO uses MP_REGISTER_MODULE
// CIRCUITPY_RANDOM uses MP_REGISTER_MODULE

#if CIRCUITPY_RE
#define MICROPY_PY_URE (1)
#define RE_MODULE            { MP_ROM_QSTR(MP_QSTR_re), MP_ROM_PTR(&mp_module_ure) },
#else
#define RE_MODULE
#endif

// CIRCUITPY_RGBMATRIX uses MP_REGISTER_MODULE
// CIRCUITPY_ROTARYIO uses MP_REGISTER_MODULE

#if CIRCUITPY_RP2PIO
extern const struct _mp_obj_module_t rp2pio_module;
#define RP2PIO_MODULE            { MP_OBJ_NEW_QSTR(MP_QSTR_rp2pio),(mp_obj_t)&rp2pio_module },
#else
#define RP2PIO_MODULE
#endif

// CIRCUITPY_RTC uses MP_REGISTER_MODULE

#if CIRCUITPY_SAMD
extern const struct _mp_obj_module_t samd_module;
#define SAMD_MODULE            { MP_OBJ_NEW_QSTR(MP_QSTR_samd),(mp_obj_t)&samd_module },
#else
#define SAMD_MODULE
#endif

// CIRCUITPY_SDCARDIO uses MP_REGISTER_MODULE

#if CIRCUITPY_SDIOIO
extern const struct _mp_obj_module_t sdioio_module;
#define SDIOIO_MODULE           { MP_OBJ_NEW_QSTR(MP_QSTR_sdioio), (mp_obj_t)&sdioio_module },
#else
#define SDIOIO_MODULE
#endif

// CIRCUITPY_SHARPDISPLAY uses MP_REGISTER_MODULE

#if CIRCUITPY_SOCKETPOOL
extern const struct _mp_obj_module_t socketpool_module;
#define SOCKETPOOL_MODULE           { MP_OBJ_NEW_QSTR(MP_QSTR_socketpool), (mp_obj_t)&socketpool_module },
#else
#define SOCKETPOOL_MODULE
#endif

#if CIRCUITPY_SSL
extern const struct _mp_obj_module_t ssl_module;
#define SSL_MODULE           { MP_OBJ_NEW_QSTR(MP_QSTR_ssl), (mp_obj_t)&ssl_module },
#else
#define SSL_MODULE
#endif

// CIRCUITPY_STAGE uses MP_REGISTER_MODULE
// CIRCUITPY_STORAGE uses MP_REGISTER_MODULE
// CIRCUITPY_STRUCT uses MP_REGISTER_MODULE
// CIRCUITPY_SUPERVISOR uses MP_REGISTER_MODULE
// CIRCUITPY_SYNTHIO uses MP_REGISTER_MODULE

#if CIRCUITPY_TIME
extern const struct _mp_obj_module_t time_module;
#define TIME_MODULE            { MP_OBJ_NEW_QSTR(MP_QSTR_time), (mp_obj_t)&time_module },
#define TIME_MODULE_ALT_NAME   { MP_OBJ_NEW_QSTR(MP_QSTR__time), (mp_obj_t)&time_module },
#else
#define TIME_MODULE
#define TIME_MODULE_ALT_NAME
#endif

// CIRCUITPY_TOUCHIO uses MP_REGISTER_MODULE
// CIRCUITPY_TRACEBACK uses MP_REGISTER_MODULE

#if CIRCUITPY_UHEAP
extern const struct _mp_obj_module_t uheap_module;
#define UHEAP_MODULE           { MP_OBJ_NEW_QSTR(MP_QSTR_uheap),(mp_obj_t)&uheap_module },
#else
#define UHEAP_MODULE
#endif

// CIRCUITPY_USB_CDC uses MP_REGISTER_MODULE
// CIRCUITPY_USB_HID uses MP_REGISTER_MODULE
// CIRCUITPY_USB_MIDI uses MP_REGISTER_MODULE

#if CIRCUITPY_USTACK
extern const struct _mp_obj_module_t ustack_module;
#define USTACK_MODULE          { MP_OBJ_NEW_QSTR(MP_QSTR_ustack),(mp_obj_t)&ustack_module },
#else
#define USTACK_MODULE
#endif

#if defined(CIRCUITPY_ULAB) && CIRCUITPY_ULAB
// ulab requires reverse special methods
#if defined(MICROPY_PY_REVERSE_SPECIAL_METHODS) && !MICROPY_PY_REVERSE_SPECIAL_METHODS
#error "ulab requires MICROPY_PY_REVERSE_SPECIAL_METHODS"
#endif
#define ULAB_MODULE \
    { MP_ROM_QSTR(MP_QSTR_ulab), MP_ROM_PTR(&ulab_user_cmodule) },
#else
#define ULAB_MODULE
#endif

// This is not a top-level module; it's microcontroller.watchdog.
#if CIRCUITPY_WATCHDOG
extern const struct _mp_obj_module_t watchdog_module;
#define WATCHDOG_MODULE { MP_ROM_QSTR(MP_QSTR_watchdog), MP_ROM_PTR(&watchdog_module) },
#else
#define WATCHDOG_MODULE
#endif

#if CIRCUITPY_WIFI
extern const struct _mp_obj_module_t wifi_module;
#define WIFI_MODULE { MP_ROM_QSTR(MP_QSTR_wifi), MP_ROM_PTR(&wifi_module) },
#else
#define WIFI_MODULE
#endif

// CIRCUITPY_MSGPACK uses MP_REGISTER_MODULE

// Define certain native modules with weak links so they can be replaced with Python
// implementations. This list may grow over time.
#define MICROPY_PORT_BUILTIN_MODULE_WEAK_LINKS \
    OS_MODULE \
    TIME_MODULE \

// Native modules that are weak links can be accessed directly
// by prepending their name with an underscore. This list should correspond to
// MICROPY_PORT_BUILTIN_MODULE_WEAK_LINKS, assuming you want the native modules
// to be accessible when overriden.
#define MICROPY_PORT_BUILTIN_MODULE_ALT_NAMES \
    OS_MODULE_ALT_NAME \
    TIME_MODULE_ALT_NAME \

// This is an inclusive list that should correspond to the CIRCUITPY_XXX list above,
// including dependencies.
// Some of these definitions will be blank depending on what is turned on and off.
// Some are omitted because they're in MICROPY_PORT_BUILTIN_MODULE_WEAK_LINKS above.
#define MICROPY_PORT_BUILTIN_MODULES_STRONG_LINKS \
    AESIO_MODULE \
    ALARM_MODULE \
    AUDIOPWMIO_MODULE \
    BINASCII_MODULE \
    BITOPS_MODULE \
    CAMERA_MODULE \
    CANIO_MODULE \
    DUALBANK_MODULE \
    ERRNO_MODULE \
    ESPIDF_MODULE \
    _EVE_MODULE \
    GNSS_MODULE \
    I2CPERIPHERAL_MODULE \
    IPADDRESS_MODULE \
    IMAGECAPTURE_MODULE \
    JSON_MODULE \
    MEMORYMONITOR_MODULE \
    PEW_MODULE \
    RE_MODULE \
    RP2PIO_MODULE \
    SAMD_MODULE \
    SDIOIO_MODULE \
    SOCKETPOOL_MODULE \
    SSL_MODULE \
    UHEAP_MODULE \
    USTACK_MODULE \
    WATCHDOG_MODULE \
    WIFI_MODULE \

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

#define CIRCUITPY_VERBOSE_BLE 0

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

#ifndef USB_MIDI_EP_NUM_OUT
#define USB_MIDI_EP_NUM_OUT (0)
#endif

#ifndef USB_MIDI_EP_NUM_IN
#define USB_MIDI_EP_NUM_IN (0)
#endif


#endif  // __INCLUDED_MPCONFIG_CIRCUITPY_H
