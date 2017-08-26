/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Josef Gajdusek
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

#include "etshal.h"
#include "py/objtuple.h"
#include "py/objstr.h"
#include "genhdr/mpversion.h"
#include "user_interface.h"

STATIC const qstr os_uname_info_fields[] = {
    MP_QSTR_sysname, MP_QSTR_nodename,
    MP_QSTR_release, MP_QSTR_version, MP_QSTR_machine
};
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_sysname_obj, MICROPY_PY_SYS_PLATFORM);
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_nodename_obj, MICROPY_PY_SYS_PLATFORM);
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_version_obj, MICROPY_GIT_TAG " on " MICROPY_BUILD_DATE);
STATIC const MP_DEFINE_STR_OBJ(os_uname_info_machine_obj, MICROPY_HW_BOARD_NAME " with " MICROPY_HW_MCU_NAME);

STATIC mp_obj_tuple_t os_uname_info_obj = {
    .base = {&mp_type_attrtuple},
    .len = 5,
    .items = {
        (mp_obj_t)&os_uname_info_sysname_obj,
        (mp_obj_t)&os_uname_info_nodename_obj,
        NULL,
        (mp_obj_t)&os_uname_info_version_obj,
        (mp_obj_t)&os_uname_info_machine_obj,
        (void *)os_uname_info_fields,
    }
};

mp_obj_t common_hal_os_uname(void) {
    // We must populate the "release" field each time in case it was GC'd since the last call.
    const char *ver = system_get_sdk_version();
    os_uname_info_obj.items[2] = mp_obj_new_str(ver, strlen(ver), false);
    return (mp_obj_t)&os_uname_info_obj;
}

static uint32_t last_random;
bool common_hal_os_urandom(uint8_t* buffer, uint32_t length) {
    uint32_t i = 0;
    while (i < length) {
        uint32_t new_random = last_random;
        while (new_random == last_random) {
            new_random = *WDEV_HWRNG;
        }
        for (int j = 0; j < 4 && i < length; j++) {
            buffer[i] = new_random & 0xff;
            i++;
            new_random >>= 8;
        }
    }
    return true;
}
