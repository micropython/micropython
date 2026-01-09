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

typedef struct _mp_obj_sensor_trigger_t {
    struct sensor_trigger trigger;
    mp_obj_t callback;
    struct _mp_obj_sensor_trigger_t *next;
} mp_obj_sensor_trigger_t;

typedef struct _mp_obj_sensor_t {
    mp_obj_base_t base;
    const struct device *dev;
    mp_obj_sensor_trigger_t *trigger_list;  // List of active triggers for this sensor
    struct _mp_obj_sensor_t *next;  // For linked list of active sensors
} mp_obj_sensor_t;

// Linked list of sensors with active callbacks
static mp_obj_sensor_t *sensor_callback_list = NULL;

static mp_obj_t sensor_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    mp_obj_sensor_t *o = mp_obj_malloc(mp_obj_sensor_t, type);
    o->dev = zephyr_device_find(args[0]);
    o->trigger_list = NULL;
    o->next = NULL;
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

// Zephyr sensor trigger handler (runs in interrupt context)
static void sensor_trigger_handler(const struct device *dev, const struct sensor_trigger *trigger) {
    // Find the sensor object that matches this device
    for (mp_obj_sensor_t *sensor = sensor_callback_list; sensor != NULL; sensor = sensor->next) {
        if (sensor->dev == dev) {
            // Check all triggers for this sensor
            for (mp_obj_sensor_trigger_t *sensor_trigger = sensor->trigger_list;
                 sensor_trigger != NULL;
                 sensor_trigger = sensor_trigger->next) {
                if (sensor_trigger->trigger.type == trigger->type &&
                    sensor_trigger->trigger.chan == trigger->chan) {
                    // Schedule callback on main thread - pass sensor object only
                    // The callback can query the sensor for updated data
                    mp_sched_schedule(sensor_trigger->callback, MP_OBJ_FROM_PTR(sensor));
                }
            }
            break;
        }
    }
}

static mp_obj_t mp_sensor_trigger_set(size_t n_args, const mp_obj_t *args) {
    mp_obj_t self_in = args[0];
    mp_obj_t trigger_type_in = args[1];
    mp_obj_t callback_in;
    int channel;

    // Handle optional channel parameter
    if (n_args == 3) {
        // trigger_set(trigger_type, callback) - use default channel
        callback_in = args[2];
        channel = SENSOR_CHAN_ALL;
    } else {
        // trigger_set(trigger_type, channel, callback)
        mp_obj_t channel_in = args[2];
        callback_in = args[3];
        channel = mp_obj_get_int(channel_in);
    }

    mp_obj_sensor_t *self = MP_OBJ_TO_PTR(self_in);
    int trigger_type = mp_obj_get_int(trigger_type_in);

    // If disabling callback, find and remove the specific trigger
    if (callback_in == mp_const_none) {
        mp_obj_sensor_trigger_t **curr = &self->trigger_list;
        while (*curr != NULL) {
            if ((*curr)->trigger.type == trigger_type && (*curr)->trigger.chan == channel) {
                mp_obj_sensor_trigger_t *to_remove = *curr;
                *curr = to_remove->next;

                // Disable this specific trigger in hardware
                int ret = sensor_trigger_set(self->dev, &to_remove->trigger, NULL);
                if (ret != 0) {
                    mp_raise_OSError(-ret);
                }

                // Free the trigger object
                m_del_obj(mp_obj_sensor_trigger_t, to_remove);
                break;
            }
            curr = &(*curr)->next;
        }

        // If no more triggers for this sensor, remove from sensor callback list
        if (self->trigger_list == NULL) {
            mp_obj_sensor_t **sensor_curr = &sensor_callback_list;
            while (*sensor_curr != NULL) {
                if (*sensor_curr == self) {
                    *sensor_curr = self->next;
                    self->next = NULL;
                    break;
                }
                sensor_curr = &(*sensor_curr)->next;
            }
        }
        return mp_const_none;
    }

    // Check if this trigger already exists - if so, update the callback
    for (mp_obj_sensor_trigger_t *sensor_trigger = self->trigger_list;
         sensor_trigger != NULL;
         sensor_trigger = sensor_trigger->next) {
        if (sensor_trigger->trigger.type == trigger_type &&
            sensor_trigger->trigger.chan == channel) {
            sensor_trigger->callback = callback_in;
            return mp_const_none;
        }
    }

    // Create new trigger entry
    mp_obj_sensor_trigger_t *new_trigger = m_new_obj(mp_obj_sensor_trigger_t);
    new_trigger->trigger.type = trigger_type;
    new_trigger->trigger.chan = channel;
    new_trigger->callback = callback_in;
    new_trigger->next = self->trigger_list;
    self->trigger_list = new_trigger;

    // Add sensor to callback list if not already present
    bool sensor_found = false;
    for (mp_obj_sensor_t *sensor = sensor_callback_list; sensor != NULL; sensor = sensor->next) {
        if (sensor == self) {
            sensor_found = true;
            break;
        }
    }
    if (!sensor_found) {
        self->next = sensor_callback_list;
        sensor_callback_list = self;
    }

    // Set the trigger with Zephyr
    int ret = sensor_trigger_set(self->dev, &new_trigger->trigger, sensor_trigger_handler);
    if (ret != 0) {
        // Remove the trigger we just added since hardware setup failed
        self->trigger_list = new_trigger->next;
        m_del_obj(mp_obj_sensor_trigger_t, new_trigger);
        mp_raise_OSError(-ret);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(sensor_trigger_set_obj, 3, 4, mp_sensor_trigger_set);

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
    { MP_ROM_QSTR(MP_QSTR_trigger_set), MP_ROM_PTR(&sensor_trigger_set_obj) },
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

// Sensor trigger types
#define T(name) { MP_ROM_QSTR(MP_QSTR_TRIG_##name), MP_ROM_INT(SENSOR_TRIG_##name) }
    T(TIMER),
    T(DATA_READY),
    T(DELTA),
    T(NEAR_FAR),
    T(THRESHOLD),
    T(TAP),
    T(DOUBLE_TAP),
    T(FREEFALL),
    T(MOTION),
    T(STATIONARY),
    T(FIFO_WATERMARK),
    T(FIFO_FULL),
#undef T

// Sensor channel types
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

// Register root pointer to prevent GC of sensor callback list and trigger lists
MP_REGISTER_ROOT_POINTER(void *sensor_callback_list);

#endif // MICROPY_PY_ZSENSOR
