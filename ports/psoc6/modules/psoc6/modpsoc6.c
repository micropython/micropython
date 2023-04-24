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

// micropython includes
#include "py/runtime.h"


// port-specific includes
#include "modpsoc6.h"


STATIC const mp_rom_map_elem_t psoc6_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),            MP_ROM_QSTR(MP_QSTR_psoc6) },
    #if MICROPY_ENABLE_EXT_QSPI_FLASH
    { MP_ROM_QSTR(MP_QSTR_QSPI_Flash),          MP_ROM_PTR(&psoc6_qspi_flash_type) },
    #else
    { MP_ROM_QSTR(MP_QSTR_Flash),               MP_ROM_PTR(&psoc6_flash_type) },
    #endif
};
STATIC MP_DEFINE_CONST_DICT(psoc6_module_globals, psoc6_module_globals_table);


const mp_obj_module_t mp_module_psoc6 = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&psoc6_module_globals,
};


MP_REGISTER_MODULE(MP_QSTR_psoc6, mp_module_psoc6);
