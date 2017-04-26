/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Damien P. George
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

#include <stdio.h>

#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "extmod/machine_pulse.h"
#include "drivers/dht/dht.h"

STATIC mp_obj_t dht_readinto(mp_obj_t pin_in, mp_obj_t buf_in) {
    mp_hal_pin_obj_t pin = mp_hal_get_pin_obj(pin_in);
    mp_hal_pin_open_drain(pin);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_WRITE);

    if (bufinfo.len < 5) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_ValueError, "buffer too small"));
    }

    // issue start command
    mp_hal_pin_od_high(pin);
    mp_hal_delay_ms(250);
    mp_hal_pin_od_low(pin);
    mp_hal_delay_ms(18);

    mp_uint_t irq_state = mp_hal_quiet_timing_enter();

    // release the line so the device can respond
    mp_hal_pin_od_high(pin);
    mp_hal_delay_us_fast(10);

    // wait for device to respond
    mp_uint_t ticks = mp_hal_ticks_us();
    while (mp_hal_pin_read(pin) != 0) {
        if ((mp_uint_t)(mp_hal_ticks_us() - ticks) > 100) {
            goto timeout;
        }
    }

    // time pulse, should be 80us
    ticks = machine_time_pulse_us(pin, 1, 150);
    if ((mp_int_t)ticks < 0) {
        goto timeout;
    }

    // time 40 pulses for data (either 26us or 70us)
    uint8_t *buf = bufinfo.buf;
    for (int i = 0; i < 40; ++i) {
        ticks = machine_time_pulse_us(pin, 1, 100);
        if ((mp_int_t)ticks < 0) {
            goto timeout;
        }
        buf[i / 8] = (buf[i / 8] << 1) | (ticks > 48);
    }

    mp_hal_quiet_timing_exit(irq_state);
    return mp_const_none;

timeout:
    mp_hal_quiet_timing_exit(irq_state);
    mp_raise_OSError(MP_ETIMEDOUT);
}
MP_DEFINE_CONST_FUN_OBJ_2(dht_readinto_obj, dht_readinto);
