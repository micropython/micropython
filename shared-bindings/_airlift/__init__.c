// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Tim Cocks for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "py/obj.h"
#include "py/runtime.h"

#include "shared-bindings/_airlift/__init__.h"

//| """Shared access to an ESP32 co-processor running nina-fw
//|
//| The `_airlift` module lets libraries send commands to the ESP32 that the core
//| also uses, so that user code and the core can use the same co-processor.
//| It is meant for libraries such as ESP32SPI, not for user code.
//| """
//|
//| MAX_SOCKETS: int
//| """The number of sockets in nina-fw. Commands naming a higher socket are rejected."""
//|
//| MAX_TRANSFER: int
//| """The longest command, in bytes, that the ESP32 receives intact."""
//|
//| MAX_READ: int
//| """The largest ``GET_DATABUF_TCP`` read the ESP32 can reply to."""
//|

//| def init() -> None:
//|     """Claim the ESP32's pins and the board SPI bus, and hard-reset the ESP32.
//|     The ESP32 stays claimed across soft reloads. Does nothing if it is already
//|     claimed. Raises `ValueError` if a pin is in use.
//|
//|     Temporary: the core will claim the ESP32 itself when web workflow is configured."""
//|     ...
//|
static mp_obj_t _airlift_init(void) {
    common_hal__airlift_init();
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(_airlift_init_obj, _airlift_init);

//| def active() -> bool:
//|     """True if the core has claimed the ESP32 and `transact` can be used."""
//|     ...
//|
static mp_obj_t _airlift_active(void) {
    return mp_obj_new_bool(common_hal__airlift_active());
}
static MP_DEFINE_CONST_FUN_OBJ_0(_airlift_active_obj, _airlift_active);

//| def reset() -> None:
//|     """Hard-reset the ESP32 through its reset pin. This closes every socket and
//|     disconnects from WiFi. Takes about 0.75 seconds."""
//|     ...
//|
static mp_obj_t _airlift_reset(void) {
    common_hal__airlift_reset();
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(_airlift_reset_obj, _airlift_reset);

//| def release() -> None:
//|     """Give up the ESP32's pins, so that code can drive them directly. Does
//|     nothing if they aren't claimed."""
//|     ...
//|
static mp_obj_t _airlift_release(void) {
    common_hal__airlift_release();
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(_airlift_release_obj, _airlift_release);

//| def transact(
//|     cmd: int,
//|     params: Optional[Sequence[Union[ReadableBuffer, Sequence[int]]]] = None,
//|     *,
//|     reply_params: Optional[int] = 1,
//|     sent_param_len_16: bool = False,
//|     recv_param_len_16: bool = False,
//| ) -> List[bytearray]:
//|     """Send one command and return its reply parameters. This has the same
//|     contract as ESP32SPI's ``ESP_SPIcontrol._send_command_get_response()``,
//|     and raises the same exceptions.
//|
//|     :param int cmd: the nina-fw command
//|     :param params: the parameters, each a buffer or a sequence of byte values
//|     :param int reply_params: how many parameters the reply must have, or ``None`` to accept any number
//|     :param bool sent_param_len_16: send each parameter length as 16 bits
//|     :param bool recv_param_len_16: read each reply parameter length as 16 bits
//|     """
//|     ...
//|
static mp_obj_t _airlift_transact(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_cmd, ARG_params, ARG_reply_params, ARG_sent_param_len_16, ARG_recv_param_len_16 };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_cmd, MP_ARG_REQUIRED | MP_ARG_INT },
        { MP_QSTR_params, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_reply_params, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NEW_SMALL_INT(1)} },
        { MP_QSTR_sent_param_len_16, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
        { MP_QSTR_recv_param_len_16, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint8_t cmd = mp_arg_validate_int_range(args[ARG_cmd].u_int, 0, 255, MP_QSTR_cmd);
    int expected_replies = -1;
    if (args[ARG_reply_params].u_obj != mp_const_none) {
        expected_replies = mp_arg_validate_int_range(mp_obj_get_int(args[ARG_reply_params].u_obj),
            0, 255, MP_QSTR_reply_params);
    }

    size_t n_params = 0;
    mp_obj_t *items = NULL;
    if (args[ARG_params].u_obj != mp_const_none) {
        mp_obj_get_array(args[ARG_params].u_obj, &n_params, &items);
    }
    // ESP32SPI sends at most 5 parameters.
    airlift_param_t params[16];
    mp_arg_validate_length_max(n_params, MP_ARRAY_SIZE(params), MP_QSTR_params);
    // Parameters given as sequences of ints, such as (socket,), are packed here.
    uint8_t packed[64];
    size_t packed_len = 0;
    for (size_t i = 0; i < n_params; i++) {
        mp_buffer_info_t bufinfo;
        if (mp_get_buffer(items[i], &bufinfo, MP_BUFFER_READ)) {
            params[i].buf = bufinfo.buf;
            params[i].len = bufinfo.len;
            continue;
        }
        size_t len;
        mp_obj_t *values;
        mp_obj_get_array(items[i], &len, &values);
        mp_arg_validate_length_max(packed_len + len, sizeof(packed), MP_QSTR_params);
        params[i].buf = packed + packed_len;
        params[i].len = len;
        for (size_t j = 0; j < len; j++) {
            packed[packed_len++] = mp_arg_validate_int_range(mp_obj_get_int(values[j]), 0, 255, MP_QSTR_params);
        }
    }

    return common_hal__airlift_transact(cmd, params, n_params, args[ARG_sent_param_len_16].u_bool,
        expected_replies, args[ARG_recv_param_len_16].u_bool);
}
static MP_DEFINE_CONST_FUN_OBJ_KW(_airlift_transact_obj, 1, _airlift_transact);

static const mp_rom_map_elem_t _airlift_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR__airlift) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&_airlift_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&_airlift_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&_airlift_reset_obj) },
    { MP_ROM_QSTR(MP_QSTR_release), MP_ROM_PTR(&_airlift_release_obj) },
    { MP_ROM_QSTR(MP_QSTR_transact), MP_ROM_PTR(&_airlift_transact_obj) },
    { MP_ROM_QSTR(MP_QSTR_MAX_SOCKETS), MP_ROM_INT(CIRCUITPY_AIRLIFT_MAX_SOCKETS) },
    { MP_ROM_QSTR(MP_QSTR_MAX_TRANSFER), MP_ROM_INT(AIRLIFT_MAX_TRANSFER) },
    { MP_ROM_QSTR(MP_QSTR_MAX_READ), MP_ROM_INT(AIRLIFT_MAX_READ) },
};

static MP_DEFINE_CONST_DICT(_airlift_module_globals, _airlift_module_globals_table);

const mp_obj_module_t _airlift_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&_airlift_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR__airlift, _airlift_module);
