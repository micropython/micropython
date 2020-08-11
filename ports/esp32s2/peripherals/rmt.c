/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Lucian Copeland for Adafruit Industries
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

#include "rmt.h"
#include "py/runtime.h"

bool rmt_reserved_channels[RMT_CHANNEL_MAX];

rmt_channel_t esp32s2_peripherals_find_and_reserve_rmt(void) {
    for (size_t i = 0; i < RMT_CHANNEL_MAX; i++) {
        if (!rmt_reserved_channels[i]) {
            rmt_reserved_channels[i] = true;
            return i;
        }
    }
    mp_raise_RuntimeError(translate("All timers in use"));
    return false;
}

void esp32s2_peripherals_free_rmt(rmt_channel_t chan) {
    rmt_reserved_channels[chan] = false;
    rmt_driver_uninstall(chan);
}
