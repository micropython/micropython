/*
 * This file is part of the MicroPython project, http://micropython.org/
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
#ifndef MICROPY_INCLUDED_PY_MPSTATE_H
#define MICROPY_INCLUDED_PY_MPSTATE_H

#include <stdint.h>

#include "py/mpconfig.h"
#include "py/mpthread.h"
#include "py/misc.h"
#include "py/nlr.h"
#include "py/obj.h"
#include "py/objlist.h"
#include "py/objexcept.h"

// This file contains structures defining the state of the MicroPython
// memory system, runtime and virtual machine.  The state is a global
// variable, but in the future it is hoped that the state can become local.

enum {
    #if MICROPY_PY_SYS_PS1_PS2
    MP_SYS_MUTABLE_PS1,
    MP_SYS_MUTABLE_PS2,
    #endif
    #if MICROPY_PY_SYS_TRACEBACKLIMIT
    MP_SYS_MUTABLE_TRACEBACKLIMIT,
    #endif
    MP_SYS_MUTABLE_NUM,
};

// This structure contains dynamic configuration for the compiler.
#if MICROPY_DYNAMIC_COMPILER
typedef struct mp_dynamic_compiler_t {
    uint8_t small_int_bits; // must be <= host small_int_bits
    uint8_t native_arch;
    uint8_t nlr_buf_num_regs;
} mp_dynamic_compiler_t;
extern mp_dynamic_compiler_t mp_dynamic_compiler;
#endif

// These are the values for sched_state
#define MP_SCHED_IDLE (1)
#define MP_SCHED_LOCKED (-1)
#define MP_SCHED_PENDING (0) // 0 so it's a quick check in the VM

typedef struct _mp_sched_item_t {
    mp_obj_t func;
    mp_obj_t arg;
} mp_sched_item_t;

// This structure holds information about a single contiguous area of
// memory reserved for the memory manager.
typedef struct _mp_state_mem_area_t {
    #if MICROPY_GC_SPLIT_HEAP
    struct _mp_state_mem_area_t *next;
    #endif

    byte *gc_alloc_table_start;
    size_t gc_alloc_table_byte_len;
    #if MICROPY_ENABLE_FINALISER
    byte *gc_finaliser_table_start;
    #endif
    byte *gc_pool_start;
    byte *gc_pool_end;

    size_t gc_last_free_atb_index;
} mp_state_mem_area_t;

// This structure hold information about the memory allocation system.
typedef struct _mp_state_mem_t {
    #if MICROPY_MEM_STATS
    size_t total_bytes_allocated;
    size_t current_bytes_allocated;
    size_t peak_bytes_allocated;
    #endif

    mp_state_mem_area_t area;

    int gc_stack_overflow;
    MICROPY_GC_STACK_ENTRY_TYPE gc_block_stack[MICROPY_ALLOC_GC_STACK_SIZE];
    #if MICROPY_GC_SPLIT_HEAP
    // Array that tracks the area for each block on gc_block_stack.
    mp_state_mem_area_t *gc_area_stack[MICROPY_ALLOC_GC_STACK_SIZE];
    #endif

    // This variable controls auto garbage collection.  If set to 0 then the
    // GC won't automatically run when gc_alloc can't find enough blocks.  But
    // you can still allocate/free memory and also explicitly call gc_collect.
    uint16_t gc_auto_collect_enabled;

    #if MICROPY_GC_ALLOC_THRESHOLD
    size_t gc_alloc_amount;
    size_t gc_alloc_threshold;
    #endif

    #if MICROPY_GC_SPLIT_HEAP
    mp_state_mem_area_t *gc_last_free_area;
    #endif

    #if MICROPY_PY_GC_COLLECT_RETVAL
    size_t gc_collected;
    #endif

    #if MICROPY_PY_THREAD && !MICROPY_PY_THREAD_GIL
    // This is a global mutex used to make the GC thread-safe.
    mp_thread_mutex_t gc_mutex;
    #endif
} mp_state_mem_t;

// This structure hold runtime and VM information.  It includes a section
// which contains root pointers that must be scanned by the GC.
typedef struct _mp_state_vm_t {
    //
    // CONTINUE ROOT POINTER SECTION
    // This must start at the start of this structure and follows
    // the state in the mp_state_thread_t structure, continuing
    // the root pointer section from there.
    //

    qstr_pool_t *last_pool;

    #if MICROPY_TRACKED_ALLOC
    struct _m_tracked_node_t *m_tracked_head;
    #endif

    // non-heap memory for creating an exception if we can't allocate RAM
    mp_obj_exception_t mp_emergency_exception_obj;

    // memory for exception arguments if we can't allocate RAM
    #if MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF
    #if MICROPY_EMERGENCY_EXCEPTION_BUF_SIZE > 0
    // statically allocated buf (needs to be aligned to mp_obj_t)
    mp_obj_t mp_emergency_exception_buf[MICROPY_EMERGENCY_EXCEPTION_BUF_SIZE / sizeof(mp_obj_t)];
    #else
    // dynamically allocated buf
    byte *mp_emergency_exception_buf;
    #endif
    #endif

    #if MICROPY_KBD_EXCEPTION
    // exception object of type KeyboardInterrupt
    mp_obj_exception_t mp_kbd_exception;
    #endif

    // dictionary with loaded modules (may be exposed as sys.modules)
    mp_obj_dict_t mp_loaded_modules_dict;

    // dictionary for the __main__ module
    mp_obj_dict_t dict_main;

    // dictionary for overridden builtins
    #if MICROPY_CAN_OVERRIDE_BUILTINS
    mp_obj_dict_t *mp_module_builtins_override_dict;
    #endif

    // Include any root pointers registered with MP_REGISTER_ROOT_POINTER().
    #ifndef NO_QSTR
    // Only include root pointer definitions when not doing qstr extraction, because
    // the qstr extraction stage also generates the root pointers header file.
    #include "genhdr/root_pointers.h"
    #endif

    //
    // END ROOT POINTER SECTION
    ////////////////////////////////////////////////////////////

    // pointer and sizes to store interned string data
    // (qstr_last_chunk can be root pointer but is also stored in qstr pool)
    char *qstr_last_chunk;
    size_t qstr_last_alloc;
    size_t qstr_last_used;

    #if MICROPY_PY_THREAD && !MICROPY_PY_THREAD_GIL
    // This is a global mutex used to make qstr interning thread-safe.
    mp_thread_mutex_t qstr_mutex;
    #endif

    #if MICROPY_ENABLE_COMPILER
    mp_uint_t mp_optimise_value;
    #if MICROPY_EMIT_NATIVE
    uint8_t default_emit_opt; // one of MP_EMIT_OPT_xxx
    #endif
    #endif

    // size of the emergency exception buf, if it's dynamically allocated
    #if MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF && MICROPY_EMERGENCY_EXCEPTION_BUF_SIZE == 0
    mp_int_t mp_emergency_exception_buf_size;
    #endif

    #if MICROPY_ENABLE_SCHEDULER
    volatile int16_t sched_state;

    #if MICROPY_SCHEDULER_STATIC_NODES
    // These will usually point to statically allocated memory.  They are not
    // traced by the GC.  They are assumed to be zero'd out before mp_init() is
    // called (usually because this struct lives in the BSS).
    struct _mp_sched_node_t *sched_head;
    struct _mp_sched_node_t *sched_tail;
    #endif

    // These index sched_queue.
    uint8_t sched_len;
    uint8_t sched_idx;
    #endif

    #if MICROPY_PY_THREAD_GIL
    // This is a global mutex used to make the VM/runtime thread-safe.
    mp_thread_mutex_t gil_mutex;
    #endif

    #if MICROPY_OPT_MAP_LOOKUP_CACHE
    // See mp_map_lookup.
    uint8_t map_lookup_cache[MICROPY_OPT_MAP_LOOKUP_CACHE_SIZE];
    #endif
} mp_state_vm_t;

// This structure holds state that is specific to a given thread.
// Everything in this structure is scanned for root pointers.
typedef struct _mp_state_thread_t {
    // Stack top at the start of program
    char *stack_top;

    #if MICROPY_STACK_CHECK
    size_t stack_limit;
    #endif

    #if MICROPY_ENABLE_PYSTACK
    uint8_t *pystack_start;
    uint8_t *pystack_end;
    uint8_t *pystack_cur;
    #endif

    // Locking of the GC is done per thread.
    uint16_t gc_lock_depth;

    ////////////////////////////////////////////////////////////
    // START ROOT POINTER SECTION
    // Everything that needs GC scanning must start here, and
    // is followed by state in the mp_state_vm_t structure.
    //

    mp_obj_dict_t *dict_locals;
    mp_obj_dict_t *dict_globals;

    nlr_buf_t *nlr_top;

    // pending exception object (MP_OBJ_NULL if not pending)
    volatile mp_obj_t mp_pending_exception;

    // If MP_OBJ_STOP_ITERATION is propagated then this holds its argument.
    mp_obj_t stop_iteration_arg;

    #if MICROPY_PY_SYS_SETTRACE
    mp_obj_t prof_trace_callback;
    bool prof_callback_is_executing;
    struct _mp_code_state_t *current_code_state;
    #endif
} mp_state_thread_t;

// This structure combines the above 3 structures.
// The order of the entries are important for root pointer scanning in the GC to work.
typedef struct _mp_state_ctx_t {
    mp_state_thread_t thread;
    mp_state_vm_t vm;
    mp_state_mem_t mem;
} mp_state_ctx_t;

extern mp_state_ctx_t mp_state_ctx;

#define MP_STATE_VM(x) (mp_state_ctx.vm.x)
#define MP_STATE_MEM(x) (mp_state_ctx.mem.x)
#define MP_STATE_MAIN_THREAD(x) (mp_state_ctx.thread.x)

#if MICROPY_PY_THREAD
extern mp_state_thread_t *mp_thread_get_state(void);
#define MP_STATE_THREAD(x) (mp_thread_get_state()->x)
#else
#define MP_STATE_THREAD(x)  MP_STATE_MAIN_THREAD(x)
#endif

#endif // MICROPY_INCLUDED_PY_MPSTATE_H
