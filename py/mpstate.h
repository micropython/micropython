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
#ifndef __MICROPY_INCLUDED_PY_MPSTATE_H__
#define __MICROPY_INCLUDED_PY_MPSTATE_H__

#include <stdint.h>

#include "py/mpconfig.h"
#include "py/misc.h"
#include "py/nlr.h"
#include "py/obj.h"
#include "py/objexcept.h"

extern nlr_buf_t *nlr_top;

// should be zerod on init
typedef struct _mp_state_mem_t {
    #if MICROPY_MEM_STATS
    size_t total_bytes_allocated;
    size_t current_bytes_allocated;
    size_t peak_bytes_allocated;
    #endif

    byte *gc_alloc_table_start;
    mp_uint_t gc_alloc_table_byte_len;
    #if MICROPY_ENABLE_FINALISER
    byte *gc_finaliser_table_start;
    #endif
    // We initialise gc_pool_start to a dummy value so it stays out of the bss
    // section.  This makes sure we don't trace this pointer in a collect cycle.
    // If we did trace it, it would make the first block of the heap always
    // reachable, and hence we can never free that block.
    mp_uint_t *gc_pool_start;
    mp_uint_t *gc_pool_end;

    int gc_stack_overflow;
    mp_uint_t gc_stack[MICROPY_ALLOC_GC_STACK_SIZE];
    mp_uint_t *gc_sp;
    uint16_t gc_lock_depth;

    // This variable controls auto garbage collection.  If set to 0 then the
    // GC won't automatically run when gc_alloc can't find enough blocks.  But
    // you can still allocate/free memory and also explicitly call gc_collect.
    uint16_t gc_auto_collect_enabled;

    mp_uint_t gc_last_free_atb_index;
} mp_state_mem_t;

// zero
typedef struct _mp_state_vm_t {
    // root pointer section (need last_pool at start, stack_top after end)

    qstr_pool_t *last_pool;

    #if MICROPY_CAN_OVERRIDE_BUILTINS
    mp_obj_dict_t *mp_module_builtins_override_dict;
    #endif

    // Local non-heap memory for allocating an exception when we run out of RAM
    mp_obj_exception_t mp_emergency_exception_obj;

    #if MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF
    #if MICROPY_EMERGENCY_EXCEPTION_BUF_SIZE > 0
    byte mp_emergency_exception_buf[MICROPY_EMERGENCY_EXCEPTION_BUF_SIZE];
    #else
    byte *mp_emergency_exception_buf;
    #endif
    #endif

    mp_map_t mp_loaded_modules_map; // TODO: expose as sys.modules

    // pending exception object (MP_OBJ_NULL if not pending)
    mp_obj_t mp_pending_exception;

    // dictionary for the __main__ module
    mp_obj_dict_t dict_main;

    // include any root pointers defined by a port
    MICROPY_PORT_ROOT_POINTERS

    // Stack top at the start of program
    char *stack_top;

    #if MICROPY_STACK_CHECK
    mp_uint_t stack_limit;
    #endif

    mp_uint_t mp_optimise_value;

    #if MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF && MICROPY_EMERGENCY_EXCEPTION_BUF_SIZE == 0
    mp_int_t mp_emergency_exception_buf_size;
    #endif

} mp_state_vm_t;

typedef struct _mp_state_ctx_t {
    // locals and globals need to be pointers because they can be the same in outer module scope
    mp_obj_dict_t *dict_locals;
    mp_obj_dict_t *dict_globals;
    // mp_state_mem_t *mem;
    // mp_state_vm_t *vm;
} mp_state_ctx_t;

extern mp_state_mem_t mp_state_mem;
#define MP_STATE_MEM(x) (mp_state_mem.x)

extern mp_state_vm_t mp_state_vm;
#define MP_STATE_VM(x) (mp_state_vm.x)

extern mp_state_ctx_t mp_state_ctx;
#define MP_STATE_CTX(x) (mp_state_ctx.x)

#endif // __MICROPY_INCLUDED_PY_MPSTATE_H__
