/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 microDev
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

#include "peripherals/timer.h"

#define TIMER_NEVER_RESET 2
#define TIMER_FREE 1
#define TIMER_BUSY 0

static uint8_t timer_state[2][2];

void peripherals_timer_reset(void) {
    timer_index_t timer;
    for (uint8_t i = 0; i < 2; i++) {
        for (uint8_t j = 0; j < 2; j++) {
            if (timer_state[i][j] == TIMER_BUSY) {
                timer.idx = (timer_idx_t)j;
                timer.group = (timer_group_t)i;
                timer_state[i][j] = TIMER_FREE;
                peripherals_timer_deinit(&timer);
            }
        }
    }
}

bool peripherals_timer_init(const timer_config_t *config, timer_index_t *timer) {
    bool break_loop = false;

    // get free timer
    for (uint8_t i = 0; i < 2; i++) {
        for (uint8_t j = 0; j < 2; j++) {
            if (timer_state[i][j] == TIMER_FREE) {
                timer->idx = (timer_idx_t)j;
                timer->group = (timer_group_t)i;
                timer_state[i][j] = TIMER_BUSY;
                break_loop = true;
                break;
            } else if (i == 1 && j == 1) {
                timer->idx = TIMER_MAX;
                timer->group = TIMER_GROUP_MAX;
                return false;
            }
        }
        if (break_loop) {
            break;
        }
    }

    timer->hw = (timer->group == 0) ? &TIMERG0 : &TIMERG1;

    // initialize timer module
    timer_init(timer->group, timer->idx, config);
    timer_set_counter_value(timer->group, timer->idx, 0);

    return true;
}

void peripherals_timer_deinit(timer_index_t *timer) {
    if (timer->group == TIMER_GROUP_MAX || timer->idx == TIMER_MAX) {
        return;
    }
    timer_deinit(timer->group, timer->idx);
    int i = timer->group;
    int j = timer->idx;
    timer->group = TIMER_GROUP_MAX;
    timer->idx = TIMER_MAX;
    timer_state[i][j] = TIMER_FREE;
}

void peripherals_timer_never_reset(timer_index_t *timer) {
    timer_deinit(timer->group, timer->idx);
    int i = timer->group;
    int j = timer->idx;
    timer_state[i][j] = TIMER_NEVER_RESET;
}
