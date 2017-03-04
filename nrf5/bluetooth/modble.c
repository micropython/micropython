/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Glenn Ruben Bakke
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
#include "py/runtime.h"

#if MICROPY_PY_BLE

#include "led.h"
#include "mpconfigboard.h"
#include "ble_drv.h"

/// \method enable()
/// Enable BLE softdevice.
mp_obj_t ble_obj_enable(void) {
    printf("SoftDevice enabled\n");
    uint32_t err_code = ble_drv_stack_enable();
    if (err_code < 0) {
        // TODO: raise exception.
    }
    return mp_const_none;
}

/// \method disable()
/// Disable BLE softdevice.
mp_obj_t ble_obj_disable(void) {
    ble_drv_stack_disable();
    return mp_const_none;
}

/// \method enabled()
/// Get state of whether the softdevice is enabled or not.
mp_obj_t ble_obj_enabled(void) {
    uint8_t is_enabled = ble_drv_stack_enabled();
    mp_int_t enabled = is_enabled;
    return MP_OBJ_NEW_SMALL_INT(enabled);
}

/// \method address_print()
/// Print device address.
mp_obj_t ble_obj_address_print(void) {
    ble_drv_address_get();
    return mp_const_none;
}

/// \method advertise()
/// Bluetooth Low Energy advertise.
mp_obj_t ble_obj_advertise(void) {
    ble_drv_advertise();
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(ble_obj_enable_obj, ble_obj_enable);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(ble_obj_disable_obj, ble_obj_disable);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(ble_obj_enabled_obj, ble_obj_enabled);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(ble_obj_address_print_obj, ble_obj_address_print);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(ble_obj_advertise_obj, ble_obj_advertise);

STATIC const mp_map_elem_t ble_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_ble) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_enable), (mp_obj_t)&ble_obj_enable_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_disable), (mp_obj_t)&ble_obj_disable_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_enabled), (mp_obj_t)&ble_obj_enabled_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_address_print), (mp_obj_t)&ble_obj_address_print_obj},
    { MP_OBJ_NEW_QSTR(MP_QSTR_advertise), (mp_obj_t)&ble_obj_advertise_obj},
};


STATIC MP_DEFINE_CONST_DICT(ble_module_globals, ble_module_globals_table);

const mp_obj_module_t ble_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&ble_module_globals,
};

#endif // MICROPY_PY_BLE
