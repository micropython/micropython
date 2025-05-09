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
#include "fsl_common.h"

uint32_t trng_random_u32(void);

// Config level
#define MICROPY_CONFIG_ROM_LEVEL (MICROPY_CONFIG_ROM_LEVEL_FULL_FEATURES)

// Memory allocation policies
#if MICROPY_HW_SDRAM_AVAIL
#define MICROPY_GC_STACK_ENTRY_TYPE         uint32_t
#else
#define MICROPY_GC_STACK_ENTRY_TYPE         uint16_t
#endif
#define MICROPY_ALLOC_PATH_MAX              (256)

// MicroPython emitters
#define MICROPY_PERSISTENT_CODE_LOAD        (1)
#define MICROPY_EMIT_THUMB                  (1)
#define MICROPY_EMIT_INLINE_THUMB           (1)

// Optimisations

// Python internal features
#define MICROPY_TRACKED_ALLOC               (MICROPY_SSL_MBEDTLS)
#define MICROPY_READER_VFS                  (1)
#define MICROPY_ENABLE_GC                   (1)
#define MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF  (1)
#define MICROPY_LONGINT_IMPL                (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_SCHEDULER_DEPTH             (8)
#define MICROPY_SCHEDULER_STATIC_NODES      (1)
#define MICROPY_VFS                         (1)

// Control over Python builtins
#define MICROPY_PY_BUILTINS_HELP_TEXT       mimxrt_help_text
#define MICROPY_PY_SYS_PLATFORM             "mimxrt"

// Extended modules
#define MICROPY_EPOCH_IS_1970               (1)
#define MICROPY_PY_TIME_GMTIME_LOCALTIME_MKTIME (1)
#define MICROPY_PY_TIME_TIME_TIME_NS        (1)
#define MICROPY_PY_TIME_INCLUDEFILE         "ports/mimxrt/modtime.c"
#define MICROPY_PY_OS_INCLUDEFILE           "ports/mimxrt/modos.c"
#define MICROPY_PY_OS_DUPTERM               (3)
#define MICROPY_PY_OS_DUPTERM_NOTIFY        (1)
#define MICROPY_PY_OS_SYNC                  (1)
#define MICROPY_PY_OS_UNAME                 (1)
#define MICROPY_PY_OS_URANDOM               (1)
#define MICROPY_PY_RANDOM_SEED_INIT_FUNC    (trng_random_u32())
#define MICROPY_PY_MACHINE                  (1)
#define MICROPY_PY_MACHINE_INCLUDEFILE      "ports/mimxrt/modmachine.c"
#define MICROPY_PY_MACHINE_RESET            (1)
#define MICROPY_PY_MACHINE_BARE_METAL_FUNCS (1)
#define MICROPY_PY_MACHINE_BOOTLOADER       (1)
#define MICROPY_PY_MACHINE_DISABLE_IRQ_ENABLE_IRQ (1)
#define MICROPY_PY_MACHINE_ADC              (1)
#define MICROPY_PY_MACHINE_ADC_INCLUDEFILE  "ports/mimxrt/machine_adc.c"
#define MICROPY_PY_MACHINE_ADC_READ_UV      (1)
#define MICROPY_PY_MACHINE_PIN_MAKE_NEW     mp_pin_make_new
#define MICROPY_PY_MACHINE_BITSTREAM        (1)
#define MICROPY_PY_MACHINE_DHT_READINTO     (1)
#define MICROPY_PY_MACHINE_PULSE            (1)
#define MICROPY_PY_MACHINE_PWM              (1)
#define MICROPY_PY_MACHINE_PWM_INCLUDEFILE  "ports/mimxrt/machine_pwm.c"
#define MICROPY_PY_MACHINE_I2C              (1)
#ifndef MICROPY_PY_MACHINE_I2S
#define MICROPY_PY_MACHINE_I2S              (0)
#endif
#define MICROPY_PY_MACHINE_I2S_INCLUDEFILE  "ports/mimxrt/machine_i2s.c"
#define MICROPY_PY_MACHINE_I2S_CONSTANT_RX  (RX)
#define MICROPY_PY_MACHINE_I2S_CONSTANT_TX  (TX)
#define MICROPY_PY_MACHINE_I2S_MCK          (1)
#define MICROPY_PY_MACHINE_I2S_RING_BUF     (1)
#ifndef MICROPY_PY_MACHINE_SDCARD
#define MICROPY_PY_MACHINE_SDCARD           (1)
#endif
#define MICROPY_PY_MACHINE_SOFTI2C          (1)
#define MICROPY_PY_MACHINE_SPI              (1)
#define MICROPY_PY_MACHINE_SOFTSPI          (1)
#define MICROPY_PY_MACHINE_TIMER            (1)
#define MICROPY_PY_MACHINE_WDT              (1)
#define MICROPY_PY_MACHINE_WDT_INCLUDEFILE  "ports/mimxrt/machine_wdt.c"
#define MICROPY_PY_MACHINE_WDT_TIMEOUT_MS   (1)
#define MICROPY_SOFT_TIMER_TICKS_MS         systick_ms
#define MICROPY_PY_MACHINE_UART             (1)
#define MICROPY_PY_MACHINE_UART_INCLUDEFILE "ports/mimxrt/machine_uart.c"
#define MICROPY_PY_MACHINE_UART_SENDBREAK   (1)
#ifndef MICROPY_PY_MACHINE_UART_IRQ
#define MICROPY_PY_MACHINE_UART_IRQ         (1)
#endif
#define MICROPY_PY_ONEWIRE                  (1)
#define MICROPY_PY_MACHINE_BOOTLOADER       (1)

// fatfs configuration used in ffconf.h
#define MICROPY_FATFS_ENABLE_LFN            (2)
#define MICROPY_FATFS_LFN_CODE_PAGE         437 /* 1=SFN/ANSI 437=LFN/U.S.(OEM) */
#define MICROPY_FATFS_USE_LABEL             (1)
#define MICROPY_FATFS_RPATH                 (2)
#define MICROPY_FATFS_MULTI_PARTITION       (1)
#define MICROPY_FATFS_MAX_SS                (4096)
#define MICROPY_FATFS_EXFAT                 (1)

#ifndef MICROPY_PY_NETWORK
#define MICROPY_PY_NETWORK                  (1)
#endif
#ifndef MICROPY_PY_SOCKET
#define MICROPY_PY_SOCKET                   (1)
#endif
#define MICROPY_PY_WEBSOCKET                (MICROPY_PY_LWIP)
#define MICROPY_PY_WEBREPL                  (MICROPY_PY_LWIP)
#define MICROPY_PY_LWIP_SOCK_RAW            (MICROPY_PY_LWIP)
#define MICROPY_PY_HASHLIB_MD5              (MICROPY_PY_SSL)
#define MICROPY_PY_HASHLIB_SHA1             (MICROPY_PY_SSL)
#define MICROPY_PY_CRYPTOLIB                (MICROPY_PY_SSL)
#ifndef MICROPY_PY_NETWORK_PPP_LWIP
#define MICROPY_PY_NETWORK_PPP_LWIP         (MICROPY_PY_LWIP)
#endif
#define MICROPY_PY_LWIP_PPP                 (MICROPY_PY_NETWORK_PPP_LWIP)

#ifndef MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE
#define MICROPY_PY_BLUETOOTH_ENABLE_CENTRAL_MODE (1)
#endif

#ifndef MICROPY_PY_BLUETOOTH_ENABLE_L2CAP_CHANNELS
#define MICROPY_PY_BLUETOOTH_ENABLE_L2CAP_CHANNELS (MICROPY_BLUETOOTH_NIMBLE)
#endif

#ifndef MICROPY_PY_NETWORK_HOSTNAME_DEFAULT
#define MICROPY_PY_NETWORK_HOSTNAME_DEFAULT "mpy-mimxrt"
#endif

#define MICROPY_HW_ENABLE_USBDEV            (1)
// Enable USB-CDC serial port
#ifndef MICROPY_HW_USB_CDC
#define MICROPY_HW_USB_CDC                  (1)
#define MICROPY_HW_USB_CDC_1200BPS_TOUCH    (1)
#endif
// Enable USB Mass Storage with FatFS filesystem.
#ifndef MICROPY_HW_USB_MSC
#define MICROPY_HW_USB_MSC                  (0)
#endif

// Hooks to add builtins

#if defined(IOMUX_TABLE_ENET)
extern const struct _mp_obj_type_t network_lan_type;
#define MICROPY_HW_NIC_ETH                  { MP_ROM_QSTR(MP_QSTR_LAN), MP_ROM_PTR(&network_lan_type) },
#else
#define MICROPY_HW_NIC_ETH
#endif

#if MICROPY_PY_NETWORK_CYW43
extern const struct _mp_obj_type_t mp_network_cyw43_type;
#define MICROPY_HW_NIC_CYW43                { MP_ROM_QSTR(MP_QSTR_WLAN), MP_ROM_PTR(&mp_network_cyw43_type) },
#else
#define MICROPY_HW_NIC_CYW43
#endif

#ifndef MICROPY_BOARD_NETWORK_INTERFACES
#define MICROPY_BOARD_NETWORK_INTERFACES
#endif

#define MICROPY_PORT_NETWORK_INTERFACES \
    MICROPY_HW_NIC_ETH  \
    MICROPY_HW_NIC_CYW43 \
    MICROPY_BOARD_NETWORK_INTERFACES \

#ifndef MICROPY_BOARD_ROOT_POINTERS
#define MICROPY_BOARD_ROOT_POINTERS
#endif

// Additional entries for use with pendsv_schedule_dispatch.
#ifndef MICROPY_BOARD_PENDSV_ENTRIES
#define MICROPY_BOARD_PENDSV_ENTRIES
#endif

#define MP_STATE_PORT MP_STATE_VM

// Miscellaneous settings
#ifndef MICROPY_HW_USB_VID
#define MICROPY_HW_USB_VID (0xf055)
#endif

#ifndef MICROPY_HW_USB_PID
#define MICROPY_HW_USB_PID (0x9802)
#endif

#ifndef  MICROPY_EVENT_POLL_HOOK
#define MICROPY_EVENT_POLL_HOOK \
    do { \
        extern void mp_handle_pending(bool); \
        mp_handle_pending(true); \
        __WFE(); \
    } while (0);
#endif

#define MICROPY_MAKE_POINTER_CALLABLE(p) ((void *)((mp_uint_t)(p) | 1))

#define MP_HAL_CLEANINVALIDATE_DCACHE(addr, size) \
    (SCB_CleanInvalidateDCache_by_Addr((uint32_t *)((uint32_t)addr & ~0x1f), \
    ((uint32_t)((uint8_t *)addr + size + 0x1f) & ~0x1f) - ((uint32_t)addr & ~0x1f)))

#define MP_HAL_CLEAN_DCACHE(addr, size) \
    (SCB_CleanDCache_by_Addr((uint32_t *)((uint32_t)addr & ~0x1f), \
    ((uint32_t)((uint8_t *)addr + size + 0x1f) & ~0x1f) - ((uint32_t)addr & ~0x1f)))

#define MP_SSIZE_MAX (0x7fffffff)
typedef int mp_int_t; // must be pointer size
typedef unsigned mp_uint_t; // must be pointer size
typedef long mp_off_t;

// Need an implementation of the log2 function which is not a macro.
#define MP_NEED_LOG2 (1)

// Need to provide a declaration/definition of alloca()
#include <alloca.h>
