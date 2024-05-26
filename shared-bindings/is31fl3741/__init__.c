// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Mark Komus
//
// SPDX-License-Identifier: MIT

#include <stdint.h>
#include <stdbool.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/busio/I2C.h"
#include "shared-bindings/is31fl3741/IS31FL3741.h"
#include "shared-bindings/is31fl3741/FrameBuffer.h"

static const mp_rom_map_elem_t is31fl3741_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_is31fl3741) },
    { MP_ROM_QSTR(MP_QSTR_IS31FL3741), MP_ROM_PTR(&is31fl3741_IS31FL3741_type) },
    { MP_ROM_QSTR(MP_QSTR_IS31FL3741_FrameBuffer), MP_ROM_PTR(&is31fl3741_framebuffer_type) },
};

static MP_DEFINE_CONST_DICT(is31fl3741_module_globals, is31fl3741_module_globals_table);

const mp_obj_module_t is31fl3741_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&is31fl3741_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_is31fl3741, is31fl3741_module);
