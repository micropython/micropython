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

#include <inttypes.h>
#include <stdio.h>

#include "py/mpstate.h"
#include "py/misc.h"
#include "py/rootstack.h"
#include "py/gc.h"
#include "py/runtime.h"

#if 0
#define RS_DEBUG (1)
#define RS_DEBUG_printf printf
#else
#define RS_DEBUG_printf(...) (void)0
#endif

#if MICROPY_ROOT_STACK

// State for root stack
#define RS_KIND_PTR (0)
#define RS_KIND_IND (1)
static root_stack_elem_t root_stack[1024];

void m_rs_init(void) {
    // Init root stack
    MP_STATE_THREAD(root_stack_cur) = &root_stack[0];
}

void m_rs_deinit(void) {
    // Check if root stack has anything left
    if (MP_STATE_THREAD(root_stack_cur) != &root_stack[0]) {
        printf("REMAINING ROOT STACK:\n");
        gc_rs_dump();
    }
}

void m_rs_mark(void) {
    // Mark all those blocks that are on the root stack
    for (root_stack_elem_t *r = &root_stack[0]; r < MP_STATE_THREAD(root_stack_cur); ++r) {
        void *p;
        if (r->kind == RS_KIND_PTR) {
            p = r->ptr;
        } else {
            p = *(void**)r->ptr;
        }
        gc_collect_root(&p, 1);
    }
}

void gc_rs_dump(void) {
    for (root_stack_elem_t *r = &root_stack[0]; r < MP_STATE_THREAD(root_stack_cur); ++r) {
        uintptr_t *p;
        if (r->kind == RS_KIND_PTR) {
            p = r->ptr;
        } else {
            p = *(uintptr_t**)r->ptr;
            printf(" [ind=%p]", r->ptr);
        }
        size_t n = gc_nbytes(p);
        if (n == 0) {
            printf(" %p outside heap\n", p);
        } else {
            #define F "%016" PRIxPTR
            printf(" %p sz=%u [" F ":" F ":" F ":" F "] [%.*s]\n", p, (uint)n, p[0], p[1], p[2], p[3], 0, (char*)p);
            #undef F
        }
    }
}

#if !MICROPY_ROOT_STACK_FAST
void gc_rs_assert(int kind, void *ptr) {
    RS_DEBUG_printf("gc_rs_assert(%p) sz=%lu\n", ptr, MP_STATE_THREAD(root_stack_cur) - &root_stack[0]);
    //for (void **r = &root_stack[0]; r < MP_STATE_THREAD(root_stack_cur); ++r) { printf(" %p", *r); } printf("\n");
    if (MP_STATE_THREAD(root_stack_cur) == &root_stack[0]) {
        printf("gc_rs_assert(%p): assert empty stack\n", ptr);
        return;
    }
    if (MP_STATE_THREAD(root_stack_cur)[-1].kind != kind
        || MP_STATE_THREAD(root_stack_cur)[-1].ptr != ptr) {
        printf("gc_rs_assert(%p): mismatch %p != %p\n", ptr, MP_STATE_THREAD(root_stack_cur)[-1].ptr, ptr);
        return;
    }
}
#endif

void gc_rs_push(int kind, void *ptr) {
    RS_DEBUG_printf("gc_rs_push(%p) sz=%lu\n", ptr, MP_STATE_THREAD(root_stack_cur) - &root_stack[0]);
    //for (void **r = &root_stack[0]; r < MP_STATE_THREAD(root_stack_cur); ++r) { printf(" %p", *r); } printf("\n");
    assert(MP_STATE_THREAD(root_stack_cur) < &root_stack[MP_ARRAY_SIZE(root_stack)]);
    MP_STATE_THREAD(root_stack_cur)->kind = kind;
    MP_STATE_THREAD(root_stack_cur)->ptr = ptr;
    ++MP_STATE_THREAD(root_stack_cur);
}

#if !MICROPY_ROOT_STACK_FAST
void gc_rs_pop(int kind, void *ptr) {
    RS_DEBUG_printf("gc_rs_pop(%p) sz=%lu\n", ptr, MP_STATE_THREAD(root_stack_cur) - &root_stack[0]);
    //for (void **r = &root_stack[0]; r < MP_STATE_THREAD(root_stack_cur); ++r) { printf(" %p", *r); } printf("\n");
    if (MP_STATE_THREAD(root_stack_cur) == &root_stack[0]) {
        printf("gc_rs_pop(%p): pop empty stack\n", ptr);
        return;
    }
    if (MP_STATE_THREAD(root_stack_cur)[-1].kind != kind
        || MP_STATE_THREAD(root_stack_cur)[-1].ptr != ptr) {
        printf("gc_rs_pop(%p): mismatch %p != %p\n", ptr, MP_STATE_THREAD(root_stack_cur)[-1].ptr, ptr);
        return;
    }
    --MP_STATE_THREAD(root_stack_cur);
}

void m_rs_push_obj(mp_obj_t o) {
    if (MP_OBJ_IS_OBJ(o)) {
        gc_rs_push_checked(MP_OBJ_TO_PTR(o));
    } else {
        // not needed to trace, just for checking correctness
        gc_rs_push(0, o);
    }
}

void m_rs_pop_obj(mp_obj_t o) {
    if (MP_OBJ_IS_OBJ(o)) {
        gc_rs_pop_checked(MP_OBJ_TO_PTR(o));
    } else {
        // not needed to trace, just for checking correctness
        gc_rs_pop(0, o);
    }
}
#endif

void mp_obj_list_append_rs(mp_obj_t self, mp_obj_t arg) {
    m_rs_push_obj(arg);
    mp_obj_list_append(self, arg);
    m_rs_pop_obj(arg);
}

#endif
