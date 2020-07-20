/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Linaro Limited
 * Copyright 2020 NXP
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
#include <alloca.h>

// Include Zephyr's autoconf.h, which should be made first by Zephyr makefiles
#include "autoconf.h"
// Included here to get basic Zephyr environment (macros, etc.)
#include <zephyr.h>

// Usually passed from Makefile
#ifndef MICROPY_HEAP_SIZE
#define MICROPY_HEAP_SIZE (16 * 1024)
#endif

#define MICROPY_ENABLE_SOURCE_LINE          IS_ENABLED(CONFIG_MICROPY_ENABLE_SOURCE_LINE)
#define MICROPY_STACK_CHECK                 IS_ENABLED(CONFIG_MICROPY_STACK_CHECK)
#define MICROPY_ENABLE_GC                   IS_ENABLED(CONFIG_MICROPY_ENABLE_GC)
#define MICROPY_HELPER_REPL                 IS_ENABLED(CONFIG_MICROPY_HELPER_REPL)
#define MICROPY_REPL_AUTO_INDENT            IS_ENABLED(CONFIG_MICROPY_REPL_AUTO_INDENT)
#define MICROPY_KBD_EXCEPTION               IS_ENABLED(CONFIG_MICROPY_KBD_EXCEPTION)
#define MICROPY_CPYTHON_COMPAT              IS_ENABLED(CONFIG_MICROPY_CPYTHON_COMPAT)
#define MICROPY_PY_ASYNC_AWAIT              IS_ENABLED(CONFIG_MICROPY_PY_ASYNC_AWAIT)
#define MICROPY_PY_ATTRTUPLE                IS_ENABLED(CONFIG_MICROPY_PY_ATTRTUPLE)
#define MICROPY_PY_BUILTINS_ENUMERATE       IS_ENABLED(CONFIG_MICROPY_PY_BUILTINS_ENUMERATE)
#define MICROPY_PY_BUILTINS_FILTER          IS_ENABLED(CONFIG_MICROPY_PY_BUILTINS_FILTER)
#define MICROPY_PY_BUILTINS_MIN_MAX         IS_ENABLED(CONFIG_MICROPY_PY_BUILTINS_MIN_MAX)
#define MICROPY_PY_BUILTINS_PROPERTY        IS_ENABLED(CONFIG_MICROPY_PY_BUILTINS_PROPERTY)
#define MICROPY_PY_BUILTINS_RANGE_ATTRS     IS_ENABLED(CONFIG_MICROPY_PY_BUILTINS_RANGE_ATTRS)
#define MICROPY_PY_BUILTINS_REVERSED        IS_ENABLED(CONFIG_MICROPY_PY_BUILTINS_REVERSED)
#define MICROPY_PY_BUILTINS_SET             IS_ENABLED(CONFIG_MICROPY_PY_BUILTINS_SET)
#define MICROPY_PY_BUILTINS_SLICE           IS_ENABLED(CONFIG_MICROPY_PY_BUILTINS_SLICE)
#define MICROPY_PY_BUILTINS_STR_COUNT       IS_ENABLED(CONFIG_MICROPY_PY_BUILTINS_STR_COUNT)
#define MICROPY_PY_BUILTINS_HELP            IS_ENABLED(CONFIG_MICROPY_PY_BUILTINS_HELP)
#define MICROPY_PY_BUILTINS_HELP_TEXT       zephyr_help_text
#define MICROPY_PY_ARRAY                    IS_ENABLED(CONFIG_MICROPY_PY_ARRAY)
#define MICROPY_PY_COLLECTIONS              IS_ENABLED(CONFIG_MICROPY_PY_COLLECTIONS)
#define MICROPY_PY_CMATH                    IS_ENABLED(CONFIG_MICROPY_PY_CMATH)
#define MICROPY_PY_IO                       IS_ENABLED(CONFIG_MICROPY_PY_IO)
#define MICROPY_PY_MICROPYTHON_MEM_INFO     IS_ENABLED(CONFIG_MICROPY_PY_MICROPYTHON_MEM_INFO)
#define MICROPY_PY_MACHINE                  IS_ENABLED(CONFIG_MICROPY_PY_MACHINE)
#define MICROPY_PY_MACHINE_I2C              IS_ENABLED(CONFIG_MICROPY_PY_MACHINE_I2C)
#define MICROPY_PY_MACHINE_PIN_MAKE_NEW     mp_pin_make_new
#define MICROPY_MODULE_WEAK_LINKS           IS_ENABLED(CONFIG_MICROPY_MODULE_WEAK_LINKS)
#define MICROPY_PY_STRUCT                   IS_ENABLED(CONFIG_MICROPY_PY_STRUCT)
#define MICROPY_PY_UERRNO                   IS_ENABLED(CONFIG_MICROPY_PY_UERRNO)
#define MICROPY_PY_USOCKET                  IS_ENABLED(CONFIG_MICROPY_PY_USOCKET)
#define MICROPY_PY_UBINASCII                IS_ENABLED(CONFIG_MICROPY_PY_UBINASCII)
#define MICROPY_PY_UHASHLIB                 IS_ENABLED(CONFIG_MICROPY_PY_UHASHLIB)
#define MICROPY_PY_UOS                      IS_ENABLED(CONFIG_MICROPY_PY_UOS)
#define MICROPY_PY_UTIME                    IS_ENABLED(CONFIG_MICROPY_PY_UTIME)
#define MICROPY_PY_UTIME_MP_HAL             IS_ENABLED(CONFIG_MICROPY_PY_UTIME_MP_HAL)
#define MICROPY_PY_ZEPHYR                   IS_ENABLED(CONFIG_MICROPY_PY_ZEPHYR)
#define MICROPY_PY_ZSENSOR                  IS_ENABLED(CONFIG_MICROPY_PY_ZSENSOR)
#define MICROPY_PY_SYS_MODULES              IS_ENABLED(CONFIG_MICROPY_PY_SYS_MODULES)

#ifdef CONFIG_MICROPY_LONGINT_IMPL_NONE
#define MICROPY_LONGINT_IMPL (MICROPY_LONGINT_IMPL_NONE)
#elif CONFIG_MICROPY_LONGINT_IMPL_LONGLONG
#define MICROPY_LONGINT_IMPL (MICROPY_LONGINT_IMPL_LONGLONG)
#elif CONFIG_MICROPY_LONGINT_IMPL_MPZ
#define MICROPY_LONGINT_IMPL (MICROPY_LONGINT_IMPL_MPZ)
#endif

#ifdef CONFIG_MICROPY_FLOAT_IMPL_NONE
#define MICROPY_FLOAT_IMPL (MICROPY_FLOAT_IMPL_NONE)
#elif CONFIG_MICROPY_FLOAT_IMPL_FLOAT
#define MICROPY_FLOAT_IMPL (MICROPY_FLOAT_IMPL_FLOAT)
#elif CONFIG_MICROPY_FLOAT_IMPL_DOUBLE
#define MICROPY_FLOAT_IMPL (MICROPY_FLOAT_IMPL_DOUBLE)
#endif

#define MICROPY_PY_BUILTINS_COMPLEX         IS_ENABLED(CONFIG_MICROPY_PY_BUILTINS_COMPLEX)
#define MICROPY_ENABLE_SCHEDULER            IS_ENABLED(CONFIG_MICROPY_ENABLE_SCHEDULER)
#define MICROPY_VFS                         IS_ENABLED(CONFIG_MICROPY_VFS)
#define MICROPY_READER_VFS                  (MICROPY_VFS)

// fatfs configuration used in ffconf.h
#define MICROPY_FATFS_ENABLE_LFN       (1)
#define MICROPY_FATFS_LFN_CODE_PAGE    437 /* 1=SFN/ANSI 437=LFN/U.S.(OEM) */
#define MICROPY_FATFS_USE_LABEL        (1)
#define MICROPY_FATFS_RPATH            (2)
#define MICROPY_FATFS_NORTC            (1)

#define MICROPY_COMP_CONST_FOLDING          IS_ENABLED(CONFIG_MICROPY_COMP_CONST_FOLDING)
#define MICROPY_COMP_CONST                  IS_ENABLED(CONFIG_MICROPY_COMP_CONST)
#define MICROPY_COMP_DOUBLE_TUPLE_ASSIGN    IS_ENABLED(CONFIG_MICROPY_COMP_DOUBLE_TUPLE_ASSIGN)

#ifdef CONFIG_MICROPY_PY_SYS_PLATFORM
#define MICROPY_PY_SYS_PLATFORM "zephyr"
#endif

#ifdef CONFIG_BOARD
#define MICROPY_HW_BOARD_NAME "zephyr-" CONFIG_BOARD
#else
#define MICROPY_HW_BOARD_NAME "zephyr-generic"
#endif

#ifdef CONFIG_SOC
#define MICROPY_HW_MCU_NAME CONFIG_SOC
#else
#define MICROPY_HW_MCU_NAME "unknown-cpu"
#endif

#define MICROPY_MODULE_FROZEN_STR           IS_ENABLED(CONFIG_MICROPY_MODULE_FROZEN_STR)

typedef int mp_int_t; // must be pointer size
typedef unsigned mp_uint_t; // must be pointer size
typedef long mp_off_t;

#define MP_STATE_PORT MP_STATE_VM

#define MICROPY_PORT_ROOT_POINTERS \
    const char *readline_hist[8]; \
    void *machine_pin_irq_list; /* Linked list of pin irq objects */

extern const struct _mp_obj_module_t mp_module_machine;
extern const struct _mp_obj_module_t mp_module_time;
extern const struct _mp_obj_module_t mp_module_uos;
extern const struct _mp_obj_module_t mp_module_usocket;
extern const struct _mp_obj_module_t mp_module_zephyr;
extern const struct _mp_obj_module_t mp_module_zsensor;

#if MICROPY_PY_MACHINE
#define MICROPY_PY_MACHINE_DEF { MP_ROM_QSTR(MP_QSTR_machine), MP_ROM_PTR(&mp_module_machine) },
#else
#define MICROPY_PY_MACHINE_DEF
#endif

#if MICROPY_PY_UOS
#define MICROPY_PY_UOS_DEF { MP_ROM_QSTR(MP_QSTR_uos), MP_ROM_PTR(&mp_module_uos) },
#else
#define MICROPY_PY_UOS_DEF
#endif

#if MICROPY_PY_USOCKET
#define MICROPY_PY_USOCKET_DEF { MP_ROM_QSTR(MP_QSTR_usocket), MP_ROM_PTR(&mp_module_usocket) },
#else
#define MICROPY_PY_USOCKET_DEF
#endif

#if MICROPY_PY_UTIME
#define MICROPY_PY_UTIME_DEF { MP_ROM_QSTR(MP_QSTR_utime), MP_ROM_PTR(&mp_module_time) },
#else
#define MICROPY_PY_UTIME_DEF
#endif

#if MICROPY_PY_ZEPHYR
#define MICROPY_PY_ZEPHYR_DEF { MP_ROM_QSTR(MP_QSTR_zephyr), MP_ROM_PTR(&mp_module_zephyr) },
#else
#define MICROPY_PY_ZEPHYR_DEF
#endif

#if MICROPY_PY_ZSENSOR
#define MICROPY_PY_ZSENSOR_DEF { MP_ROM_QSTR(MP_QSTR_zsensor), MP_ROM_PTR(&mp_module_zsensor) },
#else
#define MICROPY_PY_ZSENSOR_DEF
#endif

#define MICROPY_PORT_BUILTIN_MODULES \
    MICROPY_PY_MACHINE_DEF \
    MICROPY_PY_UOS_DEF \
    MICROPY_PY_USOCKET_DEF \
    MICROPY_PY_UTIME_DEF \
    MICROPY_PY_ZEPHYR_DEF \
    MICROPY_PY_ZSENSOR_DEF \

// extra built in names to add to the global namespace
#define MICROPY_PORT_BUILTINS \
    { MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&mp_builtin_open_obj) },

#define MICROPY_BEGIN_ATOMIC_SECTION irq_lock
#define MICROPY_END_ATOMIC_SECTION irq_unlock
