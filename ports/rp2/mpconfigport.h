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

#define MICROPY_PY_LVGL                         (1)
#define MICROPY_PY_LODEPNG                      (1)

// Board and hardware specific configuration
#define MICROPY_HW_MCU_NAME                     "RP2040"
#define MICROPY_HW_ENABLE_UART_REPL             (0) // useful if there is no USB
#define MICROPY_HW_ENABLE_USBDEV                (1)

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

// Python internal features
#define MICROPY_READER_VFS                      (1)
#define MICROPY_ENABLE_GC                       (1)
#define MICROPY_ENABLE_FINALISER                (1)
#define MICROPY_STACK_CHECK                     (1)
#define MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF  (1)
#define MICROPY_KBD_EXCEPTION                   (1)
#define MICROPY_HELPER_REPL                     (1)
#define MICROPY_REPL_AUTO_INDENT                (1)
#define MICROPY_LONGINT_IMPL                    (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_ENABLE_SOURCE_LINE              (1)
#define MICROPY_FLOAT_IMPL                      (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_STREAMS_NON_BLOCK               (1)
#define MICROPY_MODULE_BUILTIN_INIT             (1)
#define MICROPY_MODULE_WEAK_LINKS               (1)
#define MICROPY_CAN_OVERRIDE_BUILTINS           (1)
#define MICROPY_ENABLE_SCHEDULER                (1)
#define MICROPY_SCHEDULER_DEPTH                 (8)

// Fine control over Python builtins, classes, modules, etc
#define MICROPY_PY_FUNCTION_ATTRS               (1)
#define MICROPY_PY_DESCRIPTORS                  (1)
#define MICROPY_PY_DELATTR_SETATTR              (1)
#define MICROPY_PY_FSTRINGS                     (1)
#define MICROPY_PY_BUILTINS_STR_UNICODE         (1)
#define MICROPY_PY_BUILTINS_STR_CENTER          (1)
#define MICROPY_PY_BUILTINS_STR_PARTITION       (1)
#define MICROPY_PY_BUILTINS_STR_SPLITLINES      (1)
#define MICROPY_PY_BUILTINS_MEMORYVIEW          (1)
#define MICROPY_PY_BUILTINS_SLICE_ATTRS         (1)
#define MICROPY_PY_BUILTINS_SLICE_INDICES       (1)
#define MICROPY_PY_BUILTINS_FROZENSET           (1)
#define MICROPY_PY_BUILTINS_ROUND_INT           (1)
#define MICROPY_PY_ALL_SPECIAL_METHODS          (1)
#define MICROPY_PY_REVERSE_SPECIAL_METHODS      (1)
#define MICROPY_PY_BUILTINS_COMPILE             (1)
#define MICROPY_PY_BUILTINS_INPUT               (1)
#define MICROPY_PY_BUILTINS_POW3                (1)
#define MICROPY_PY_BUILTINS_HELP                (1)
#define MICROPY_PY_BUILTINS_HELP_TEXT           rp2_help_text
#define MICROPY_PY_BUILTINS_HELP_MODULES        (1)
#define MICROPY_PY_MICROPYTHON_MEM_INFO         (1)
#define MICROPY_PY_ARRAY_SLICE_ASSIGN           (1)
#define MICROPY_PY_COLLECTIONS_DEQUE            (1)
#define MICROPY_PY_COLLECTIONS_ORDEREDDICT      (1)
#define MICROPY_PY_MATH_SPECIAL_FUNCTIONS       (1)
#define MICROPY_PY_MATH_FACTORIAL               (1)
#define MICROPY_PY_MATH_ISCLOSE                 (1)
#define MICROPY_PY_CMATH                        (1)
#define MICROPY_PY_IO_IOBASE                    (1)
#define MICROPY_PY_IO_FILEIO                    (1)
#define MICROPY_PY_SYS_MAXSIZE                  (1)
#define MICROPY_PY_SYS_STDFILES                 (1)
#define MICROPY_PY_SYS_STDIO_BUFFER             (1)
#define MICROPY_PY_SYS_PLATFORM                 "rp2"
#define MICROPY_PY_UERRNO                       (1)
#define MICROPY_PY_THREAD                       (1)
#define MICROPY_PY_THREAD_GIL                   (0)

// Extended modules
#define MICROPY_EPOCH_IS_1970                   (1)
#define MICROPY_PY_UASYNCIO                     (1)
#define MICROPY_PY_UCTYPES                      (1)
#define MICROPY_PY_UZLIB                        (1)
#define MICROPY_PY_UJSON                        (1)
#define MICROPY_PY_URE                          (1)
#define MICROPY_PY_URE_MATCH_GROUPS             (1)
#define MICROPY_PY_URE_MATCH_SPAN_START_END     (1)
#define MICROPY_PY_URE_SUB                      (1)
#define MICROPY_PY_UHASHLIB                     (1)
#define MICROPY_PY_UBINASCII                    (1)
#define MICROPY_PY_UBINASCII_CRC32              (1)
#define MICROPY_PY_UTIME_MP_HAL                 (1)
#define MICROPY_PY_URANDOM                      (1)
#define MICROPY_PY_URANDOM_EXTRA_FUNCS          (1)
#define MICROPY_PY_URANDOM_SEED_INIT_FUNC       (rosc_random_u32())
#define MICROPY_PY_USELECT                      (1)
#define MICROPY_PY_MACHINE                      (1)
#define MICROPY_PY_MACHINE_PIN_MAKE_NEW         mp_pin_make_new
#define MICROPY_PY_MACHINE_PULSE                (1)
#define MICROPY_PY_MACHINE_I2C                  (1)
#define MICROPY_PY_MACHINE_SPI                  (1)
#define MICROPY_PY_MACHINE_SPI_MSB              (SPI_MSB_FIRST)
#define MICROPY_PY_MACHINE_SPI_LSB              (SPI_LSB_FIRST)
#define MICROPY_PY_FRAMEBUF                     (1)
#define MICROPY_VFS                             (1)
#define MICROPY_VFS_LFS2                        (1)
#define MICROPY_VFS_FAT                         (1)

// fatfs configuration
#define MICROPY_FATFS_ENABLE_LFN                (1)
#define MICROPY_FATFS_LFN_CODE_PAGE             437 /* 1=SFN/ANSI 437=LFN/U.S.(OEM) */
#define MICROPY_FATFS_RPATH                     (2)

// Use VfsLfs2's types for fileio/textio
#define mp_type_fileio mp_type_vfs_lfs2_fileio
#define mp_type_textio mp_type_vfs_lfs2_textio

// Use VFS's functions for import stat and builtin open
#define mp_import_stat mp_vfs_import_stat
#define mp_builtin_open_obj mp_vfs_open_obj

// Hooks to add builtins

#define MICROPY_PORT_BUILTINS \
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&mp_builtin_open_obj) },

extern const struct _mp_obj_module_t mp_module_machine;
extern const struct _mp_obj_module_t mp_module_onewire;
extern const struct _mp_obj_module_t mp_module_rp2;
extern const struct _mp_obj_module_t mp_module_uos;
extern const struct _mp_obj_module_t mp_module_utime;
extern const struct _mp_obj_module_t mp_module_lvgl;
extern const struct _mp_obj_module_t mp_module_lodepng;

#if MICROPY_PY_LVGL
#define MICROPY_PORT_LVGL_DEF \
    { MP_OBJ_NEW_QSTR(MP_QSTR_lvgl), (mp_obj_t)&mp_module_lvgl },

#else
#define MICROPY_PORT_LVGL_DEF
#endif

#if MICROPY_PY_LODEPNG
#define MICROPY_PORT_LODEPNG_DEF { MP_OBJ_NEW_QSTR(MP_QSTR_lodepng), (mp_obj_t)&mp_module_lodepng },
#else
#define MICROPY_PORT_LODEPNG_DEF
#endif

#define MICROPY_PORT_BUILTIN_MODULES \
    { MP_OBJ_NEW_QSTR(MP_QSTR_machine), (mp_obj_t)&mp_module_machine }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR__onewire), (mp_obj_t)&mp_module_onewire }, \
    { MP_OBJ_NEW_QSTR(MP_QSTR__rp2), (mp_obj_t)&mp_module_rp2 }, \
    { MP_ROM_QSTR(MP_QSTR_uos), MP_ROM_PTR(&mp_module_uos) }, \
    { MP_ROM_QSTR(MP_QSTR_utime), MP_ROM_PTR(&mp_module_utime) }, \
    MICROPY_PORT_LVGL_DEF \
    MICROPY_PORT_LODEPNG_DEF \

#if MICROPY_PY_LVGL
#ifndef MICROPY_INCLUDED_PY_MPSTATE_H
#define MICROPY_INCLUDED_PY_MPSTATE_H
#include "lib/lv_bindings/lvgl/src/misc/lv_gc.h"
#undef MICROPY_INCLUDED_PY_MPSTATE_H
#else
#include "lib/lv_bindings/lvgl/src/misc/lv_gc.h"
#endif
#else
#define LV_ROOTS
#endif

#ifndef MICROPY_BOARD_ROOT_POINTERS
#define MICROPY_BOARD_ROOT_POINTERS
#endif

#define MICROPY_PORT_ROOT_POINTERS \
    LV_ROOTS \
    void *mp_lv_user_data; \
    const char *readline_hist[8]; \
    void *machine_pin_irq_obj[30]; \
    void *rp2_pio_irq_obj[2]; \
    void *rp2_state_machine_irq_obj[8]; \
    void *rp2_uart_rx_buffer[2]; \
    void *rp2_uart_tx_buffer[2]; \
    MICROPY_BOARD_ROOT_POINTERS \

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
