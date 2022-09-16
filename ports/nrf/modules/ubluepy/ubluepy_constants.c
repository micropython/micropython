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

#include "py/obj.h"
#include "py/runtime.h"

#if MICROPY_PY_UBLUEPY

#include "modubluepy.h"

STATIC const mp_rom_map_elem_t ubluepy_constants_ad_types_locals_dict_table[] = {
    // GAP AD Types
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_FLAGS),                              MP_ROM_INT(0x01) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_16BIT_SERVICE_UUID_MORE_AVAILABLE),  MP_ROM_INT(0x02) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_16BIT_SERVICE_UUID_COMPLETE),        MP_ROM_INT(0x03) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_32BIT_SERVICE_UUID_MORE_AVAILABLE),  MP_ROM_INT(0x04) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_32BIT_SERVICE_UUID_COMPLETE),        MP_ROM_INT(0x05) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_128BIT_SERVICE_UUID_MORE_AVAILABLE), MP_ROM_INT(0x06) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_128BIT_SERVICE_UUID_COMPLETE),       MP_ROM_INT(0x07) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_SHORT_LOCAL_NAME),                   MP_ROM_INT(0x08) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_COMPLETE_LOCAL_NAME),                MP_ROM_INT(0x09) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_TX_POWER_LEVEL),                     MP_ROM_INT(0x0A) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_CLASS_OF_DEVICE),                    MP_ROM_INT(0x0D) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_SIMPLE_PAIRING_HASH_C),              MP_ROM_INT(0x0E) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_SIMPLE_PAIRING_RANDOMIZER_R),        MP_ROM_INT(0x0F) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_SECURITY_MANAGER_TK_VALUE),          MP_ROM_INT(0x10) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_SECURITY_MANAGER_OOB_FLAGS),         MP_ROM_INT(0x11) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_SLAVE_CONNECTION_INTERVAL_RANGE),    MP_ROM_INT(0x12) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_SOLICITED_SERVICE_UUIDS_16BIT),      MP_ROM_INT(0x14) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_SOLICITED_SERVICE_UUIDS_128BIT),     MP_ROM_INT(0x15) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_SERVICE_DATA),                       MP_ROM_INT(0x16) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_PUBLIC_TARGET_ADDRESS),              MP_ROM_INT(0x17) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_RANDOM_TARGET_ADDRESS),              MP_ROM_INT(0x18) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_APPEARANCE),                         MP_ROM_INT(0x19) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_ADVERTISING_INTERVAL),               MP_ROM_INT(0x1A) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_LE_BLUETOOTH_DEVICE_ADDRESS),        MP_ROM_INT(0x1B) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_LE_ROLE),                            MP_ROM_INT(0x1C) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_SIMPLE_PAIRING_HASH_C256),           MP_ROM_INT(0x1D) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_SIMPLE_PAIRING_RANDOMIZER_R256),     MP_ROM_INT(0x1E) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_SERVICE_DATA_32BIT_UUID),            MP_ROM_INT(0x20) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_SERVICE_DATA_128BIT_UUID),           MP_ROM_INT(0x21) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_URI),                                MP_ROM_INT(0x24) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_3D_INFORMATION_DATA),                MP_ROM_INT(0x3D) },
    { MP_ROM_QSTR(MP_QSTR_AD_TYPE_MANUFACTURER_SPECIFIC_DATA),         MP_ROM_INT(0xFF) },
};

STATIC MP_DEFINE_CONST_DICT(ubluepy_constants_ad_types_locals_dict, ubluepy_constants_ad_types_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    ubluepy_constants_ad_types_type,
    MP_QSTR_ad_types,
    MP_TYPE_FLAG_NONE,
    locals_dict, &ubluepy_constants_ad_types_locals_dict
    );

STATIC const mp_rom_map_elem_t ubluepy_constants_locals_dict_table[] = {
    // GAP events
    { MP_ROM_QSTR(MP_QSTR_EVT_GAP_CONNECTED),       MP_ROM_INT(16) },
    { MP_ROM_QSTR(MP_QSTR_EVT_GAP_DISCONNECTED),    MP_ROM_INT(17) },
    { MP_ROM_QSTR(MP_QSTR_EVT_GATTS_WRITE),         MP_ROM_INT(80) },
    { MP_ROM_QSTR(MP_QSTR_UUID_CCCD),               MP_ROM_INT(0x2902) },

    { MP_ROM_QSTR(MP_QSTR_ADDR_TYPE_PUBLIC),        MP_ROM_INT(UBLUEPY_ADDR_TYPE_PUBLIC) },
    { MP_ROM_QSTR(MP_QSTR_ADDR_TYPE_RANDOM_STATIC), MP_ROM_INT(UBLUEPY_ADDR_TYPE_RANDOM_STATIC) },

    { MP_ROM_QSTR(MP_QSTR_ad_types),                MP_ROM_PTR(&ubluepy_constants_ad_types_type) },
};

STATIC MP_DEFINE_CONST_DICT(ubluepy_constants_locals_dict, ubluepy_constants_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    ubluepy_constants_type,
    MP_QSTR_constants,
    MP_TYPE_FLAG_NONE,
    locals_dict, &ubluepy_constants_locals_dict
    );

#endif // MICROPY_PY_UBLUEPY
