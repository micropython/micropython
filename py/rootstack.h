/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018-2019 Damien P. George
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

#pragma once

#include "py/mpstate.h"

#define MICROPY_ROOT_STACK (1)
#define MICROPY_EXTENSIVE_GC_COLLECT (1)
#define MICROPY_ROOT_STACK_FAST (0)

typedef struct _root_stack_elem_t {
    int kind;
    void *ptr;
} root_stack_elem_t;

#if MICROPY_ROOT_STACK

void m_rs_init(void);
void m_rs_deinit(void);
void m_rs_mark(void);
void gc_rs_dump(void);
void gc_rs_push(int kind, void *ptr);
#if MICROPY_ROOT_STACK_FAST
static inline void gc_rs_assert(int kind, void *ptr) { (void)kind; (void)ptr; }
static inline void gc_rs_pop(int kind, void *ptr) { (void)kind; (void)ptr; --MP_STATE_THREAD(root_stack_cur); }
#else
void gc_rs_assert(int kind, void *ptr);
void gc_rs_pop(int kind, void *ptr);
#endif
void gc_rs_push_checked(void *ptr);
void gc_rs_pop_checked(void *ptr);

static inline void m_rs_dump(void) { gc_rs_dump(); }
static inline void m_rs_assert(void *ptr) { gc_rs_assert(0, ptr); }
static inline void m_rs_push_ptr(void *ptr) { gc_rs_push(0, ptr); }
static inline void m_rs_push_ind(void *ptr) { gc_rs_push(1, ptr); }
static inline void m_rs_pop_ptr(void *ptr) { gc_rs_pop(0, ptr); }
static inline void m_rs_pop_ind(void *ptr) { gc_rs_pop(1, ptr); }

#else

static inline void m_rs_init(void) { }
static inline void m_rs_deinit(void) { }
static inline void m_rs_dump(void) { }
static inline void m_rs_assert(void *ptr) { (void)ptr; }
static inline void m_rs_push_ptr(void *ptr) { (void)ptr; }
static inline void m_rs_push_ind(void *ptr) { (void)ptr; }
static inline void m_rs_pop_ptr(void *ptr) { (void)ptr; }
static inline void m_rs_pop_ind(void *ptr) { (void)ptr; }

#endif
