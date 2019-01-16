/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
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

#include "shared-bindings/displayio/ParallelBus.h"

#include <stdint.h>

#include "lib/utils/context_manager_helpers.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"
#include "supervisor/shared/translate.h"

//| .. currentmodule:: displayio
//|
//| :class:`ParallelBus` -- Manage updating a display over SPI four wire protocol
//| ==========================================================================
//|
//| Manage updating a display over SPI four wire protocol in the background while Python code runs.
//| It doesn't handle display initialization.
//|
//| .. warning:: This will be changed before 4.0.0. Consider it very experimental.
//|
//| .. class:: ParallelBus(*, data0, command, chip_select, reset, write, bus_width=8)
//|
//|   Create a ParallelBus object associated with the given pins.
//|
STATIC mp_obj_t displayio_parallelbus_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_raise_NotImplementedError(translate("displayio is a work in progress"));
    return mp_const_none;
}


//|   .. method:: send(command, data)
//|
//|
STATIC mp_obj_t displayio_parallelbus_obj_send(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_raise_NotImplementedError(translate("displayio is a work in progress"));

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(displayio_parallelbus_send_obj, 1, displayio_parallelbus_obj_send);

STATIC const mp_rom_map_elem_t displayio_parallelbus_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&displayio_parallelbus_send_obj) },
};
STATIC MP_DEFINE_CONST_DICT(displayio_parallelbus_locals_dict, displayio_parallelbus_locals_dict_table);

const mp_obj_type_t displayio_parallelbus_type = {
    { &mp_type_type },
    .name = MP_QSTR_ParallelBus,
    .make_new = displayio_parallelbus_make_new,
    .locals_dict = (mp_obj_dict_t*)&displayio_parallelbus_locals_dict,
};
