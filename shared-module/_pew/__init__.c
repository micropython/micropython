/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Radomir Dopieralski for Adafruit Industries
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

#include <stdbool.h>

#include "py/mpstate.h"
#include "__init__.h"
#include "PewPew.h"

#include "shared-bindings/digitalio/DigitalInOut.h"


void pew_tick(void) {
    digitalio_digitalinout_obj_t *pin;
    pew_obj_t* pew = MP_STATE_VM(pew_singleton);
    if (!pew) { return; }

    pin = MP_OBJ_TO_PTR(pew->cols[pew->col]);
    common_hal_digitalio_digitalinout_set_value(pin, true);
    pew->col += 1;
    if (pew->col >= pew->cols_size) {
        pew->col = 0;
        pew->turn += 1;
        if (pew->turn >= 4) {
            pew->turn = 0;
        }
    }
    for (size_t x = 0; x < pew->rows_size; ++x) {
        pin = MP_OBJ_TO_PTR(pew->rows[x]);
        uint8_t color = pew->buffer[(pew->col) * (pew->rows_size) + x];
        bool value = true;
        switch (pew->turn) {
            case 0:
                if (color & 0x03) { value = true; }
                break;
            case 1:
            case 2:
                if (color & 0x02) { value = true; }
                break;
        }
        common_hal_digitalio_digitalinout_set_value(pin, value);
    }
    pin = MP_OBJ_TO_PTR(pew->cols[pew->col]);
    common_hal_digitalio_digitalinout_set_value(pin, false);
}

void pew_reset(void) {
    MP_STATE_VM(pew_singleton) = NULL;
}
