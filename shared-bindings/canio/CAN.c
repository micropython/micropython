/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Jeff Epler for Adafruit Industries
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

#include "py/enum.h"
#include "common-hal/canio/CAN.h"
#include "common-hal/canio/Listener.h"
#include "shared-bindings/canio/__init__.h"
#include "shared-bindings/canio/CAN.h"
#include "shared-bindings/canio/Listener.h"
#include "shared-bindings/canio/Match.h"
#include "shared-bindings/canio/Message.h"
#include "shared-bindings/microcontroller/Pin.h"

#include "py/objproperty.h"
#include "py/runtime.h"

//|
//| class CAN:
//|     """CAN bus protocol"""
//|
//|     def __init__(self,
//|             tx: microcontroller.Pin,
//|             rx: microcontroller.Pin,
//|             *,
//|             baudrate: int = 250000,
//|             loopback: bool = False,
//|             silent: bool = False,
//|             auto_restart: bool = False,
//|         ) -> None:
//|         """A common shared-bus protocol.  The rx and tx pins are generally
//|         connected to a transceiver which controls the H and L pins on a
//|         shared bus.
//|
//|         :param ~microcontroller.Pin rx: the pin to receive with
//|         :param ~microcontroller.Pin tx: the pin to transmit with
//|         :param int baudrate: The bit rate of the bus in Hz.  All devices on the bus must agree on this value.
//|         :param bool loopback: When True  the ``rx`` pin's value is ignored, and the device receives the packets it sends.
//|         :param bool silent: When True the ``tx`` pin is always driven to the high logic level.  This mode can be used to "sniff" a CAN bus without interfering.
//|         :param bool auto_restart: If True, will restart communications after entering bus-off state
//|         """
//|         ...
//|
STATIC mp_obj_t canio_can_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_tx, ARG_rx, ARG_baudrate, ARG_loopback, ARG_silent, ARG_auto_restart, NUM_ARGS };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_tx, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_rx, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_baudrate, MP_ARG_INT, {.u_int = 250000} },
        { MP_QSTR_loopback, MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_silent, MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_auto_restart, MP_ARG_BOOL, {.u_bool = false} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    MP_STATIC_ASSERT(MP_ARRAY_SIZE(allowed_args) == NUM_ARGS);

    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mcu_pin_obj_t *rx_pin = validate_obj_is_free_pin_or_none(args[ARG_rx].u_obj);
    mcu_pin_obj_t *tx_pin = validate_obj_is_free_pin_or_none(args[ARG_tx].u_obj);
    if (!rx_pin && !tx_pin) {
        mp_raise_ValueError(translate("tx and rx cannot both be None"));
    }

    canio_can_obj_t *self = m_new_obj(canio_can_obj_t);
    self->base.type = &canio_can_type;
    common_hal_canio_can_construct(self, tx_pin, rx_pin, args[ARG_baudrate].u_int, args[ARG_loopback].u_bool, args[ARG_silent].u_bool);

    common_hal_canio_can_auto_restart_set(self, args[ARG_auto_restart].u_bool);

    return MP_OBJ_FROM_PTR(self);
}


//|     auto_restart: bool
//|     """If True, will restart communications after entering bus-off state"""
//|
STATIC mp_obj_t canio_can_auto_restart_get(mp_obj_t self_in) {
    canio_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_canio_can_check_for_deinit(self);
    return mp_obj_new_bool(common_hal_canio_can_auto_restart_get(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(canio_can_auto_restart_get_obj, canio_can_auto_restart_get);

STATIC mp_obj_t canio_can_auto_restart_set(mp_obj_t self_in, mp_obj_t flag_in) {
    canio_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_canio_can_check_for_deinit(self);
    common_hal_canio_can_auto_restart_set(self, mp_obj_is_true(flag_in));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(canio_can_auto_restart_set_obj, canio_can_auto_restart_set);

STATIC const mp_obj_property_t canio_can_auto_restart_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&canio_can_auto_restart_get_obj,
              (mp_obj_t)&canio_can_auto_restart_set_obj,
              (mp_obj_t)mp_const_none},
};


//|     baudrate: int
//|     """The baud rate (read-only)"""
//|
STATIC mp_obj_t canio_can_baudrate_get(mp_obj_t self_in) {
    canio_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_canio_can_check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_canio_can_baudrate_get(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(canio_can_baudrate_get_obj, canio_can_baudrate_get);

STATIC const mp_obj_property_t canio_can_baudrate_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&canio_can_baudrate_get_obj,
              (mp_obj_t)mp_const_none,
              (mp_obj_t)mp_const_none},
};

//|     transmit_error_count: int
//|     """The number of transmit errors (read-only).  Increased for a detected transmission error, decreased for successful transmission.  Limited to the range from 0 to 255 inclusive.  Also called TEC."""
//|
STATIC mp_obj_t canio_can_transmit_error_count_get(mp_obj_t self_in) {
    canio_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_canio_can_check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_canio_can_transmit_error_count_get(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(canio_can_transmit_error_count_get_obj, canio_can_transmit_error_count_get);

STATIC const mp_obj_property_t canio_can_transmit_error_count_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&canio_can_transmit_error_count_get_obj,
              (mp_obj_t)mp_const_none,
              (mp_obj_t)mp_const_none},
};

//|     receive_error_count: int
//|     """The number of receive errors (read-only).  Increased for a detected reception error, decreased for successful reception.  Limited to the range from 0 to 255 inclusive.  Also called REC."""
//|
STATIC mp_obj_t canio_can_receive_error_count_get(mp_obj_t self_in) {
    canio_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_canio_can_check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_canio_can_receive_error_count_get(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(canio_can_receive_error_count_get_obj, canio_can_receive_error_count_get);

STATIC const mp_obj_property_t canio_can_receive_error_count_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&canio_can_receive_error_count_get_obj,
              (mp_obj_t)mp_const_none,
              (mp_obj_t)mp_const_none},
};

//|     state: BusState
//|     """The current state of the bus. (read-only)"""
STATIC mp_obj_t canio_can_state_get(mp_obj_t self_in) {
    canio_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_canio_can_check_for_deinit(self);
    return cp_enum_find(&canio_bus_state_type, common_hal_canio_can_state_get(self));
}

MP_DEFINE_CONST_FUN_OBJ_1(canio_can_state_get_obj, canio_can_state_get);

STATIC const mp_obj_property_t canio_can_state_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&canio_can_state_get_obj,
              (mp_obj_t)mp_const_none,
              (mp_obj_t)mp_const_none},
};


//|     def restart(self) -> None:
//|         """If the device is in the bus off state, restart it."""
//|         ...
//|
STATIC mp_obj_t canio_can_restart(mp_obj_t self_in) {
    canio_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_canio_can_check_for_deinit(self);
    common_hal_canio_can_restart(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(canio_can_restart_obj, canio_can_restart);

//|     def listen(self, matches: Optional[Sequence[Match]]=None, *, timeout: float=10) -> Listener:
//|         """Start receiving messages that match any one of the filters.
//|
//|         Creating a listener is an expensive operation and can interfere with reception of messages by other listeners.
//|
//|         There is an implementation-defined maximum number of listeners and limit to the complexity of the filters.
//|
//|         If the hardware cannot support all the requested matches, a ValueError is raised.  Note that generally there are some number of hardware filters shared among all fifos.
//|
//|         A message can be received by at most one Listener.  If more than one listener matches a message, it is undefined which one actually receives it.
//|
//|         An empty filter list causes all messages to be accepted.
//|
//|         Timeout dictates how long receive() and next() will block.
//|
//|         Platform specific notes:
//|
//|         SAM E5x supports two Listeners.  Filter blocks are shared between the two
//|         listeners.  There are 4 standard filter blocks and 4 extended filter blocks.
//|         Each block can either match 2 single addresses or a mask of addresses.
//|         The number of filter blocks can be increased, up to a hardware maximum, by
//|         rebuilding CircuitPython, but this decreases the CircuitPython free
//|         memory even if canio is not used.
//|
//|         STM32F405 supports two Listeners.  Filter blocks are shared between the two listeners.
//|         There are 14 filter blocks.  Each block can match 2 standard addresses with
//|         mask or 1 extended address with mask.
//|
//|         ESP32S2 supports one Listener.  There is a single filter block, which can either match a
//|         standard address with mask or an extended address with mask.
//|         """
//|         ...
//|
STATIC mp_obj_t canio_can_listen(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    canio_can_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    common_hal_canio_can_check_for_deinit(self);

    enum { ARG_matches, ARG_timeout, NUM_ARGS };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_matches, MP_ARG_OBJ, {.u_obj = 0} },
        { MP_QSTR_timeout, MP_ARG_OBJ, {.u_obj = 0} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    MP_STATIC_ASSERT(MP_ARRAY_SIZE(allowed_args) == NUM_ARGS);

    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    size_t nmatch = 0;
    mp_obj_t *match_objects = NULL;

    if (args[ARG_matches].u_obj) {
        mp_obj_get_array(args[ARG_matches].u_obj, &nmatch, &match_objects);
    }

    canio_match_obj_t *matches[nmatch];
    for (size_t i = 0; i < nmatch; i++) {
        const mp_obj_type_t *type = mp_obj_get_type(match_objects[i]);
        if (type != &canio_match_type) {
            mp_raise_TypeError_varg(translate("expected '%q' but got '%q'"), MP_QSTR_Match, type->name);
        }
        matches[i] = MP_OBJ_TO_PTR(match_objects[i]);
    }

    float timeout = args[ARG_timeout].u_obj ? mp_obj_get_float(args[ARG_timeout].u_obj) : 10.0f;
    canio_listener_obj_t *listener = m_new_obj(canio_listener_obj_t);
    listener->base.type = &canio_listener_type;
    common_hal_canio_listener_construct(listener, self, nmatch, matches, timeout);
    return listener;
}
MP_DEFINE_CONST_FUN_OBJ_KW(canio_can_listen_obj, 1, canio_can_listen);

//|     loopback: bool
//|     """True if the device was created in loopback mode, False
//|     otherwise (read-only)"""
//|
STATIC mp_obj_t canio_can_loopback_get(mp_obj_t self_in) {
    canio_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_canio_can_check_for_deinit(self);
    return mp_obj_new_bool(common_hal_canio_can_loopback_get(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(canio_can_loopback_get_obj, canio_can_loopback_get);

STATIC const mp_obj_property_t canio_can_loopback_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&canio_can_loopback_get_obj,
              (mp_obj_t)mp_const_none,
              (mp_obj_t)mp_const_none},
};


//|     def send(self, message: Union[RemoteTransmissionRequest, Message]) -> None:
//|         """Send a message on the bus with the given data and id.
//|         If the message could not be sent due to a full fifo or a bus error condition, RuntimeError is raised.
//|         """
//|         ...
//|
STATIC mp_obj_t canio_can_send(mp_obj_t self_in, mp_obj_t message_in) {
    canio_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_canio_can_check_for_deinit(self);
    const mp_obj_type_t *message_type = mp_obj_get_type(message_in);
    if (message_type != &canio_message_type && message_type != &canio_remote_transmission_request_type) {
        mp_raise_TypeError_varg(translate("expected '%q' or '%q' but got '%q'"), MP_QSTR_Message, MP_QSTR_RemoteTransmissionRequest, message_type->name);
    }

    canio_message_obj_t *message = message_in;
    common_hal_canio_can_send(self, message);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(canio_can_send_obj, canio_can_send);

//|     silent: bool
//|     """True if the device was created in silent mode, False
//|     otherwise (read-only)"""
//|
STATIC mp_obj_t canio_can_silent_get(mp_obj_t self_in) {
    canio_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_canio_can_check_for_deinit(self);
    return mp_obj_new_bool(common_hal_canio_can_silent_get(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(canio_can_silent_get_obj, canio_can_silent_get);

STATIC const mp_obj_property_t canio_can_silent_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&canio_can_silent_get_obj,
              (mp_obj_t)mp_const_none,
              (mp_obj_t)mp_const_none},
};


//|     def deinit(self) -> None:
//|         """Deinitialize this object, freeing its hardware resources"""
//|         ...
//|
STATIC mp_obj_t canio_can_deinit(mp_obj_t self_in) {
    canio_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_canio_can_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(canio_can_deinit_obj, canio_can_deinit);

//|     def __enter__(self) -> CAN:
//|         """Returns self, to allow the object to be used in a `with` statement for resource control"""
//|         ...
//|
STATIC mp_obj_t canio_can_enter(mp_obj_t self_in) {
    canio_can_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_canio_can_check_for_deinit(self);
    return self_in;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(canio_can_enter_obj, canio_can_enter);

//|     def __exit__(self, unused1: Optional[Type[BaseException]], unused2: Optional[BaseException], unused3: Optional[TracebackType]) -> None:
//|         """Calls deinit()"""
//|         ...
STATIC mp_obj_t canio_can_exit(size_t num_args, const mp_obj_t args[]) {
    canio_can_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    common_hal_canio_can_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(canio_can_exit_obj, 4, 4, canio_can_exit);

STATIC const mp_rom_map_elem_t canio_can_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&canio_can_enter_obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&canio_can_exit_obj) },
    { MP_ROM_QSTR(MP_QSTR_auto_restart), MP_ROM_PTR(&canio_can_auto_restart_obj) },
    { MP_ROM_QSTR(MP_QSTR_baudrate), MP_ROM_PTR(&canio_can_baudrate_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&canio_can_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_listen), MP_ROM_PTR(&canio_can_listen_obj) },
    { MP_ROM_QSTR(MP_QSTR_loopback), MP_ROM_PTR(&canio_can_loopback_obj) },
    { MP_ROM_QSTR(MP_QSTR_receive_error_count), MP_ROM_PTR(&canio_can_receive_error_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_restart), MP_ROM_PTR(&canio_can_restart_obj) },
    { MP_ROM_QSTR(MP_QSTR_send), MP_ROM_PTR(&canio_can_send_obj) },
    { MP_ROM_QSTR(MP_QSTR_silent), MP_ROM_PTR(&canio_can_silent_obj) },
    { MP_ROM_QSTR(MP_QSTR_state), MP_ROM_PTR(&canio_can_state_obj) },
    { MP_ROM_QSTR(MP_QSTR_transmit_error_count), MP_ROM_PTR(&canio_can_transmit_error_count_obj) },
};
STATIC MP_DEFINE_CONST_DICT(canio_can_locals_dict, canio_can_locals_dict_table);

const mp_obj_type_t canio_can_type = {
    { &mp_type_type },
    .name = MP_QSTR_CAN,
    .make_new = canio_can_make_new,
    .locals_dict = (mp_obj_t)&canio_can_locals_dict,
};
