/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Linaro Limited
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

#include "py/runtime.h"

#include <zephyr.h>
#include <drivers/sensor.h>

#if MICROPY_PY_ZSENSOR

typedef struct _mp_obj_sensor_t {
    mp_obj_base_t base;
    struct device *dev;
} mp_obj_sensor_t;

STATIC mp_obj_t sensor_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    mp_obj_sensor_t *o = m_new_obj(mp_obj_sensor_t);
    o->base.type = type;
    o->dev = device_get_binding(mp_obj_str_get_str(args[0]));
    if (o->dev == NULL) {
        mp_raise_ValueError("dev not found");
    }
    return MP_OBJ_FROM_PTR(o);
}

STATIC mp_obj_t sensor_measure(mp_obj_t self_in) {
    mp_obj_sensor_t *self = MP_OBJ_TO_PTR(self_in);
    int st = sensor_sample_fetch(self->dev);
    if (st != 0) {
        mp_raise_OSError(-st);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(sensor_measure_obj, sensor_measure);

STATIC void sensor_get_internal(mp_obj_t self_in, mp_obj_t channel_in, struct sensor_value *res) {
    mp_obj_sensor_t *self = MP_OBJ_TO_PTR(self_in);

    int st = sensor_channel_get(self->dev, mp_obj_get_int(channel_in), res);
    if (st != 0) {
        mp_raise_OSError(-st);
    }
}

STATIC mp_obj_t sensor_get_float(mp_obj_t self_in, mp_obj_t channel_in) {
    struct sensor_value val;
    sensor_get_internal(self_in, channel_in, &val);
    return mp_obj_new_float(val.val1 + (mp_float_t)val.val2 / 1000000);
}
MP_DEFINE_CONST_FUN_OBJ_2(sensor_get_float_obj, sensor_get_float);

STATIC mp_obj_t sensor_get_micros(mp_obj_t self_in, mp_obj_t channel_in) {
    struct sensor_value val;
    sensor_get_internal(self_in, channel_in, &val);
    return MP_OBJ_NEW_SMALL_INT(val.val1 * 1000000 + val.val2);
}
MP_DEFINE_CONST_FUN_OBJ_2(sensor_get_micros_obj, sensor_get_micros);

STATIC mp_obj_t sensor_get_millis(mp_obj_t self_in, mp_obj_t channel_in) {
    struct sensor_value val;
    sensor_get_internal(self_in, channel_in, &val);
    return MP_OBJ_NEW_SMALL_INT(val.val1 * 1000 + val.val2 / 1000);
}
MP_DEFINE_CONST_FUN_OBJ_2(sensor_get_millis_obj, sensor_get_millis);

STATIC mp_obj_t sensor_get_int(mp_obj_t self_in, mp_obj_t channel_in) {
    struct sensor_value val;
    sensor_get_internal(self_in, channel_in, &val);
    return MP_OBJ_NEW_SMALL_INT(val.val1);
}
MP_DEFINE_CONST_FUN_OBJ_2(sensor_get_int_obj, sensor_get_int);

STATIC const mp_rom_map_elem_t sensor_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_measure), MP_ROM_PTR(&sensor_measure_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_float), MP_ROM_PTR(&sensor_get_float_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_micros), MP_ROM_PTR(&sensor_get_micros_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_millis), MP_ROM_PTR(&sensor_get_millis_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_int), MP_ROM_PTR(&sensor_get_int_obj) },
};

STATIC MP_DEFINE_CONST_DICT(sensor_locals_dict, sensor_locals_dict_table);

STATIC const mp_obj_type_t sensor_type = {
    { &mp_type_type },
    .name = MP_QSTR_Sensor,
    .make_new = sensor_make_new,
    .locals_dict = (void*)&sensor_locals_dict,
};

STATIC const mp_rom_map_elem_t mp_module_zsensor_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_zsensor) },
    { MP_ROM_QSTR(MP_QSTR_Sensor), MP_ROM_PTR(&sensor_type) },

#define C(name) { MP_ROM_QSTR(MP_QSTR_ ## name), MP_ROM_INT(SENSOR_CHAN_ ## name) }
    C(ACCEL_X),
    C(ACCEL_Y),
    C(ACCEL_Z),
    C(GYRO_X),
    C(GYRO_Y),
    C(GYRO_Z),
    C(MAGN_X),
    C(MAGN_Y),
    C(MAGN_Z),
    C(DIE_TEMP),
    C(PRESS),
    C(PROX),
    C(HUMIDITY),
    C(LIGHT),
    C(ALTITUDE),
#undef C
};

STATIC MP_DEFINE_CONST_DICT(mp_module_zsensor_globals, mp_module_zsensor_globals_table);

const mp_obj_module_t mp_module_zsensor = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_zsensor_globals,
};

#endif //MICROPY_PY_UHASHLIB
