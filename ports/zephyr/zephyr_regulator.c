/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2026 Vogl Electronic GmbH
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

#ifdef CONFIG_REGULATOR

#include <zephyr/kernel.h>
#include <zephyr/drivers/regulator.h>
#include "zephyr_device.h"

typedef struct _zephyr_regulator_obj_t {
    mp_obj_base_t base;
    const struct device *dev;
} zephyr_regulator_obj_t;

static mp_obj_t zephyr_regulator_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);
    zephyr_regulator_obj_t *o = mp_obj_malloc(zephyr_regulator_obj_t, type);
    o->dev = zephyr_device_find(args[0]);
    if (!DEVICE_API_IS(regulator, o->dev)) {
        #if MICROPY_ERROR_REPORTING <= MICROPY_ERROR_REPORTING_TERSE
        mp_raise_ValueError(MP_ERROR_TEXT("not a regulator device"));
        #else
        mp_raise_msg_varg(&mp_type_ValueError, MP_ERROR_TEXT("device %s is not a regulator"), o->dev->name);
        #endif
    }
    return MP_OBJ_FROM_PTR(o);
}

static void zephyr_regulator_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    zephyr_regulator_obj_t *self = self_in;
    const char *name = zephyr_device_get_name(self->dev);
    mp_printf(print, "<Regulator %s>", name);
}

static mp_obj_t zephyr_regulator_enable(mp_obj_t self_in) {
    zephyr_regulator_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int ret = regulator_enable(self->dev);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(zephyr_regulator_enable_obj, zephyr_regulator_enable);

static mp_obj_t zephyr_regulator_disable(mp_obj_t self_in) {
    zephyr_regulator_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int ret = regulator_disable(self->dev);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(zephyr_regulator_disable_obj, zephyr_regulator_disable);

static mp_obj_t zephyr_regulator_is_enabled(mp_obj_t self_in) {
    zephyr_regulator_obj_t *self = MP_OBJ_TO_PTR(self_in);

    return mp_obj_new_bool(regulator_is_enabled(self->dev));
}
MP_DEFINE_CONST_FUN_OBJ_1(zephyr_regulator_is_enabled_obj, zephyr_regulator_is_enabled);

static mp_obj_t zephyr_regulator_list_voltages(mp_obj_t self_in) {
    zephyr_regulator_obj_t *self = MP_OBJ_TO_PTR(self_in);
    unsigned int cnt;
    mp_obj_list_t *list;
    int32_t volt_uv;

    cnt = regulator_count_voltages(self->dev);

    list = MP_OBJ_TO_PTR(mp_obj_new_list(cnt, NULL));

    for (unsigned int i = 0U; i < cnt; i++) {

        (void)regulator_list_voltage(self->dev, i, &volt_uv);

        list->items[i] = mp_obj_new_int(volt_uv);
    }

    return MP_OBJ_FROM_PTR(list);
}
MP_DEFINE_CONST_FUN_OBJ_1(zephyr_regulator_list_voltages_obj, zephyr_regulator_list_voltages);

static mp_obj_t zephyr_regulator_set_voltage(size_t n_args, const mp_obj_t *args) {
    zephyr_regulator_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    int32_t min_uv = mp_obj_get_int(args[1]);
    int32_t max_uv = (n_args == 3) ? mp_obj_get_int(args[2]) : min_uv;

    int ret = regulator_set_voltage(self->dev, min_uv, max_uv);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(zephyr_regulator_set_voltage_obj, 2, 3, zephyr_regulator_set_voltage);

static mp_obj_t zephyr_regulator_get_voltage(mp_obj_t self_in) {
    zephyr_regulator_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int32_t volt_uv;
    int ret = regulator_get_voltage(self->dev, &volt_uv);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }
    return mp_obj_new_int(volt_uv);
}
MP_DEFINE_CONST_FUN_OBJ_1(zephyr_regulator_get_voltage_obj, zephyr_regulator_get_voltage);


static mp_obj_t zephyr_regulator_list_current_limits(mp_obj_t self_in) {
    zephyr_regulator_obj_t *self = MP_OBJ_TO_PTR(self_in);
    unsigned int cnt;
    mp_obj_list_t *list;
    int32_t current_ua;

    cnt = regulator_count_current_limits(self->dev);

    list = MP_OBJ_TO_PTR(mp_obj_new_list(cnt, NULL));

    for (unsigned int i = 0U; i < cnt; i++) {

        (void)regulator_list_current_limit(self->dev, i, &current_ua);

        list->items[i] = mp_obj_new_int(current_ua);
    }

    return MP_OBJ_FROM_PTR(list);
}
MP_DEFINE_CONST_FUN_OBJ_1(zephyr_regulator_list_current_limits_obj, zephyr_regulator_list_current_limits);

static mp_obj_t zephyr_regulator_set_current_limit(size_t n_args, const mp_obj_t *args) {
    zephyr_regulator_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    int32_t min_ua = mp_obj_get_int(args[1]);
    int32_t max_ua = (n_args == 3) ? mp_obj_get_int(args[2]) : min_ua;

    int ret = regulator_set_current_limit(self->dev, min_ua, max_ua);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(zephyr_regulator_set_current_limit_obj, 2, 3, zephyr_regulator_set_current_limit);

static mp_obj_t zephyr_regulator_get_current_limit(mp_obj_t self_in) {
    zephyr_regulator_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int32_t current_ua;
    int ret = regulator_get_current_limit(self->dev, &current_ua);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }
    return mp_obj_new_int(current_ua);
}
MP_DEFINE_CONST_FUN_OBJ_1(zephyr_regulator_get_current_limit_obj, zephyr_regulator_get_current_limit);

static mp_obj_t zephyr_regulator_set_active_discharge(mp_obj_t self_in, mp_obj_t enable_in) {
    zephyr_regulator_obj_t *self = MP_OBJ_TO_PTR(self_in);

    int ret = regulator_set_active_discharge(self->dev, mp_obj_is_true(enable_in));
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(zephyr_regulator_set_active_discharge_obj, zephyr_regulator_set_active_discharge);

static mp_obj_t zephyr_regulator_get_active_discharge(mp_obj_t self_in) {
    zephyr_regulator_obj_t *self = MP_OBJ_TO_PTR(self_in);
    bool active_discharge;
    int ret = regulator_get_active_discharge(self->dev, &active_discharge);
    if (ret < 0) {
        mp_raise_OSError(-ret);
    }
    return mp_obj_new_bool(active_discharge);
}
MP_DEFINE_CONST_FUN_OBJ_1(zephyr_regulator_get_active_discharge_obj, zephyr_regulator_get_active_discharge);

static const mp_rom_map_elem_t zephyr_regulator_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_enable), MP_ROM_PTR(&zephyr_regulator_enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable), MP_ROM_PTR(&zephyr_regulator_disable_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_enabled), MP_ROM_PTR(&zephyr_regulator_is_enabled_obj) },
    { MP_ROM_QSTR(MP_QSTR_list_voltages), MP_ROM_PTR(&zephyr_regulator_list_voltages_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_voltage), MP_ROM_PTR(&zephyr_regulator_set_voltage_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_voltage), MP_ROM_PTR(&zephyr_regulator_get_voltage_obj) },
    { MP_ROM_QSTR(MP_QSTR_list_current_limits), MP_ROM_PTR(&zephyr_regulator_list_current_limits_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_current_limit), MP_ROM_PTR(&zephyr_regulator_set_current_limit_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_current_limit), MP_ROM_PTR(&zephyr_regulator_get_current_limit_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_active_discharge), MP_ROM_PTR(&zephyr_regulator_set_active_discharge_obj) },
    { MP_ROM_QSTR(MP_QSTR_get_active_discharge), MP_ROM_PTR(&zephyr_regulator_get_active_discharge_obj) },
};

static MP_DEFINE_CONST_DICT(zephyr_regulator_locals_dict, zephyr_regulator_locals_dict_table);

const mp_obj_type_t zephyr_regulator_type;

MP_DEFINE_CONST_OBJ_TYPE(
    zephyr_regulator_type,
    MP_QSTR_Regulator,
    MP_TYPE_FLAG_NONE,
    make_new, zephyr_regulator_make_new,
    print, zephyr_regulator_print,
    locals_dict, &zephyr_regulator_locals_dict
    );

#endif // CONFIG_REGULATOR
