/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

#include "py/obj.h"
#include "py/objint.h"
#include "extmod/modmachine.h"
#include "rfcore.h"
#include "portmodules.h"
#include "subghz.h"

#if MICROPY_PY_STM

#include "genhdr/modstm_mpz.h"

static const mp_rom_map_elem_t stm_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_stm) },

    { MP_ROM_QSTR(MP_QSTR_mem8), MP_ROM_PTR(&machine_mem8_obj) },
    { MP_ROM_QSTR(MP_QSTR_mem16), MP_ROM_PTR(&machine_mem16_obj) },
    { MP_ROM_QSTR(MP_QSTR_mem32), MP_ROM_PTR(&machine_mem32_obj) },

    #if MICROPY_PY_STM_CONST
    #include "genhdr/modstm_const.h"
    #endif

    #if defined(STM32WB)
    { MP_ROM_QSTR(MP_QSTR_rfcore_status), MP_ROM_PTR(&rfcore_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_rfcore_fw_version), MP_ROM_PTR(&rfcore_fw_version_obj) },
    { MP_ROM_QSTR(MP_QSTR_rfcore_sys_hci), MP_ROM_PTR(&rfcore_sys_hci_obj) },
    #endif

    #if defined(STM32WL)
    { MP_ROM_QSTR(MP_QSTR_subghz_cs), MP_ROM_PTR(&subghz_cs_obj) },
    { MP_ROM_QSTR(MP_QSTR_subghz_irq), MP_ROM_PTR(&subghz_irq_obj) },
    { MP_ROM_QSTR(MP_QSTR_subghz_is_busy), MP_ROM_PTR(&subghz_is_busy_obj) },
    #endif
};

static MP_DEFINE_CONST_DICT(stm_module_globals, stm_module_globals_table);

const mp_obj_module_t stm_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&stm_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_stm, stm_module);

#endif // MICROPY_PY_STM
