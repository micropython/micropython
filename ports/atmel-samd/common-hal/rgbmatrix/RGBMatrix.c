// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2020 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stddef.h>

#include "common-hal/rgbmatrix/RGBMatrix.h"

#include "samd/timers.h"
#include "timer_handler.h"

void *common_hal_rgbmatrix_timer_allocate(rgbmatrix_rgbmatrix_obj_t *self) {
    uint8_t timer_index = find_free_timer();
    if (timer_index == 0xff) {
        return NULL;
    }
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
}
