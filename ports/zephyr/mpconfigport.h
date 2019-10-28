/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Linaro Limited
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

#define MICROPY_ENABLE_SOURCE_LINE  (1)
#define MICROPY_STACK_CHECK         (1)
#define MICROPY_ENABLE_GC           (1)
#define MICROPY_HELPER_REPL         (1)
#define MICROPY_REPL_AUTO_INDENT    (1)
#define MICROPY_KBD_EXCEPTION       (1)
#define MICROPY_CPYTHON_COMPAT      (0)
#define MICROPY_PY_ASYNC_AWAIT      (0)
#define MICROPY_PY_ATTRTUPLE        (0)
#define MICROPY_PY_BUILTINS_ENUMERATE (0)
#define MICROPY_PY_BUILTINS_FILTER  (0)
#define MICROPY_PY_BUILTINS_MIN_MAX (0)
#define MICROPY_PY_BUILTINS_PROPERTY (0)
#define MICROPY_PY_BUILTINS_RANGE_ATTRS (0)
#define MICROPY_PY_BUILTINS_REVERSED (0)
#define MICROPY_PY_BUILTINS_SET     (0)
#define MICROPY_PY_BUILTINS_STR_COUNT (0)
#define MICROPY_PY_BUILTINS_HELP    (1)
#define MICROPY_PY_BUILTINS_HELP_TEXT zephyr_help_text
#define MICROPY_PY_ARRAY            (0)
#define MICROPY_PY_COLLECTIONS      (0)
#define MICROPY_PY_CMATH            (0)
#define MICROPY_PY_IO               (0)
#define MICROPY_PY_MICROPYTHON_MEM_INFO (1)
#define MICROPY_PY_MACHINE          (1)
#define MICROPY_PY_MACHINE_I2C      (1)
#define MICROPY_PY_MACHINE_I2C_MAKE_NEW machine_hard_i2c_make_new
#define MICROPY_PY_MACHINE_PIN_MAKE_NEW mp_pin_make_new
#define MICROPY_MODULE_WEAK_LINKS   (1)
#define MICROPY_PY_STRUCT           (0)
#ifdef CONFIG_NETWORKING
// If we have networking, we likely want errno comfort
#define MICROPY_PY_UERRNO           (1)
#define MICROPY_PY_USOCKET          (1)
#endif
#define MICROPY_PY_UBINASCII        (1)
#define MICROPY_PY_UHASHLIB         (1)
#define MICROPY_PY_UTIME            (1)
#define MICROPY_PY_UTIME_MP_HAL     (1)
#define MICROPY_PY_ZEPHYR           (1)
#define MICROPY_PY_ZSENSOR          (1)
#define MICROPY_PY_SYS_MODULES      (0)
#define MICROPY_LONGINT_IMPL (MICROPY_LONGINT_IMPL_LONGLONG)
#define MICROPY_FLOAT_IMPL (MICROPY_FLOAT_IMPL_FLOAT)
#define MICROPY_PY_BUILTINS_COMPLEX (0)

// Saving extra crumbs to make sure binary fits in 128K
#define MICROPY_COMP_CONST_FOLDING  (0)
#define MICROPY_COMP_CONST (0)
#define MICROPY_COMP_DOUBLE_TUPLE_ASSIGN (0)

#define MICROPY_PY_SYS_PLATFORM "zephyr"

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

#define MICROPY_MODULE_FROZEN_STR   (1)

typedef int mp_int_t; // must be pointer size
typedef unsigned mp_uint_t; // must be pointer size
typedef long mp_off_t;

#define MP_STATE_PORT MP_STATE_VM

#define MICROPY_PORT_ROOT_POINTERS \
    const char *readline_hist[8];

extern const struct _mp_obj_module_t mp_module_machine;
extern const struct _mp_obj_module_t mp_module_time;
extern const struct _mp_obj_module_t mp_module_usocket;
extern const struct _mp_obj_module_t mp_module_zephyr;
extern const struct _mp_obj_module_t mp_module_zsensor;

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
    { MP_ROM_QSTR(MP_QSTR_machine), MP_ROM_PTR(&mp_module_machine) }, \
    MICROPY_PY_USOCKET_DEF \
    MICROPY_PY_UTIME_DEF \
    MICROPY_PY_ZEPHYR_DEF \
    MICROPY_PY_ZSENSOR_DEF \

// extra built in names to add to the global namespace
#define MICROPY_PORT_BUILTINS \

