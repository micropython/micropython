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
#include "hardware/spi.h"
#include "hardware/sync.h"
#include "pico/binary_info.h"
#include "mpconfigboard.h"
#if MICROPY_HW_USB_MSC
#include "hardware/flash.h"
#endif

// Board and hardware specific configuration
#define MICROPY_HW_MCU_NAME                     "RP2040"
#define MICROPY_HW_ENABLE_UART_REPL             (0) // useful if there is no USB
#define MICROPY_HW_ENABLE_USBDEV                (1)

#ifndef MICROPY_CONFIG_ROM_LEVEL
#define MICROPY_CONFIG_ROM_LEVEL                (MICROPY_CONFIG_ROM_LEVEL_EXTRA_FEATURES)
#endif

// Memory allocation policies
#define MICROPY_GC_STACK_ENTRY_TYPE             uint16_t
#define MICROPY_ALLOC_PATH_MAX                  (128)
#define MICROPY_QSTR_BYTES_IN_HASH              (1)

// MicroPython emitters
#define MICROPY_PERSISTENT_CODE_LOAD            (1)
#define MICROPY_EMIT_THUMB                      (1)
#define MICROPY_EMIT_THUMB_ARMV7M               (0)
#define MICROPY_EMIT_INLINE_THUMB               (1)
#define MICROPY_EMIT_INLINE_THUMB_FLOAT         (0)
#define MICROPY_EMIT_INLINE_THUMB_ARMV7M        (0)

// Optimisations
#define MICROPY_OPT_COMPUTED_GOTO               (1)

// Python internal features
#define MICROPY_READER_VFS                      (1)
#define MICROPY_ENABLE_GC                       (1)
#define MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF  (1)
#define MICROPY_LONGINT_IMPL                    (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_FLOAT_IMPL                      (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_USE_INTERNAL_ERRNO              (1)
#define MICROPY_SCHEDULER_DEPTH                 (8)

// Fine control over Python builtins, classes, modules, etc
#define MICROPY_PY_BUILTINS_HELP_TEXT           rp2_help_text
#define MICROPY_PY_SYS_PLATFORM                 "rp2"
#define MICROPY_PY_THREAD                       (1)
#define MICROPY_PY_THREAD_GIL                   (0)

// Extended modules
#define MICROPY_EPOCH_IS_1970                   (1)
#define MICROPY_PY_UOS_INCLUDEFILE              "ports/rp2/moduos.c"
#define MICROPY_PY_UOS_UNAME                    (1)
#define MICROPY_PY_UOS_URANDOM                  (1)
#define MICROPY_PY_URE_MATCH_GROUPS             (1)
#define MICROPY_PY_URE_MATCH_SPAN_START_END     (1)
#define MICROPY_PY_UCRYPTOLIB                   (1)
#define MICROPY_PY_UTIME_MP_HAL                 (1)
#define MICROPY_PY_URANDOM_SEED_INIT_FUNC       (rosc_random_u32())
#define MICROPY_PY_MACHINE                      (1)
#define MICROPY_PY_MACHINE_PIN_MAKE_NEW         mp_pin_make_new
#define MICROPY_PY_MACHINE_BITSTREAM            (1)
#define MICROPY_PY_MACHINE_PULSE                (1)
#define MICROPY_PY_MACHINE_PWM                  (1)
#define MICROPY_PY_MACHINE_PWM_DUTY_U16_NS      (1)
#define MICROPY_PY_MACHINE_PWM_INCLUDEFILE      "ports/rp2/machine_pwm.c"
#define MICROPY_PY_MACHINE_I2C                  (1)
#define MICROPY_PY_MACHINE_SOFTI2C              (1)
#define MICROPY_PY_MACHINE_SPI                  (1)
#define MICROPY_PY_MACHINE_SPI_MSB              (SPI_MSB_FIRST)
#define MICROPY_PY_MACHINE_SPI_LSB              (SPI_LSB_FIRST)
#define MICROPY_PY_MACHINE_SOFTSPI              (1)
#define MICROPY_PY_ONEWIRE                      (1)
#define MICROPY_VFS                             (1)
#define MICROPY_VFS_LFS2                        (1)
#define MICROPY_VFS_FAT                         (1)
#define MICROPY_SSL_MBEDTLS                     (1)

// fatfs configuration
#define MICROPY_FATFS_ENABLE_LFN                (1)
#define MICROPY_FATFS_LFN_CODE_PAGE             437 /* 1=SFN/ANSI 437=LFN/U.S.(OEM) */
#define MICROPY_FATFS_RPATH                     (2)
#if MICROPY_HW_USB_MSC
#define MICROPY_FATFS_USE_LABEL                 (1)
#define MICROPY_FATFS_MULTI_PARTITION           (1)
// Set FatFS block size to flash sector size to avoid caching
// the flash sector in memory to support smaller block sizes.
#define MICROPY_FATFS_MAX_SS                    (FLASH_SECTOR_SIZE)
#endif

#if MICROPY_VFS_FAT && MICROPY_HW_USB_MSC
#define mp_type_fileio mp_type_vfs_fat_fileio
#define mp_type_textio mp_type_vfs_fat_textio
#elif MICROPY_VFS_LFS2
// Use VfsLfs2's types for fileio/textio
#define mp_type_fileio mp_type_vfs_lfs2_fileio
#define mp_type_textio mp_type_vfs_lfs2_textio
#endif

// Use VFS's functions for import stat and builtin open
#define mp_import_stat mp_vfs_import_stat
#define mp_builtin_open_obj mp_vfs_open_obj

// Hooks to add builtins

#define MICROPY_PORT_BUILTINS \
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&mp_builtin_open_obj) },

extern const struct _mp_obj_module_t mp_module_network;
extern const struct _mp_obj_module_t mp_module_onewire;
extern const struct _mp_obj_module_t mp_module_rp2;
extern const struct _mp_obj_module_t mp_module_usocket;
extern const struct _mp_obj_module_t mp_module_utime;

#if MICROPY_PY_USOCKET
#define SOCKET_BUILTIN_MODULE               { MP_ROM_QSTR(MP_QSTR_usocket), MP_ROM_PTR(&mp_module_usocket) },
#else
#define SOCKET_BUILTIN_MODULE
#endif
#if MICROPY_PY_NETWORK
#define NETWORK_BUILTIN_MODULE              { MP_ROM_QSTR(MP_QSTR_network), MP_ROM_PTR(&mp_module_network) },
#define NETWORK_ROOT_POINTERS               mp_obj_list_t mod_network_nic_list;
#else
#define NETWORK_BUILTIN_MODULE
#define NETWORK_ROOT_POINTERS
#endif

#if MICROPY_PY_BLUETOOTH
#define MICROPY_PORT_ROOT_POINTER_BLUETOOTH struct _machine_uart_obj_t *mp_bthci_uart;
#else
#define MICROPY_PORT_ROOT_POINTER_BLUETOOTH
#endif

#if MICROPY_BLUETOOTH_NIMBLE
struct _mp_bluetooth_nimble_root_pointers_t;
struct _mp_bluetooth_nimble_malloc_t;
#define MICROPY_PORT_ROOT_POINTER_BLUETOOTH_NIMBLE struct _mp_bluetooth_nimble_malloc_t *bluetooth_nimble_memory; struct _mp_bluetooth_nimble_root_pointers_t *bluetooth_nimble_root_pointers;
#else
#define MICROPY_PORT_ROOT_POINTER_BLUETOOTH_NIMBLE
#endif

#if MICROPY_PY_NETWORK_NINAW10
// This Network interface requires the extended socket state.
#ifndef MICROPY_PY_USOCKET_EXTENDED_STATE
#define MICROPY_PY_USOCKET_EXTENDED_STATE   (1)
#endif
// It also requires an additional root pointer for the SPI object.
#define MICROPY_PORT_ROOT_POINTER_NINAW10   struct _machine_spi_obj_t *mp_wifi_spi;
extern const struct _mod_network_nic_type_t mod_network_nic_type_nina;
#define MICROPY_HW_NIC_NINAW10              { MP_ROM_QSTR(MP_QSTR_WLAN), MP_ROM_PTR(&mod_network_nic_type_nina) },
#else
#define MICROPY_HW_NIC_NINAW10
#define MICROPY_PORT_ROOT_POINTER_NINAW10
#endif

#define MICROPY_PORT_BUILTIN_MODULES \
    { MP_OBJ_NEW_QSTR(MP_QSTR__onewire), (mp_obj_t)&mp_module_onewire }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR__rp2), (mp_obj_t)&mp_module_rp2 }, \
    { MP_ROM_QSTR(MP_QSTR_utime), MP_ROM_PTR(&mp_module_utime) }, \
    SOCKET_BUILTIN_MODULE \
    NETWORK_BUILTIN_MODULE \

#ifndef MICROPY_BOARD_NETWORK_INTERFACES
#define MICROPY_BOARD_NETWORK_INTERFACES
#endif

#define MICROPY_PORT_NETWORK_INTERFACES \
    MICROPY_HW_NIC_NINAW10  \
    MICROPY_BOARD_NETWORK_INTERFACES \

#ifndef MICROPY_BOARD_ROOT_POINTERS
#define MICROPY_BOARD_ROOT_POINTERS
#endif

#define MICROPY_PORT_ROOT_POINTERS \
    const char *readline_hist[8]; \
    void *machine_pin_irq_obj[30]; \
    void *rp2_pio_irq_obj[2]; \
    void *rp2_state_machine_irq_obj[8]; \
    void *rp2_uart_rx_buffer[2]; \
    void *rp2_uart_tx_buffer[2]; \
    void *machine_i2s_obj[2]; \
    NETWORK_ROOT_POINTERS \
    MICROPY_BOARD_ROOT_POINTERS \
    MICROPY_PORT_ROOT_POINTER_NINAW10 \
    MICROPY_PORT_ROOT_POINTER_BLUETOOTH \
        MICROPY_PORT_ROOT_POINTER_BLUETOOTH_NIMBLE \

#define MP_STATE_PORT MP_STATE_VM

// Miscellaneous settings

// TODO need to look and see if these could/should be spinlock/mutex
#define MICROPY_BEGIN_ATOMIC_SECTION()     save_and_disable_interrupts()
#define MICROPY_END_ATOMIC_SECTION(state)  restore_interrupts(state)

#if MICROPY_HW_ENABLE_USBDEV
#define MICROPY_HW_USBDEV_TASK_HOOK extern void tud_task(void); tud_task();
#define MICROPY_VM_HOOK_COUNT (10)
#define MICROPY_VM_HOOK_INIT static uint vm_hook_divisor = MICROPY_VM_HOOK_COUNT;
#define MICROPY_VM_HOOK_POLL if (--vm_hook_divisor == 0) { \
        vm_hook_divisor = MICROPY_VM_HOOK_COUNT; \
        MICROPY_HW_USBDEV_TASK_HOOK \
}
#define MICROPY_VM_HOOK_LOOP MICROPY_VM_HOOK_POLL
#define MICROPY_VM_HOOK_RETURN MICROPY_VM_HOOK_POLL
#else
#define MICROPY_HW_USBDEV_TASK_HOOK
#endif

#define MICROPY_EVENT_POLL_HOOK \
    do { \
        extern void mp_handle_pending(bool); \
        mp_handle_pending(true); \
        best_effort_wfe_or_timeout(make_timeout_time_ms(1)); \
        MICROPY_HW_USBDEV_TASK_HOOK \
    } while (0);

#define MICROPY_MAKE_POINTER_CALLABLE(p) ((void *)((mp_uint_t)(p) | 1))

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
