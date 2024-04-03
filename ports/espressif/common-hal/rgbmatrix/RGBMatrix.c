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

#include "bindings/espidf/__init__.h"

#include "driver/gptimer.h"

void *common_hal_rgbmatrix_timer_allocate(rgbmatrix_rgbmatrix_obj_t *self) {
    gptimer_config_t config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 40000000,
        .flags = {
            .intr_shared = true
        }
    };
    gptimer_handle_t timer;

    CHECK_ESP_RESULT(gptimer_new_timer(&config, &timer));
    return timer;
}

void common_hal_rgbmatrix_timer_enable(void *ptr) {
    // protomatter does this.
}

void common_hal_rgbmatrix_timer_disable(void *ptr) {
    if (ptr == NULL) {
        return;
    }
    gptimer_handle_t timer = (gptimer_handle_t)ptr;
    gptimer_stop(timer);
}

void common_hal_rgbmatrix_timer_free(void *ptr) {
    if (ptr == NULL) {
        return;
    }
    gptimer_handle_t timer = (gptimer_handle_t)ptr;
    gptimer_disable(timer);
    gptimer_del_timer(timer);
}
