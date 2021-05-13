/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
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

#include <stddef.h>

#include "common-hal/rgbmatrix/RGBMatrix.h"

#include "samd/timers.h"
#include "timer_handler.h"

void *common_hal_rgbmatrix_timer_allocate(rgbmatrix_rgbmatrix_obj_t *self) {
    uint8_t timer_index = find_free_timer();
    if (timer_index == 0xff) {
        return NULL;
    }
    timer_never_reset(timer_index, true);
    return tc_insts[timer_index];
}

static uint8_t tc_index_from_ptr(void *ptr) {
    for (uint8_t i = TC_INST_NUM; i > 0; i--) {
        if (tc_insts[i] == ptr) {
            return i;
        }
    }
    return 0xff;
}

void common_hal_rgbmatrix_timer_enable(void *ptr) {
    uint8_t timer_index = tc_index_from_ptr(ptr);
    if (timer_index == 0xff) {
        return;
    }
    set_timer_handler(true, timer_index, TC_HANDLER_RGBMATRIX);
    turn_on_clocks(true, timer_index, 1);
}

void common_hal_rgbmatrix_timer_disable(void *ptr) {
    uint8_t timer_index = tc_index_from_ptr(ptr);
    if (timer_index == 0xff) {
        return;
    }
    set_timer_handler(true, timer_index, TC_HANDLER_NO_INTERRUPT);
    tc_set_enable(ptr, false);
}

void common_hal_rgbmatrix_timer_free(void *ptr) {
    uint8_t timer_index = tc_index_from_ptr(ptr);
    if (timer_index == 0xff) {
        return;
    }
    tc_set_enable(ptr, false);
    tc_reset(ptr);
    timer_reset_ok(timer_index, true);
}
