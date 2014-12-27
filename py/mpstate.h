/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Damien P. George
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

#ifndef __MICROPY_INCLUDED_MPSTATE_H__
#define __MICROPY_INCLUDED_MPSTATE_H__

#include <stdint.h>

#include "qstr.h"
#include "nlr.h"
#include "obj.h"
#include "objtuple.h"

#define MICROPY_GC_STACK_SIZE (64) // tunable; minimum is 1

typedef struct _mp_obj_exception_t {
    mp_obj_base_t base;
    mp_obj_t traceback; // a list object, holding (file,line,block) as numbers (not Python objects); a hack for now
    mp_obj_tuple_t *args;
} mp_obj_exception_t;

typedef struct _mp_state_t {
    //////// qstr.c ////////

    qstr_pool_t *last_pool;

    //////// stackctrl.c ////////

    // Stack top at the start of program
    char *stack_top;

    //////// lexer.c ////////

    mp_uint_t mp_optimise_value;

    //////// runtime.c ////////

    // pending exception object (MP_OBJ_NULL if not pending)
    mp_obj_t mp_pending_exception;

    // locals and globals need to be pointers because they can be the same in outer module scope
    mp_obj_dict_t *dict_locals;
    mp_obj_dict_t *dict_globals;

    // dictionary for the __main__ module
    mp_obj_dict_t dict_main;

    #if MICROPY_CAN_OVERRIDE_BUILTINS
    mp_obj_dict_t *mp_module_builtins_override_dict;
    #endif

    //////// gc.c ////////

    #if MICROPY_ENABLE_GC
    byte *gc_alloc_table_start;
    mp_uint_t gc_alloc_table_byte_len;
    #if MICROPY_ENABLE_FINALISER
    byte *gc_finaliser_table_start;
    #endif
    mp_uint_t *gc_pool_end;

    int gc_stack_overflow;
    mp_uint_t gc_stack[MICROPY_GC_STACK_SIZE];
    mp_uint_t *gc_sp;
    uint16_t gc_lock_depth;
    uint16_t gc_auto_collect_enabled;
    mp_uint_t gc_last_free_atb_index;
    #endif

    //////// malloc.c ////////

    #if MICROPY_MEM_STATS
    size_t total_bytes_allocated;
    size_t current_bytes_allocated;
    size_t peak_bytes_allocated;
    #endif

    //////// objmodule.c ////////

    mp_map_t mp_loaded_modules_map; // TODO: expose as sys.modules

    //////// objexcept.c ////////

    // Local non-heap memory for allocating an exception when we run out of RAM
    mp_obj_exception_t mp_emergency_exception_obj;

    // Optionally allocated buffer for storing the first argument of an exception
    // allocated when the heap is locked.
    #if MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF
    #if MICROPY_EMERGENCY_EXCEPTION_BUF_SIZE > 0
    byte mp_emergency_exception_buf[MICROPY_EMERGENCY_EXCEPTION_BUF_SIZE];
    #else
    mp_int_t mp_emergency_exception_buf_size;
    byte *mp_emergency_exception_buf;
    #endif
    #endif

} mp_state_t;

#if MICROPY_STACK_CHECK
extern mp_uint_t stack_limit;
#endif

#if MICROPY_ENABLE_GC
extern mp_uint_t *gc_pool_start;
#endif

extern nlr_buf_t *nlr_top;
extern mp_state_t mp_state;

#endif // __MICROPY_INCLUDED_MPSTATE_H__
