/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
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
#include <string.h>

#include "py/objstr.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"

#include "quectel_version.h"

static mp_obj_t helios_platform(void) {	
    char platform[64] = {0};
    snprintf(platform, sizeof(platform), "%s%d.%d.%d", "heliossdk-v", VERSION_MAJOR, VERSION_MINOR, VERSION_MICRO);
    return mp_obj_new_str(platform, strlen(platform));
}
static MP_DEFINE_CONST_FUN_OBJ_0(helios_platform_obj, helios_platform);

static const mp_rom_map_elem_t helios_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_helios) },

    { MP_ROM_QSTR(MP_QSTR_platform), MP_ROM_PTR(&helios_platform_obj) },
};

static MP_DEFINE_CONST_DICT(helios_module_globals, helios_module_globals_table);

const mp_obj_module_t helios_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&helios_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_helios, helios_module);