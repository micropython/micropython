/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Glenn Ruben Bakke
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

#include <stdbool.h>

#include "py/runtime.h"
#include "nrf_power.h"

#if BLUETOOTH_SD
#include "nrf_soc.h"
#include "ble_drv.h"
#define BLUETOOTH_STACK_ENABLED() (ble_drv_stack_enabled())

static void dcdc_set_sd_aware(bool state) {
    if (BLUETOOTH_STACK_ENABLED()) {
        sd_power_dcdc_mode_set(state);
    } else {
        nrf_power_dcdcen_set(state);
    }
}
#endif // BLUETOOTH_SD

mp_obj_t dcdc_set(mp_obj_t self_in) {
    bool dcdc_state = false;

    if (mp_obj_is_true(self_in)) {
        dcdc_state = true;
    }
#if BLUETOOTH_SD
    dcdc_set_sd_aware(dcdc_state);
#else
    nrf_power_dcdcen_set(dcdc_state);
#endif
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(dcdc_set_obj, dcdc_set);

STATIC const mp_rom_map_elem_t nrf_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_nrf) },
    { MP_ROM_QSTR(MP_QSTR_dcdc_set), MP_ROM_PTR(&dcdc_set_obj) },
};

STATIC MP_DEFINE_CONST_DICT(nrf_module_globals, nrf_module_globals_table);

const mp_obj_module_t nrf_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&nrf_module_globals,
};
