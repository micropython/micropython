/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#ifndef MICROPY_INCLUDED_PY_NLR_H
#define MICROPY_INCLUDED_PY_NLR_H

// non-local return
// exception handling, basically a stack of setjmp/longjmp buffers

#include <limits.h>
#include <assert.h>

#include "py/mpconfig.h"

// If MICROPY_NLR_SETJMP is not enabled then auto-detect the machine arch
#if !MICROPY_NLR_SETJMP
// A lot of nlr-related things need different treatment on Windows
#if defined(_WIN32) || defined(__CYGWIN__)
#define MICROPY_NLR_OS_WINDOWS 1
#else
#define MICROPY_NLR_OS_WINDOWS 0
#endif
#if defined(__i386__)
    #define MICROPY_NLR_X86 (1)
    #define MICROPY_NLR_NUM_REGS (6)
#elif defined(__x86_64__)
    #define MICROPY_NLR_X64 (1)
    #if MICROPY_NLR_OS_WINDOWS
        #define MICROPY_NLR_NUM_REGS (10)
    #else
        #define MICROPY_NLR_NUM_REGS (8)
    #endif
#elif defined(__thumb2__) || defined(__thumb__) || defined(__arm__)
    #define MICROPY_NLR_THUMB (1)
    #if defined(__SOFTFP__)
        #define MICROPY_NLR_NUM_REGS (10)
    #else
        // With hardware FP registers s16-s31 are callee save so in principle
        // should be saved and restored by the NLR code.  gcc only uses s16-s21
        // so only save/restore those as an optimisation.
        #define MICROPY_NLR_NUM_REGS (10 + 6)
    #endif
#elif defined(__xtensa__)
    #define MICROPY_NLR_XTENSA (1)
    #define MICROPY_NLR_NUM_REGS (10)
#else
    #define MICROPY_NLR_SETJMP (1)
    //#warning "No native NLR support for this arch, using setjmp implementation"
#endif
#endif

#if MICROPY_NLR_SETJMP
#include <setjmp.h>
#endif

typedef struct _nlr_buf_t nlr_buf_t;
struct _nlr_buf_t {
    // the entries here must all be machine word size
    nlr_buf_t *prev;
    void *ret_val; // always a concrete object (an exception instance)

    #if MICROPY_NLR_SETJMP
    jmp_buf jmpbuf;
    #else
    void *regs[MICROPY_NLR_NUM_REGS];
    #endif

    #if MICROPY_ENABLE_PYSTACK
    void *pystack;
    #endif
};

// Helper macros to save/restore the pystack state
#if MICROPY_ENABLE_PYSTACK
#define MP_NLR_SAVE_PYSTACK(nlr_buf) (nlr_buf)->pystack = MP_STATE_THREAD(pystack_cur)
#define MP_NLR_RESTORE_PYSTACK(nlr_buf) MP_STATE_THREAD(pystack_cur) = (nlr_buf)->pystack
#else
#define MP_NLR_SAVE_PYSTACK(nlr_buf) (void)nlr_buf
#define MP_NLR_RESTORE_PYSTACK(nlr_buf) (void)nlr_buf
#endif

// Helper macro to use at the start of a specific nlr_jump implementation
#define MP_NLR_JUMP_HEAD(val, top) \
    nlr_buf_t **_top_ptr = &MP_STATE_THREAD(nlr_top); \
    nlr_buf_t *top = *_top_ptr; \
    if (top == NULL) { \
        nlr_jump_fail(val); \
    } \
    top->ret_val = val; \
    MP_NLR_RESTORE_PYSTACK(top); \
    *_top_ptr = top->prev; \

#if MICROPY_NLR_SETJMP
// nlr_push() must be defined as a macro, because "The stack context will be
// invalidated if the function which called setjmp() returns."
// For this case it is safe to call nlr_push_tail() first.
#define nlr_push(buf) (nlr_push_tail(buf), setjmp((buf)->jmpbuf))
#else
unsigned int nlr_push(nlr_buf_t *);
#endif

unsigned int nlr_push_tail(nlr_buf_t *top);
void nlr_pop(void);
NORETURN void nlr_jump(void *val);

// This must be implemented by a port.  It's called by nlr_jump
// if no nlr buf has been pushed.  It must not return, but rather
// should bail out with a fatal error.
NORETURN void nlr_jump_fail(void *val);

// use nlr_raise instead of nlr_jump so that debugging is easier
#ifndef MICROPY_DEBUG_NLR
#define nlr_raise(val) nlr_jump(MP_OBJ_TO_PTR(val))
#else
#include "mpstate.h"
#define nlr_raise(val) \
    do { \
        /*printf("nlr_raise: nlr_top=%p\n", MP_STATE_THREAD(nlr_top)); \
        fflush(stdout);*/ \
        void *_val = MP_OBJ_TO_PTR(val); \
        assert(_val != NULL); \
        assert(mp_obj_is_exception_instance(val)); \
        nlr_jump(_val); \
    } while (0)

#if !MICROPY_NLR_SETJMP
#define nlr_push(val) \
    assert(MP_STATE_THREAD(nlr_top) != val),nlr_push(val)

/*
#define nlr_push(val) \
    printf("nlr_push: before: nlr_top=%p, val=%p\n", MP_STATE_THREAD(nlr_top), val),assert(MP_STATE_THREAD(nlr_top) != val),nlr_push(val)
*/
#endif

#endif

#endif // MICROPY_INCLUDED_PY_NLR_H
