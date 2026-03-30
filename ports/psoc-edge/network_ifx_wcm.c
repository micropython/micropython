/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019-2023 Damien P. George
 * Copyright (c) 2023 Infineon Technologies AG
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

// Placeholder implementation of the IFX WCM network module.
// Full WiFi/WCM integration to be added in a subsequent commit.

#include "py/runtime.h"
#include "py/objstr.h"
#include "extmod/modnetwork.h"
#include "network_ifx_wcm.h"

#if MICROPY_PY_NETWORK_IFX_WCM

// ---------------------------------------------------------------------------
// Stub init / deinit called from main.c
// ---------------------------------------------------------------------------

void network_init(void) {
}

void network_deinit(void) {
}

// ---------------------------------------------------------------------------
// Minimal MicroPython type — make_new + print so the module registers cleanly
// ---------------------------------------------------------------------------

static mp_obj_t network_ifx_wcm_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_raise_NotImplementedError(MP_ERROR_TEXT("WiFi not yet implemented"));
}

static void network_ifx_wcm_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_printf(print, "<IFX_WCM>");
}

static const mp_rom_map_elem_t network_ifx_wcm_locals_dict_table[] = {
};
static MP_DEFINE_CONST_DICT(network_ifx_wcm_locals_dict, network_ifx_wcm_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    mp_network_ifx_wcm_type,
    MP_QSTR_IFX_WCM,
    MP_TYPE_FLAG_NONE,
    make_new, network_ifx_wcm_make_new,
    print, network_ifx_wcm_print,
    locals_dict, &network_ifx_wcm_locals_dict
    );

#endif // MICROPY_PY_NETWORK_IFX_WCM
