// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2021 Jeff Epler
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/qrio/__init__.h"
#include "shared-bindings/qrio/QRDecoder.h"
#include "shared-bindings/qrio/QRInfo.h"
#include "shared-bindings/qrio/PixelPolicy.h"
#include "py/obj.h"
#include "py/enum.h"

//| """Low-level QR code decoding
//|
//| Provides the `QRDecoder` object used for decoding QR codes.  For more
//| information about working with QR codes, see
//| `this Learn guide <https://learn.adafruit.com/scan-qr-codes-with-circuitpython>`_.
//|
//| .. note:: This module only handles decoding QR codes.  If you are looking
//|     to generate a QR code, use the
//|     `adafruit_miniqr library <https://github.com/adafruit/Adafruit_CircuitPython_miniQR>`_"""

static const mp_rom_map_elem_t qrio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_qrio) },
    { MP_ROM_QSTR(MP_QSTR_QRInfo), MP_ROM_PTR(&qrio_qrinfo_type_obj) },
    { MP_ROM_QSTR(MP_QSTR_QRDecoder), MP_ROM_PTR(&qrio_qrdecoder_type_obj) },
    { MP_ROM_QSTR(MP_QSTR_PixelPolicy), MP_ROM_PTR(&qrio_pixel_policy_type) },
};

static MP_DEFINE_CONST_DICT(qrio_module_globals, qrio_module_globals_table);

const mp_obj_module_t qrio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&qrio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_qrio, qrio_module);
