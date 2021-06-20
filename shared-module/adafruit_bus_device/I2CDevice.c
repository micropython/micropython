/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Mark Komus
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

#include "shared-bindings/adafruit_bus_device/I2CDevice.h"
#include "shared-bindings/busio/I2C.h"
#include "py/mperrno.h"
#include "py/nlr.h"
#include "py/runtime.h"
#include "lib/utils/interrupt_char.h"

void common_hal_adafruit_bus_device_i2cdevice_construct(adafruit_bus_device_i2cdevice_obj_t *self, mp_obj_t *i2c, uint8_t device_address) {
    self->i2c = i2c;
    self->device_address = device_address;
}

void common_hal_adafruit_bus_device_i2cdevice_lock(adafruit_bus_device_i2cdevice_obj_t *self) {
    mp_obj_t dest[2];
    mp_load_method(self->i2c, MP_QSTR_try_lock, dest);

    mp_obj_t success = mp_call_method_n_kw(0, 0, dest);

    while (!mp_obj_is_true(success)) {
        RUN_BACKGROUND_TASKS;
        if (mp_hal_is_interrupted()) {
            break;
        }

        success = mp_call_method_n_kw(0, 0, dest);
    }
}

void common_hal_adafruit_bus_device_i2cdevice_unlock(adafruit_bus_device_i2cdevice_obj_t *self) {
    mp_obj_t dest[2];
    mp_load_method(self->i2c, MP_QSTR_unlock, dest);
    mp_call_method_n_kw(0, 0, dest);
}

void common_hal_adafruit_bus_device_i2cdevice_probe_for_device(adafruit_bus_device_i2cdevice_obj_t *self) {
    common_hal_adafruit_bus_device_i2cdevice_lock(self);

    mp_buffer_info_t write_bufinfo;
    mp_obj_t write_buffer = mp_obj_new_bytearray_of_zeros(0);
    mp_get_buffer_raise(write_buffer, &write_bufinfo, MP_BUFFER_READ);

    mp_obj_t dest[4];

    /* catch exceptions that may be thrown while probing for the device */
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_load_method(self->i2c, MP_QSTR_writeto, dest);
        dest[2] = MP_OBJ_NEW_SMALL_INT(self->device_address);
        dest[3] = write_buffer;
        mp_call_method_n_kw(2, 0, dest);
        nlr_pop();
    } else {
        common_hal_adafruit_bus_device_i2cdevice_unlock(self);

        if (mp_obj_is_subclass_fast(MP_OBJ_FROM_PTR(((mp_obj_base_t *)nlr.ret_val)->type), MP_OBJ_FROM_PTR(&mp_type_OSError))) {
            mp_raise_ValueError_varg(translate("No I2C device at address: %x"), self->device_address);
        } else {
            /* In case we receive an unrelated exception pass it up */
            nlr_raise(MP_OBJ_FROM_PTR(nlr.ret_val));
        }
    }

    common_hal_adafruit_bus_device_i2cdevice_unlock(self);
}
