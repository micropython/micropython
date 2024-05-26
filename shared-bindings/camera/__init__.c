// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright 2020 Sony Semiconductor Solutions Corporation
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "shared-bindings/camera/Camera.h"
#include "shared-bindings/util.h"

//| """Support for camera input
//|
//| The `camera` module contains classes to control the camera and take pictures."""
static const mp_rom_map_elem_t camera_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_camera) },
    { MP_ROM_QSTR(MP_QSTR_Camera), MP_ROM_PTR(&camera_type) },

    // Enum-like Classes.
    { MP_ROM_QSTR(MP_QSTR_ImageFormat), MP_ROM_PTR(&camera_imageformat_type) },
};

static MP_DEFINE_CONST_DICT(camera_module_globals, camera_module_globals_table);

const mp_obj_module_t camera_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&camera_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_camera, camera_module);
