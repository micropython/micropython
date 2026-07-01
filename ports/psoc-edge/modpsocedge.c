/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020-2021 Damien P. George
 * Copyright (c) 2022-2024 Infineon Technologies AG
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
#include "cy_syslib.h"
#include "modmachine.h"
#include "modpsocedge.h"

// psoc_edge.system_reset_cause(): returns the hardware reset cause captured at
// boot by machine_init().  Unlike machine.reset_cause(), this is not clobbered
// by machine_deinit() on each CTRL+D, so the CI watchdog test can read the
// correct WDT cause even after a soft-reset.
mp_obj_t psocedge_system_reset_cause(void) {
    uint32_t reason = machine_get_hw_reset_reason();

    uint32_t result;
    if (reason & CY_SYSLIB_RESET_HWWDT) {
        result = SYSTEM_RESET_WDT;
    } else if (reason & CY_SYSLIB_RESET_DPSLP_FAULT) {
        result = SYSTEM_RESET_DEEPSLEEP_FAULT;
    } else if (reason & (CY_SYSLIB_RESET_XRES | CY_SYSLIB_RESET_SOFT)) {
        result = SYSTEM_RESET_SOFT;
    } else {
        result = SYSTEM_RESET_NONE;
    }
    return MP_OBJ_NEW_SMALL_INT(result);
}
MP_DEFINE_CONST_FUN_OBJ_0(psocedge_system_reset_cause_obj, psocedge_system_reset_cause);

static const mp_rom_map_elem_t psoc_edge_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),         MP_ROM_QSTR(MP_QSTR_psoc_edge) },
    { MP_ROM_QSTR(MP_QSTR_system_reset_cause), MP_ROM_PTR(&psocedge_system_reset_cause_obj) },
    #if MICROPY_ENABLE_EXT_QSPI_FLASH
    { MP_ROM_QSTR(MP_QSTR_QSPI_Flash),       MP_ROM_PTR(&psoc_edge_qspi_flash_type) },
    #endif
};
static MP_DEFINE_CONST_DICT(psoc_edge_module_globals, psoc_edge_module_globals_table);

const mp_obj_module_t mp_module_psoc_edge = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&psoc_edge_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_psoc_edge, mp_module_psoc_edge);
