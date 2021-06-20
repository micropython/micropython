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

#include "shared-bindings/displayio/I2CDisplay.h"

#include <stdint.h>
#include <string.h>

#include "lib/utils/context_manager_helpers.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"
#include "shared-module/displayio/__init__.h"
#include "supervisor/shared/translate.h"

//| class I2CDisplay:
//|     """Manage updating a display over I2C in the background while Python code runs.
//|     It doesn't handle display initialization."""
//|
//|     def __init__(self, i2c_bus: busio.I2C, *, device_address: int, reset: Optional[microcontroller.Pin] = None) -> None:
//|         """Create a I2CDisplay object associated with the given I2C bus and reset pin.
//|
//|         The I2C bus and pins are then in use by the display until `displayio.release_displays()` is
//|         called even after a reload. (It does this so CircuitPython can use the display after your code
//|         is done.) So, the first time you initialize a display bus in code.py you should call
//|         :py:func:`displayio.release_displays` first, otherwise it will error after the first code.py run.
//|
//|         :param busio.I2C i2c_bus: The I2C bus that make up the clock and data lines
//|         :param int device_address: The I2C address of the device
//|         :param microcontroller.Pin reset: Reset pin. When None only software reset can be used"""
//|         ...
//|
STATIC mp_obj_t displayio_i2cdisplay_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_i2c_bus, ARG_device_address, ARG_reset };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_i2c_bus, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_device_address, MP_ARG_INT | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_reset, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_none} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mcu_pin_obj_t *reset = validate_obj_is_free_pin_or_none(args[ARG_reset].u_obj);

    mp_obj_t i2c = args[ARG_i2c_bus].u_obj;
    displayio_i2cdisplay_obj_t *self = &allocate_display_bus_or_raise()->i2cdisplay_bus;
    self->base.type = &displayio_i2cdisplay_type;

    common_hal_displayio_i2cdisplay_construct(self,
        MP_OBJ_TO_PTR(i2c), args[ARG_device_address].u_int, reset);
    return self;
}

//|     def reset(self) -> None:
//|         """Performs a hardware reset via the reset pin. Raises an exception if called when no reset pin
//|         is available."""
//|         ...
//|
STATIC mp_obj_t displayio_i2cdisplay_obj_reset(mp_obj_t self_in) {
    displayio_i2cdisplay_obj_t *self = self_in;

    if (!common_hal_displayio_i2cdisplay_reset(self)) {
        mp_raise_RuntimeError(translate("no reset pin available"));
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_i2cdisplay_reset_obj, displayio_i2cdisplay_obj_reset);

//|     def send(self, command: int, data: ReadableBuffer) -> None:
//|         """Sends the given command value followed by the full set of data. Display state, such as
//|         vertical scroll, set via ``send`` may or may not be reset once the code is done."""
//|         ...
//|
STATIC mp_obj_t displayio_i2cdisplay_obj_send(mp_obj_t self, mp_obj_t command_obj, mp_obj_t data_obj) {
    mp_int_t command_int = MP_OBJ_SMALL_INT_VALUE(command_obj);
    if (!mp_obj_is_small_int(command_obj) || command_int > 255 || command_int < 0) {
        mp_raise_ValueError(translate("Command must be an int between 0 and 255"));
    }
    uint8_t command = command_int;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data_obj, &bufinfo, MP_BUFFER_READ);

    // Wait for display bus to be available.
    while (!common_hal_displayio_i2cdisplay_begin_transaction(self)) {
        RUN_BACKGROUND_TASKS;
    }
    uint8_t full_command[bufinfo.len + 1];
    full_command[0] = command;
    memcpy(full_command + 1, ((uint8_t *)bufinfo.buf), bufinfo.len);
    common_hal_displayio_i2cdisplay_send(self, DISPLAY_COMMAND, CHIP_SELECT_UNTOUCHED, full_command, bufinfo.len + 1);
    common_hal_displayio_i2cdisplay_end_transaction(self);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_3(displayio_i2cdisplay_send_obj, displayio_i2cdisplay_obj_send);

STATIC const mp_rom_map_elem_t displayio_i2cdisplay_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&displayio_i2cdisplay_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&displayio_i2cdisplay_send_obj) },
};
STATIC MP_DEFINE_CONST_DICT(displayio_i2cdisplay_locals_dict, displayio_i2cdisplay_locals_dict_table);

const mp_obj_type_t displayio_i2cdisplay_type = {
    { &mp_type_type },
    .name = MP_QSTR_I2CDisplay,
    .make_new = displayio_i2cdisplay_make_new,
    .locals_dict = (mp_obj_dict_t *)&displayio_i2cdisplay_locals_dict,
};
