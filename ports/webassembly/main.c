/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2021 Damien P. George and 2017, 2018 Rami Ali
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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "py/builtin.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/mperrno.h"
#include "extmod/vfs.h"
#include "extmod/vfs_posix.h"
#include "shared/runtime/pyexec.h"

#include "emscripten.h"
#include <emscripten/stack.h>  // emscripten_stack_get_current (JSPI C-stack scan)
#include "lexer_dedent.h"
#include "library.h"
#include "proxy_c.h"

// This counter tracks the current depth of calls into C code that originated
// externally, ie from JavaScript.  When the counter is 0 that corresponds to
// the top-level call into C.
static size_t external_call_depth = 0;

#if MICROPY_ENABLE_VM_YIELD
// Of those external calls, how many are suspend-capable entry points
// (mp_js_do_exec / _import). Suspending to the event loop is only safe when every
// external frame is one of these, i.e. external_call_depth equals this —
// otherwise a synchronous proxy re-entry sits in between. Under JSPI that suspend
// would fail outright ("trying to suspend without WebAssembly.promising"); under
// Asyncify it could unwind through a frame the JS caller isn't awaiting.
static size_t mp_js_suspendable_depth = 0;
#define MP_JS_SUSPENDABLE_INC() (++mp_js_suspendable_depth)
#define MP_JS_SUSPENDABLE_DEC() (--mp_js_suspendable_depth)
#else
#define MP_JS_SUSPENDABLE_INC()
#define MP_JS_SUSPENDABLE_DEC()
#endif

// Emscripten defaults to a 64k C-stack, so our limit should be less than that.
#define CSTACK_SIZE (32 * 1024)

// emscripten_scan_registers() spills and scans CPU registers for GC roots, but
// it is implemented via Asyncify and is unavailable under JSPI (calling it there
// reads out of bounds). JSPI builds set this to 0 and instead scan the C stack
// explicitly between a tracked top-of-stack and the current SP (see below).
#ifndef MICROPY_GC_SCAN_REGISTERS
#define MICROPY_GC_SCAN_REGISTERS (1)
#endif

// Under JSPI the running program executes on a stack-switched stack whose bounds
// emscripten doesn't know (emscripten_scan_stack would read out of bounds, and
// registers can't be spilled). So we track the top of the live MicroPython C
// stack ourselves — the address of a local at the outermost entry point — and
// the mid-loop GC scans from there down to the current SP. This is the only
// place GC roots held by a long-running loop can live (the value stack and
// locals are in the alloca'd code_state on the C stack), so a conservative scan
// of that range preserves them without needing the registers.
#if MICROPY_GC_SPLIT_HEAP_AUTO && !MICROPY_GC_SCAN_REGISTERS
#define MICROPY_GC_TRACK_CSTACK (1)
#else
#define MICROPY_GC_TRACK_CSTACK (0)
#endif

#if MICROPY_GC_TRACK_CSTACK
static void *mp_js_cstack_top = NULL;
// Record the top of the C stack at the outermost external (JS->wasm) entry. Used
// as a macro so the marker's address is in the *caller's* frame (mp_js_do_exec /
// mp_js_do_import), above every live MicroPython root.
#define MP_JS_NOTE_CSTACK_TOP() do { \
        volatile char _stack_marker; \
        if (external_call_depth == 1) { mp_js_cstack_top = (void *)&_stack_marker; } \
} while (0)
#else
#define MP_JS_NOTE_CSTACK_TOP()
#endif

#if MICROPY_GC_SPLIT_HEAP_AUTO
static void gc_collect_top_level(mp_obj_t root_obj);
#if MICROPY_ENABLE_VM_YIELD
// Collect now if a collection is pending, scanning the running program's C stack
// (and, on Asyncify, registers) for roots. Defined with the rest of the GC glue
// below; called from the VM yield hook so long-running loops that never reach the
// top level still reclaim garbage instead of growing the heap. See mp_js_yield().
static void gc_collect_if_pending(void);
#endif
#endif

void external_call_depth_inc(void) {
    ++external_call_depth;
}

void external_call_depth_dec(mp_obj_t root_obj) {
    --external_call_depth;
    // The condition always reads external_call_depth (so it isn't flagged as
    // set-but-unused in configs that don't otherwise read it); the body is only
    // needed when the GC collects at the top level.
    if (external_call_depth == 0) {
        #if MICROPY_GC_SPLIT_HEAP_AUTO
        gc_collect_top_level(root_obj);
        #endif
    }
}

#if MICROPY_ENABLE_VM_YIELD

// Cooperative yield. The MicroPython entry points are built to suspend to the JS
// event loop mid-execution — via Asyncify, or JSPI Wasm stack switching — so we
// can hand the event loop a turn. This keeps the page responsive during long
// running or self-looping Python (input delivered, output flushed) without the
// script having to cooperate. It is driven from the VM hook (see mpconfigport.h)
// and mp_hal_delay_ms().
//
// Yields only happen while it is safe to suspend, i.e. inside one of the
// suspend-capable entry points (external_call_depth > 0), and are throttled to
// roughly one frame so compute-bound code isn't crippled.

#ifndef MICROPY_VM_YIELD_INTERVAL_MS
#define MICROPY_VM_YIELD_INTERVAL_MS (16.0)
#endif

static double mp_js_last_yield_ms = -1.0;

// True only when the stack can actually be suspended: we're inside a
// suspend-capable entry point and there is no synchronous proxy re-entry above us.
static inline bool mp_js_can_suspend(void) {
    return mp_js_suspendable_depth > 0 && external_call_depth == mp_js_suspendable_depth;
}

// Optional per-variant hook, invoked at a yield point while it is safe to
// suspend (a host embedding can override it to e.g. block here while paused).
// The default does nothing.
MP_WEAK void mp_js_yield_hook(void) {
}

void mp_js_yield(void) {
    #if MICROPY_VARIANT_ENABLE_JS_HOOK
    // The cooperative yield takes over the VM hook from the plain JS_HOOK path
    // (see mpconfigport.h), so poll the JS-side interrupt hook here to preserve
    // mid-execution Ctrl-C (node stdin) for variants that enable it. Runs on every
    // call, independent of whether it is safe to suspend below.
    extern void mp_js_hook(void);
    mp_js_hook();
    #endif
    #if MICROPY_GC_SPLIT_HEAP_AUTO
    // Reclaim garbage mid-loop. Under split-heap-auto gc_collect() defers to the
    // top level (where the C stack is clean); a program that loops forever never
    // gets there, so do a real, root-scanning collection here at the VM hook — a
    // safe point between bytecodes — whenever the heap has asked to grow.
    gc_collect_if_pending();
    #endif
    if (!mp_js_can_suspend()) {
        return;
    }
    double now = emscripten_get_now();
    if (mp_js_last_yield_ms < 0.0) {
        mp_js_last_yield_ms = now;  // prime the throttle without yielding
        return;
    }
    if (now - mp_js_last_yield_ms >= MICROPY_VM_YIELD_INTERVAL_MS) {
        mp_js_yield_hook();
        mp_js_last_yield_ms = emscripten_get_now();
        emscripten_sleep(0);
    }
}

// Reset the yield throttle; call after something else has just yielded (e.g. a
// display flip) to avoid an immediate redundant yield.
void mp_js_yield_reset(void) {
    mp_js_last_yield_ms = emscripten_get_now();
}

// Sleep via the event loop for the whole delay when it is safe to suspend
// (responsive, no busy-spin); returns false if suspending isn't safe so the
// caller can fall back to a busy wait. Used by mp_hal_delay_ms().
//
// The delay is sliced so a scheduled exception (e.g. a KeyboardInterrupt posted
// by the host to stop a script) breaks the sleep promptly instead of having to
// wait out the whole duration: mp_handle_pending() raises it between slices.
#ifndef MICROPY_VM_SLEEP_SLICE_MS
#define MICROPY_VM_SLEEP_SLICE_MS (16)
#endif

bool mp_js_sleep_ms(mp_uint_t ms) {
    if (!mp_js_can_suspend()) {
        return false;
    }
    mp_uint_t remaining = ms;
    while (remaining > 0) {
        mp_uint_t step = remaining < MICROPY_VM_SLEEP_SLICE_MS ? remaining : MICROPY_VM_SLEEP_SLICE_MS;
        emscripten_sleep(step);
        remaining -= step;
        mp_handle_pending(true);  // raises (does not return) if an exception is pending
    }
    mp_js_yield_reset();
    return true;
}

#endif

void mp_js_init(int pystack_size, int heap_size) {
    mp_cstack_init_with_sp_here(CSTACK_SIZE);

    #if MICROPY_ENABLE_PYSTACK
    mp_obj_t *pystack = (mp_obj_t *)malloc(pystack_size * sizeof(mp_obj_t));
    mp_pystack_init(pystack, pystack + pystack_size);
    #endif

    #if MICROPY_ENABLE_GC
    char *heap = (char *)malloc(heap_size * sizeof(char));
    gc_init(heap, heap + heap_size);
    #endif

    #if MICROPY_GC_SPLIT_HEAP_AUTO
    #if MICROPY_ENABLE_VM_YIELD
    // This build collects mid-execution from the VM hook (gc_collect_if_pending),
    // so the allocation threshold is the memory/smoothness knob for self-looping
    // programs: a collection is requested once this many bytes have been allocated
    // since the last one, bounding transient garbage to roughly this much while
    // keeping the (Asyncify-expensive) collection infrequent. ~40 MB gives a few
    // tens of MB of working set with a smooth frame rate on heavy demos.
    #ifndef MICROPY_GC_LOOP_ALLOC_THRESHOLD
    #define MICROPY_GC_LOOP_ALLOC_THRESHOLD (40 * 1024 * 1024)
    #endif
    MP_STATE_MEM(gc_alloc_threshold) = MICROPY_GC_LOOP_ALLOC_THRESHOLD / MICROPY_BYTES_PER_GC_BLOCK;
    #else
    // Pure defer-to-top-level build (no mid-loop collect): keep the threshold low
    // so a collection is requested before the heap fills, then runs at the top
    // level via gc_collect_pending / gc_collect_top_level().
    MP_STATE_MEM(gc_alloc_threshold) = 16 * 1024 / MICROPY_BYTES_PER_GC_BLOCK;
    #endif
    #endif

    mp_init();

    #if MICROPY_VFS_POSIX
    {
        // Mount the host FS at the root of our internal VFS
        mp_obj_t args[2] = {
            MP_OBJ_TYPE_GET_SLOT(&mp_type_vfs_posix, make_new)(&mp_type_vfs_posix, 0, 0, NULL),
            MP_OBJ_NEW_QSTR(qstr_from_str("/")),
        };
        mp_vfs_mount(2, args, (mp_map_t *)&mp_const_empty_map);
        MP_STATE_VM(vfs_cur) = MP_STATE_VM(vfs_mount_table);
    }
    mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_lib));
    #endif
}

void mp_js_register_js_module(const char *name, uint32_t *value) {
    mp_obj_t module_name = MP_OBJ_NEW_QSTR(qstr_from_str(name));
    mp_obj_t module = proxy_convert_js_to_mp_obj_cside(value);
    mp_map_t *mp_loaded_modules_map = &MP_STATE_VM(mp_loaded_modules_dict).map;
    mp_map_lookup(mp_loaded_modules_map, module_name, MP_MAP_LOOKUP_ADD_IF_NOT_FOUND)->value = module;
}

void mp_js_do_import(const char *name, uint32_t *out) {
    external_call_depth_inc();
    MP_JS_SUSPENDABLE_INC();
    MP_JS_NOTE_CSTACK_TOP();
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_obj_t ret = mp_import_name(qstr_from_str(name), mp_const_none, MP_OBJ_NEW_SMALL_INT(0));
        // Return the leaf of the import, eg for "a.b.c" return "c".
        const char *m = name;
        const char *n = name;
        for (;; ++n) {
            if (*n == '\0' || *n == '.') {
                if (m != name) {
                    ret = mp_load_attr(ret, qstr_from_strn(m, n - m));
                }
                m = n + 1;
                if (*n == '\0') {
                    break;
                }
            }
        }
        nlr_pop();
        proxy_convert_mp_to_js_obj_cside(ret, out);
        external_call_depth_dec(ret);
    } else {
        // uncaught exception
        proxy_convert_mp_to_js_exc_cside(nlr.ret_val, out);
        external_call_depth_dec(nlr.ret_val);
    }
    MP_JS_SUSPENDABLE_DEC();
}

void mp_js_do_exec(const char *src, size_t len, uint32_t *out) {
    external_call_depth_inc();
    MP_JS_SUSPENDABLE_INC();
    MP_JS_NOTE_CSTACK_TOP();
    mp_parse_input_kind_t input_kind = MP_PARSE_FILE_INPUT;
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_lexer_t *lex = mp_lexer_new_from_str_len_dedent(MP_QSTR__lt_stdin_gt_, src, len, 0);
        qstr source_name = lex->source_name;
        mp_parse_tree_t parse_tree = mp_parse(lex, input_kind);
        mp_obj_t module_fun = mp_compile(&parse_tree, source_name, false);
        mp_obj_t ret = mp_call_function_0(module_fun);
        nlr_pop();
        proxy_convert_mp_to_js_obj_cside(ret, out);
        external_call_depth_dec(ret);
    } else {
        // uncaught exception
        proxy_convert_mp_to_js_exc_cside(nlr.ret_val, out);
        external_call_depth_dec(nlr.ret_val);
    }
    MP_JS_SUSPENDABLE_DEC();
}

void mp_js_do_exec_async(const char *src, size_t len, uint32_t *out) {
    mp_compile_allow_top_level_await = true;
    mp_js_do_exec(src, len, out);
    mp_compile_allow_top_level_await = false;
}

void mp_js_repl_init(void) {
    pyexec_event_repl_init();
}

int mp_js_repl_process_char(int c) {
    external_call_depth_inc();
    int ret = pyexec_event_repl_process_char(c);
    external_call_depth_dec(MP_OBJ_NULL);
    return ret;
}

// Used by the root-scanning collectors below: the non-split-heap gc_collect() and
// (when the mid-loop collector is compiled, i.e. an Asyncify yield build) the
// register-scanning gc_collect_if_pending(). Other configs scan neither, so guard
// the definition to avoid an unused-function warning.
#if !MICROPY_GC_SPLIT_HEAP_AUTO || (MICROPY_GC_SCAN_REGISTERS && MICROPY_ENABLE_VM_YIELD)
static void gc_scan_func(void *begin, void *end) {
    gc_collect_root((void **)begin, (void **)end - (void **)begin + 1);
}
#endif

#if MICROPY_GC_SPLIT_HEAP_AUTO

static bool gc_collect_pending = false;

// The largest new region that is available to become Python heap.
size_t gc_get_max_new_split(void) {
    return 128 * 1024 * 1024;
}

// Don't collect anything.  Instead require the heap to grow.
void gc_collect(void) {
    gc_collect_pending = true;
}

// Collect at the top-level, where there are no root pointers from stack/registers.
static void gc_collect_top_level(mp_obj_t root_obj) {
    if (gc_collect_pending) {
        gc_collect_pending = false;
        gc_collect_start();
        gc_collect_root(&root_obj, 1);
        gc_collect_end();
    }
}

// Collect mid-execution from the VM yield hook. Unlike gc_collect_top_level()
// this scans the running program's C stack (and registers, on Asyncify) for
// roots, so live locals/temporaries are preserved — making it safe to call while
// Python code is on the stack, like the non-split-heap gc_collect() does.
//
// It runs whenever a collection is pending. How often that is — and therefore how
// much transient garbage builds up, and how often the (Asyncify-expensive) scan
// runs — is governed by gc_alloc_threshold (set in mp_js_init): a collection is
// requested once that many bytes have been allocated since the last one. So the
// memory/smoothness trade-off for self-looping programs is tuned there, not here.
//
// The mid-loop collector and its cstack helper are reachable only from
// mp_js_yield(), so they compile only when the cooperative yield is enabled.
#if MICROPY_ENABLE_VM_YIELD
#if MICROPY_GC_TRACK_CSTACK
// Conservatively scan the live C-stack range [current SP, tracked top] for roots.
static void gc_collect_cstack(void) {
    void *sp = (void *)emscripten_stack_get_current();
    void *top = mp_js_cstack_top;
    if (top != NULL && sp < top) {
        gc_collect_root((void **)sp, ((char *)top - (char *)sp) / sizeof(void *));
    }
}
#endif

static void gc_collect_if_pending(void) {
    if (!gc_collect_pending) {
        return;
    }
    gc_collect_pending = false;
    gc_collect_start();
    #if MICROPY_GC_SCAN_REGISTERS
    emscripten_scan_stack(gc_scan_func);
    emscripten_scan_registers(gc_scan_func);
    #elif MICROPY_GC_TRACK_CSTACK
    gc_collect_cstack();
    #endif
    gc_collect_end();
}
#endif // MICROPY_ENABLE_VM_YIELD

#else

void gc_collect(void) {
    gc_collect_start();
    emscripten_scan_stack(gc_scan_func);
    #if MICROPY_GC_SCAN_REGISTERS
    emscripten_scan_registers(gc_scan_func);
    #endif
    gc_collect_end();
}

#endif

#if !MICROPY_VFS
mp_lexer_t *mp_lexer_new_from_file(qstr filename) {
    mp_raise_OSError(MP_ENOENT);
}

mp_import_stat_t mp_import_stat(const char *path) {
    return MP_IMPORT_STAT_NO_EXIST;
}

mp_obj_t mp_builtin_open(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(mp_builtin_open_obj, 1, mp_builtin_open);
#endif

void nlr_jump_fail(void *val) {
    while (1) {
        ;
    }
}

void MP_NORETURN __fatal_error(const char *msg) {
    while (1) {
        ;
    }
}

#ifndef NDEBUG
void MP_WEAK __assert_func(const char *file, int line, const char *func, const char *expr) {
    printf("Assertion '%s' failed, at file %s:%d\n", expr, file, line);
    __fatal_error("Assertion failed");
}
#endif
