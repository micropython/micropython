/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Scott Shawcroft for Adafruit Industries
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

#include <stdint.h>

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/usb_midi/__init__.h"
#include "shared-bindings/usb_midi/PortIn.h"
#include "shared-bindings/usb_midi/PortOut.h"

#include "py/runtime.h"

//| """MIDI over USB
//|
//| The `usb_midi` module contains classes to transmit and receive MIDI messages over USB."""
//|
//| ports: Tuple[Union[PortIn, PortOut], ...]
//| """Tuple of all MIDI ports. Each item is ether `PortIn` or `PortOut`."""
//|

//| def configure_usb(enabled: True) -> None:
//|     """Configure the USB MIDI device.
//|     Can be called in ``boot.py``, before USB is connected.
//|
//|     :param enabled bool: Enable or disable the USB MIDI Device.
//|       True to enable; False to disable. Enabled by default."""
//|     ...
//|
STATIC mp_obj_t usb_midi_configure_usb(mp_obj_t enabled) {
    if (!common_hal_usb_midi_configure_usb(mp_obj_is_true(enabled))) {
        mp_raise_RuntimeError(translate("Cannot change USB devices now"));
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_midi_configure_usb_obj, usb_midi_configure_usb);

mp_map_elem_t usb_midi_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__),      MP_ROM_QSTR(MP_QSTR_usb_midi) },
    { MP_ROM_QSTR(MP_QSTR_configure_usb), MP_OBJ_FROM_PTR(&usb_midi_configure_usb_obj) },
    { MP_ROM_QSTR(MP_QSTR_ports),         mp_const_empty_tuple },
    { MP_ROM_QSTR(MP_QSTR_PortIn),        MP_OBJ_FROM_PTR(&usb_midi_portin_type) },
    { MP_ROM_QSTR(MP_QSTR_PortOut),       MP_OBJ_FROM_PTR(&usb_midi_portout_type) },
};

// This isn't const so we can set ports dynamically.
MP_DEFINE_MUTABLE_DICT(usb_midi_module_globals, usb_midi_module_globals_table);

const mp_obj_module_t usb_midi_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&usb_midi_module_globals,
};
