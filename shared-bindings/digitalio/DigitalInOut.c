/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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
#include <string.h>

#include "lib/utils/context_manager_helpers.h"

#include "py/nlr.h"
#include "py/objtype.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/mphal.h"

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/digitalio/DigitalInOut.h"

//| .. currentmodule:: digitalio
//|
//| :class:`DigitalInOut` -- digital input and output
//| =========================================================
//|
//| A DigitalInOut is used to digitally control I/O pins. For analog control of
//| a pin, see the :py:class:`~digitalio.AnalogIn` and
//| :py:class:`~digitalio.AnalogOut` classes.
//|

//| .. class:: DigitalInOut(pin)
//|
//|   Create a new DigitalInOut object associated with the pin. Defaults to input
//|   with no pull. Use :py:meth:`switch_to_input` and
//|   :py:meth:`switch_to_output` to change the direction.
//|
//|   :param ~microcontroller.Pin pin: The pin to control
//|
STATIC mp_obj_t digitalio_digitalinout_make_new(const mp_obj_type_t *type,
        mp_uint_t n_args, mp_uint_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 1, MP_OBJ_FUN_ARGS_MAX, true);

    digitalio_digitalinout_obj_t *self = m_new_obj(digitalio_digitalinout_obj_t);
    self->base.type = &digitalio_digitalinout_type;

    assert_pin(args[0], false);
    mcu_pin_obj_t *pin = MP_OBJ_TO_PTR(args[0]);
    assert_pin_free(pin);
    common_hal_digitalio_digitalinout_construct(self, pin);

    return (mp_obj_t)self;
}

//|   .. method:: deinit()
//|
//|      Turn off the DigitalInOut and release the pin for other use.
//|
STATIC mp_obj_t digitalio_digitalinout_obj_deinit(mp_obj_t self_in) {
    digitalio_digitalinout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_digitalio_digitalinout_deinit(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(digitalio_digitalinout_deinit_obj, digitalio_digitalinout_obj_deinit);

//|   .. method:: __enter__()
//|
//|      No-op used by Context Managers.
//|
//  Provided by context manager helper.

//|   .. method:: __exit__()
//|
//|      Automatically deinitializes the hardware when exiting a context. See
//|      :ref:`lifetime-and-contextmanagers` for more info.
//|
STATIC mp_obj_t digitalio_digitalinout_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_digitalio_digitalinout_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(digitalio_digitalinout_obj___exit___obj, 4, 4, digitalio_digitalinout_obj___exit__);

//|
//|   .. method:: switch_to_output(value=False, drive_mode=DriveMode.PUSH_PULL)
//|
//|       Set the drive mode and value and then switch to writing out digital
//|       values.
//|
//|       :param bool value: default value to set upon switching
//|       :param DriveMode drive_mode: drive mode for the output
//|
typedef struct {
    mp_obj_base_t base;
} digitalio_digitalinout_drive_mode_obj_t;
extern const digitalio_digitalinout_drive_mode_obj_t digitalio_digitalinout_drive_mode_push_pull_obj;
extern const digitalio_digitalinout_drive_mode_obj_t digitalio_digitalinout_drive_mode_open_drain_obj;

STATIC mp_obj_t digitalio_digitalinout_switch_to_output(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_value, ARG_drive_mode };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_value,      MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_drive_mode, MP_ARG_OBJ, {.u_rom_obj = &digitalio_digitalinout_drive_mode_push_pull_obj} },
    };
    digitalio_digitalinout_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    enum digitalinout_drive_mode_t drive_mode = DRIVE_MODE_PUSH_PULL;
    if (args[ARG_drive_mode].u_rom_obj == &digitalio_digitalinout_drive_mode_open_drain_obj) {
        drive_mode = DRIVE_MODE_OPEN_DRAIN;
    }
    // do the transfer
    common_hal_digitalio_digitalinout_switch_to_output(self, args[ARG_value].u_bool, drive_mode);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(digitalio_digitalinout_switch_to_output_obj, 1, digitalio_digitalinout_switch_to_output);

//|   .. method:: switch_to_input(pull=None)
//|
//|     Set the pull and then switch to read in digital values.
//|
//|     :param Pull pull: pull configuration for the input
//|
//|     Example usage::
//|
//|       import digitalio
//|       import board
//|
//|       switch = digitalio.DigitalInOut(board.SLIDE_SWITCH)
//|       switch.switch_to_input(pull=digitalio.DigitalInOut.Pull.UP)
//|       # Or, after switch_to_input
//|       switch.pull = digitalio.DigitalInOut.Pull.UP
//|       print(switch.value)
//|
typedef struct {
    mp_obj_base_t base;
} digitalio_digitalinout_pull_obj_t;
extern const digitalio_digitalinout_pull_obj_t digitalio_digitalinout_pull_up_obj;
extern const digitalio_digitalinout_pull_obj_t digitalio_digitalinout_pull_down_obj;

STATIC mp_obj_t digitalio_digitalinout_switch_to_input(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_pull };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pull, MP_ARG_OBJ, {.u_rom_obj = mp_const_none} },
    };
    digitalio_digitalinout_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    enum digitalinout_pull_t pull = PULL_NONE;
    if (args[ARG_pull].u_rom_obj == &digitalio_digitalinout_pull_up_obj) {
        pull = PULL_UP;
    }else if (args[ARG_pull].u_rom_obj == &digitalio_digitalinout_pull_down_obj) {
        pull = PULL_DOWN;
    }
    // do the transfer
    common_hal_digitalio_digitalinout_switch_to_input(self, pull);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(digitalio_digitalinout_switch_to_input_obj, 1, digitalio_digitalinout_switch_to_input);

//|   .. attribute:: direction
//|
//|     The direction of the pin.
//|
//|     Setting this will use the defaults from the corresponding
//|     :py:meth:`switch_to_input` or :py:meth:`switch_to_output` method. If
//|     you want to set pull, value or drive mode prior to switching, then use
//|     those methods instead.
//|
typedef struct {
    mp_obj_base_t base;
} digitalio_digitalinout_direction_obj_t;
extern const digitalio_digitalinout_direction_obj_t digitalio_digitalinout_direction_in_obj;
extern const digitalio_digitalinout_direction_obj_t digitalio_digitalinout_direction_out_obj;

STATIC mp_obj_t digitalio_digitalinout_obj_get_direction(mp_obj_t self_in) {
    digitalio_digitalinout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    enum digitalinout_direction_t direction = common_hal_digitalio_digitalinout_get_direction(self);
    if (direction == DIRECTION_IN) {
        return (mp_obj_t)&digitalio_digitalinout_direction_in_obj;
    }
    return (mp_obj_t)&digitalio_digitalinout_direction_out_obj;
}
MP_DEFINE_CONST_FUN_OBJ_1(digitalio_digitalinout_get_direction_obj, digitalio_digitalinout_obj_get_direction);

STATIC mp_obj_t digitalio_digitalinout_obj_set_direction(mp_obj_t self_in, mp_obj_t value) {
    digitalio_digitalinout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (value == &digitalio_digitalinout_direction_in_obj) {
        common_hal_digitalio_digitalinout_switch_to_input(self, PULL_NONE);
    } else if (value == &digitalio_digitalinout_direction_out_obj) {
        common_hal_digitalio_digitalinout_switch_to_output(self, false, DRIVE_MODE_PUSH_PULL);
    } else {
        mp_raise_ValueError("Invalid direction.");
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(digitalio_digitalinout_set_direction_obj, digitalio_digitalinout_obj_set_direction);

const mp_obj_property_t digitalio_digitalinout_direction_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&digitalio_digitalinout_get_direction_obj,
              (mp_obj_t)&digitalio_digitalinout_set_direction_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|   .. attribute:: value
//|
//|     The digital logic level of the pin.
//|
STATIC mp_obj_t digitalio_digitalinout_obj_get_value(mp_obj_t self_in) {
    digitalio_digitalinout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    bool value = common_hal_digitalio_digitalinout_get_value(self);
    return mp_obj_new_bool(value);
}
MP_DEFINE_CONST_FUN_OBJ_1(digitalio_digitalinout_get_value_obj, digitalio_digitalinout_obj_get_value);

STATIC mp_obj_t digitalio_digitalinout_obj_set_value(mp_obj_t self_in, mp_obj_t value) {
    digitalio_digitalinout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (common_hal_digitalio_digitalinout_get_direction(self) == DIRECTION_IN) {
        mp_raise_AttributeError("Cannot set value when direction is input.");
        return mp_const_none;
    }
    common_hal_digitalio_digitalinout_set_value(self, mp_obj_is_true(value));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(digitalio_digitalinout_set_value_obj, digitalio_digitalinout_obj_set_value);

const mp_obj_property_t digitalio_digitalinout_value_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&digitalio_digitalinout_get_value_obj,
              (mp_obj_t)&digitalio_digitalinout_set_value_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|   .. attribute:: drive_mode
//|
//|     Get or set the pin drive mode.
//|
STATIC mp_obj_t digitalio_digitalinout_obj_get_drive_mode(mp_obj_t self_in) {
    digitalio_digitalinout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (common_hal_digitalio_digitalinout_get_direction(self) == DIRECTION_IN) {
        mp_raise_AttributeError("Drive mode not used when direction is input.");
        return mp_const_none;
    }
    enum digitalinout_drive_mode_t drive_mode = common_hal_digitalio_digitalinout_get_drive_mode(self);
    if (drive_mode == DRIVE_MODE_PUSH_PULL) {
        return (mp_obj_t)&digitalio_digitalinout_drive_mode_push_pull_obj;
    }
    return (mp_obj_t)&digitalio_digitalinout_drive_mode_open_drain_obj;
}
MP_DEFINE_CONST_FUN_OBJ_1(digitalio_digitalinout_get_drive_mode_obj, digitalio_digitalinout_obj_get_drive_mode);

STATIC mp_obj_t digitalio_digitalinout_obj_set_drive_mode(mp_obj_t self_in, mp_obj_t drive_mode) {
    digitalio_digitalinout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (common_hal_digitalio_digitalinout_get_direction(self) == DIRECTION_IN) {
        mp_raise_AttributeError("Drive mode not used when direction is input.");
        return mp_const_none;
    }
    enum digitalinout_drive_mode_t c_drive_mode = DRIVE_MODE_PUSH_PULL;
    if (drive_mode == &digitalio_digitalinout_drive_mode_open_drain_obj) {
        c_drive_mode = DRIVE_MODE_OPEN_DRAIN;
    }
    common_hal_digitalio_digitalinout_set_drive_mode(self, c_drive_mode);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(digitalio_digitalinout_set_drive_mode_obj, digitalio_digitalinout_obj_set_drive_mode);

const mp_obj_property_t digitalio_digitalinout_drive_mode_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&digitalio_digitalinout_get_drive_mode_obj,
              (mp_obj_t)&digitalio_digitalinout_set_drive_mode_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//|   .. attribute:: pull
//|
//|     Get or set the pin pull.
//|
//|     :raises AttributeError: if the direction is ~`Direction.OUT`.
//|
STATIC mp_obj_t digitalio_digitalinout_obj_get_pull(mp_obj_t self_in) {
    digitalio_digitalinout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (common_hal_digitalio_digitalinout_get_direction(self) == DIRECTION_OUT) {
        mp_raise_AttributeError("Pull not used when direction is output.");
        return mp_const_none;
    }
    enum digitalinout_pull_t pull = common_hal_digitalio_digitalinout_get_pull(self);
    if (pull == PULL_UP) {
        return (mp_obj_t)&digitalio_digitalinout_pull_up_obj;
    } else if (pull == PULL_DOWN) {
        return (mp_obj_t)&digitalio_digitalinout_pull_down_obj;
    }
    return (mp_obj_t)&mp_const_none_obj;
}
MP_DEFINE_CONST_FUN_OBJ_1(digitalio_digitalinout_get_pull_obj, digitalio_digitalinout_obj_get_pull);

STATIC mp_obj_t digitalio_digitalinout_obj_set_pull(mp_obj_t self_in, mp_obj_t pull_obj) {
    digitalio_digitalinout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (common_hal_digitalio_digitalinout_get_direction(self) == DIRECTION_OUT) {
        mp_raise_AttributeError("Pull not used when direction is output.");
        return mp_const_none;
    }
    enum digitalinout_pull_t pull = PULL_NONE;
    if (pull_obj == &digitalio_digitalinout_pull_up_obj) {
        pull = PULL_UP;
    } else if (pull_obj == &digitalio_digitalinout_pull_down_obj) {
        pull = PULL_DOWN;
    }
    common_hal_digitalio_digitalinout_set_pull(self, pull);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(digitalio_digitalinout_set_pull_obj, digitalio_digitalinout_obj_set_pull);

const mp_obj_property_t digitalio_digitalinout_pull_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&digitalio_digitalinout_get_pull_obj,
              (mp_obj_t)&digitalio_digitalinout_set_pull_obj,
              (mp_obj_t)&mp_const_none_obj},
};

//| .. class:: digitalio.DigitalInOut.Direction
//|
//|     Enum-like class to define which direction the digital values are
//|     going.
//|
//|     .. data:: IN
//|
//|       Read digital data in
//|
//|     .. data:: OUT
//|
//|       Write digital data out
//|
const mp_obj_type_t digitalio_digitalinout_direction_type;

const digitalio_digitalinout_direction_obj_t digitalio_digitalinout_direction_in_obj = {
    { &digitalio_digitalinout_direction_type },
};

const digitalio_digitalinout_direction_obj_t digitalio_digitalinout_direction_out_obj = {
    { &digitalio_digitalinout_direction_type },
};

STATIC const mp_rom_map_elem_t digitalio_digitalinout_direction_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_IN),    MP_ROM_PTR(&digitalio_digitalinout_direction_in_obj) },
    { MP_ROM_QSTR(MP_QSTR_OUT),   MP_ROM_PTR(&digitalio_digitalinout_direction_out_obj) },
};
STATIC MP_DEFINE_CONST_DICT(digitalio_digitalinout_direction_locals_dict, digitalio_digitalinout_direction_locals_dict_table);

STATIC void digitalio_digitalinout_direction_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    qstr direction = MP_QSTR_IN;
    if (MP_OBJ_TO_PTR(self_in) == MP_ROM_PTR(&digitalio_digitalinout_direction_out_obj)) {
        direction = MP_QSTR_OUT;
    }
    mp_printf(print, "%q.%q.%q.%q", MP_QSTR_digitalio, MP_QSTR_DigitalInOut, MP_QSTR_Direction, direction);
}

const mp_obj_type_t digitalio_digitalinout_direction_type = {
    { &mp_type_type },
    .name = MP_QSTR_Direction,
    .print = digitalio_digitalinout_direction_print,
    .locals_dict = (mp_obj_t)&digitalio_digitalinout_direction_locals_dict,
};

//| .. class:: digitalio.DigitalInOut.DriveMode
//|
//|     Enum-like class to define the drive mode used when outputting
//|     digital values.
//|
//|     .. data:: PUSH_PULL
//|
//|       Output both high and low digital values
//|
//|     .. data:: OPEN_DRAIN
//|
//|       Output low digital values but go into high z for digital high. This is
//|       useful for i2c and other protocols that share a digital line.
//|
const mp_obj_type_t digitalio_digitalinout_drive_mode_type;

const digitalio_digitalinout_drive_mode_obj_t digitalio_digitalinout_drive_mode_push_pull_obj = {
    { &digitalio_digitalinout_drive_mode_type },
};

const digitalio_digitalinout_drive_mode_obj_t digitalio_digitalinout_drive_mode_open_drain_obj = {
    { &digitalio_digitalinout_drive_mode_type },
};

STATIC const mp_rom_map_elem_t digitalio_digitalinout_drive_mode_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_PUSH_PULL),    MP_ROM_PTR(&digitalio_digitalinout_drive_mode_push_pull_obj) },
    { MP_ROM_QSTR(MP_QSTR_OPEN_DRAIN),   MP_ROM_PTR(&digitalio_digitalinout_drive_mode_open_drain_obj) },
};
STATIC MP_DEFINE_CONST_DICT(digitalio_digitalinout_drive_mode_locals_dict, digitalio_digitalinout_drive_mode_locals_dict_table);

STATIC void digitalio_digitalinout_drive_mode_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    qstr drive_mode = MP_QSTR_PUSH_PULL;
    if (MP_OBJ_TO_PTR(self_in) == MP_ROM_PTR(&digitalio_digitalinout_drive_mode_open_drain_obj)) {
        drive_mode = MP_QSTR_OPEN_DRAIN;
    }
    mp_printf(print, "%q.%q.%q.%q", MP_QSTR_digitalio, MP_QSTR_DigitalInOut, MP_QSTR_DriveMode, drive_mode);
}

const mp_obj_type_t digitalio_digitalinout_drive_mode_type = {
    { &mp_type_type },
    .name = MP_QSTR_DriveMode,
    .print = digitalio_digitalinout_drive_mode_print,
    .locals_dict = (mp_obj_t)&digitalio_digitalinout_drive_mode_locals_dict,
};

//| .. class:: digitalio.DigitalInOut.Pull
//|
//|     Enum-like class to define the pull value, if any, used while reading
//|     digital values in.
//|
//|     .. data:: UP
//|
//|       When the input line isn't being driven the pull up can pull the state
//|       of the line high so it reads as true.
//|
//|     .. data:: DOWN
//|
//|       When the input line isn't being driven the pull down can pull the
//|       state of the line low so it reads as false.
//|
const mp_obj_type_t digitalio_digitalinout_pull_type;

const digitalio_digitalinout_pull_obj_t digitalio_digitalinout_pull_up_obj = {
    { &digitalio_digitalinout_pull_type },
};

const digitalio_digitalinout_pull_obj_t digitalio_digitalinout_pull_down_obj = {
    { &digitalio_digitalinout_pull_type },
};

STATIC const mp_rom_map_elem_t digitalio_digitalinout_pull_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_UP),    MP_ROM_PTR(&digitalio_digitalinout_pull_up_obj) },
    { MP_ROM_QSTR(MP_QSTR_DOWN),  MP_ROM_PTR(&digitalio_digitalinout_pull_down_obj) },
};
STATIC MP_DEFINE_CONST_DICT(digitalio_digitalinout_pull_locals_dict, digitalio_digitalinout_pull_locals_dict_table);

STATIC void digitalio_digitalinout_pull_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    qstr pull = MP_QSTR_UP;
    if (MP_OBJ_TO_PTR(self_in) == MP_ROM_PTR(&digitalio_digitalinout_pull_down_obj)) {
        pull = MP_QSTR_DOWN;
    }
    mp_printf(print, "%q.%q.%q.%q", MP_QSTR_digitalio, MP_QSTR_DigitalInOut, MP_QSTR_Pull, pull);
}

const mp_obj_type_t digitalio_digitalinout_pull_type = {
    { &mp_type_type },
    .name = MP_QSTR_Pull,
    .print = digitalio_digitalinout_pull_print,
    .locals_dict = (mp_obj_t)&digitalio_digitalinout_pull_locals_dict,
};

STATIC const mp_rom_map_elem_t digitalio_digitalinout_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_deinit),                 MP_ROM_PTR(&digitalio_digitalinout_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__),              MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__),               MP_ROM_PTR(&digitalio_digitalinout_obj___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_switch_to_output),   MP_ROM_PTR(&digitalio_digitalinout_switch_to_output_obj) },
    { MP_ROM_QSTR(MP_QSTR_switch_to_input),    MP_ROM_PTR(&digitalio_digitalinout_switch_to_input_obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_direction),          MP_ROM_PTR(&digitalio_digitalinout_direction_obj) },
    { MP_ROM_QSTR(MP_QSTR_value),              MP_ROM_PTR(&digitalio_digitalinout_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_drive_mode),         MP_ROM_PTR(&digitalio_digitalinout_drive_mode_obj) },
    { MP_ROM_QSTR(MP_QSTR_pull),               MP_ROM_PTR(&digitalio_digitalinout_pull_obj) },

    // Nested Enum-like Classes.
    { MP_ROM_QSTR(MP_QSTR_Direction),          MP_ROM_PTR(&digitalio_digitalinout_direction_type) },
    { MP_ROM_QSTR(MP_QSTR_DriveMode),          MP_ROM_PTR(&digitalio_digitalinout_drive_mode_type) },
    { MP_ROM_QSTR(MP_QSTR_Pull),               MP_ROM_PTR(&digitalio_digitalinout_pull_type) },
};

STATIC MP_DEFINE_CONST_DICT(digitalio_digitalinout_locals_dict, digitalio_digitalinout_locals_dict_table);

const mp_obj_type_t digitalio_digitalinout_type = {
    { &mp_type_type },
    .name = MP_QSTR_DigitalInOut,
    .make_new = digitalio_digitalinout_make_new,
    .locals_dict = (mp_obj_t)&digitalio_digitalinout_locals_dict,
};
