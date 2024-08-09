/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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

#include <stdint.h>
#include "hardware/flash.h"
#include "hardware/spi.h"
#include "hardware/sync.h"
#include "pico/binary_info.h"
#include "pico/multicore.h"
#include "mpconfigboard.h"

// Board and hardware specific configuration
#if PICO_RP2040
#define MICROPY_HW_MCU_NAME                     "RP2040"
#elif PICO_RP2350 && PICO_ARM
#define MICROPY_HW_MCU_NAME                     "RP2350"
#elif PICO_RP2350 && PICO_RISCV
#define MICROPY_HW_MCU_NAME                     "RP2350-RISCV"
#else
#error Unknown MCU
#endif

#ifndef MICROPY_HW_ENABLE_UART_REPL
#define MICROPY_HW_ENABLE_UART_REPL             (0) // useful if there is no USB
#endif
#ifndef MICROPY_HW_ENABLE_USBDEV
#define MICROPY_HW_ENABLE_USBDEV                (1)
#endif

#if MICROPY_HW_ENABLE_USBDEV
// Enable USB-CDC serial port
#ifndef MICROPY_HW_USB_CDC
#define MICROPY_HW_USB_CDC (1)
#endif
// Enable USB Mass Storage with FatFS filesystem.
#ifndef MICROPY_HW_USB_MSC
#define MICROPY_HW_USB_MSC (0)
#endif

#ifndef MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE
#define MICROPY_HW_ENABLE_USB_RUNTIME_DEVICE    (1) // Support machine.USBDevice
#endif
#endif

// Number of bytes of flash to allocate to the ROMFS partition.
#ifndef MICROPY_HW_ROMFS_BYTES
#define MICROPY_HW_ROMFS_BYTES (0)
#endif

// Number of bytes of flash to allocate to read/write filesystem storage.
#ifndef MICROPY_HW_FLASH_STORAGE_BYTES
#define MICROPY_HW_FLASH_STORAGE_BYTES (1408 * 1024)
#endif

#ifndef MICROPY_CONFIG_ROM_LEVEL
#define MICROPY_CONFIG_ROM_LEVEL                (MICROPY_CONFIG_ROM_LEVEL_EXTRA_FEATURES)
#endif

#ifndef MICROPY_HW_ENABLE_PSRAM
#define MICROPY_HW_ENABLE_PSRAM (0)
#endif

// Memory allocation policies
#if MICROPY_HW_ENABLE_PSRAM
#define MICROPY_GC_STACK_ENTRY_TYPE             uint32_t
#define MICROPY_ALLOC_GC_STACK_SIZE             (1024) // Avoid slowdown when GC stack overflow causes a full sweep of PSRAM-backed heap
#else
#define MICROPY_GC_STACK_ENTRY_TYPE             uint16_t
#endif
#ifndef MICROPY_GC_SPLIT_HEAP
#define MICROPY_GC_SPLIT_HEAP                   MICROPY_HW_ENABLE_PSRAM // whether PSRAM is added to or replaces the heap
#endif
#define MICROPY_ALLOC_PATH_MAX                  (128)
#define MICROPY_QSTR_BYTES_IN_HASH              (1)

// MicroPython emitters
#define MICROPY_PERSISTENT_CODE_LOAD            (1)
#if PICO_ARM
#define MICROPY_EMIT_THUMB                      (1)
#define MICROPY_EMIT_INLINE_THUMB               (1)
#if PICO_RP2040
#define MICROPY_EMIT_THUMB_ARMV7M               (0)
#define MICROPY_EMIT_INLINE_THUMB_FLOAT         (0)
#endif
#elif PICO_RISCV
#define MICROPY_EMIT_RV32                       (1)
#define MICROPY_EMIT_INLINE_RV32                (1)
#endif

// Optimisations
#define MICROPY_OPT_COMPUTED_GOTO               (1)

// Python internal features
#define MICROPY_TRACKED_ALLOC                   (MICROPY_SSL_MBEDTLS || MICROPY_BLUETOOTH_BTSTACK)
#define MICROPY_READER_VFS                      (1)
#define MICROPY_ENABLE_GC                       (1)
#define MICROPY_STACK_CHECK_MARGIN              (256)
#define MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF  (1)
#define MICROPY_LONGINT_IMPL                    (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_FLOAT_IMPL                      (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_SCHEDULER_DEPTH                 (8)
#define MICROPY_SCHEDULER_STATIC_NODES          (1)
#ifndef MICROPY_USE_INTERNAL_ERRNO
#define MICROPY_USE_INTERNAL_ERRNO              (1)
#endif

// Fine control over Python builtins, classes, modules, etc
#define MICROPY_PY_BUILTINS_HELP_TEXT           rp2_help_text
#define MICROPY_PY_SYS_PLATFORM                 "rp2"
#ifndef MICROPY_PY_THREAD
#define MICROPY_PY_THREAD                       (1)
#define MICROPY_PY_THREAD_GIL                   (0)
#define MICROPY_THREAD_YIELD()                  mp_handle_pending(true)
#endif

// Extended modules
#define MICROPY_EPOCH_IS_1970                   (1)
#define MICROPY_PY_OS_INCLUDEFILE               "ports/rp2/modos.c"
#ifndef MICROPY_PY_OS_DUPTERM
#define MICROPY_PY_OS_DUPTERM                   (1)
#define MICROPY_PY_OS_DUPTERM_NOTIFY            (1)
#endif
#define MICROPY_PY_OS_SYNC                      (1)
#define MICROPY_PY_OS_UNAME                     (1)
#define MICROPY_PY_OS_URANDOM                   (1)
#define MICROPY_PY_RE_MATCH_GROUPS              (1)
#define MICROPY_PY_RE_MATCH_SPAN_START_END      (1)
#define MICROPY_PY_HASHLIB_SHA1                 (1)
#define MICROPY_PY_CRYPTOLIB                    (1)
#define MICROPY_PY_TIME_GMTIME_LOCALTIME_MKTIME (1)
#define MICROPY_PY_TIME_TIME_TIME_NS            (1)
#define MICROPY_PY_TIME_INCLUDEFILE             "ports/rp2/modtime.c"
#define MICROPY_PY_RANDOM_SEED_INIT_FUNC        (rosc_random_u32())
#define MICROPY_PY_MACHINE                      (1)
#define MICROPY_PY_MACHINE_INCLUDEFILE          "ports/rp2/modmachine.c"
#define MICROPY_PY_MACHINE_RESET                (1)
#define MICROPY_PY_MACHINE_BARE_METAL_FUNCS     (1)
#define MICROPY_PY_MACHINE_BOOTLOADER           (1)
#define MICROPY_PY_MACHINE_DISABLE_IRQ_ENABLE_IRQ (1)
#define MICROPY_PY_MACHINE_ADC                  (1)
#define MICROPY_PY_MACHINE_ADC_INCLUDEFILE      "ports/rp2/machine_adc.c"
#define MICROPY_PY_MACHINE_PIN_MAKE_NEW         mp_pin_make_new
#define MICROPY_PY_MACHINE_BITSTREAM            (1)
#define MICROPY_PY_MACHINE_DHT_READINTO         (1)
#define MICROPY_PY_MACHINE_PULSE                (1)
#define MICROPY_PY_MACHINE_PWM                  (1)
#define MICROPY_PY_MACHINE_PWM_INCLUDEFILE      "ports/rp2/machine_pwm.c"
#define MICROPY_PY_MACHINE_I2C                  (1)
#define MICROPY_PY_MACHINE_SOFTI2C              (1)
#define MICROPY_PY_MACHINE_I2S                  (1)
#define MICROPY_PY_MACHINE_I2S_INCLUDEFILE      "ports/rp2/machine_i2s.c"
#define MICROPY_PY_MACHINE_I2S_CONSTANT_RX      (RX)
#define MICROPY_PY_MACHINE_I2S_CONSTANT_TX      (TX)
#define MICROPY_PY_MACHINE_I2S_RING_BUF         (1)
#define MICROPY_PY_MACHINE_SPI                  (1)
#define MICROPY_PY_MACHINE_SPI_MSB              (SPI_MSB_FIRST)
#define MICROPY_PY_MACHINE_SPI_LSB              (SPI_LSB_FIRST)
#define MICROPY_PY_MACHINE_SOFTSPI              (1)
#define MICROPY_PY_MACHINE_UART                 (1)
#define MICROPY_PY_MACHINE_UART_INCLUDEFILE     "ports/rp2/machine_uart.c"
#define MICROPY_PY_MACHINE_UART_SENDBREAK       (1)
#define MICROPY_PY_MACHINE_UART_IRQ             (1)
#define MICROPY_PY_MACHINE_WDT                  (1)
#define MICROPY_PY_MACHINE_WDT_INCLUDEFILE      "ports/rp2/machine_wdt.c"
#define MICROPY_PY_MACHINE_FREQ_NUM_ARGS_MAX    (2)
#define MICROPY_PY_ONEWIRE                      (1)
#define MICROPY_VFS                             (1)
#define MICROPY_VFS_LFS2                        (1)
#define MICROPY_VFS_FAT                         (1)
#define MICROPY_VFS_ROM                         (MICROPY_HW_ROMFS_BYTES > 0)
#define MICROPY_SSL_MBEDTLS                     (1)
#define MICROPY_PY_LWIP_PPP                     (MICROPY_PY_NETWORK_PPP_LWIP)
#define MICROPY_PY_LWIP_SOCK_RAW                (MICROPY_PY_LWIP)

// Hardware timer alarm index. Available range 0-3.
// Number 3 is currently used by pico-sdk (PICO_TIME_DEFAULT_ALARM_POOL_HARDWARE_ALARM_NUM)
#define MICROPY_HW_SOFT_TIMER_ALARM_NUM         (2)

// fatfs configuration
#define MICROPY_FATFS_ENABLE_LFN                (2)
#define MICROPY_FATFS_LFN_CODE_PAGE             437 /* 1=SFN/ANSI 437=LFN/U.S.(OEM) */
#define MICROPY_FATFS_RPATH                     (2)
#if MICROPY_HW_USB_MSC
#define MICROPY_FATFS_USE_LABEL                 (1)
#define MICROPY_FATFS_MULTI_PARTITION           (1)
// Set FatFS block size to flash sector size to avoid caching
// the flash sector in memory to support smaller block sizes.
#define MICROPY_FATFS_MAX_SS                    (FLASH_SECTOR_SIZE)
#endif

#ifndef MICROPY_BOARD_ENTER_BOOTLOADER
#define MICROPY_BOARD_ENTER_BOOTLOADER(nargs, args)
#endif

// By default networking should include sockets, ssl, websockets, webrepl, dupterm.
#if MICROPY_PY_NETWORK
#ifndef MICROPY_PY_NETWORK_HOSTNAME_DEFAULT
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "mpy-rp2"
#endif

#ifndef MICROPY_PY_SOCKET
#define MICROPY_PY_SOCKET               (1)
#endif
#ifndef MICROPY_PY_SSL
#define MICROPY_PY_SSL                  (1)
#endif
#ifndef MICROPY_PY_WEBSOCKET
#define MICROPY_PY_WEBSOCKET            (1)
#endif
#ifndef MICROPY_PY_WEBREPL
#define MICROPY_PY_WEBREPL              (1)
#endif

#ifndef MICROPY_PY_NETWORK_PPP_LWIP
#define MICROPY_PY_NETWORK_PPP_LWIP     (0)
#endif
#endif

#if MICROPY_PY_NETWORK_CYW43
extern const struct _mp_obj_type_t mp_network_cyw43_type;
#define MICROPY_HW_NIC_CYW43 \
    { MP_ROM_QSTR(MP_QSTR_WLAN), MP_ROM_PTR(&mp_network_cyw43_type) }, \
    { MP_ROM_QSTR(MP_QSTR_STAT_IDLE), MP_ROM_INT(CYW43_LINK_DOWN) }, \
    { MP_ROM_QSTR(MP_QSTR_STAT_CONNECTING), MP_ROM_INT(CYW43_LINK_JOIN) }, \
    { MP_ROM_QSTR(MP_QSTR_STAT_WRONG_PASSWORD), MP_ROM_INT(CYW43_LINK_BADAUTH) }, \
    { MP_ROM_QSTR(MP_QSTR_STAT_NO_AP_FOUND), MP_ROM_INT(CYW43_LINK_NONET) }, \
    { MP_ROM_QSTR(MP_QSTR_STAT_CONNECT_FAIL), MP_ROM_INT(CYW43_LINK_FAIL) }, \
    { MP_ROM_QSTR(MP_QSTR_STAT_GOT_IP), MP_ROM_INT(CYW43_LINK_UP) },
#else
#define MICROPY_HW_NIC_CYW43
#endif

#if MICROPY_PY_NETWORK_NINAW10
// This Network interface requires the extended socket state.
#ifndef MICROPY_PY_SOCKET_EXTENDED_STATE
#define MICROPY_PY_SOCKET_EXTENDED_STATE    (1)
#endif
extern const struct _mp_obj_type_t mod_network_nic_type_nina;
#define MICROPY_HW_NIC_NINAW10              { MP_ROM_QSTR(MP_QSTR_WLAN), MP_ROM_PTR(&mod_network_nic_type_nina) },
#else
#define MICROPY_HW_NIC_NINAW10
#endif

#if MICROPY_PY_NETWORK_WIZNET5K
extern const struct _mp_obj_type_t mod_network_nic_type_wiznet5k;
#define MICROPY_HW_NIC_WIZNET5K             { MP_ROM_QSTR(MP_QSTR_WIZNET5K), MP_ROM_PTR(&mod_network_nic_type_wiznet5k) },
#else
#define MICROPY_HW_NIC_WIZNET5K
#endif

#ifndef MICROPY_BOARD_NETWORK_INTERFACES
#define MICROPY_BOARD_NETWORK_INTERFACES
#endif

#define MICROPY_PORT_NETWORK_INTERFACES \
    MICROPY_HW_NIC_CYW43 \
    MICROPY_HW_NIC_NINAW10  \
    MICROPY_HW_NIC_WIZNET5K \
    MICROPY_BOARD_NETWORK_INTERFACES \

// Additional entries for use with pendsv_schedule_dispatch.
#ifndef MICROPY_BOARD_PENDSV_ENTRIES
#define MICROPY_BOARD_PENDSV_ENTRIES
#endif

#define MP_STATE_PORT MP_STATE_VM

// Miscellaneous settings

#ifndef MICROPY_HW_USB_VID
#define MICROPY_HW_USB_VID (0x2E8A) // Raspberry Pi
#endif
#ifndef MICROPY_HW_USB_PID
#define MICROPY_HW_USB_PID (0x0005) // RP2 MicroPython
#endif

#ifndef MICROPY_HW_BOOTSEL_DELAY_US
#define MICROPY_HW_BOOTSEL_DELAY_US 8
#endif

#if PICO_ARM
#define MICROPY_MAKE_POINTER_CALLABLE(p) ((void *)((mp_uint_t)(p) | 1))
#endif

#define MP_SSIZE_MAX (0x7fffffff)
typedef intptr_t mp_int_t; // must be pointer size
typedef uintptr_t mp_uint_t; // must be pointer size
typedef intptr_t mp_off_t;

// We need to provide a declaration/definition of alloca()
#include <alloca.h>

#define BINARY_INFO_TAG_MICROPYTHON BINARY_INFO_MAKE_TAG('M', 'P')
#define BINARY_INFO_ID_MP_FROZEN 0x4a99d719
#define MICROPY_FROZEN_LIST_ITEM(name, file) bi_decl(bi_string(BINARY_INFO_TAG_MICROPYTHON, BINARY_INFO_ID_MP_FROZEN, name))

extern uint32_t rosc_random_u32(void);
extern void lwip_lock_acquire(void);
extern void lwip_lock_release(void);

#if MICROPY_PY_BLUETOOTH || MICROPY_PY_BLUETOOTH_CYW43
// Bluetooth code only runs in the scheduler, no locking/mutex required.
#define MICROPY_PY_BLUETOOTH_ENTER uint32_t atomic_state = 0;
#define MICROPY_PY_BLUETOOTH_EXIT (void)atomic_state;
#endif

#ifndef MICROPY_BOARD_STARTUP
#define MICROPY_BOARD_STARTUP()
#endif

#ifndef MICROPY_BOARD_EARLY_INIT
#define MICROPY_BOARD_EARLY_INIT()
#endif

#ifndef MICROPY_BOARD_START_SOFT_RESET
#define MICROPY_BOARD_START_SOFT_RESET()
#endif

#ifndef MICROPY_BOARD_END_SOFT_RESET
#define MICROPY_BOARD_END_SOFT_RESET()
#endif
