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

#include "peripherals/nrf/timers.h"

extern void _PM_IRQ_HANDLER(void);

void *common_hal_rgbmatrix_timer_allocate(rgbmatrix_rgbmatrix_obj_t *self) {
    nrfx_timer_t *timer = nrf_peripherals_allocate_timer_or_throw();
    nrf_peripherals_timer_never_reset(timer);
    return timer->p_reg;
}


static void rgbmatrix_event_handler(nrf_timer_event_t event_type, void *p_context) {
    _PM_IRQ_HANDLER();
}

void common_hal_rgbmatrix_timer_enable(void *ptr) {
    nrfx_timer_t *timer = nrf_peripherals_timer_from_reg(ptr);
    static const nrfx_timer_config_t timer_config = {
        .frequency = NRF_TIMER_FREQ_16MHz,
        .mode = NRF_TIMER_MODE_TIMER,
        .bit_width = NRF_TIMER_BIT_WIDTH_16,
        .interrupt_priority = NRFX_TIMER_DEFAULT_CONFIG_IRQ_PRIORITY,
        .p_context = NULL,
    };
    nrfx_timer_init(timer, &timer_config, &rgbmatrix_event_handler);
}

void common_hal_rgbmatrix_timer_disable(void *ptr) {
    nrfx_timer_t *timer = nrf_peripherals_timer_from_reg(ptr);
    nrfx_timer_uninit(timer);
}

void common_hal_rgbmatrix_timer_free(void *ptr) {
    nrfx_timer_t *timer = nrf_peripherals_timer_from_reg(ptr);
    nrf_peripherals_free_timer(timer);
}
