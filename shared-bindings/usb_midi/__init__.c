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

//| def disable() -> None:
//|     """Disable presenting a USB MIDI device to the host.
//|     The device is normally enabled by default, but on some boards with limited endpoints
//|     including ESP32-S2 and certain STM boards, it is disabled by default.
//|     Can be called in ``boot.py``, before USB is connected."""
//|     ...
//|
STATIC mp_obj_t usb_midi_disable(void) {
    if (!common_hal_usb_midi_disable()) {
        mp_raise_RuntimeError(translate("Cannot change USB devices now"));
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(usb_midi_disable_obj, usb_midi_disable);

//| def enable() -> None:
//|     """Enable presenting a USB MIDI device to the host.
//|     The device is enabled by default, so you do not normally need to call this function.
//|     Can be called in ``boot.py``, before USB is connected.
//|
//|     If you enable too many devices at once, you will run out of USB endpoints.
//|     The number of available endpoints varies by microcontroller.
//|     CircuitPython will go into safe mode after running boot.py to inform you if
//|     not enough endpoints are available.
//|     """
//|     ...
//|
STATIC mp_obj_t usb_midi_enable(void) {
    if (!common_hal_usb_midi_enable()) {
        mp_raise_RuntimeError(translate("Cannot change USB devices now"));
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(usb_midi_enable_obj, usb_midi_enable);

mp_map_elem_t usb_midi_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_usb_midi) },
    { MP_ROM_QSTR(MP_QSTR_disable),  MP_OBJ_FROM_PTR(&usb_midi_disable_obj) },
    { MP_ROM_QSTR(MP_QSTR_enable),   MP_OBJ_FROM_PTR(&usb_midi_enable_obj) },
    { MP_ROM_QSTR(MP_QSTR_ports),    mp_const_empty_tuple },
    { MP_ROM_QSTR(MP_QSTR_PortIn),   MP_OBJ_FROM_PTR(&usb_midi_portin_type) },
    { MP_ROM_QSTR(MP_QSTR_PortOut),  MP_OBJ_FROM_PTR(&usb_midi_portout_type) },
};

// This isn't const so we can set ports dynamically.
MP_DEFINE_MUTABLE_DICT(usb_midi_module_globals, usb_midi_module_globals_table);

const mp_obj_module_t usb_midi_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&usb_midi_module_globals,
};
