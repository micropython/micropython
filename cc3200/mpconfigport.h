/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 * Copyright (c) 2015 Daniel Campora
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

#ifndef __INCLUDED_MPCONFIGPORT_H
#define __INCLUDED_MPCONFIGPORT_H

#include <stdint.h>

// options to control how Micro Python is built

#define MICROPY_ALLOC_PATH_MAX                      (128)
#define MICROPY_EMIT_THUMB                          (0)
#define MICROPY_EMIT_INLINE_THUMB                   (0)
#define MICROPY_COMP_MODULE_CONST                   (1)
#define MICROPY_ENABLE_GC                           (1)
#define MICROPY_ENABLE_FINALISER                    (1)
#define MICROPY_STACK_CHECK                         (0)
#define MICROPY_HELPER_REPL                         (1)
#define MICROPY_ENABLE_SOURCE_LINE                  (1)
#define MICROPY_ENABLE_DOC_STRING                   (0)
#define MICROPY_ERROR_REPORTING                     (MICROPY_ERROR_REPORTING_TERSE)
#define MICROPY_LONGINT_IMPL                        (MICROPY_LONGINT_IMPL_MPZ)
#define MICROPY_FLOAT_IMPL                          (MICROPY_FLOAT_IMPL_NONE)
#define MICROPY_OPT_COMPUTED_GOTO                   (0)
#define MICROPY_OPT_CACHE_MAP_LOOKUP_IN_BYTECODE    (0)

/* Enable FatFS LFNs
    0: Disable LFN feature.
    1: Enable LFN with static working buffer on the BSS. Always NOT reentrant.
    2: Enable LFN with dynamic working buffer on the STACK.
    3: Enable LFN with dynamic working buffer on the HEAP.
*/
#define MICROPY_ENABLE_LFN                          (0)
#define MICROPY_LFN_CODE_PAGE                       (1)
#define MICROPY_STREAMS_NON_BLOCK                   (1)
#define MICROPY_MODULE_WEAK_LINKS                   (0)
#define MICROPY_CAN_OVERRIDE_BUILTINS               (0)
#define MICROPY_PY_BUILTINS_STR_UNICODE             (1)
#define MICROPY_PY_BUILTINS_MEMORYVIEW              (1)
#define MICROPY_PY_BUILTINS_FROZENSET               (1)
#define MICROPY_PY_BUILTINS_EXECFILE                (1)
#define MICROPY_PY_MICROPYTHON_MEM_INFO             (0)
#define MICROPY_PY_SYS_EXIT                         (1)
#define MICROPY_PY_SYS_STDFILES                     (1)
#define MICROPY_PY_CMATH                            (0)
#define MICROPY_PY_IO                               (1)
#define MICROPY_PY_IO_FILEIO                        (1)
#define MICROPY_PY_UCTYPES                          (1)
#define MICROPY_PY_UZLIB                            (0)
#define MICROPY_PY_UJSON                            (1)
#define MICROPY_PY_URE                              (1)
#define MICROPY_PY_UHEAPQ                           (1)
#define MICROPY_PY_UHASHLIB                         (0)

#define MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF      (1)
#define MICROPY_EMERGENCY_EXCEPTION_BUF_SIZE        (0)

// extra built in names to add to the global namespace
extern const struct _mp_obj_fun_builtin_t mp_builtin_help_obj;
extern const struct _mp_obj_fun_builtin_t mp_builtin_input_obj;
extern const struct _mp_obj_fun_builtin_t mp_builtin_open_obj;
#define MICROPY_PORT_BUILTINS \
    { MP_OBJ_NEW_QSTR(MP_QSTR_help),  (mp_obj_t)&mp_builtin_help_obj },   \
    { MP_OBJ_NEW_QSTR(MP_QSTR_input), (mp_obj_t)&mp_builtin_input_obj },  \
    { MP_OBJ_NEW_QSTR(MP_QSTR_open),  (mp_obj_t)&mp_builtin_open_obj },   \

// extra built in modules to add to the list of known ones
extern const struct _mp_obj_module_t pyb_module;
extern const struct _mp_obj_module_t mp_module_ure;
extern const struct _mp_obj_module_t mp_module_ujson;
extern const struct _mp_obj_module_t mp_module_uheapq;
extern const struct _mp_obj_module_t mp_module_uos;
extern const struct _mp_obj_module_t mp_module_utime;
extern const struct _mp_obj_module_t mp_module_uselect;
extern const struct _mp_obj_module_t mp_module_usocket;
extern const struct _mp_obj_module_t mp_module_network;

#define MICROPY_PORT_BUILTIN_MODULES \
    { MP_OBJ_NEW_QSTR(MP_QSTR_pyb),     (mp_obj_t)&pyb_module },          \
    { MP_OBJ_NEW_QSTR(MP_QSTR_os),      (mp_obj_t)&mp_module_uos },       \
    { MP_OBJ_NEW_QSTR(MP_QSTR_time),    (mp_obj_t)&mp_module_utime },     \
    { MP_OBJ_NEW_QSTR(MP_QSTR_select),  (mp_obj_t)&mp_module_uselect },   \
    { MP_OBJ_NEW_QSTR(MP_QSTR_socket),  (mp_obj_t)&mp_module_usocket },   \
    { MP_OBJ_NEW_QSTR(MP_QSTR_network), (mp_obj_t)&mp_module_network },   \
    { MP_OBJ_NEW_QSTR(MP_QSTR_re),      (mp_obj_t)&mp_module_ure },       \
    { MP_OBJ_NEW_QSTR(MP_QSTR_json),    (mp_obj_t)&mp_module_ujson },     \
    { MP_OBJ_NEW_QSTR(MP_QSTR_heapq),   (mp_obj_t)&mp_module_uheapq },    \

// extra constants
#define MICROPY_PORT_CONSTANTS \
    { MP_OBJ_NEW_QSTR(MP_QSTR_pyb),     (mp_obj_t)&pyb_module },          \

// vm state and root pointers for the gc
#define MP_STATE_PORT MP_STATE_VM
#define MICROPY_PORT_ROOT_POINTERS                                        \
    const char *readline_hist[8];                                         \
    mp_obj_t mp_const_user_interrupt;                                     \
    mp_obj_t pyb_config_main;                                             \
    mp_obj_list_t pyb_extint_list;                                        \
    mp_obj_list_t pyb_uart_list;                                          \
    mp_obj_list_t mod_network_nic_list;                                   \


// type definitions for the specific machine
#define BYTES_PER_WORD                              (4)
#define MICROPY_MAKE_POINTER_CALLABLE(p)            ((void*)((mp_uint_t)(p) | 1))

#define UINT_FMT                                    "%u"
#define INT_FMT                                     "%d"

typedef int32_t         mp_int_t;                   // must be pointer size
typedef unsigned int    mp_uint_t;                  // must be pointer size
typedef void            *machine_ptr_t;             // must be of pointer size
typedef const void      *machine_const_ptr_t;       // must be of pointer size
typedef long            mp_off_t;

#define MICROPY_BEGIN_ATOMIC_SECTION()              disable_irq()
#define MICROPY_END_ATOMIC_SECTION(state)           enable_irq(state)

// assembly functions to handle critical sections, interrupt
// disabling/enabling and sleep mode enter/exit
#include "cc3200_asm.h"

// There is no classical C heap in bare-metal ports, only Python
// garbage-collected heap. For completeness, emulate C heap via
// GC heap. Note that MicroPython core never uses malloc() and friends,
// so these defines are mostly to help extension module writers.
#define malloc                                      gc_alloc
#define free                                        gc_free
#define realloc                                     gc_realloc

// We need to provide a declaration/definition of alloca()
#include <alloca.h>

// Include board specific configuration
#include "mpconfigboard.h"

#define MICROPY_HAL_H                               "cc3200_hal.h"
#define MICROPY_PIN_DEFS_PORT_H                     "pin_defs_cc3200.h"
#define MICROPY_PORT_HAS_TELNET                     (1)
#define MICROPY_PORT_HAS_FTP                        (1)

#endif // __INCLUDED_MPCONFIGPORT_H
