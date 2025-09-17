/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2023 Damien P. George
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

#include "py/mpstate.h"

#if !MICROPY_NLR_SETJMP
// When not using setjmp, nlr_push_tail is called from inline asm so needs special care
#if MICROPY_NLR_X86 && MICROPY_NLR_OS_WINDOWS
// On these 32-bit platforms make sure nlr_push_tail doesn't have a leading underscore
unsigned int nlr_push_tail(nlr_buf_t *nlr) asm ("nlr_push_tail");
#else
// LTO can't see inside inline asm functions so explicitly mark nlr_push_tail as used
__attribute__((used)) unsigned int nlr_push_tail(nlr_buf_t *nlr);
#endif
#endif

unsigned int nlr_push_tail(nlr_buf_t *nlr) {
    nlr_buf_t **top = &MP_STATE_THREAD(nlr_top);
    nlr->prev = *top;
    MP_NLR_SAVE_PYSTACK(nlr);
    *top = nlr;
    return 0; // normal return
}

void nlr_pop(void) {
    nlr_buf_t **top = &MP_STATE_THREAD(nlr_top);
    *top = (*top)->prev;
}

void nlr_push_jump_callback(nlr_jump_callback_node_t *node, nlr_jump_callback_fun_t fun) {
    nlr_jump_callback_node_t **top = &MP_STATE_THREAD(nlr_jump_callback_top);
    node->prev = *top;
    node->fun = fun;
    *top = node;
}

void nlr_pop_jump_callback(bool run_callback) {
    nlr_jump_callback_node_t **top = &MP_STATE_THREAD(nlr_jump_callback_top);
    nlr_jump_callback_node_t *cur = *top;
    *top = (*top)->prev;
    if (run_callback) {
        cur->fun(cur);
    }
}

// This function pops and runs all callbacks that were registered after `nlr`
// was pushed (via nlr_push).  It assumes:
//  - a descending C stack,
//  - that all nlr_jump_callback_node_t's in the linked-list pointed to by
//    nlr_jump_callback_top are on the C stack
// It works by popping each node in turn until the next node is NULL or above
// the `nlr` pointer on the C stack (and so pushed before `nlr` was pushed).
void nlr_call_jump_callbacks(nlr_buf_t *nlr) {
    nlr_jump_callback_node_t **top = &MP_STATE_THREAD(nlr_jump_callback_top);
    while (*top != NULL && (void *)*top < (void *)nlr) {
        nlr_pop_jump_callback(true);
    }
}

#if MICROPY_ENABLE_VM_ABORT
MP_NORETURN void nlr_jump_abort(void) {
    MP_STATE_THREAD(nlr_top) = MP_STATE_VM(nlr_abort);
    nlr_jump(NULL);
}
#endif
