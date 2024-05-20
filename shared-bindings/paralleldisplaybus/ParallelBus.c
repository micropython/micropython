// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2019 Scott Shawcroft for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/paralleldisplaybus/ParallelBus.h"

#include <stdint.h>

#include "shared/runtime/context_manager_helpers.h"
#include "py/binary.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"
#include "shared-module/displayio/__init__.h"

//| class ParallelBus:
//|     """Manage updating a display over 8-bit parallel bus in the background while Python code runs. This
//|     protocol may be referred to as 8080-I Series Parallel Interface in datasheets. It doesn't handle
//|     display initialization."""
//|
//|     def __init__(
//|         self,
//|         *,
//|         command: microcontroller.Pin,
//|         chip_select: microcontroller.Pin,
//|         write: microcontroller.Pin,
//|         data0: Optional[microcontroller.Pin] = None,
//|         data_pins: Optional[Sequence[microcontroller.Pin]] = None,
//|         read: Optional[microcontroller.Pin],
//|         reset: Optional[microcontroller.Pin] = None,
//|         frequency: int = 30_000_000,
//|     ) -> None:
//|         """Create a ParallelBus object associated with the given pins. The bus is inferred from data0
//|         by implying the next 7 additional pins on a given GPIO port.
//|
//|         The parallel bus and pins are then in use by the display until `displayio.release_displays()`
//|         is called even after a reload. (It does this so CircuitPython can use the display after your
//|         code is done.) So, the first time you initialize a display bus in code.py you should call
//|         :py:func:`displayio.release_displays` first, otherwise it will error after the first code.py run.
//|
//|         :param microcontroller.Pin data_pins: A list of data pins.  Specify exactly one of ``data_pins`` or ``data0``.
//|         :param microcontroller.Pin data0: The first data pin. The rest are implied
//|         :param microcontroller.Pin command: Data or command pin
//|         :param microcontroller.Pin chip_select: Chip select pin
//|         :param microcontroller.Pin write: Write pin
//|         :param microcontroller.Pin read: Read pin, optional
//|         :param microcontroller.Pin reset: Reset pin, optional
//|         :param int frequency: The communication frequency in Hz for the display on the bus"""
//|         ...
static mp_obj_t paralleldisplaybus_parallelbus_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_data0, ARG_data_pins, ARG_command, ARG_chip_select, ARG_write, ARG_read, ARG_reset, ARG_frequency };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_data0, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_none } },
        { MP_QSTR_data_pins, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_none } },
        { MP_QSTR_command, MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_chip_select, MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_write, MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_read, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_none } },
        { MP_QSTR_reset, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_none } },
        { MP_QSTR_frequency, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 30000000 } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mcu_pin_obj_t *command = validate_obj_is_free_pin(args[ARG_command].u_obj, MP_QSTR_command);
    const mcu_pin_obj_t *chip_select = validate_obj_is_free_pin(args[ARG_chip_select].u_obj, MP_QSTR_chip_select);
    const mcu_pin_obj_t *write = validate_obj_is_free_pin(args[ARG_write].u_obj, MP_QSTR_write);
    const mcu_pin_obj_t *read = validate_obj_is_free_pin_or_none(args[ARG_read].u_obj, MP_QSTR_read);
    const mcu_pin_obj_t *reset = validate_obj_is_free_pin_or_none(args[ARG_reset].u_obj, MP_QSTR_reset);

    paralleldisplaybus_parallelbus_obj_t *self = &allocate_display_bus_or_raise()->parallel_bus;
    self->base.type = &paralleldisplaybus_parallelbus_type;

    bool specified_data0 = args[ARG_data0].u_obj != mp_const_none;
    bool specified_data_pins = args[ARG_data_pins].u_obj != mp_const_none;

    if (specified_data0 == specified_data_pins) {
        mp_raise_ValueError(MP_ERROR_TEXT("Specify exactly one of data0 or data_pins"));
    }

    if (specified_data0) {
        const mcu_pin_obj_t *data0 = validate_obj_is_free_pin(args[ARG_data0].u_obj, MP_QSTR_data0);
        common_hal_paralleldisplaybus_parallelbus_construct(self, data0, command, chip_select, write, read, reset, args[ARG_frequency].u_int);
    } else {
        uint8_t num_pins;
        const mcu_pin_obj_t *data_pins[16];
        validate_list_is_free_pins(MP_QSTR_data_pins, data_pins, (mp_int_t)MP_ARRAY_SIZE(data_pins), args[ARG_data_pins].u_obj, &num_pins);
        common_hal_paralleldisplaybus_parallelbus_construct_nonsequential(self, num_pins, data_pins, command, chip_select, write, read, reset, args[ARG_frequency].u_int);
    }
    return self;
}

//|     def reset(self) -> None:
//|         """Performs a hardware reset via the reset pin. Raises an exception if called when no reset pin
//|         is available."""
//|         ...

static mp_obj_t paralleldisplaybus_parallelbus_obj_reset(mp_obj_t self_in) {
    paralleldisplaybus_parallelbus_obj_t *self = self_in;

    if (!common_hal_paralleldisplaybus_parallelbus_reset(self)) {
        mp_raise_RuntimeError_varg(MP_ERROR_TEXT("No %q pin"), MP_QSTR_reset);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(paralleldisplaybus_parallelbus_reset_obj, paralleldisplaybus_parallelbus_obj_reset);

//|     def send(self, command: int, data: ReadableBuffer) -> None:
//|         """Sends the given command value followed by the full set of data. Display state, such as
//|         vertical scroll, set via ``send`` may or may not be reset once the code is done."""
//|         ...
//|
static mp_obj_t paralleldisplaybus_parallelbus_obj_send(mp_obj_t self, mp_obj_t command_obj, mp_obj_t data_obj) {
    mp_int_t command_int = mp_arg_validate_int_range(mp_obj_get_int(command_obj), 0, 255, MP_QSTR_command);

    uint8_t command = command_int;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data_obj, &bufinfo, MP_BUFFER_READ);

    // Wait for display bus to be available.
    while (!common_hal_paralleldisplaybus_parallelbus_begin_transaction(self)) {
        RUN_BACKGROUND_TASKS;
    }
    common_hal_paralleldisplaybus_parallelbus_send(self, DISPLAY_COMMAND, CHIP_SELECT_UNTOUCHED, &command, 1);
    common_hal_paralleldisplaybus_parallelbus_send(self, DISPLAY_DATA, CHIP_SELECT_UNTOUCHED, ((uint8_t *)bufinfo.buf), bufinfo.len);
    common_hal_paralleldisplaybus_parallelbus_end_transaction(self);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_3(paralleldisplaybus_parallelbus_send_obj, paralleldisplaybus_parallelbus_obj_send);

static const mp_rom_map_elem_t paralleldisplaybus_parallelbus_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&paralleldisplaybus_parallelbus_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&paralleldisplaybus_parallelbus_send_obj) },
};
static MP_DEFINE_CONST_DICT(paralleldisplaybus_parallelbus_locals_dict, paralleldisplaybus_parallelbus_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    paralleldisplaybus_parallelbus_type,
    MP_QSTR_ParallelBus,
    MP_TYPE_FLAG_NONE,
    make_new, paralleldisplaybus_parallelbus_make_new,
    locals_dict, &paralleldisplaybus_parallelbus_locals_dict
    );
