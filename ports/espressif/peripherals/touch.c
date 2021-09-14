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

#include "peripherals/touch.h"

static bool touch_inited = false;
static bool touch_never_reset = false;

void peripherals_touch_reset(void) {
    if (touch_inited && !touch_never_reset) {
        touch_pad_deinit();
        touch_inited = false;
    }
}

void peripherals_touch_never_reset(const bool enable) {
    touch_never_reset = enable;
}

void peripherals_touch_init(const touch_pad_t touchpad) {
    if (!touch_inited) {
        touch_pad_init();
        touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
    }
    // touch_pad_config() must be done before touch_pad_fsm_start() the first time.
    // Otherwise the calibration is wrong and we get maximum raw values if there is
    // a trace of any significant length on the pin.
    touch_pad_config(touchpad);
    if (!touch_inited) {
        touch_pad_fsm_start();
        touch_inited = true;
    }
}
