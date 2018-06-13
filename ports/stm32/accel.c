/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#include <string.h>

#include "py/mphal.h"
#include "py/runtime.h"
#include "pin.h"
#include "i2c.h"
#include "accel.h"

#if MICROPY_HW_HAS_MMA7660

/// \moduleref pyb
/// \class Accel - accelerometer control
///
/// Accel is an object that controls the accelerometer.  Example usage:
///
///     accel = pyb.Accel()
///     for i in range(10):
///         print(accel.x(), accel.y(), accel.z())
///
/// Raw values are between -32 and 31.

#define MMA_ADDR (76)
#define MMA_REG_X (0)
#define MMA_REG_Y (1)
#define MMA_REG_Z (2)
#define MMA_REG_TILT (3)
#define MMA_REG_MODE (7)
#define MMA_AXIS_SIGNED_VALUE(i) (((i) & 0x3f) | ((i) & 0x20 ? (~0x1f) : 0))

void accel_init(void) {
    // PB5 is connected to AVDD; pull high to enable MMA accel device
    mp_hal_pin_low(MICROPY_HW_MMA_AVDD_PIN); // turn off AVDD
    mp_hal_pin_output(MICROPY_HW_MMA_AVDD_PIN);
}

STATIC void accel_start(void) {
    // start the I2C bus in master mode
    i2c_init(I2C1, MICROPY_HW_I2C1_SCL, MICROPY_HW_I2C1_SDA, 400000);

    // turn off AVDD, wait 30ms, turn on AVDD, wait 30ms again
    mp_hal_pin_low(MICROPY_HW_MMA_AVDD_PIN); // turn off
    mp_hal_delay_ms(30);
    mp_hal_pin_high(MICROPY_HW_MMA_AVDD_PIN); // turn on
    mp_hal_delay_ms(30);

    int ret;
    for (int i = 0; i < 4; i++) {
        ret = i2c_writeto(I2C1, MMA_ADDR, NULL, 0, true);
        if (ret == 0) {
            break;
        }
    }

    if (ret != 0) {
        mp_raise_msg(&mp_type_OSError, "accelerometer not found");
    }

    // set MMA to active mode
    uint8_t data[2] = {MMA_REG_MODE, 1}; // active mode
    i2c_writeto(I2C1, MMA_ADDR, data, 2, true);

    // wait for MMA to become active
    mp_hal_delay_ms(30);
}

/******************************************************************************/
/* MicroPython bindings                                                      */

#define NUM_AXIS (3)
#define FILT_DEPTH (4)

typedef struct _pyb_accel_obj_t {
    mp_obj_base_t base;
    int16_t buf[NUM_AXIS * FILT_DEPTH];
} pyb_accel_obj_t;

STATIC pyb_accel_obj_t pyb_accel_obj;

/// \classmethod \constructor()
/// Create and return an accelerometer object.
///
/// Note: if you read accelerometer values immediately after creating this object
/// you will get 0.  It takes around 20ms for the first sample to be ready, so,
/// unless you have some other code between creating this object and reading its
/// values, you should put a `pyb.delay(20)` after creating it.  For example:
///
///     accel = pyb.Accel()
///     pyb.delay(20)
///     print(accel.x())
STATIC mp_obj_t pyb_accel_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // check arguments
    mp_arg_check_num(n_args, n_kw, 0, 0, false);

    // init accel object
    pyb_accel_obj.base.type = &pyb_accel_type;
    accel_start();

    return &pyb_accel_obj;
}

STATIC mp_obj_t read_axis(int axis) {
    uint8_t data[1] = { axis };
    i2c_writeto(I2C1, MMA_ADDR, data, 1, false);
    i2c_readfrom(I2C1, MMA_ADDR, data, 1, true);
    return mp_obj_new_int(MMA_AXIS_SIGNED_VALUE(data[0]));
}

/// \method x()
/// Get the x-axis value.
STATIC mp_obj_t pyb_accel_x(mp_obj_t self_in) {
    return read_axis(MMA_REG_X);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_accel_x_obj, pyb_accel_x);

/// \method y()
/// Get the y-axis value.
STATIC mp_obj_t pyb_accel_y(mp_obj_t self_in) {
    return read_axis(MMA_REG_Y);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_accel_y_obj, pyb_accel_y);

/// \method z()
/// Get the z-axis value.
STATIC mp_obj_t pyb_accel_z(mp_obj_t self_in) {
    return read_axis(MMA_REG_Z);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_accel_z_obj, pyb_accel_z);

/// \method tilt()
/// Get the tilt register.
STATIC mp_obj_t pyb_accel_tilt(mp_obj_t self_in) {
    uint8_t data[1] = { MMA_REG_TILT };
    i2c_writeto(I2C1, MMA_ADDR, data, 1, false);
    i2c_readfrom(I2C1, MMA_ADDR, data, 1, true);
    return mp_obj_new_int(data[0]);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_accel_tilt_obj, pyb_accel_tilt);

/// \method filtered_xyz()
/// Get a 3-tuple of filtered x, y and z values.
STATIC mp_obj_t pyb_accel_filtered_xyz(mp_obj_t self_in) {
    pyb_accel_obj_t *self = self_in;

    memmove(self->buf, self->buf + NUM_AXIS, NUM_AXIS * (FILT_DEPTH - 1) * sizeof(int16_t));

    uint8_t data[NUM_AXIS] = { MMA_REG_X };
    i2c_writeto(I2C1, MMA_ADDR, data, 1, false);
    i2c_readfrom(I2C1, MMA_ADDR, data, 3, true);

    mp_obj_t tuple[NUM_AXIS];
    for (int i = 0; i < NUM_AXIS; i++) {
        self->buf[NUM_AXIS * (FILT_DEPTH - 1) + i] = MMA_AXIS_SIGNED_VALUE(data[i]);
        int32_t val = 0;
        for (int j = 0; j < FILT_DEPTH; j++) {
            val += self->buf[i + NUM_AXIS * j];
        }
        tuple[i] = mp_obj_new_int(val);
    }

    return mp_obj_new_tuple(3, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(pyb_accel_filtered_xyz_obj, pyb_accel_filtered_xyz);

STATIC mp_obj_t pyb_accel_read(mp_obj_t self_in, mp_obj_t reg) {
    uint8_t data[1] = { mp_obj_get_int(reg) };
    i2c_writeto(I2C1, MMA_ADDR, data, 1, false);
    i2c_writeto(I2C1, MMA_ADDR, data, 1, true);
    return mp_obj_new_int(data[0]);
}
MP_DEFINE_CONST_FUN_OBJ_2(pyb_accel_read_obj, pyb_accel_read);

STATIC mp_obj_t pyb_accel_write(mp_obj_t self_in, mp_obj_t reg, mp_obj_t val) {
    uint8_t data[2] = { mp_obj_get_int(reg), mp_obj_get_int(val) };
    i2c_writeto(I2C1, MMA_ADDR, data, 2, true);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_3(pyb_accel_write_obj, pyb_accel_write);

STATIC const mp_rom_map_elem_t pyb_accel_locals_dict_table[] = {
    // TODO add init, deinit, and perhaps reset methods
    { MP_ROM_QSTR(MP_QSTR_x), MP_ROM_PTR(&pyb_accel_x_obj) },
    { MP_ROM_QSTR(MP_QSTR_y), MP_ROM_PTR(&pyb_accel_y_obj) },
    { MP_ROM_QSTR(MP_QSTR_z), MP_ROM_PTR(&pyb_accel_z_obj) },
    { MP_ROM_QSTR(MP_QSTR_tilt), MP_ROM_PTR(&pyb_accel_tilt_obj) },
    { MP_ROM_QSTR(MP_QSTR_filtered_xyz), MP_ROM_PTR(&pyb_accel_filtered_xyz_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&pyb_accel_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&pyb_accel_write_obj) },
};

STATIC MP_DEFINE_CONST_DICT(pyb_accel_locals_dict, pyb_accel_locals_dict_table);

const mp_obj_type_t pyb_accel_type = {
    { &mp_type_type },
    .name = MP_QSTR_Accel,
    .make_new = pyb_accel_make_new,
    .locals_dict = (mp_obj_dict_t*)&pyb_accel_locals_dict,
};

#endif // MICROPY_HW_HAS_MMA7660
