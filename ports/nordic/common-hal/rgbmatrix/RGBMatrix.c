// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

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
