/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2019 Elvis Pfutzenreuter <epxx@epxx.co>
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_PS2IO_PS2_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_PS2IO_PS2_H

#include "common-hal/microcontroller/Pin.h"
#include "common-hal/ps2io/Ps2.h"

extern const mp_obj_type_t ps2io_ps2_type;

extern void common_hal_ps2io_ps2_construct(ps2io_ps2_obj_t *self,
    const mcu_pin_obj_t *data_pin, const mcu_pin_obj_t *clk_pin);
extern void common_hal_ps2io_ps2_deinit(ps2io_ps2_obj_t *self);
extern bool common_hal_ps2io_ps2_deinited(ps2io_ps2_obj_t *self);
extern uint16_t common_hal_ps2io_ps2_get_len(ps2io_ps2_obj_t *self);
extern int16_t common_hal_ps2io_ps2_popleft(ps2io_ps2_obj_t *self);
extern int16_t common_hal_ps2io_ps2_sendcmd(ps2io_ps2_obj_t *self, uint8_t b);
extern uint16_t common_hal_ps2io_ps2_clear_errors(ps2io_ps2_obj_t *self);

#endif // MICROPY_INCLUDED_SHARED_BINDINGS_PS2IO_PS2_H
