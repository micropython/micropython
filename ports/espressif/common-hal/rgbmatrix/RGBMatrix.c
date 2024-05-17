// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

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
