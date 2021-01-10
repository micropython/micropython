/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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


#include <string.h>

#include "supervisor/board.h"
#include "py/mpconfig.h"
#include "shared-bindings/nvm/ByteArray.h"
#include "common-hal/microcontroller/Pin.h"
#include "hal/include/hal_gpio.h"
#include "shared-bindings/pwmio/PWMOut.h"

nvm_bytearray_obj_t bootcnt = {
    .base = {
        .type = &nvm_bytearray_type
            },
    .len = ( uint32_t) 8192,
    .start_address = (uint8_t*) (0x00080000 - 8192)
    };


void board_init(void) {
    pwmio_pwmout_obj_t pwm;
    common_hal_pwmio_pwmout_construct(&pwm, &pin_PA23, 4096, 2, false);
    common_hal_pwmio_pwmout_never_reset(&pwm);
}

bool board_requests_safe_mode(void) {
    return false;
}

void reset_board(void) {
    uint8_t value_out = 0;
    common_hal_nvm_bytearray_get_bytes(&bootcnt,0,1,&value_out);
    ++value_out;
    common_hal_nvm_bytearray_set_bytes(&bootcnt,0,&value_out,1);
}
