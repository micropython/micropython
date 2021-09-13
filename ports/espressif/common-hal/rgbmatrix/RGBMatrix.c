/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Jeff Epler for Adafruit Industries
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

#include "peripherals/timer.h"

void *common_hal_rgbmatrix_timer_allocate(rgbmatrix_rgbmatrix_obj_t *self) {
    const timer_config_t config = {
        .alarm_en = false,
        .counter_en = false,
        .intr_type = TIMER_INTR_LEVEL,
        .counter_dir = TIMER_COUNT_UP,
        .auto_reload = true,
        .divider = 2 // 40MHz
    };

    timer_index_t *timer = malloc(sizeof(timer_index_t));
    bool res = peripherals_timer_init(&config, timer);
    if (!res) {
        return NULL;
    }
    peripherals_timer_never_reset(timer);
    return timer;
}

extern bool _PM_esp32timerCallback(void *arg);

void common_hal_rgbmatrix_timer_enable(void *ptr) {
}

void common_hal_rgbmatrix_timer_disable(void *ptr) {
    timer_index_t *timer = (timer_index_t *)ptr;
    if (timer->idx == TIMER_MAX) {
        return;
    }
    timer_pause(timer->group, timer->idx);
    timer_disable_intr(timer->group, timer->idx);
    timer_isr_callback_remove(timer->group, timer->idx);
}

void common_hal_rgbmatrix_timer_free(void *ptr) {
    timer_index_t *timer = (timer_index_t *)ptr;
    if (timer->idx == TIMER_MAX) {
        return;
    }
    common_hal_rgbmatrix_timer_disable(ptr);
    peripherals_timer_deinit(timer);
    timer->idx = TIMER_MAX;
}
