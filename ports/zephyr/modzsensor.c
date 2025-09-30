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

#include <zephyr/kernel.h>
#include <zephyr/drivers/sensor.h>
#include "zephyr_device.h"

#if MICROPY_PY_ZSENSOR

typedef struct _mp_obj_sensor_t {
    mp_obj_base_t base;
    const struct device *dev;
} mp_obj_sensor_t;

static mp_obj_t sensor_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    mp_obj_sensor_t *o = mp_obj_malloc(mp_obj_sensor_t, type);
    o->dev = zephyr_device_find(args[0]);
    return MP_OBJ_FROM_PTR(o);
}

static mp_obj_t sensor_measure(mp_obj_t self_in) {
    mp_obj_sensor_t *self = MP_OBJ_TO_PTR(self_in);
    int st = sensor_sample_fetch(self->dev);
    if (st != 0) {
        mp_raise_OSError(-st);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(sensor_measure_obj, sensor_measure);

static void sensor_get_internal(mp_obj_t self_in, mp_obj_t channel_in, struct sensor_value *res) {
    mp_obj_sensor_t *self = MP_OBJ_TO_PTR(self_in);

    int st = sensor_channel_get(self->dev, mp_obj_get_int(channel_in), res);
    if (st != 0) {
        mp_raise_OSError(-st);
    }
}

static mp_obj_t sensor_get_float(mp_obj_t self_in, mp_obj_t channel_in) {
    struct sensor_value val;
    sensor_get_internal(self_in, channel_in, &val);
    return mp_obj_new_float(val.val1 + (mp_float_t)val.val2 / 1000000);
}
MP_DEFINE_CONST_FUN_OBJ_2(sensor_get_float_obj, sensor_get_float);

static mp_obj_t sensor_get_micros(mp_obj_t self_in, mp_obj_t channel_in) {
    struct sensor_value val;
    sensor_get_internal(self_in, channel_in, &val);
    return MP_OBJ_NEW_SMALL_INT(val.val1 * 1000000 + val.val2);
}
MP_DEFINE_CONST_FUN_OBJ_2(sensor_get_micros_obj, sensor_get_micros);

static mp_obj_t sensor_get_millis(mp_obj_t self_in, mp_obj_t channel_in) {
    struct sensor_value val;
    sensor_get_internal(self_in, channel_in, &val);
    return MP_OBJ_NEW_SMALL_INT(val.val1 * 1000 + val.val2 / 1000);
}
MP_DEFINE_CONST_FUN_OBJ_2(sensor_get_millis_obj, sensor_get_millis);

static mp_obj_t sensor_get_int(mp_obj_t self_in, mp_obj_t channel_in) {
    struct sensor_value val;
    sensor_get_internal(self_in, channel_in, &val);
    return MP_OBJ_NEW_SMALL_INT(val.val1);
}
MP_DEFINE_CONST_FUN_OBJ_2(sensor_get_int_obj, sensor_get_int);

static void sensor_attr_get_internal(mp_obj_t self_in, mp_obj_t channel_in, mp_obj_t attr_in, struct sensor_value *res) {
    mp_obj_sensor_t *self = MP_OBJ_TO_PTR(self_in);

    int st = sensor_attr_get(self->dev, mp_obj_get_int(channel_in), mp_obj_get_int(attr_in), res);
    if (st != 0) {
        mp_raise_OSError(-st);
    }
}

static mp_obj_t sensor_attr_get_float(mp_obj_t self_in, mp_obj_t channel_in, mp_obj_t attr_in) {
    struct sensor_value val;
    sensor_attr_get_internal(self_in, channel_in, attr_in, &val);
    return mp_obj_new_float(val.val1 + (mp_float_t)val.val2 / 1000000);
}
MP_DEFINE_CONST_FUN_OBJ_3(sensor_attr_get_float_obj, sensor_attr_get_float);

static mp_obj_t sensor_attr_get_micros(mp_obj_t self_in, mp_obj_t channel_in, mp_obj_t attr_in) {
    struct sensor_value val;
    sensor_attr_get_internal(self_in, channel_in, attr_in, &val);
    return MP_OBJ_NEW_SMALL_INT(val.val1 * 1000000 + val.val2);
}
MP_DEFINE_CONST_FUN_OBJ_3(sensor_attr_get_micros_obj, sensor_attr_get_micros);

static mp_obj_t sensor_attr_get_millis(mp_obj_t self_in, mp_obj_t channel_in, mp_obj_t attr_in) {
    struct sensor_value val;
    sensor_attr_get_internal(self_in, channel_in, attr_in, &val);
    return MP_OBJ_NEW_SMALL_INT(val.val1 * 1000 + val.val2 / 1000);
}
MP_DEFINE_CONST_FUN_OBJ_3(sensor_attr_get_millis_obj, sensor_attr_get_millis);

static mp_obj_t sensor_attr_get_int(mp_obj_t self_in, mp_obj_t channel_in, mp_obj_t attr_in) {
    struct sensor_value val;
    sensor_attr_get_internal(self_in, channel_in, attr_in, &val);
    return MP_OBJ_NEW_SMALL_INT(val.val1);
}
MP_DEFINE_CONST_FUN_OBJ_3(sensor_attr_get_int_obj, sensor_attr_get_int);

static mp_obj_t mp_sensor_attr_set(size_t n_args, const mp_obj_t *args) {
    mp_obj_sensor_t *self = MP_OBJ_TO_PTR(args[0]);
    mp_obj_t channel_in = args[1];
    mp_obj_t attr_in = args[2];
    struct sensor_value val;
    if (n_args == 4) {
        // One float argument
        float v = mp_obj_get_float(args[3]);
        val.val1 = (int32_t)v;
        val.val2 = (int32_t)((v - val.val1) * 1000000);
    } else {
        // Two integer arguments
        val.val1 = mp_obj_get_int(args[3]);
        val.val2 = mp_obj_get_int(args[4]);
    }
    int st = sensor_attr_set(self->dev, mp_obj_get_int(channel_in), mp_obj_get_int(attr_in), &val);
    if (st != 0) {
        mp_raise_OSError(-st);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(sensor_attr_set_obj, 4, 5, mp_sensor_attr_set);

static const mp_rom_map_elem_t sensor_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_measure), MP_ROM_PTR(&sensor_measure_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_float), MP_ROM_PTR(&sensor_get_float_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_micros), MP_ROM_PTR(&sensor_get_micros_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_millis), MP_ROM_PTR(&sensor_get_millis_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_int), MP_ROM_PTR(&sensor_get_int_obj) },
    { MP_ROM_QSTR(MP_QSTR_attr_get_float), MP_ROM_PTR(&sensor_attr_get_float_obj) },
    { MP_ROM_QSTR(MP_QSTR_attr_get_micros), MP_ROM_PTR(&sensor_attr_get_micros_obj) },
    { MP_ROM_QSTR(MP_QSTR_attr_get_millis), MP_ROM_PTR(&sensor_attr_get_millis_obj) },
    { MP_ROM_QSTR(MP_QSTR_attr_get_int), MP_ROM_PTR(&sensor_attr_get_int_obj) },
    { MP_ROM_QSTR(MP_QSTR_attr_set), MP_ROM_PTR(&sensor_attr_set_obj) },
};

static MP_DEFINE_CONST_DICT(sensor_locals_dict, sensor_locals_dict_table);

static MP_DEFINE_CONST_OBJ_TYPE(
    sensor_type,
    MP_QSTR_Sensor,
    MP_TYPE_FLAG_NONE,
    make_new, sensor_make_new,
    locals_dict, &sensor_locals_dict
    );

static const mp_rom_map_elem_t mp_module_zsensor_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_zsensor) },
    { MP_ROM_QSTR(MP_QSTR_Sensor), MP_ROM_PTR(&sensor_type) },

#define C(name) { MP_ROM_QSTR(MP_QSTR_##name), MP_ROM_INT(SENSOR_CHAN_##name) }
    C(ACCEL_X),
    C(ACCEL_Y),
    C(ACCEL_Z),
    C(ACCEL_XYZ),
    C(GYRO_X),
    C(GYRO_Y),
    C(GYRO_Z),
    C(GYRO_XYZ),
    C(MAGN_X),
    C(MAGN_Y),
    C(MAGN_Z),
    C(MAGN_XYZ),
    C(DIE_TEMP),
    C(AMBIENT_TEMP),
    C(PRESS),
    C(PROX),
    C(HUMIDITY),
    C(LIGHT),
    C(IR),
    C(RED),
    C(GREEN),
    C(BLUE),
    C(ALTITUDE),
    C(PM_1_0),
    C(PM_2_5),
    C(PM_10),
    C(DISTANCE),
    C(CO2),
    C(VOC),
    C(GAS_RES),
    C(VOLTAGE),
#undef C
#define C(name) { MP_ROM_QSTR(MP_QSTR_ATTR_##name), MP_ROM_INT(SENSOR_ATTR_##name) }
    C(SAMPLING_FREQUENCY),
    C(LOWER_THRESH),
    C(UPPER_THRESH),
    C(SLOPE_TH),
    C(SLOPE_DUR),
    C(HYSTERESIS),
    C(OVERSAMPLING),
    C(FULL_SCALE),
    C(OFFSET),
    C(CALIB_TARGET),
    C(CONFIGURATION),
    C(CALIBRATION),
    C(FEATURE_MASK),
    C(ALERT),
    C(FF_DUR),
    C(BATCH_DURATION),
    C(GAIN),
    C(RESOLUTION),
#undef C
};

static MP_DEFINE_CONST_DICT(mp_module_zsensor_globals, mp_module_zsensor_globals_table);

const mp_obj_module_t mp_module_zsensor = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_zsensor_globals,
};

MP_REGISTER_MODULE(MP_QSTR_zsensor, mp_module_zsensor);

#endif // MICROPY_PY_HASHLIB
