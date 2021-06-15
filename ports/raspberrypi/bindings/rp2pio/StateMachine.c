/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2021 Scott Shawcroft for Adafruit Industries
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

// This file contains all of the Python API definitions for the
// rp2pio.StateMachine class.

#include <string.h>

#include "shared-bindings/microcontroller/Pin.h"
#include "bindings/rp2pio/StateMachine.h"
#include "shared-bindings/util.h"

#include "lib/utils/buffer_helper.h"
#include "lib/utils/context_manager_helpers.h"
#include "lib/utils/interrupt_char.h"
#include "py/binary.h"
#include "py/mperrno.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "supervisor/shared/translate.h"


//| class StateMachine:
//|     """A single PIO StateMachine
//|
//|     The programmable I/O peripheral on the RP2 series of microcontrollers is
//|     unique. It is a collection of generic state machines that can be
//|     used for a variety of protocols. State machines may be independent or
//|     coordinated. Program memory and IRQs are shared between the state machines
//|     in a particular PIO instance. They are independent otherwise.
//|
//|     This class is designed to facilitate sharing of PIO resources. By default,
//|     it is assumed that the state machine is used on its own and can be placed
//|     in either PIO. State machines with the same program will be placed in the
//|     same PIO if possible."""
//|
//|     def __init__(self,
//|                  program: ReadableBuffer,
//|                  frequency: int,
//|                  *,
//|                  init: Optional[ReadableBuffer] = None,
//|                  first_out_pin: Optional[microcontroller.Pin] = None,
//|                  out_pin_count: int = 1,
//|                  initial_out_pin_state: int = 0,
//|                  initial_out_pin_direction: int = 0xffffffff,
//|                  first_in_pin: Optional[microcontroller.Pin] = None,
//|                  in_pin_count: int = 1,
//|                  pull_in_pin_up: int = 0,
//|                  pull_in_pin_down: int = 0,
//|                  first_set_pin: Optional[microcontroller.Pin] = None,
//|                  set_pin_count: int = 1,
//|                  initial_set_pin_state: int = 0,
//|                  initial_set_pin_direction: int = 0x1f,
//|                  first_sideset_pin: Optional[microcontroller.Pin] = None,
//|                  sideset_pin_count: int = 1,
//|                  initial_sideset_pin_state: int = 0,
//|                  initial_sideset_pin_direction: int = 0x1f,
//|                  exclusive_pin_use: bool = True,
//|                  auto_pull: bool = False,
//|                  pull_threshold : int = 32,
//|                  out_shift_right : bool = True,
//|                  wait_for_txstall: bool = True,
//|                  auto_push: bool = False,
//|                  push_threshold : int = 32,
//|                  in_shift_right : bool = True) -> None:
//|
//|         """Construct a StateMachine object on the given pins with the given program.
//|
//|         :param ReadableBuffer program: the program to run with the state machine
//|         :param int frequency: the target clock frequency of the state machine. Actual may be less.
//|         :param ReadableBuffer init: a program to run once at start up. This is run after program
//|              is started so instructions may be intermingled
//|         :param ~microcontroller.Pin first_out_pin: the first pin to use with the OUT instruction
//|         :param int out_pin_count: the count of consecutive pins to use with OUT starting at first_out_pin
//|         :param int initial_out_pin_state: the initial output value for out pins starting at first_out_pin
//|         :param int initial_out_pin_direction: the initial output direction for out pins starting at first_out_pin
//|         :param ~microcontroller.Pin first_in_pin: the first pin to use with the IN instruction
//|         :param int in_pin_count: the count of consecutive pins to use with IN starting at first_in_pin
//|         :param int pull_in_pin_up: a 1-bit in this mask sets pull up on the corresponding in pin
//|         :param int pull_in_pin_down: a 1-bit in this mask sets pull down on the corresponding in pin. Setting both pulls enables a "bus keep" function, i.e. a weak pull to whatever is current high/low state of GPIO.
//|         :param ~microcontroller.Pin first_set_pin: the first pin to use with the SET instruction
//|         :param int set_pin_count: the count of consecutive pins to use with SET starting at first_set_pin
//|         :param int initial_set_pin_state: the initial output value for set pins starting at first_set_pin
//|         :param int initial_set_pin_direction: the initial output direction for set pins starting at first_set_pin
//|         :param ~microcontroller.Pin first_sideset_pin: the first pin to use with a side set
//|         :param int sideset_pin_count: the count of consecutive pins to use with a side set starting at first_sideset_pin
//|         :param int initial_sideset_pin_state: the initial output value for sideset pins starting at first_sideset_pin
//|         :param int initial_sideset_pin_direction: the initial output direction for sideset pins starting at first_sideset_pin
//|         :param bool exclusive_pin_use: When True, do not share any pins with other state machines. Pins are never shared with other peripherals
//|         :param bool auto_pull: When True, automatically load data from the tx FIFO into the
//|             output shift register (OSR) when an OUT instruction shifts more than pull_threshold bits
//|         :param int pull_threshold: Number of bits to shift before loading a new value into the OSR from the tx FIFO
//|         :param bool out_shift_right: When True, data is shifted out the right side (LSB) of the
//|             OSR. It is shifted out the left (MSB) otherwise. NOTE! This impacts data alignment
//|             when the number of bytes is not a power of two (1, 2 or 4 bytes).
//|         :param bool wait_for_txstall: When True, writing data out will block until the TX FIFO and OSR are empty
//|             and an instruction is stalled waiting for more data. When False, data writes won't
//|             wait for the OSR to empty (only the TX FIFO) so make sure you give enough time before
//|             deiniting or stopping the state machine.
//|         :param bool auto_push: When True, automatically save data from input shift register
//|              (ISR) into the rx FIFO when an IN instruction shifts more than push_threshold bits
//|         :param int push_threshold: Number of bits to shift before saving the ISR value to the RX FIFO
//|         :param bool in_shift_right: When True, data is shifted into the right side (LSB) of the
//|             ISR. It is shifted into the left (MSB) otherwise. NOTE! This impacts data alignment
//|             when the number of bytes is not a power of two (1, 2 or 4 bytes)."""
//|         ...
//|

STATIC mp_obj_t rp2pio_statemachine_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    rp2pio_statemachine_obj_t *self = m_new_obj(rp2pio_statemachine_obj_t);
    self->base.type = &rp2pio_statemachine_type;
    enum { ARG_program, ARG_frequency, ARG_init,
           ARG_first_out_pin, ARG_out_pin_count, ARG_initial_out_pin_state, ARG_initial_out_pin_direction,
           ARG_first_in_pin, ARG_in_pin_count,
           ARG_pull_in_pin_up, ARG_pull_in_pin_down,
           ARG_first_set_pin, ARG_set_pin_count, ARG_initial_set_pin_state, ARG_initial_set_pin_direction,
           ARG_first_sideset_pin, ARG_sideset_pin_count, ARG_initial_sideset_pin_state, ARG_initial_sideset_pin_direction,
           ARG_exclusive_pin_use,
           ARG_auto_pull, ARG_pull_threshold, ARG_out_shift_right,
           ARG_wait_for_txstall,
           ARG_auto_push, ARG_push_threshold, ARG_in_shift_right};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_program, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_frequency, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_init, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },

        { MP_QSTR_first_out_pin, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_out_pin_count, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_initial_out_pin_state, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_initial_out_pin_direction, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0xffffffff} },

        { MP_QSTR_first_in_pin, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_in_pin_count, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_pull_in_pin_up, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_pull_in_pin_down, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },

        { MP_QSTR_first_set_pin, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_set_pin_count, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_initial_set_pin_state, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_initial_set_pin_direction, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0x1f} },

        { MP_QSTR_first_sideset_pin, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_sideset_pin_count, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_initial_sideset_pin_state, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_initial_sideset_pin_direction, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0x1f} },

        { MP_QSTR_exclusive_pin_use, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
        { MP_QSTR_auto_pull, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_pull_threshold, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 32} },
        { MP_QSTR_out_shift_right, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
        { MP_QSTR_wait_for_txstall, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
        { MP_QSTR_auto_push, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_push_threshold, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 32} },
        { MP_QSTR_in_shift_right, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_program].u_obj, &bufinfo, MP_BUFFER_READ);

    mp_buffer_info_t init_bufinfo;
    init_bufinfo.len = 0;
    mp_get_buffer(args[ARG_init].u_obj, &init_bufinfo, MP_BUFFER_READ);

    // We don't validate pin in use here because we may be ok sharing them within a PIO.
    mcu_pin_obj_t *first_out_pin = validate_obj_is_pin_or_none(args[ARG_first_out_pin].u_obj);
    if (args[ARG_out_pin_count].u_int < 1) {
        mp_raise_ValueError(translate("Pin count must be at least 1"));
    }
    mcu_pin_obj_t *first_in_pin = validate_obj_is_pin_or_none(args[ARG_first_in_pin].u_obj);
    if (args[ARG_in_pin_count].u_int < 1) {
        mp_raise_ValueError(translate("Pin count must be at least 1"));
    }
    mcu_pin_obj_t *first_set_pin = validate_obj_is_pin_or_none(args[ARG_first_set_pin].u_obj);
    if (args[ARG_set_pin_count].u_int < 1) {
        mp_raise_ValueError(translate("Pin count must be at least 1"));
    }
    if (args[ARG_set_pin_count].u_int > 5) {
        mp_raise_ValueError(translate("Set pin count must be between 1 and 5"));
    }
    mcu_pin_obj_t *first_sideset_pin = validate_obj_is_pin_or_none(args[ARG_first_sideset_pin].u_obj);
    if (args[ARG_sideset_pin_count].u_int < 1) {
        mp_raise_ValueError(translate("Pin count must be at least 1"));
    }
    if (args[ARG_sideset_pin_count].u_int > 5) {
        mp_raise_ValueError(translate("Side set pin count must be between 1 and 5"));
    }

    mp_int_t pull_threshold = args[ARG_pull_threshold].u_int;
    mp_int_t push_threshold = args[ARG_push_threshold].u_int;
    if (pull_threshold < 1 || pull_threshold > 32) {
        mp_raise_ValueError(translate("pull_threshold must be between 1 and 32"));
    }
    if (push_threshold < 1 || push_threshold > 32) {
        mp_raise_ValueError(translate("push_threshold must be between 1 and 32"));
    }

    if (bufinfo.len < 2) {
        mp_raise_ValueError(translate("Program must contain at least one 16-bit instruction."));
    }
    if (bufinfo.len % 2 != 0) {
        mp_raise_ValueError(translate("Program size invalid"));
    }
    if (bufinfo.len > 64) {
        mp_raise_ValueError(translate("Program too large"));
    }

    if (init_bufinfo.len % 2 != 0) {
        mp_raise_ValueError(translate("Init program size invalid"));
    }

    common_hal_rp2pio_statemachine_construct(self,
        bufinfo.buf, bufinfo.len / 2,
        args[ARG_frequency].u_int,
        init_bufinfo.buf, init_bufinfo.len / 2,
        first_out_pin, args[ARG_out_pin_count].u_int, args[ARG_initial_out_pin_state].u_int, args[ARG_initial_out_pin_direction].u_int,
        first_in_pin, args[ARG_in_pin_count].u_int, args[ARG_pull_in_pin_up].u_int, args[ARG_pull_in_pin_down].u_int,
        first_set_pin, args[ARG_set_pin_count].u_int, args[ARG_initial_set_pin_state].u_int, args[ARG_initial_set_pin_direction].u_int,
        first_sideset_pin, args[ARG_sideset_pin_count].u_int, args[ARG_initial_sideset_pin_state].u_int, args[ARG_initial_sideset_pin_direction].u_int,
        0,
        args[ARG_exclusive_pin_use].u_bool,
        args[ARG_auto_pull].u_bool, pull_threshold, args[ARG_out_shift_right].u_bool,
        args[ARG_wait_for_txstall].u_bool,
        args[ARG_auto_push].u_bool, push_threshold, args[ARG_in_shift_right].u_bool);
    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Turn off the state machine and release its resources."""
//|         ...
//|
STATIC mp_obj_t rp2pio_statemachine_obj_deinit(mp_obj_t self_in) {
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_rp2pio_statemachine_deinit(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(rp2pio_statemachine_deinit_obj, rp2pio_statemachine_obj_deinit);

//|     def __enter__(self) -> StateMachine:
//|         """No-op used by Context Managers.
//|         Provided by context manager helper."""
//|         ...
//|

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
STATIC mp_obj_t rp2pio_statemachine_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_rp2pio_statemachine_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rp2pio_statemachine_obj___exit___obj, 4, 4, rp2pio_statemachine_obj___exit__);


STATIC void check_for_deinit(rp2pio_statemachine_obj_t *self) {
    if (common_hal_rp2pio_statemachine_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def restart(self) -> None:
//|         """Resets this state machine, runs any init and enables the clock."""
// TODO: "and any others given. They must share an underlying PIO. An exception will be raised otherwise.""
//|         ...
//|
STATIC mp_obj_t rp2pio_statemachine_restart(mp_obj_t self_obj) {
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(self_obj);
    check_for_deinit(self);

    common_hal_rp2pio_statemachine_restart(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(rp2pio_statemachine_restart_obj, rp2pio_statemachine_restart);


//|     def run(self, instructions: ReadableBuffer) -> None:
//|         """Runs all given instructions. They will likely be interleaved with
//|            in-memory instructions. Make sure this doesn't wait for input!
//|
//|            This can be used to output internal state to the RX FIFO and then
//|            read with `readinto`."""
//|         ...
//|
STATIC mp_obj_t rp2pio_statemachine_run(mp_obj_t self_obj, mp_obj_t instruction_obj) {
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(self_obj);
    check_for_deinit(self);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(instruction_obj, &bufinfo, MP_BUFFER_READ);

    common_hal_rp2pio_statemachine_run(self, bufinfo.buf, bufinfo.len);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(rp2pio_statemachine_run_obj, rp2pio_statemachine_run);

//|     def stop(self) -> None:
//|         """Stops the state machine clock. Use `restart` to enable it."""
//|         ...
//|
STATIC mp_obj_t rp2pio_statemachine_stop(mp_obj_t self_obj) {
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(self_obj);
    check_for_deinit(self);

    common_hal_rp2pio_statemachine_stop(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(rp2pio_statemachine_stop_obj, rp2pio_statemachine_stop);

//|     def write(self, buffer: ReadableBuffer, *, start: int = 0, end: Optional[int] = None) -> None:
//|         """Write the data contained in ``buffer`` to the state machine. If the buffer is empty, nothing happens.
//|
//|         :param ~_typing.ReadableBuffer buffer: Write out the data in this buffer
//|         :param int start: Start of the slice of ``buffer`` to write out: ``buffer[start:end]``
//|         :param int end: End of the slice; this index is not included. Defaults to ``len(buffer)``"""
//|         ...
//|
STATIC mp_obj_t rp2pio_statemachine_write(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer, ARG_start, ARG_end };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_start,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_end,        MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
    };
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_READ);
    int32_t start = args[ARG_start].u_int;
    size_t length = bufinfo.len;
    normalize_buffer_bounds(&start, args[ARG_end].u_int, &length);
    if (length == 0) {
        return mp_const_none;
    }

    uint8_t *original_pointer = bufinfo.buf;
    int stride_in_bytes = mp_binary_get_size('@', bufinfo.typecode, NULL);
    if (stride_in_bytes > 4) {
        mp_raise_ValueError(translate("Buffer elements must be 4 bytes long or less"));
    }

    bool ok = common_hal_rp2pio_statemachine_write(self, ((uint8_t *)bufinfo.buf) + start, length, stride_in_bytes);
    if (mp_hal_is_interrupted()) {
        return mp_const_none;
    }
    if (!ok) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(rp2pio_statemachine_write_obj, 2, rp2pio_statemachine_write);


//|     def readinto(self, buffer: WriteableBuffer, *, start: int = 0, end: Optional[int] = None) -> None:
//|         """Read into ``buffer``. If the number of bytes to read is 0, nothing happens. The buffer
//|            include any data added to the fifo even if it was added before this was called.
//|
//|         :param ~_typing.WriteableBuffer buffer: Read data into this buffer
//|         :param int start: Start of the slice of ``buffer`` to read into: ``buffer[start:end]``
//|         :param int end: End of the slice; this index is not included. Defaults to ``len(buffer)``"""
//|         ...
//|

STATIC mp_obj_t rp2pio_statemachine_readinto(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer, ARG_start, ARG_end };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_start,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_end,        MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
    };
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_WRITE);
    int32_t start = args[ARG_start].u_int;
    size_t length = bufinfo.len;
    normalize_buffer_bounds(&start, args[ARG_end].u_int, &length);

    if (length == 0) {
        return mp_const_none;
    }

    uint8_t *original_pointer = bufinfo.buf;
    int stride_in_bytes = mp_binary_get_size('@', bufinfo.typecode, NULL);
    if (stride_in_bytes > 4) {
        mp_raise_ValueError(translate("Buffer elements must be 4 bytes long or less"));
    }

    bool ok = common_hal_rp2pio_statemachine_readinto(self, ((uint8_t *)bufinfo.buf) + start, length, stride_in_bytes);
    if (!ok) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(rp2pio_statemachine_readinto_obj, 2, rp2pio_statemachine_readinto);

//|     def write_readinto(self, buffer_out: ReadableBuffer, buffer_in: WriteableBuffer, *, out_start: int = 0, out_end: Optional[int] = None, in_start: int = 0, in_end: Optional[int] = None) -> None:
//|         """Write out the data in ``buffer_out`` while simultaneously reading data into ``buffer_in``.
//|         The lengths of the slices defined by ``buffer_out[out_start:out_end]`` and ``buffer_in[in_start:in_end]``
//|         may be different. The function will return once both are filled.
//|         If buffer slice lengths are both 0, nothing happens.
//|
//|         :param ~_typing.ReadableBuffer buffer_out: Write out the data in this buffer
//|         :param ~_typing.WriteableBuffer buffer_in: Read data into this buffer
//|         :param int out_start: Start of the slice of buffer_out to write out: ``buffer_out[out_start:out_end]``
//|         :param int out_end: End of the slice; this index is not included. Defaults to ``len(buffer_out)``
//|         :param int in_start: Start of the slice of ``buffer_in`` to read into: ``buffer_in[in_start:in_end]``
//|         :param int in_end: End of the slice; this index is not included. Defaults to ``len(buffer_in)``"""
//|         ...
//|

STATIC mp_obj_t rp2pio_statemachine_write_readinto(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer_out, ARG_buffer_in, ARG_out_start, ARG_out_end, ARG_in_start, ARG_in_end };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer_out,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_buffer_in,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_out_start,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_out_end,       MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
        { MP_QSTR_in_start,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_in_end,        MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
    };
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t buf_out_info;
    mp_get_buffer_raise(args[ARG_buffer_out].u_obj, &buf_out_info, MP_BUFFER_READ);
    int32_t out_start = args[ARG_out_start].u_int;
    size_t out_length = buf_out_info.len;
    normalize_buffer_bounds(&out_start, args[ARG_out_end].u_int, &out_length);

    mp_buffer_info_t buf_in_info;
    mp_get_buffer_raise(args[ARG_buffer_in].u_obj, &buf_in_info, MP_BUFFER_WRITE);
    int32_t in_start = args[ARG_in_start].u_int;
    size_t in_length = buf_in_info.len;
    normalize_buffer_bounds(&in_start, args[ARG_in_end].u_int, &in_length);

    if (out_length == 0 && in_length == 0) {
        return mp_const_none;
    }

    int in_stride_in_bytes = mp_binary_get_size('@', buf_in_info.typecode, NULL);
    if (in_stride_in_bytes > 4) {
        mp_raise_ValueError(translate("In-buffer elements must be <= 4 bytes long"));
    }

    int out_stride_in_bytes = mp_binary_get_size('@', buf_out_info.typecode, NULL);
    if (out_stride_in_bytes > 4) {
        mp_raise_ValueError(translate("Out-buffer elements must be <= 4 bytes long"));
    }

    bool ok = common_hal_rp2pio_statemachine_write_readinto(self,
        ((uint8_t *)buf_out_info.buf) + out_start,
        out_length,
        out_stride_in_bytes,
        ((uint8_t *)buf_in_info.buf) + in_start,
        in_length,
        in_stride_in_bytes);
    if (!ok) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(rp2pio_statemachine_write_readinto_obj, 2, rp2pio_statemachine_write_readinto);

//|     def clear_rxfifo(self) -> None:
//|         """Clears any unread bytes in the rxfifo."""
//|         ...
//|
STATIC mp_obj_t rp2pio_statemachine_obj_clear_rxfifo(mp_obj_t self_in) {
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_rp2pio_statemachine_clear_rxfifo(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(rp2pio_statemachine_clear_rxfifo_obj, rp2pio_statemachine_obj_clear_rxfifo);

//|     frequency: int
//|     """The actual state machine frequency. This may not match the frequency requested
//|     due to internal limitations."""
//|

STATIC mp_obj_t rp2pio_statemachine_obj_get_frequency(mp_obj_t self_in) {
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_rp2pio_statemachine_get_frequency(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(rp2pio_statemachine_get_frequency_obj, rp2pio_statemachine_obj_get_frequency);

STATIC mp_obj_t rp2pio_statemachine_obj_set_frequency(mp_obj_t self_in, mp_obj_t frequency) {
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    common_hal_rp2pio_statemachine_set_frequency(self, mp_obj_get_int(frequency));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(rp2pio_statemachine_set_frequency_obj, rp2pio_statemachine_obj_set_frequency);

const mp_obj_property_t rp2pio_statemachine_frequency_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&rp2pio_statemachine_get_frequency_obj,
              (mp_obj_t)&rp2pio_statemachine_set_frequency_obj,
              MP_ROM_NONE},
};

//|     rxstall: bool
//|     """True when the state machine has stalled due to a full RX FIFO since the last
//|        `clear_rxfifo` call."""
//|

STATIC mp_obj_t rp2pio_statemachine_obj_get_rxstall(mp_obj_t self_in) {
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_rp2pio_statemachine_get_rxstall(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(rp2pio_statemachine_get_rxstall_obj, rp2pio_statemachine_obj_get_rxstall);

const mp_obj_property_t rp2pio_statemachine_rxstall_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&rp2pio_statemachine_get_rxstall_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

//|     in_waiting: int
//|     """The number of words available to readinto"""
//|

STATIC mp_obj_t rp2pio_statemachine_obj_get_in_waiting(mp_obj_t self_in) {
    rp2pio_statemachine_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_rp2pio_statemachine_get_in_waiting(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(rp2pio_statemachine_get_in_waiting_obj, rp2pio_statemachine_obj_get_in_waiting);

const mp_obj_property_t rp2pio_statemachine_in_waiting_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&rp2pio_statemachine_get_in_waiting_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};

STATIC const mp_rom_map_elem_t rp2pio_statemachine_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&rp2pio_statemachine_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&rp2pio_statemachine_obj___exit___obj) },

    { MP_ROM_QSTR(MP_QSTR_stop), MP_ROM_PTR(&rp2pio_statemachine_stop_obj) },
    { MP_ROM_QSTR(MP_QSTR_restart), MP_ROM_PTR(&rp2pio_statemachine_restart_obj) },
    { MP_ROM_QSTR(MP_QSTR_run), MP_ROM_PTR(&rp2pio_statemachine_run_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear_rxfifo), MP_ROM_PTR(&rp2pio_statemachine_clear_rxfifo_obj) },

    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&rp2pio_statemachine_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&rp2pio_statemachine_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_readinto), MP_ROM_PTR(&rp2pio_statemachine_write_readinto_obj) },

    { MP_ROM_QSTR(MP_QSTR_frequency), MP_ROM_PTR(&rp2pio_statemachine_frequency_obj) },
    { MP_ROM_QSTR(MP_QSTR_rxstall), MP_ROM_PTR(&rp2pio_statemachine_rxstall_obj) },
    { MP_ROM_QSTR(MP_QSTR_in_waiting), MP_ROM_PTR(&rp2pio_statemachine_in_waiting_obj) },
};
STATIC MP_DEFINE_CONST_DICT(rp2pio_statemachine_locals_dict, rp2pio_statemachine_locals_dict_table);

const mp_obj_type_t rp2pio_statemachine_type = {
    { &mp_type_type },
    .name = MP_QSTR_StateMachine,
    .make_new = rp2pio_statemachine_make_new,
    .locals_dict = (mp_obj_dict_t *)&rp2pio_statemachine_locals_dict,
};

rp2pio_statemachine_obj_t *validate_obj_is_statemachine(mp_obj_t obj) {
    if (!mp_obj_is_type(obj, &rp2pio_statemachine_type)) {
        mp_raise_TypeError_varg(translate("Expected a %q"), rp2pio_statemachine_type.name);
    }
    return MP_OBJ_TO_PTR(obj);
}
