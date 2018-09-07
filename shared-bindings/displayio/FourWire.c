/*
 * This file is part of the Micro Python project, http://micropython.org/
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

#include "shared-bindings/displayio/FourWire.h"

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
//| :class:`FourWire` -- Manage updating a display over SPI four wire protocol
//| ==========================================================================
//|
//| Manage updating a display over SPI four wire protocol in the background while Python code runs.
//| It doesn't handle display initialization.
//|
//| .. warning:: This will be changed before 4.0.0. Consider it very experimental.
//|
//| .. class:: FourWire(*, clock, data, command, chip_select, width, height, colstart, rowstart,
//|                     color_depth, set_column_command, set_row_command, write_ram_command)
//|
//|   Create a FourWire object associated with the given pins.
//|
STATIC mp_obj_t displayio_fourwire_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *pos_args) {
    mp_raise_NotImplementedError(translate("displayio is a work in progress"));
    return mp_const_none;
}


//|   .. method:: send(command, data)
//|
//|
STATIC mp_obj_t displayio_fourwire_obj_send(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_raise_NotImplementedError(translate("displayio is a work in progress"));

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(displayio_fourwire_send_obj, 1, displayio_fourwire_obj_send);

//|   .. method:: show(group)
//|
//|     Switches do displaying the given group of elements.
//|
STATIC mp_obj_t displayio_fourwire_obj_show(mp_obj_t self_in, mp_obj_t group_in) {
    displayio_fourwire_obj_t *self = MP_OBJ_TO_PTR(self_in);
    displayio_group_t* group = MP_OBJ_TO_PTR(group_in);
    common_hal_displayio_fourwire_show(self, group);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(displayio_fourwire_show_obj, displayio_fourwire_obj_show);

//|   .. method:: refresh_soon()
//|
//|     Queues up a display refresh that happens in the background.
//|
STATIC mp_obj_t displayio_fourwire_obj_refresh_soon(mp_obj_t self_in) {
    displayio_fourwire_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_displayio_fourwire_refresh_soon(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_fourwire_refresh_soon_obj, displayio_fourwire_obj_refresh_soon);

//|   .. method:: wait_for_frame()
//|
//|     Waits until the next frame has been transmitted to the display unless the wait count is
//|     behind the rendered frames. In that case, this will return immediately with the wait count.
//|
STATIC mp_obj_t displayio_fourwire_obj_wait_for_frame(mp_obj_t self_in) {
    displayio_fourwire_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_displayio_fourwire_wait_for_frame(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_fourwire_wait_for_frame_obj, displayio_fourwire_obj_wait_for_frame);


STATIC const mp_rom_map_elem_t displayio_fourwire_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&displayio_fourwire_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_show), MP_ROM_PTR(&displayio_fourwire_show_obj) },
    { MP_ROM_QSTR(MP_QSTR_refresh_soon), MP_ROM_PTR(&displayio_fourwire_refresh_soon_obj) },
    { MP_ROM_QSTR(MP_QSTR_wait_for_frame), MP_ROM_PTR(&displayio_fourwire_wait_for_frame_obj) },
};
STATIC MP_DEFINE_CONST_DICT(displayio_fourwire_locals_dict, displayio_fourwire_locals_dict_table);

const mp_obj_type_t displayio_fourwire_type = {
    { &mp_type_type },
    .name = MP_QSTR_FourWire,
    .make_new = displayio_fourwire_make_new,
    .locals_dict = (mp_obj_dict_t*)&displayio_fourwire_locals_dict,
};
