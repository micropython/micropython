/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Glenn Ruben Bakke
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
#include "py/obj.h"
#include "py/runtime.h"
#include "py/objstr.h"
#include "py/objlist.h"
#include "py/objarray.h"
#include "py/objtuple.h"
#include "py/qstr.h"

#if MICROPY_PY_UBLUEPY_CENTRAL

#include "ble_drv.h"

STATIC void ubluepy_scan_entry_print(const mp_print_t *print, mp_obj_t o, mp_print_kind_t kind) {
    ubluepy_scan_entry_obj_t * self = (ubluepy_scan_entry_obj_t *)o;
    (void)self;
    mp_printf(print, "ScanEntry");
}

/// \method addr()
/// Return address as text string.
///
STATIC mp_obj_t scan_entry_get_addr(mp_obj_t self_in) {
    ubluepy_scan_entry_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return self->addr;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bluepy_scan_entry_get_addr_obj, scan_entry_get_addr);

/// \method addr_type()
/// Return address type value.
///
STATIC mp_obj_t scan_entry_get_addr_type(mp_obj_t self_in) {
    ubluepy_scan_entry_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(self->addr_type);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bluepy_scan_entry_get_addr_type_obj, scan_entry_get_addr_type);

/// \method rssi()
/// Return RSSI value.
///
STATIC mp_obj_t scan_entry_get_rssi(mp_obj_t self_in) {
    ubluepy_scan_entry_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(self->rssi);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(bluepy_scan_entry_get_rssi_obj, scan_entry_get_rssi);

/// \method getScanData()
/// Return list of the scan data tupples (ad_type, description, value)
///
STATIC mp_obj_t scan_entry_get_scan_data(mp_obj_t self_in) {
    ubluepy_scan_entry_obj_t * self = MP_OBJ_TO_PTR(self_in);

    mp_obj_t retval_list = mp_obj_new_list(0, NULL);

    // TODO: check if self->data is set
    mp_obj_array_t * data = MP_OBJ_TO_PTR(self->data);

    uint16_t byte_index = 0;

    while (byte_index < data->len) {
        mp_obj_tuple_t *t = MP_OBJ_TO_PTR(mp_obj_new_tuple(3, NULL));

        uint8_t adv_item_len  = ((uint8_t * )data->items)[byte_index];
        uint8_t adv_item_type = ((uint8_t * )data->items)[byte_index + 1];

        mp_obj_t description = mp_const_none;

        mp_map_t *constant_map = mp_obj_dict_get_map(ubluepy_constants_ad_types_type.locals_dict);
        mp_map_elem_t *ad_types_table = MP_OBJ_TO_PTR(constant_map->table);

        uint16_t num_of_elements = constant_map->used;

        for (uint16_t i = 0; i < num_of_elements; i++) {
            mp_map_elem_t element = (mp_map_elem_t)*ad_types_table;
            ad_types_table++;
            uint16_t element_value = mp_obj_get_int(element.value);

            if (adv_item_type == element_value) {
                qstr key_qstr = MP_OBJ_QSTR_VALUE(element.key);
                const char * text = qstr_str(key_qstr);
                size_t len = qstr_len(key_qstr);

                vstr_t vstr;
                vstr_init(&vstr, len);
                vstr_printf(&vstr, "%s", text);
                description = mp_obj_new_str(vstr.buf, vstr.len);
                vstr_clear(&vstr);
            }
        }

        t->items[0] = MP_OBJ_NEW_SMALL_INT(adv_item_type);
        t->items[1] = description;
        t->items[2] = mp_obj_new_bytearray(adv_item_len - 1,
                                           &((uint8_t * )data->items)[byte_index + 2]);
        mp_obj_list_append(retval_list, MP_OBJ_FROM_PTR(t));

        byte_index += adv_item_len + 1;
    }

    return retval_list;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ubluepy_scan_entry_get_scan_data_obj, scan_entry_get_scan_data);

STATIC const mp_rom_map_elem_t ubluepy_scan_entry_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_addr),        MP_ROM_PTR(&bluepy_scan_entry_get_addr_obj) },
    { MP_ROM_QSTR(MP_QSTR_addr_type),   MP_ROM_PTR(&bluepy_scan_entry_get_addr_type_obj) },
    { MP_ROM_QSTR(MP_QSTR_rssi),        MP_ROM_PTR(&bluepy_scan_entry_get_rssi_obj) },
    { MP_ROM_QSTR(MP_QSTR_getScanData), MP_ROM_PTR(&ubluepy_scan_entry_get_scan_data_obj) },
};

STATIC MP_DEFINE_CONST_DICT(ubluepy_scan_entry_locals_dict, ubluepy_scan_entry_locals_dict_table);

const mp_obj_type_t ubluepy_scan_entry_type = {
    { &mp_type_type },
    .name = MP_QSTR_ScanEntry,
    .print = ubluepy_scan_entry_print,
    .locals_dict = (mp_obj_dict_t*)&ubluepy_scan_entry_locals_dict
};

#endif // MICROPY_PY_UBLUEPY_CENTRAL
