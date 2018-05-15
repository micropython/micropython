/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#include "shared-bindings/busio/I2C.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "mpconfigboard.h"
#include "pins.h"
#include "py/runtime.h"



#if !defined(DEFAULT_I2C_BUS_SDA) || !defined(DEFAULT_I2C_BUS_SCL)

    STATIC mp_obj_t board_i2c(void) {
        //board_i2c_obj = NULL;
        mp_raise_NotImplementedError("no default bus");
        return NULL;
    }

#else
    STATIC mp_obj_t i2c_singleton = NULL;

    STATIC mp_obj_t board_i2c(void) {

        if (i2c_singleton == NULL) {
            busio_i2c_obj_t *self = m_new_obj(busio_i2c_obj_t);
            self->base.type = &busio_i2c_type;

            assert_pin_free(DEFAULT_I2C_BUS_SDA);
            assert_pin_free(DEFAULT_I2C_BUS_SCL);
            common_hal_busio_i2c_construct(self, DEFAULT_I2C_BUS_SCL, DEFAULT_I2C_BUS_SDA, 400000);
            i2c_singleton = (mp_obj_t)self;
        }
        return i2c_singleton;

    }
#endif

MP_DEFINE_CONST_FUN_OBJ_0(board_i2c_obj, board_i2c);
