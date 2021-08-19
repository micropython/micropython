/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 ChenYong (chenyong@rt-thread.com)
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
#include <stdint.h>
#include <string.h>

#include "py/nlr.h"
#include "py/runtime.h"
#include "modmachine.h"
#include "mphalport.h"

#ifdef MICROPYTHON_USING_MACHINE_ADC

#include <rtthread.h>
#include <drivers/adc.h>

extern const mp_obj_type_t machine_adc_type;

typedef struct _machine_adc_obj_t {
    mp_obj_base_t base;
    struct rt_adc_device *adc_device;
    uint8_t channel;
    uint8_t is_init;
} machine_adc_obj_t;

STATIC void error_check(bool status, const char *msg) {
    if (!status) {
        nlr_raise(mp_obj_new_exception_msg(&mp_type_OSError, msg));
    }
}

STATIC mp_obj_t machine_adc_make_new(const mp_obj_type_t *type,
                                  size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // create ADC object from the given pin
    machine_adc_obj_t *self = m_new_obj(machine_adc_obj_t);
    struct rt_adc_device *adc_device = RT_NULL;
    char adc_dev_name[RT_NAME_MAX] = {0};
    rt_err_t result = RT_EOK;

    // init machine adc object information
    self->channel = 0;
    self->is_init = RT_FALSE;
    self->base.type = &machine_adc_type;

    mp_arg_check_num(n_args, n_kw, 1, 2, true);

    // check input ADC device name or ID
    if (mp_obj_is_small_int(args[0])) {
        rt_snprintf(adc_dev_name, sizeof(adc_dev_name), "adc%d", mp_obj_get_int(args[0]));
    } else if (mp_obj_is_qstr(args[0])) {
        rt_strncpy(adc_dev_name, mp_obj_str_get_str(args[0]), RT_NAME_MAX);
    } else {
        error_check(0, "Input ADC device name or ID error.");
    }

    adc_device = (struct rt_adc_device *) rt_device_find(adc_dev_name);
    if (adc_device == RT_NULL || adc_device->parent.type != RT_Device_Class_Miscellaneous) {
        nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError,
                                                "ADC(%s) don't exist", adc_dev_name));
    }
    self->adc_device = adc_device;

    if (n_args == 2) {
        self->channel = mp_obj_get_int(args[1]);
        result = rt_adc_enable(self->adc_device, self->channel);
        error_check(result == RT_EOK, "ADC enable error");
        self->is_init = RT_TRUE;
    }

    return MP_OBJ_FROM_PTR(self);
}

STATIC mp_obj_t machine_adc_init(size_t n_args, const mp_obj_t *args) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    rt_err_t result = RT_EOK;

    result = rt_adc_enable(self->adc_device, mp_obj_get_int(args[1]));
    error_check(result == RT_EOK, "ADC enable error");
    self->channel = mp_obj_get_int(args[1]);
    self->is_init = RT_TRUE;

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(machine_adc_init_obj, 2, 2, machine_adc_init);

STATIC mp_obj_t machine_adc_deinit(mp_obj_t self_in) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    rt_err_t result = RT_EOK;

    if (self->is_init == RT_TRUE) {
        result = rt_adc_disable(self->adc_device, self->channel);
        error_check(result == RT_EOK, "ADC disable error");
        self->is_init = RT_FALSE;
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_adc_deinit_obj, machine_adc_deinit);

STATIC mp_obj_t machine_adc_read(mp_obj_t self_in) {
    machine_adc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    int tval = 0;

    error_check(self->is_init == RT_TRUE, "ADC device uninitialized");

    tval = rt_adc_read(self->adc_device, self->channel);
    return MP_OBJ_NEW_SMALL_INT(tval);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(machine_adc_read_obj, machine_adc_read);

STATIC const mp_rom_map_elem_t machine_adc_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init),    MP_ROM_PTR(&machine_adc_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),  MP_ROM_PTR(&machine_adc_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_read),    MP_ROM_PTR(&machine_adc_read_obj) },
};

STATIC MP_DEFINE_CONST_DICT(machine_adc_locals_dict,
                            machine_adc_locals_dict_table);

const mp_obj_type_t machine_adc_type = {
    { &mp_type_type },
    .name = MP_QSTR_ADC,
    .make_new = machine_adc_make_new,
    .locals_dict = (mp_obj_dict_t *) &machine_adc_locals_dict,
};

#endif // MICROPYTHON_USING_MACHINE_adc
