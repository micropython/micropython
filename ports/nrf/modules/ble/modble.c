/*
 * This file is part of the MicroPython project, http://micropython.org/
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

/// \method address()
/// Return device address as text string.
mp_obj_t ble_obj_address(void) {
    ble_drv_addr_t local_addr;
    ble_drv_address_get(&local_addr);

    vstr_t vstr;
    vstr_init(&vstr, 17);

    vstr_printf(&vstr, ""HEX2_FMT":"HEX2_FMT":"HEX2_FMT":" \
                         HEX2_FMT":"HEX2_FMT":"HEX2_FMT"",
                local_addr.addr[5], local_addr.addr[4], local_addr.addr[3],
                local_addr.addr[2], local_addr.addr[1], local_addr.addr[0]);

    mp_obj_t mac_str = mp_obj_new_str(vstr.buf, vstr.len);

    vstr_clear(&vstr);

    return mac_str;
}

static MP_DEFINE_CONST_FUN_OBJ_0(ble_obj_enable_obj, ble_obj_enable);
static MP_DEFINE_CONST_FUN_OBJ_0(ble_obj_disable_obj, ble_obj_disable);
static MP_DEFINE_CONST_FUN_OBJ_0(ble_obj_enabled_obj, ble_obj_enabled);
static MP_DEFINE_CONST_FUN_OBJ_0(ble_obj_address_obj, ble_obj_address);

static const mp_rom_map_elem_t ble_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ble) },
    { MP_ROM_QSTR(MP_QSTR_enable),   MP_ROM_PTR(&ble_obj_enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_disable),  MP_ROM_PTR(&ble_obj_disable_obj) },
    { MP_ROM_QSTR(MP_QSTR_enabled),  MP_ROM_PTR(&ble_obj_enabled_obj) },
    { MP_ROM_QSTR(MP_QSTR_address),  MP_ROM_PTR(&ble_obj_address_obj) },
};


static MP_DEFINE_CONST_DICT(ble_module_globals, ble_module_globals_table);

const mp_obj_module_t ble_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&ble_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_ble, ble_module);

#endif // MICROPY_PY_BLE
