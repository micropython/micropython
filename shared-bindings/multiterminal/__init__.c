/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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
#include "shared-bindings/multiterminal/__init__.h"

#include "py/obj.h"
#include "py/mphal.h"
#include "py/runtime.h"
#include "supervisor/shared/translate.h"

//| """Manage additional terminal sources
//|
//| The `multiterminal` module allows you to configure an additional serial
//| terminal source. Incoming characters are accepted from both the internal
//| serial connection and the optional secondary connection."""
//|

//| def get_secondary_terminal() -> Optional[typing.BinaryIO]:
//|     """Returns the current secondary terminal."""
//|     ...
//|
STATIC mp_obj_t multiterminal_obj_get_secondary_terminal() {
    return common_hal_multiterminal_get_secondary_terminal();
}
MP_DEFINE_CONST_FUN_OBJ_0(multiterminal_get_secondary_terminal_obj, multiterminal_obj_get_secondary_terminal);

//| def set_secondary_terminal(stream: typing.BinaryIO) -> None:
//|     """Read additional input from the given stream and write out back to it.
//|     This doesn't replace the core stream (usually UART or native USB) but is
//|     mixed in instead.
//|
//|     :param stream stream: secondary stream"""
//|     ...
//|
STATIC mp_obj_t multiterminal_obj_set_secondary_terminal(mp_obj_t secondary_terminal) {
    mp_obj_t write_m[3];
    mp_load_method_maybe(secondary_terminal, MP_QSTR_write, write_m);
    mp_obj_t readinto_m[3];
    mp_load_method_maybe(secondary_terminal, MP_QSTR_readinto, readinto_m);
    if (write_m[0] == MP_OBJ_NULL || readinto_m[0] == MP_OBJ_NULL) {
        mp_raise_ValueError(translate("Stream missing readinto() or write() method."));
        return mp_const_none;
    }
    common_hal_multiterminal_set_secondary_terminal(secondary_terminal);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(multiterminal_set_secondary_terminal_obj, multiterminal_obj_set_secondary_terminal);

//| def clear_secondary_terminal() -> None:
//|     """Clears the secondary terminal."""
//|     ...
//|
STATIC mp_obj_t multiterminal_obj_clear_secondary_terminal() {
    common_hal_multiterminal_clear_secondary_terminal();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(multiterminal_clear_secondary_terminal_obj, multiterminal_obj_clear_secondary_terminal);

//| def schedule_secondary_terminal_read(socket: socket.socket) -> None:
//|     """In cases where the underlying OS is doing task scheduling, this notifies
//|     the OS when more data is available on the socket to read. This is useful
//|     as a callback for lwip sockets."""
//|     ...
//|
// TODO(tannewt): This is a funny API. Replace it with a direct call into the OS
// by the lwip object.
STATIC mp_obj_t multiterminal_obj_schedule_secondary_terminal_read(mp_obj_t socket) {
    common_hal_multiterminal_schedule_secondary_terminal_read(socket);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(multiterminal_schedule_secondary_terminal_read_obj, multiterminal_obj_schedule_secondary_terminal_read);

// TODO(tannewt): Expose the internal serial connection as `primary_terminal`

STATIC const mp_rom_map_elem_t multiterminal_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_multiterminal) },
    { MP_ROM_QSTR(MP_QSTR_get_secondary_terminal), MP_ROM_PTR(&multiterminal_get_secondary_terminal_obj) },
    { MP_ROM_QSTR(MP_QSTR_set_secondary_terminal), MP_ROM_PTR(&multiterminal_set_secondary_terminal_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear_secondary_terminal), MP_ROM_PTR(&multiterminal_clear_secondary_terminal_obj) },
    { MP_ROM_QSTR(MP_QSTR_schedule_secondary_terminal_read), MP_ROM_PTR(&multiterminal_schedule_secondary_terminal_read_obj) },
};

STATIC MP_DEFINE_CONST_DICT(multiterminal_module_globals, multiterminal_module_globals_table);

const mp_obj_module_t multiterminal_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&multiterminal_module_globals,
};
