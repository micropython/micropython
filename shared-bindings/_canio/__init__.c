/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
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

//| """CAN bus access
//|
//| The `_canio` module contains low level classes to support the CAN bus
//| protocol.
//|
//| All classes change hardware state and should be deinitialized when they
//| are no longer needed if the program continues after use. To do so, either
//| call :py:meth:`!deinit` or use a context manager. See
//| :ref:`lifetime-and-contextmanagers` for more info.
//|
//| For example::
//|
//|   import _canio
//|   from board import *
//|
//|   can = _canio.BUS(board.CANRX, board.CANTX)
//|   can.write(408, b"adafruit")
//|   can.deinit()
//|
//| This example will write the data 'adafruit' onto the CAN bus to any
//| device listening for message id 408."""
//|

#include "py/obj.h"

#include "shared-bindings/_canio/__init__.h"
#include "shared-bindings/_canio/CAN.h"
#include "shared-bindings/_canio/Match.h"
#include "shared-bindings/_canio/Message.h"

STATIC const mp_rom_map_elem_t canio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__canio) },
    { MP_ROM_QSTR(MP_QSTR_CAN), MP_ROM_PTR(&canio_can_type) },
    { MP_ROM_QSTR(MP_QSTR_Listener), MP_ROM_PTR(&canio_listener_type) },
    { MP_ROM_QSTR(MP_QSTR_Match), MP_ROM_PTR(&canio_match_type) },
    { MP_ROM_QSTR(MP_QSTR_Message), MP_ROM_PTR(&canio_message_type) },
};


STATIC MP_DEFINE_CONST_DICT(canio_module_globals, canio_module_globals_table);

const mp_obj_module_t canio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&canio_module_globals,
};
