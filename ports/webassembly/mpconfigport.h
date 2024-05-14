/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2022 Damien P. George
 * Copyright (c) 2017, 2018 Rami Ali
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

// Options to control how MicroPython is built for this port, overriding
// defaults in py/mpconfig.h.

#include <stdint.h>
#include <stdlib.h> // for malloc, for MICROPY_GC_SPLIT_HEAP_AUTO

// Variant-specific definitions.
#include "mpconfigvariant.h"

#ifndef MICROPY_CONFIG_ROM_LEVEL
#define MICROPY_CONFIG_ROM_LEVEL (MICROPY_CONFIG_ROM_LEVEL_EXTRA_FEATURES)
#endif

#define MICROPY_ALLOC_PATH_MAX      (256)
#define MICROPY_PERSISTENT_CODE_LOAD (1)
#define MICROPY_COMP_ALLOW_TOP_LEVEL_AWAIT (1)
#define MICROPY_READER_VFS          (MICROPY_VFS)
#define MICROPY_ENABLE_GC           (1)
#define MICROPY_ENABLE_PYSTACK      (1)
#define MICROPY_STACK_CHECK         (0)
#define MICROPY_KBD_EXCEPTION       (1)
#define MICROPY_REPL_EVENT_DRIVEN   (1)
#define MICROPY_LONGINT_IMPL        (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_ENABLE_DOC_STRING   (1)
#define MICROPY_WARNINGS            (1)
#define MICROPY_ERROR_PRINTER       (&mp_stderr_print)
#define MICROPY_FLOAT_IMPL          (MICROPY_FLOAT_IMPL_DOUBLE)
#define MICROPY_USE_INTERNAL_ERRNO  (1)
#define MICROPY_USE_INTERNAL_PRINTF (0)

#define MICROPY_EPOCH_IS_1970       (1)
#define MICROPY_PY_RANDOM_SEED_INIT_FUNC (mp_js_random_u32())
#define MICROPY_PY_TIME_GMTIME_LOCALTIME_MKTIME (1)
#define MICROPY_PY_TIME_TIME_TIME_NS (1)
#define MICROPY_PY_TIME_INCLUDEFILE "ports/webassembly/modtime.c"
#ifndef MICROPY_VFS
#define MICROPY_VFS                 (1)
#endif
#define MICROPY_VFS_POSIX           (MICROPY_VFS)
#define MICROPY_PY_SYS_PLATFORM     "webassembly"

#ifndef MICROPY_PY_JS
#define MICROPY_PY_JS (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

#ifndef MICROPY_PY_JSFFI
#define MICROPY_PY_JSFFI (MICROPY_CONFIG_ROM_LEVEL_AT_LEAST_EXTRA_FEATURES)
#endif

#define MICROPY_EVENT_POLL_HOOK \
    do { \
        extern void mp_handle_pending(bool); \
        mp_handle_pending(true); \
    } while (0);

// Whether the VM will periodically call mp_js_hook(), which checks for
// interrupt characters on stdin (or equivalent input).
#ifndef MICROPY_VARIANT_ENABLE_JS_HOOK
#define MICROPY_VARIANT_ENABLE_JS_HOOK (0)
#endif

#if MICROPY_VARIANT_ENABLE_JS_HOOK
#define MICROPY_VM_HOOK_COUNT (10)
#define MICROPY_VM_HOOK_INIT static uint vm_hook_divisor = MICROPY_VM_HOOK_COUNT;
#define MICROPY_VM_HOOK_POLL if (--vm_hook_divisor == 0) { \
        vm_hook_divisor = MICROPY_VM_HOOK_COUNT; \
        extern void mp_js_hook(void); \
        mp_js_hook(); \
}
#define MICROPY_VM_HOOK_LOOP MICROPY_VM_HOOK_POLL
#define MICROPY_VM_HOOK_RETURN MICROPY_VM_HOOK_POLL
#endif

// type definitions for the specific machine

#define MP_SSIZE_MAX (0x7fffffff)

// This port is intended to be 32-bit, but unfortunately, int32_t for
// different targets may be defined in different ways - either as int
// or as long. This requires different printf formatting specifiers
// to print such value. So, we avoid int32_t and use int directly.
#define UINT_FMT "%u"
#define INT_FMT "%d"
typedef int mp_int_t; // must be pointer size
typedef unsigned mp_uint_t; // must be pointer size
typedef long mp_off_t;

#define MICROPY_HW_BOARD_NAME "JS"
#define MICROPY_HW_MCU_NAME "Emscripten"

#define MP_STATE_PORT MP_STATE_VM

#if MICROPY_VFS
// _GNU_SOURCE must be defined to get definitions of DT_xxx symbols from dirent.h.
#define _GNU_SOURCE
#endif

extern const struct _mp_print_t mp_stderr_print;

uint32_t mp_js_random_u32(void);
