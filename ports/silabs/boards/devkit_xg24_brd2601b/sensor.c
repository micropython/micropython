/*
 * This file is part of Adafruit for EFR32 project
 *
 * The MIT License (MIT)
 *
 * Copyright 2023 Silicon Laboratories Inc. www.silabs.com
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

#include "py/runtime.h"
#include "common-hal/busio/I2C.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "supervisor/shared/translate/translate.h"
#include "em_i2c.h"
#include "sl_i2cspm.h"
#include "sl_i2cspm_sensor_config.h"
#include "sl_sensor_lux.h"
#include "sl_sensor_rht.h"
#include "sl_sensor_imu.h"
#include "sl_sensor_hall.h"
#include "sl_sensor_pressure.h"

STATIC mp_obj_t sensor_init(busio_i2c_obj_t *i2c) {
    sl_status_t sc;

    if (!common_hal_mcu_pin_is_free(&pin_PC9)) {
        mp_raise_ValueError(translate("Pin PC9 is busy "));
        return mp_const_false;
    }

    sc = sl_sensor_rht_init();
    if (sc != SL_STATUS_OK) {
        return mp_const_false;
    }

    sc = sl_sensor_lux_init();
    if (sc != SL_STATUS_OK) {
        return mp_const_false;
    }

    sc = sl_sensor_hall_init();
    if (sc != SL_STATUS_OK) {
        return mp_const_false;
    }

    sc = sl_sensor_pressure_init();
    if (sc != SL_STATUS_OK) {
        return mp_const_false;
    }

    sl_sensor_imu_init();
    sc = sl_sensor_imu_enable(true);
    if (sc != SL_STATUS_OK) {
        return mp_const_false;
    }

    common_hal_mcu_pin_claim(&pin_PC9);
    return mp_const_true;
}

STATIC mp_obj_t sensor_deinit() {

    sl_sensor_hall_deinit();
    sl_sensor_lux_deinit();
    sl_sensor_rht_deinit();
    sl_sensor_pressure_deinit();
    sl_sensor_imu_enable(false);
    sl_sensor_imu_deinit();
    common_hal_reset_pin(&pin_PC9);
    return mp_const_true;
}

STATIC mp_obj_t sensor_get_temperature(void) {
    sl_status_t sc;
    uint32_t rh;
    int32_t t;
    sc = sl_sensor_rht_get(&rh, &t);

    if (sc != SL_STATUS_OK) {
        return mp_const_false;
    }

    return mp_obj_new_float((float)t / 1000.0f);
}

STATIC mp_obj_t sensor_get_humidity(void) {
    sl_status_t sc;
    uint32_t rh;
    int32_t t;
    sc = sl_sensor_rht_get(&rh, &t);

    if (sc != SL_STATUS_OK) {
        return mp_const_false;
    }
    return mp_obj_new_float((float)rh / 1000.0f);
}

STATIC mp_obj_t sensor_get_lux(void) {
    sl_status_t sc;
    float lux;
    sc = sl_sensor_lux_get(&lux);

    if (sc != SL_STATUS_OK) {
        return mp_const_false;
    }
    return mp_obj_new_float(lux);
}

STATIC mp_obj_t sensor_get_hall(void) {
    sl_status_t sc;
    float field_strength;
    bool alert;
    bool tamper;
    sc = sl_sensor_hall_get(&field_strength, &alert, &tamper);

    if (sc != SL_STATUS_OK) {
        return mp_const_false;
    }
    return mp_obj_new_float(field_strength);
}

STATIC mp_obj_t sensor_get_pressure(void) {
    sl_status_t sc;
    float pressure;
    sc = sl_sensor_pressure_get(&pressure);

    if (sc != SL_STATUS_OK) {
        return mp_const_false;
    }
    return mp_obj_new_float(pressure);
}

STATIC mp_obj_t sensor_imu_get(void) {
    sl_status_t sc;
    int16_t ovec[3];
    int16_t avec[3];
    mp_obj_t ovec_obj[3];
    mp_obj_t avec_obj[3];
    mp_obj_t ret[2];

    sc = sl_sensor_imu_get(ovec, avec);
    if (sc != SL_STATUS_OK) {
        return mp_const_false;
    }

    ovec_obj[0] = mp_obj_new_int(ovec[0]);
    ovec_obj[1] = mp_obj_new_int(ovec[1]);
    ovec_obj[2] = mp_obj_new_int(ovec[2]);

    avec_obj[0] = mp_obj_new_int(avec[0]);
    avec_obj[1] = mp_obj_new_int(avec[1]);
    avec_obj[2] = mp_obj_new_int(avec[2]);
    ret[0] = mp_obj_new_list(3,ovec_obj);
    ret[1] = mp_obj_new_list(3,avec_obj);
    return mp_obj_new_tuple(2,ret);
}

STATIC mp_obj_t sensor_imu_calibrate(void) {
    sl_status_t sc;
    sc = sl_sensor_imu_calibrate();
    if (sc != SL_STATUS_OK) {
        return mp_const_false;
    }
    return mp_const_true;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(sensor_init_obj,sensor_init);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(sensor_deinit_obj,sensor_deinit);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(sensor_get_temperature_obj,sensor_get_temperature);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(sensor_get_humidity_obj,sensor_get_humidity);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(sensor_get_lux_obj,sensor_get_lux);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(sensor_get_hall_obj,sensor_get_hall);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(sensor_get_pressure_obj,sensor_get_pressure);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(sensor_imu_get_obj,sensor_imu_get);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(sensor_imu_calibrate_obj,sensor_imu_calibrate);


STATIC const mp_rom_map_elem_t sensor_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_sensor) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&sensor_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&sensor_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_temperature), MP_ROM_PTR(&sensor_get_temperature_obj) },
    { MP_ROM_QSTR(MP_QSTR_humidity), MP_ROM_PTR(&sensor_get_humidity_obj) },
    { MP_ROM_QSTR(MP_QSTR_lux), MP_ROM_PTR(&sensor_get_lux_obj) },
    { MP_ROM_QSTR(MP_QSTR_hall), MP_ROM_PTR(&sensor_get_hall_obj) },
    { MP_ROM_QSTR(MP_QSTR_pressure), MP_ROM_PTR(&sensor_get_pressure_obj) },
    { MP_ROM_QSTR(MP_QSTR_imu), MP_ROM_PTR(&sensor_imu_get_obj) },
    { MP_ROM_QSTR(MP_QSTR_imu_calibrate), MP_ROM_PTR(&sensor_imu_calibrate_obj) },
};
STATIC MP_DEFINE_CONST_DICT(sensor_module_globals, sensor_globals_table);

const mp_obj_module_t sensor_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&sensor_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_sensor, sensor_module,1);
