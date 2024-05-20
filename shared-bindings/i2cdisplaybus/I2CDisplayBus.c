// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/i2cdisplaybus/I2CDisplayBus.h"

#include <stdint.h>
#include <string.h>

#include "shared/runtime/context_manager_helpers.h"
#include "py/binary.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/busio/I2C.h"
#include "shared-bindings/util.h"
#include "shared-module/displayio/__init__.h"

//| class I2CDisplayBus:
//|     """Manage updating a display over I2C in the background while Python code runs.
//|     It doesn't handle display initialization."""
//|
//|     def __init__(
//|         self,
//|         i2c_bus: busio.I2C,
//|         *,
//|         device_address: int,
//|         reset: Optional[microcontroller.Pin] = None
//|     ) -> None:
//|         """Create a I2CDisplayBus object associated with the given I2C bus and reset pin.
//|
//|         The I2C bus and pins are then in use by the display until `displayio.release_displays()` is
//|         called even after a reload. (It does this so CircuitPython can use the display after your code
//|         is done.) So, the first time you initialize a display bus in code.py you should call
//|         :py:func:`displayio.release_displays` first, otherwise it will error after the first code.py run.
//|
//|         :param busio.I2C i2c_bus: The I2C bus that make up the clock and data lines
//|         :param int device_address: The I2C address of the device
//|         :param microcontroller.Pin reset: Reset pin. When None only software reset can be used
//|         """
//|         ...
static mp_obj_t i2cdisplaybus_i2cdisplaybus_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_i2c_bus, ARG_device_address, ARG_reset };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_i2c_bus, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_device_address, MP_ARG_INT | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_reset, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_none} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mcu_pin_obj_t *reset = validate_obj_is_free_pin_or_none(args[ARG_reset].u_obj, MP_QSTR_reset);

    mp_obj_t i2c = mp_arg_validate_type(args[ARG_i2c_bus].u_obj, &busio_i2c_type, MP_QSTR_i2c_bus);
    i2cdisplaybus_i2cdisplaybus_obj_t *self = &allocate_display_bus_or_raise()->i2cdisplay_bus;
    self->base.type = &i2cdisplaybus_i2cdisplaybus_type;

    common_hal_i2cdisplaybus_i2cdisplaybus_construct(self,
        MP_OBJ_TO_PTR(i2c), args[ARG_device_address].u_int, reset);
    return self;
}

//|     def reset(self) -> None:
//|         """Performs a hardware reset via the reset pin. Raises an exception if called when no reset pin
//|         is available."""
//|         ...
static mp_obj_t i2cdisplaybus_i2cdisplaybus_obj_reset(mp_obj_t self_in) {
    i2cdisplaybus_i2cdisplaybus_obj_t *self = self_in;

    if (!common_hal_i2cdisplaybus_i2cdisplaybus_reset(self)) {
        mp_raise_RuntimeError_varg(MP_ERROR_TEXT("No %q pin"), MP_QSTR_reset);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(i2cdisplaybus_i2cdisplaybus_reset_obj, i2cdisplaybus_i2cdisplaybus_obj_reset);

//|     def send(self, command: int, data: ReadableBuffer) -> None:
//|         """Sends the given command value followed by the full set of data. Display state, such as
//|         vertical scroll, set via ``send`` may or may not be reset once the code is done."""
//|         ...
//|
static mp_obj_t i2cdisplaybus_i2cdisplaybus_obj_send(mp_obj_t self, mp_obj_t command_obj, mp_obj_t data_obj) {
    mp_int_t command_int = mp_obj_get_int(command_obj);
    mp_arg_validate_int_range(command_int, 0, 255, MP_QSTR_command);

    uint8_t command = command_int;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data_obj, &bufinfo, MP_BUFFER_READ);

    // Wait for display bus to be available.
    while (!common_hal_i2cdisplaybus_i2cdisplaybus_begin_transaction(self)) {
        RUN_BACKGROUND_TASKS;
    }
    uint8_t full_command[bufinfo.len + 1];
    full_command[0] = command;
    memcpy(full_command + 1, ((uint8_t *)bufinfo.buf), bufinfo.len);
    common_hal_i2cdisplaybus_i2cdisplaybus_send(self, DISPLAY_COMMAND, CHIP_SELECT_UNTOUCHED, full_command, bufinfo.len + 1);
    common_hal_i2cdisplaybus_i2cdisplaybus_end_transaction(self);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_3(i2cdisplaybus_i2cdisplaybus_send_obj, i2cdisplaybus_i2cdisplaybus_obj_send);

static const mp_rom_map_elem_t i2cdisplaybus_i2cdisplaybus_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&i2cdisplaybus_i2cdisplaybus_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&i2cdisplaybus_i2cdisplaybus_send_obj) },
};
static MP_DEFINE_CONST_DICT(i2cdisplaybus_i2cdisplaybus_locals_dict, i2cdisplaybus_i2cdisplaybus_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    i2cdisplaybus_i2cdisplaybus_type,
    MP_QSTR_I2CDisplayBus,
    MP_TYPE_FLAG_NONE,
    make_new, i2cdisplaybus_i2cdisplaybus_make_new,
    locals_dict, &i2cdisplaybus_i2cdisplaybus_locals_dict
    );
