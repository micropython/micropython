/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * SPDX-FileCopyrightText: Copyright (c) 2013, 2014 Damien P. George
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
#include "shared-bindings/digitalio/Direction.h"
#include "shared-bindings/digitalio/DriveMode.h"
#include "shared-bindings/digitalio/Pull.h"
#include "shared-bindings/util.h"
#include "supervisor/shared/translate.h"

//| class DigitalInOut:
//|     """Digital input and output
//|
//|     A DigitalInOut is used to digitally control I/O pins. For analog control of
//|     a pin, see the :py:class:`analogio.AnalogIn` and
//|     :py:class:`analogio.AnalogOut` classes."""
//|
//|     def __init__(self, pin: microcontroller.Pin) -> None:
//|         """Create a new DigitalInOut object associated with the pin. Defaults to input
//|         with no pull. Use :py:meth:`switch_to_input` and
//|         :py:meth:`switch_to_output` to change the direction.
//|
//|         :param ~microcontroller.Pin pin: The pin to control"""
//|         ...
//|
STATIC mp_obj_t digitalio_digitalinout_make_new(const mp_obj_type_t *type,
    mp_uint_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    mp_arg_check_num(n_args, kw_args, 1, 1, false);

    digitalio_digitalinout_obj_t *self = m_new_obj(digitalio_digitalinout_obj_t);
    self->base.type = &digitalio_digitalinout_type;

    mcu_pin_obj_t *pin = validate_obj_is_free_pin(args[0]);
    common_hal_digitalio_digitalinout_construct(self, pin);

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Turn off the DigitalInOut and release the pin for other use."""
//|         ...
//|
STATIC mp_obj_t digitalio_digitalinout_obj_deinit(mp_obj_t self_in) {
    digitalio_digitalinout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_digitalio_digitalinout_deinit(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(digitalio_digitalinout_deinit_obj, digitalio_digitalinout_obj_deinit);

//|     def __enter__(self) -> DigitalInOut:
//|         """No-op used by Context Managers."""
//|         ...
//|
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
STATIC mp_obj_t digitalio_digitalinout_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_digitalio_digitalinout_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(digitalio_digitalinout_obj___exit___obj, 4, 4, digitalio_digitalinout_obj___exit__);

STATIC void check_for_deinit(digitalio_digitalinout_obj_t *self) {
    if (common_hal_digitalio_digitalinout_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def switch_to_output(self, value: bool = False, drive_mode: DriveMode = DriveMode.PUSH_PULL) -> None:
//|         """Set the drive mode and value and then switch to writing out digital
//|         values.
//|
//|         :param bool value: default value to set upon switching
//|         :param ~digitalio.DriveMode drive_mode: drive mode for the output
//|         """
//|         ...
//|
STATIC mp_obj_t digitalio_digitalinout_switch_to_output(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_value, ARG_drive_mode };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_value,      MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_drive_mode, MP_ARG_OBJ, {.u_rom_obj = &digitalio_drive_mode_push_pull_obj} },
    };
    digitalio_digitalinout_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    digitalio_drive_mode_t drive_mode = DRIVE_MODE_PUSH_PULL;
    if (args[ARG_drive_mode].u_rom_obj == &digitalio_drive_mode_open_drain_obj) {
        drive_mode = DRIVE_MODE_OPEN_DRAIN;
    }
    // do the transfer
    digitalinout_result_t result = common_hal_digitalio_digitalinout_switch_to_output(self, args[ARG_value].u_bool, drive_mode);
    if (result == DIGITALINOUT_INPUT_ONLY) {
        mp_raise_NotImplementedError(translate("Pin is input only"));
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(digitalio_digitalinout_switch_to_output_obj, 1, digitalio_digitalinout_switch_to_output);

//|     def switch_to_input(self, pull: Optional[Pull] = None) -> None:
//|         """Set the pull and then switch to read in digital values.
//|
//|         :param Pull pull: pull configuration for the input
//|
//|         Example usage::
//|
//|           import digitalio
//|           import board
//|
//|           switch = digitalio.DigitalInOut(board.SLIDE_SWITCH)
//|           switch.switch_to_input(pull=digitalio.Pull.UP)
//|           # Or, after switch_to_input
//|           switch.pull = digitalio.Pull.UP
//|           print(switch.value)"""
//|         ...
//|
STATIC mp_obj_t digitalio_digitalinout_switch_to_input(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_pull };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pull, MP_ARG_OBJ, {.u_rom_obj = mp_const_none} },
    };
    digitalio_digitalinout_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);


    digitalio_pull_t pull = PULL_NONE;
    if (args[ARG_pull].u_rom_obj == &digitalio_pull_up_obj) {
        pull = PULL_UP;
    } else if (args[ARG_pull].u_rom_obj == &digitalio_pull_down_obj) {
        pull = PULL_DOWN;
    }
    // do the transfer
    common_hal_digitalio_digitalinout_switch_to_input(self, pull);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(digitalio_digitalinout_switch_to_input_obj, 1, digitalio_digitalinout_switch_to_input);

//|     direction: Direction
//|     """The direction of the pin.
//|
//|     Setting this will use the defaults from the corresponding
//|     :py:meth:`switch_to_input` or :py:meth:`switch_to_output` method. If
//|     you want to set pull, value or drive mode prior to switching, then use
//|     those methods instead."""
//|
typedef struct {
    mp_obj_base_t base;
} digitalio_digitalio_direction_obj_t;
extern const digitalio_digitalio_direction_obj_t digitalio_digitalio_direction_in_obj;
extern const digitalio_digitalio_direction_obj_t digitalio_digitalio_direction_out_obj;

STATIC mp_obj_t digitalio_digitalinout_obj_get_direction(mp_obj_t self_in) {
    digitalio_digitalinout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    digitalio_direction_t direction = common_hal_digitalio_digitalinout_get_direction(self);
    if (direction == DIRECTION_INPUT) {
        return (mp_obj_t)&digitalio_direction_input_obj;
    }
    return (mp_obj_t)&digitalio_direction_output_obj;
}
MP_DEFINE_CONST_FUN_OBJ_1(digitalio_digitalinout_get_direction_obj, digitalio_digitalinout_obj_get_direction);

STATIC mp_obj_t digitalio_digitalinout_obj_set_direction(mp_obj_t self_in, mp_obj_t value) {
    digitalio_digitalinout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    if (value == &digitalio_direction_input_obj) {
        common_hal_digitalio_digitalinout_switch_to_input(self, PULL_NONE);
    } else if (value == &digitalio_direction_output_obj) {
        digitalinout_result_t result = common_hal_digitalio_digitalinout_switch_to_output(self, false, DRIVE_MODE_PUSH_PULL);
        if (result == DIGITALINOUT_INPUT_ONLY) {
            mp_raise_NotImplementedError(translate("Pin is input only"));
        }
    } else {
        mp_raise_ValueError(translate("Invalid direction."));
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(digitalio_digitalinout_set_direction_obj, digitalio_digitalinout_obj_set_direction);

const mp_obj_property_t digitalio_digitalio_direction_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&digitalio_digitalinout_get_direction_obj,
              (mp_obj_t)&digitalio_digitalinout_set_direction_obj,
              MP_ROM_NONE},
};

//|     value: bool
//|     """The digital logic level of the pin."""
//|
STATIC mp_obj_t digitalio_digitalinout_obj_get_value(mp_obj_t self_in) {
    digitalio_digitalinout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    bool value = common_hal_digitalio_digitalinout_get_value(self);
    return mp_obj_new_bool(value);
}
MP_DEFINE_CONST_FUN_OBJ_1(digitalio_digitalinout_get_value_obj, digitalio_digitalinout_obj_get_value);

STATIC mp_obj_t digitalio_digitalinout_obj_set_value(mp_obj_t self_in, mp_obj_t value) {
    digitalio_digitalinout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    if (common_hal_digitalio_digitalinout_get_direction(self) == DIRECTION_INPUT) {
        mp_raise_AttributeError(translate("Cannot set value when direction is input."));
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
              MP_ROM_NONE},
};

//|     drive_mode: DriveMode
//|     """The pin drive mode. One of:
//|
//|     - `digitalio.DriveMode.PUSH_PULL`
//|     - `digitalio.DriveMode.OPEN_DRAIN`"""
//|
STATIC mp_obj_t digitalio_digitalinout_obj_get_drive_mode(mp_obj_t self_in) {
    digitalio_digitalinout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    if (common_hal_digitalio_digitalinout_get_direction(self) == DIRECTION_INPUT) {
        mp_raise_AttributeError(translate("Drive mode not used when direction is input."));
        return mp_const_none;
    }
    digitalio_drive_mode_t drive_mode = common_hal_digitalio_digitalinout_get_drive_mode(self);
    if (drive_mode == DRIVE_MODE_PUSH_PULL) {
        return (mp_obj_t)&digitalio_drive_mode_push_pull_obj;
    }
    return (mp_obj_t)&digitalio_drive_mode_open_drain_obj;
}
MP_DEFINE_CONST_FUN_OBJ_1(digitalio_digitalinout_get_drive_mode_obj, digitalio_digitalinout_obj_get_drive_mode);

STATIC mp_obj_t digitalio_digitalinout_obj_set_drive_mode(mp_obj_t self_in, mp_obj_t drive_mode) {
    digitalio_digitalinout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    if (common_hal_digitalio_digitalinout_get_direction(self) == DIRECTION_INPUT) {
        mp_raise_AttributeError(translate("Drive mode not used when direction is input."));
        return mp_const_none;
    }
    digitalio_drive_mode_t c_drive_mode = DRIVE_MODE_PUSH_PULL;
    if (drive_mode == &digitalio_drive_mode_open_drain_obj) {
        c_drive_mode = DRIVE_MODE_OPEN_DRAIN;
    }
    common_hal_digitalio_digitalinout_set_drive_mode(self, c_drive_mode);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(digitalio_digitalinout_set_drive_mode_obj, digitalio_digitalinout_obj_set_drive_mode);

const mp_obj_property_t digitalio_digitalio_drive_mode_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&digitalio_digitalinout_get_drive_mode_obj,
              (mp_obj_t)&digitalio_digitalinout_set_drive_mode_obj,
              MP_ROM_NONE},
};

//|     pull: Optional[Pull]
//|     """The pin pull direction. One of:
//|
//|     - `digitalio.Pull.UP`
//|     - `digitalio.Pull.DOWN`
//|     - `None`
//|
//|     :raises AttributeError: if `direction` is :py:data:`~digitalio.Direction.OUTPUT`."""
//|
STATIC mp_obj_t digitalio_digitalinout_obj_get_pull(mp_obj_t self_in) {
    digitalio_digitalinout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    if (common_hal_digitalio_digitalinout_get_direction(self) == DIRECTION_OUTPUT) {
        mp_raise_AttributeError(translate("Pull not used when direction is output."));
        return mp_const_none;
    }
    digitalio_pull_t pull = common_hal_digitalio_digitalinout_get_pull(self);
    if (pull == PULL_UP) {
        return (mp_obj_t)&digitalio_pull_up_obj;
    } else if (pull == PULL_DOWN) {
        return (mp_obj_t)&digitalio_pull_down_obj;
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(digitalio_digitalinout_get_pull_obj, digitalio_digitalinout_obj_get_pull);

STATIC mp_obj_t digitalio_digitalinout_obj_set_pull(mp_obj_t self_in, mp_obj_t pull_obj) {
    digitalio_digitalinout_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    if (common_hal_digitalio_digitalinout_get_direction(self) == DIRECTION_OUTPUT) {
        mp_raise_AttributeError(translate("Pull not used when direction is output."));
        return mp_const_none;
    }
    digitalio_pull_t pull = PULL_NONE;
    if (pull_obj == &digitalio_pull_up_obj) {
        pull = PULL_UP;
    } else if (pull_obj == &digitalio_pull_down_obj) {
        pull = PULL_DOWN;
    } else if (pull_obj != mp_const_none) {
        mp_raise_ValueError(translate("Unsupported pull value."));
    }
    common_hal_digitalio_digitalinout_set_pull(self, pull);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(digitalio_digitalinout_set_pull_obj, digitalio_digitalinout_obj_set_pull);

const mp_obj_property_t digitalio_digitalio_pull_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&digitalio_digitalinout_get_pull_obj,
              (mp_obj_t)&digitalio_digitalinout_set_pull_obj,
              MP_ROM_NONE},
};

STATIC const mp_rom_map_elem_t digitalio_digitalinout_locals_dict_table[] = {
    // instance methods
    { MP_ROM_QSTR(MP_QSTR_deinit),             MP_ROM_PTR(&digitalio_digitalinout_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__),          MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__),           MP_ROM_PTR(&digitalio_digitalinout_obj___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_switch_to_output),   MP_ROM_PTR(&digitalio_digitalinout_switch_to_output_obj) },
    { MP_ROM_QSTR(MP_QSTR_switch_to_input),    MP_ROM_PTR(&digitalio_digitalinout_switch_to_input_obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_direction),          MP_ROM_PTR(&digitalio_digitalio_direction_obj) },
    { MP_ROM_QSTR(MP_QSTR_value),              MP_ROM_PTR(&digitalio_digitalinout_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_drive_mode),         MP_ROM_PTR(&digitalio_digitalio_drive_mode_obj) },
    { MP_ROM_QSTR(MP_QSTR_pull),               MP_ROM_PTR(&digitalio_digitalio_pull_obj) },
};

STATIC MP_DEFINE_CONST_DICT(digitalio_digitalinout_locals_dict, digitalio_digitalinout_locals_dict_table);

const mp_obj_type_t digitalio_digitalinout_type = {
    { &mp_type_type },
    .name = MP_QSTR_DigitalInOut,
    .make_new = digitalio_digitalinout_make_new,
    .locals_dict = (mp_obj_t)&digitalio_digitalinout_locals_dict,
};

// Helper for validating digitalio.DigitalInOut arguments
digitalio_digitalinout_obj_t *assert_digitalinout(mp_obj_t obj) {
    if (!mp_obj_is_type(obj, &digitalio_digitalinout_type)) {
        mp_raise_TypeError(translate("argument num/types mismatch"));
    }
    digitalio_digitalinout_obj_t *pin = MP_OBJ_TO_PTR(obj);
    check_for_deinit(pin);
    return pin;
}
