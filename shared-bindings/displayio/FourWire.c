/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018-2019 Scott Shawcroft for Adafruit Industries
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
#include "shared-bindings/displayio/Group.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"
#include "shared-module/displayio/__init__.h"
#include "supervisor/shared/translate.h"

//| class FourWire:
//|     """Manage updating a display over SPI four wire protocol in the background while Python code runs.
//|     It doesn't handle display initialization."""
//|
//|     def __init__(self, spi_bus: busio.SPI, *, command: microcontroller.Pin, chip_select: microcontroller.Pin, reset: Optional[microcontroller.Pin] = None, baudrate: int = 24000000, polarity: int = 0, phase: int = 0) -> None:
//|         """Create a FourWire object associated with the given pins.
//|
//|         The SPI bus and pins are then in use by the display until `displayio.release_displays()` is
//|         called even after a reload. (It does this so CircuitPython can use the display after your code
//|         is done.) So, the first time you initialize a display bus in code.py you should call
//|         :py:func:`displayio.release_displays` first, otherwise it will error after the first code.py run.
//|
//|         :param busio.SPI spi_bus: The SPI bus that make up the clock and data lines
//|         :param microcontroller.Pin command: Data or command pin
//|         :param microcontroller.Pin chip_select: Chip select pin
//|         :param microcontroller.Pin reset: Reset pin. When None only software reset can be used
//|         :param int baudrate: Maximum baudrate in Hz for the display on the bus
//|         :param int polarity: the base state of the clock line (0 or 1)
//|         :param int phase: the edge of the clock that data is captured. First (0)
//|             or second (1). Rising or falling depends on clock polarity."""
//|         ...
//|
STATIC mp_obj_t displayio_fourwire_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_spi_bus, ARG_command, ARG_chip_select, ARG_reset, ARG_baudrate, ARG_polarity, ARG_phase };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_spi_bus, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_command, MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_chip_select, MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_reset, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_none} },
        { MP_QSTR_baudrate, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 24000000} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_phase, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mcu_pin_obj_t *command = validate_obj_is_free_pin(args[ARG_command].u_obj);
    mcu_pin_obj_t *chip_select = validate_obj_is_free_pin(args[ARG_chip_select].u_obj);
    mcu_pin_obj_t *reset = validate_obj_is_free_pin_or_none(args[ARG_reset].u_obj);

    mp_obj_t spi = args[ARG_spi_bus].u_obj;
    displayio_fourwire_obj_t *self = &allocate_display_bus_or_raise()->fourwire_bus;
    self->base.type = &displayio_fourwire_type;

    uint8_t polarity = args[ARG_polarity].u_int;
    if (polarity != 0 && polarity != 1) {
        mp_raise_ValueError(translate("Invalid polarity"));
    }
    uint8_t phase = args[ARG_phase].u_int;
    if (phase != 0 && phase != 1) {
        mp_raise_ValueError(translate("Invalid phase"));
    }

    common_hal_displayio_fourwire_construct(self,
        MP_OBJ_TO_PTR(spi), command, chip_select, reset, args[ARG_baudrate].u_int, polarity, phase);
    return self;
}

//|     def reset(self) -> None:
//|         """Performs a hardware reset via the reset pin. Raises an exception if called when no reset pin
//|         is available."""
//|         ...
//|
STATIC mp_obj_t displayio_fourwire_obj_reset(mp_obj_t self_in) {
    displayio_fourwire_obj_t *self = self_in;

    if (!common_hal_displayio_fourwire_reset(self)) {
        mp_raise_RuntimeError(translate("no reset pin available"));
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(displayio_fourwire_reset_obj, displayio_fourwire_obj_reset);

//|     def send(self, command: int, data: FourWire, *, toggle_every_byte: bool = False) -> None:
//|         """Sends the given command value followed by the full set of data. Display state, such as
//|         vertical scroll, set via ``send`` may or may not be reset once the code is done."""
//|         ...
//|
STATIC mp_obj_t displayio_fourwire_obj_send(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_command, ARG_data, ARG_toggle_every_byte };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_command, MP_ARG_INT | MP_ARG_REQUIRED },
        { MP_QSTR_data, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_toggle_every_byte, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_int_t command_int = args[ARG_command].u_int;
    if (command_int > 255 || command_int < 0) {
        mp_raise_ValueError(translate("Command must be an int between 0 and 255"));
    }
    displayio_fourwire_obj_t *self = pos_args[0];
    uint8_t command = command_int;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_data].u_obj, &bufinfo, MP_BUFFER_READ);

    // Wait for display bus to be available.
    while (!common_hal_displayio_fourwire_begin_transaction(self)) {
        RUN_BACKGROUND_TASKS;
    }
    display_chip_select_behavior_t chip_select = CHIP_SELECT_UNTOUCHED;
    if (args[ARG_toggle_every_byte].u_bool) {
        chip_select = CHIP_SELECT_TOGGLE_EVERY_BYTE;
    }
    common_hal_displayio_fourwire_send(self, DISPLAY_COMMAND, chip_select, &command, 1);
    common_hal_displayio_fourwire_send(self, DISPLAY_DATA, chip_select, ((uint8_t *)bufinfo.buf), bufinfo.len);
    common_hal_displayio_fourwire_end_transaction(self);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(displayio_fourwire_send_obj, 3, displayio_fourwire_obj_send);

STATIC const mp_rom_map_elem_t displayio_fourwire_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&displayio_fourwire_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&displayio_fourwire_send_obj) },
};
STATIC MP_DEFINE_CONST_DICT(displayio_fourwire_locals_dict, displayio_fourwire_locals_dict_table);

const mp_obj_type_t displayio_fourwire_type = {
    { &mp_type_type },
    .name = MP_QSTR_FourWire,
    .make_new = displayio_fourwire_make_new,
    .locals_dict = (mp_obj_dict_t *)&displayio_fourwire_locals_dict,
};
