/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2017 Damien P. George
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
// When not using setjmp, nlr_push_tail is called from inline asm so needs special c
#if MICROPY_NLR_X86 && (defined(_WIN32) || defined(__CYGWIN__))
// On these 32-bit platforms make sure nlr_push_tail doesn't have a leading undersco
unsigned int nlr_push_tail(nlr_buf_t *nlr) asm("nlr_push_tail");
#else
// LTO can't see inside inline asm functions so explicitly mark nlr_push_tail as use
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
