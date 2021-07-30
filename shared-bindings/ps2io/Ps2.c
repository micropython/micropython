/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
 * Copyright (c) 2019 Elvis Pfutzenreuter <epxx@epxx.co>
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

#include "lib/utils/context_manager_helpers.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/runtime0.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/ps2io/Ps2.h"
#include "shared-bindings/util.h"
#include "supervisor/shared/translate.h"

//| class Ps2:
//|     """Communicate with a PS/2 keyboard or mouse
//|
//|     Ps2 implements the PS/2 keyboard/mouse serial protocol, used in
//|     legacy devices. It is similar to UART but there are only two
//|     lines (Data and Clock). PS/2 devices are 5V, so bidirectional
//|     level converters must be used to connect the I/O lines to pins
//|     of 3.3V boards."""
//|
//|     def __init__(self, data_pin: microcontroller.Pin, clock_pin: microcontroller.Pin) -> None:
//|         """Create a Ps2 object associated with the given pins.
//|
//|         :param ~microcontroller.Pin data_pin: Pin tied to data wire.
//|         :param ~microcontroller.Pin clock_pin: Pin tied to clock wire.
//|           This pin must support interrupts.
//|
//|         Read one byte from PS/2 keyboard and turn on Scroll Lock LED::
//|
//|           import ps2io
//|           import board
//|
//|           kbd = ps2io.Ps2(board.D10, board.D11)
//|
//|           while len(kbd) == 0:
//|               pass
//|
//|           print(kbd.popleft())
//|           print(kbd.sendcmd(0xed))
//|           print(kbd.sendcmd(0x01))"""
//|         ...
//|
STATIC mp_obj_t ps2io_ps2_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_datapin, ARG_clkpin };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_datapin, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_clkpin, MP_ARG_REQUIRED | MP_ARG_OBJ },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mcu_pin_obj_t *clkpin = validate_obj_is_free_pin(args[ARG_clkpin].u_obj);
    const mcu_pin_obj_t *datapin = validate_obj_is_free_pin(args[ARG_datapin].u_obj);

    ps2io_ps2_obj_t *self = m_new_obj(ps2io_ps2_obj_t);
    self->base.type = &ps2io_ps2_type;

    common_hal_ps2io_ps2_construct(self, datapin, clkpin);

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Deinitialises the Ps2 and releases any hardware resources for reuse."""
//|         ...
//|
STATIC mp_obj_t ps2io_ps2_deinit(mp_obj_t self_in) {
    ps2io_ps2_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_ps2io_ps2_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(ps2io_ps2_deinit_obj, ps2io_ps2_deinit);

STATIC void check_for_deinit(ps2io_ps2_obj_t *self) {
    if (common_hal_ps2io_ps2_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def __enter__(self) -> Ps2:
//|         """No-op used by Context Managers."""
//|         ...
//|
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
STATIC mp_obj_t ps2io_ps2_obj___exit__(size_t n_args, const mp_obj_t *args) {
    mp_check_self(mp_obj_is_type(args[0], &ps2io_ps2_type));
    ps2io_ps2_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    common_hal_ps2io_ps2_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(ps2io_ps2___exit___obj, 4, 4, ps2io_ps2_obj___exit__);

//|     def popleft(self) -> int:
//|         """Removes and returns the oldest received byte. When buffer
//|         is empty, raises an IndexError exception."""
//|         ...
//|
STATIC mp_obj_t ps2io_ps2_obj_popleft(mp_obj_t self_in) {
    ps2io_ps2_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    int b = common_hal_ps2io_ps2_popleft(self);
    if (b < 0) {
        mp_raise_IndexError_varg(translate("pop from empty %q"), MP_QSTR_Ps2_space_buffer);
    }
    return MP_OBJ_NEW_SMALL_INT(b);
}
MP_DEFINE_CONST_FUN_OBJ_1(ps2io_ps2_popleft_obj, ps2io_ps2_obj_popleft);

//|     def sendcmd(self, byte: int) -> int:
//|         """Sends a command byte to PS/2. Returns the response byte, typically
//|         the general ack value (0xFA). Some commands return additional data
//|         which is available through :py:func:`popleft()`.
//|
//|         Raises a RuntimeError in case of failure. The root cause can be found
//|         by calling :py:func:`clear_errors()`. It is advisable to call
//|         :py:func:`clear_errors()` before :py:func:`sendcmd()` to flush any
//|         previous errors.
//|
//|         :param int byte: byte value of the command"""
//|         ...
//|
STATIC mp_obj_t ps2io_ps2_obj_sendcmd(mp_obj_t self_in, mp_obj_t ob) {
    ps2io_ps2_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    mp_int_t cmd = mp_obj_get_int(ob) & 0xff;
    int resp = common_hal_ps2io_ps2_sendcmd(self, cmd);
    if (resp < 0) {
        mp_raise_RuntimeError(translate("Failed sending command."));
    }
    return MP_OBJ_NEW_SMALL_INT(resp);
}
MP_DEFINE_CONST_FUN_OBJ_2(ps2io_ps2_sendcmd_obj, ps2io_ps2_obj_sendcmd);

//|     def clear_errors(self) -> None:
//|         """Returns and clears a bitmap with latest recorded communication errors.
//|
//|         Reception errors (arise asynchronously, as data is received):
//|
//|         0x01: start bit not 0
//|
//|         0x02: timeout
//|
//|         0x04: parity bit error
//|
//|         0x08: stop bit not 1
//|
//|         0x10: buffer overflow, newest data discarded
//|
//|         Transmission errors (can only arise in the course of sendcmd()):
//|
//|         0x100: clock pin didn't go to LO in time
//|
//|         0x200: clock pin didn't go to HI in time
//|
//|         0x400: data pin didn't ACK
//|
//|         0x800: clock pin didn't ACK
//|
//|         0x1000: device didn't respond to RTS
//|
//|         0x2000: device didn't send a response byte in time"""
//|         ...
//|
STATIC mp_obj_t ps2io_ps2_obj_clear_errors(mp_obj_t self_in) {
    ps2io_ps2_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    return MP_OBJ_NEW_SMALL_INT(common_hal_ps2io_ps2_clear_errors(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(ps2io_ps2_clear_errors_obj, ps2io_ps2_obj_clear_errors);

//|     def __bool__(self) -> bool: ...
//|
//|     def __len__(self) -> int:
//|         """Returns the number of received bytes in buffer, available
//|         to :py:func:`popleft()`."""
//|         ...
//|
STATIC mp_obj_t ps2_unary_op(mp_unary_op_t op, mp_obj_t self_in) {
    ps2io_ps2_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    uint16_t len = common_hal_ps2io_ps2_get_len(self);
    switch (op) {
        case MP_UNARY_OP_BOOL:
            return mp_obj_new_bool(len != 0);
        case MP_UNARY_OP_LEN:
            return MP_OBJ_NEW_SMALL_INT(len);
        default:
            return MP_OBJ_NULL;      // op not supported
    }
}

STATIC const mp_rom_map_elem_t ps2io_ps2_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&ps2io_ps2_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&ps2io_ps2___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_popleft), MP_ROM_PTR(&ps2io_ps2_popleft_obj) },
    { MP_ROM_QSTR(MP_QSTR_sendcmd), MP_ROM_PTR(&ps2io_ps2_sendcmd_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear_errors), MP_ROM_PTR(&ps2io_ps2_clear_errors_obj) },
};
STATIC MP_DEFINE_CONST_DICT(ps2io_ps2_locals_dict, ps2io_ps2_locals_dict_table);

const mp_obj_type_t ps2io_ps2_type = {
    { &mp_type_type },
    .name = MP_QSTR_Ps2,
    .flags = MP_TYPE_FLAG_EXTENDED,
    .make_new = ps2io_ps2_make_new,
    MP_TYPE_EXTENDED_FIELDS(
        .unary_op = ps2_unary_op,
        ),
    .locals_dict = (mp_obj_dict_t *)&ps2io_ps2_locals_dict,
};
