// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2025 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "py/enum.h"
#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/mipicsi/__init__.h"
#include "shared-bindings/mipicsi/Camera.h"

//| """Support for MIPI CSI-2 camera interfaces
//|
//| Captures image frames from camera sensors attached to a MIPI CSI-2
//| receiver, such as the one in the ESP32-P4. The camera sensor itself is
//| configured separately (typically over I2C); this module only receives
//| the image data the sensor transmits.
//|
//| By default, captured frames are in the same format the sensor sends,
//| described by `PixelFormat`. For example, an RGB565 sensor gives RGB565
//| frames. RAW formats contain Bayer data, with the color order given by
//| `BayerOrder`.
//|
//| `mipicsi.Camera` may be able to convert the format as it receives a
//| frame. To do this, pass the desired ``output_format`` when creating the
//| Camera. An exception will be raised if it cannot be done in hardware
//| without another buffer.
//| """

static const mp_rom_map_elem_t mipicsi_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_mipicsi) },
    { MP_ROM_QSTR(MP_QSTR_PixelFormat), MP_ROM_PTR(&mipicsi_pixel_format_type) },
    { MP_ROM_QSTR(MP_QSTR_BayerOrder), MP_ROM_PTR(&mipicsi_bayer_order_type) },
    { MP_ROM_QSTR(MP_QSTR_Camera), MP_ROM_PTR(&mipicsi_camera_type) },
};

static MP_DEFINE_CONST_DICT(mipicsi_module_globals, mipicsi_module_globals_table);

const mp_obj_module_t mipicsi_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mipicsi_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_mipicsi, mipicsi_module);
