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

// This structure contains dynamic configuration for the compiler.
#if MICROPY_DYNAMIC_COMPILER
typedef struct mp_dynamic_compiler_t {
    uint8_t small_int_bits; // must be <= host small_int_bits
    bool opt_cache_map_lookup_in_bytecode;
    bool py_builtins_str_unicode;
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

// This structure hold information about the memory allocation system.
typedef struct _mp_state_mem_t {
    #if MICROPY_MEM_STATS
    size_t total_bytes_allocated;
    size_t current_bytes_allocated;
    size_t peak_bytes_allocated;
    #endif

    byte *gc_alloc_table_start;
    size_t gc_alloc_table_byte_len;
    #if MICROPY_ENABLE_FINALISER
    byte *gc_finaliser_table_start;
    #endif
    byte *gc_pool_start;
    byte *gc_pool_end;

    int gc_stack_overflow;
    size_t gc_stack[MICROPY_ALLOC_GC_STACK_SIZE];
    uint16_t gc_lock_depth;

    // This variable controls auto garbage collection.  If set to 0 then the
    // GC won't automatically run when gc_alloc can't find enough blocks.  But
    // you can still allocate/free memory and also explicitly call gc_collect.
    uint16_t gc_auto_collect_enabled;

    #if MICROPY_GC_ALLOC_THRESHOLD
    size_t gc_alloc_amount;
    size_t gc_alloc_threshold;
    #endif

    size_t gc_last_free_atb_index;

    #if MICROPY_PY_GC_COLLECT_RETVAL
    size_t gc_collected;
    #endif

    #if MICROPY_PY_THREAD
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

    // pending exception object (MP_OBJ_NULL if not pending)
    volatile mp_obj_t mp_pending_exception;

    #if MICROPY_ENABLE_SCHEDULER
    mp_sched_item_t sched_stack[MICROPY_SCHEDULER_DEPTH];
    #endif

    // current exception being handled, for sys.exc_info()
    #if MICROPY_PY_SYS_EXC_INFO
    mp_obj_base_t *cur_exception;
    #endif

    // dictionary for the __main__ module
    mp_obj_dict_t dict_main;

    // these two lists must be initialised per port, after the call to mp_init
    mp_obj_list_t mp_sys_path_obj;
    mp_obj_list_t mp_sys_argv_obj;

    // dictionary for overridden builtins
    #if MICROPY_CAN_OVERRIDE_BUILTINS
    mp_obj_dict_t *mp_module_builtins_override_dict;
    #endif

    // include any root pointers defined by a port
    MICROPY_PORT_ROOT_POINTERS

    // root pointers for extmod

    #if MICROPY_REPL_EVENT_DRIVEN
    vstr_t *repl_line;
    #endif

    #if MICROPY_PY_OS_DUPTERM
    mp_obj_t dupterm_objs[MICROPY_PY_OS_DUPTERM];
    #endif

    #if MICROPY_PY_LWIP_SLIP
    mp_obj_t lwip_slip_stream;
    #endif

    #if MICROPY_VFS
    struct _mp_vfs_mount_t *vfs_cur;
    struct _mp_vfs_mount_t *vfs_mount_table;
    #endif

    //
    // END ROOT POINTER SECTION
    ////////////////////////////////////////////////////////////

    // pointer and sizes to store interned string data
    // (qstr_last_chunk can be root pointer but is also stored in qstr pool)
    byte *qstr_last_chunk;
    size_t qstr_last_alloc;
    size_t qstr_last_used;

    #if MICROPY_PY_THREAD
    // This is a global mutex used to make qstr interning thread-safe.
    mp_thread_mutex_t qstr_mutex;
    #endif

    #if MICROPY_ENABLE_COMPILER
    mp_uint_t mp_optimise_value;
    #endif

    // size of the emergency exception buf, if it's dynamically allocated
    #if MICROPY_ENABLE_EMERGENCY_EXCEPTION_BUF && MICROPY_EMERGENCY_EXCEPTION_BUF_SIZE == 0
    mp_int_t mp_emergency_exception_buf_size;
    #endif

    #if MICROPY_ENABLE_SCHEDULER
    volatile int16_t sched_state;
    uint16_t sched_sp;
    #endif

    #if MICROPY_PY_THREAD_GIL
    // This is a global mutex used to make the VM/runtime thread-safe.
    mp_thread_mutex_t gil_mutex;
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

    ////////////////////////////////////////////////////////////
    // START ROOT POINTER SECTION
    // Everything that needs GC scanning must start here, and
    // is followed by state in the mp_state_vm_t structure.
    //

    mp_obj_dict_t *dict_locals;
    mp_obj_dict_t *dict_globals;

    nlr_buf_t *nlr_top;
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

#if MICROPY_PY_THREAD
extern mp_state_thread_t *mp_thread_get_state(void);
#define MP_STATE_THREAD(x) (mp_thread_get_state()->x)
#else
#define MP_STATE_THREAD(x) (mp_state_ctx.thread.x)
#endif

#endif // MICROPY_INCLUDED_PY_MPSTATE_H
