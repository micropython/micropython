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

//| .. currentmodule:: displayio
//|
//| :class:`FourWire` -- Manage updating a display over SPI four wire protocol
//| ==========================================================================
//|
//| Manage updating a display over SPI four wire protocol in the background while Python code runs.
//| It doesn't handle display initialization.
//|
//| .. class:: FourWire(spi_bus, *, command, chip_select, reset=None, baudrate=24000000)
//|
//|   Create a FourWire object associated with the given pins.
//|
//|   The SPI bus and pins are then in use by the display until `displayio.release_displays()` is
//|   called even after a reload. (It does this so CircuitPython can use the display after your code
//|   is done.) So, the first time you initialize a display bus in code.py you should call
//|   :py:func`displayio.release_displays` first, otherwise it will error after the first code.py run.
//|
//|   :param busio.SPI spi_bus: The SPI bus that make up the clock and data lines
//|   :param microcontroller.Pin command: Data or command pin
//|   :param microcontroller.Pin chip_select: Chip select pin
//|   :param microcontroller.Pin reset: Reset pin. When None only software reset can be used
//|   :param int baudrate: Maximum baudrate in Hz for the display on the bus
//|
STATIC mp_obj_t displayio_fourwire_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_spi_bus, ARG_command, ARG_chip_select, ARG_reset, ARG_baudrate };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_spi_bus, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_command, MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_chip_select, MP_ARG_OBJ | MP_ARG_KW_ONLY | MP_ARG_REQUIRED },
        { MP_QSTR_reset, MP_ARG_OBJ | MP_ARG_KW_ONLY, {.u_obj = mp_const_none} },
        { MP_QSTR_baudrate, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 24000000} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t command = args[ARG_command].u_obj;
    mp_obj_t chip_select = args[ARG_chip_select].u_obj;
    assert_pin_free(command);
    assert_pin_free(chip_select);
    mp_obj_t reset = args[ARG_reset].u_obj;
    if (reset != mp_const_none) {
        assert_pin_free(reset);
    } else {
        reset = NULL;
    }

    displayio_fourwire_obj_t* self = NULL;
    mp_obj_t spi = args[ARG_spi_bus].u_obj;
    for (uint8_t i = 0; i < CIRCUITPY_DISPLAY_LIMIT; i++) {
        if (displays[i].fourwire_bus.base.type == NULL ||
            displays[i].fourwire_bus.base.type == &mp_type_NoneType) {
            self = &displays[i].fourwire_bus;
            self->base.type = &displayio_fourwire_type;
            break;
        }
    }
    if (self == NULL) {
        mp_raise_RuntimeError(translate("Too many display busses"));
    }

    common_hal_displayio_fourwire_construct(self,
        MP_OBJ_TO_PTR(spi), command, chip_select, reset, args[ARG_baudrate].u_int);
    return self;
}

//|   .. method:: send(command, data)
//|
//|     Sends the given command value followed by the full set of data. Display state, such as
//|     vertical scroll, set via ``send`` may or may not be reset once the code is done.
//|
STATIC mp_obj_t displayio_fourwire_obj_send(mp_obj_t self, mp_obj_t command_obj, mp_obj_t data_obj) {
    mp_int_t command_int = MP_OBJ_SMALL_INT_VALUE(command_obj);
    if (!MP_OBJ_IS_SMALL_INT(command_obj) || command_int > 255 || command_int < 0) {
        mp_raise_ValueError(translate("Command must be an int between 0 and 255"));
    }
    uint8_t command = command_int;
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data_obj, &bufinfo, MP_BUFFER_READ);

    // Wait for display bus to be available.
    while (!common_hal_displayio_fourwire_begin_transaction(self)) {
#ifdef MICROPY_VM_HOOK_LOOP
        MICROPY_VM_HOOK_LOOP ;
#endif
    }
    common_hal_displayio_fourwire_send(self, true, &command, 1);
    common_hal_displayio_fourwire_send(self, false, ((uint8_t*) bufinfo.buf), bufinfo.len);
    common_hal_displayio_fourwire_end_transaction(self);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_3(displayio_fourwire_send_obj, displayio_fourwire_obj_send);

STATIC const mp_rom_map_elem_t displayio_fourwire_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&displayio_fourwire_send_obj) },
};
STATIC MP_DEFINE_CONST_DICT(displayio_fourwire_locals_dict, displayio_fourwire_locals_dict_table);

const mp_obj_type_t displayio_fourwire_type = {
    { &mp_type_type },
    .name = MP_QSTR_FourWire,
    .make_new = displayio_fourwire_make_new,
    .locals_dict = (mp_obj_dict_t*)&displayio_fourwire_locals_dict,
};
