// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2018 Noralf Trønnes
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/i2ctarget/I2CTarget.h"
#include "shared-bindings/time/__init__.h"
#include "shared-bindings/util.h"

#include "shared/runtime/buffer_helper.h"
#include "shared/runtime/context_manager_helpers.h"
#include "shared/runtime/interrupt_char.h"

#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"

static mp_obj_t mp_obj_new_i2ctarget_i2c_target_request(i2ctarget_i2c_target_obj_t *target, uint8_t address, bool is_read, bool is_restart) {
    i2ctarget_i2c_target_request_obj_t *self =
        mp_obj_malloc_with_finaliser(i2ctarget_i2c_target_request_obj_t, &i2ctarget_i2c_target_request_type);
    self->target = target;
    self->address = address;
    self->is_read = is_read;
    self->is_restart = is_restart;

    return MP_OBJ_FROM_PTR(self);
}

//| class I2CTarget:
//|     """Two wire serial protocol target"""
//|
//|     def __init__(
//|         self,
//|         scl: microcontroller.Pin,
//|         sda: microcontroller.Pin,
//|         addresses: Sequence[int],
//|         smbus: bool = False,
//|     ) -> None:
//|         """I2C is a two-wire protocol for communicating between devices.
//|         This implements the target (peripheral, sensor, secondary) side.
//|
//|         :param ~microcontroller.Pin scl: The clock pin
//|         :param ~microcontroller.Pin sda: The data pin
//|         :param addresses: The I2C addresses to respond to (how many is hardware dependent).
//|         :type addresses: list[int]
//|         :param bool smbus: Use SMBUS timings if the hardware supports it"""
//|         ...
//|
static mp_obj_t i2ctarget_i2c_target_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_scl, ARG_sda, ARG_addresses, ARG_smbus };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_scl, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_sda, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_addresses, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_smbus, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mcu_pin_obj_t *scl = validate_obj_is_free_pin(args[ARG_scl].u_obj, MP_QSTR_scl);
    const mcu_pin_obj_t *sda = validate_obj_is_free_pin(args[ARG_sda].u_obj, MP_QSTR_sda);

    mp_obj_iter_buf_t iter_buf;
    mp_obj_t iterable = mp_getiter(args[ARG_addresses].u_obj, &iter_buf);
    mp_obj_t item;
    uint8_t *addresses = NULL;
    unsigned int i = 0;
    while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
        mp_uint_t value = mp_arg_validate_int_range(mp_obj_get_int(item), 0x00, 0x7f, MP_QSTR_address);
        addresses = m_renew(uint8_t, addresses, i, i + 1);
        addresses[i++] = value;
    }
    if (i == 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("addresses is empty"));
    }

    i2ctarget_i2c_target_obj_t *self = mp_obj_malloc_with_finaliser(i2ctarget_i2c_target_obj_t, &i2ctarget_i2c_target_type);
    common_hal_i2ctarget_i2c_target_construct(self, scl, sda, addresses, i, args[ARG_smbus].u_bool);

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Releases control of the underlying hardware so other classes can use it."""
//|         ...
//|
static mp_obj_t i2ctarget_i2c_target_obj_deinit(mp_obj_t self_in) {
    i2ctarget_i2c_target_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_i2ctarget_i2c_target_deinit(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(i2ctarget_i2c_target_deinit_obj, i2ctarget_i2c_target_obj_deinit);

static void check_for_deinit(i2ctarget_i2c_target_obj_t *self) {
    if (common_hal_i2ctarget_i2c_target_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def __enter__(self) -> I2CTarget:
//|         """No-op used in Context Managers."""
//|         ...
//|
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware on context exit. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
//  Provided by context manager helper.

//|     def request(self, *, timeout: float = -1) -> I2CTargetRequest:
//|         """Wait for an I2C request.
//|
//|         :param float timeout: Timeout in seconds. Zero means wait forever, a negative value means check once
//|         :return: I2CTargetRequest or None if timeout=-1 and there's no request
//|         :rtype: ~i2ctarget.I2CTargetRequest"""
//|
//|
static mp_obj_t i2ctarget_i2c_target_request(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    i2ctarget_i2c_target_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);

    enum { ARG_timeout };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_timeout,      MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NEW_SMALL_INT(-1)} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    #if MICROPY_PY_BUILTINS_FLOAT
    float f = mp_obj_get_float(args[ARG_timeout].u_obj) * 1000;
    int timeout_ms = (int)f;
    #else
    int timeout_ms = mp_obj_get_int(args[ARG_timeout].u_obj) * 1000;
    #endif

    bool forever = false;
    uint64_t timeout_end = 0;
    if (timeout_ms == 0) {
        forever = true;
    } else if (timeout_ms > 0) {
        timeout_end = common_hal_time_monotonic_ms() + timeout_ms;
    }

    int last_error = 0;

    do {
        uint8_t address;
        bool is_read;
        bool is_restart;

        RUN_BACKGROUND_TASKS;
        if (mp_hal_is_interrupted()) {
            return mp_const_none;
        }

        int status = common_hal_i2ctarget_i2c_target_is_addressed(self, &address, &is_read, &is_restart);
        if (status < 0) {
            // On error try one more time before bailing out
            if (last_error) {
                mp_raise_OSError(last_error);
            }
            last_error = -status;
            mp_hal_delay_ms(10);
            continue;
        }

        last_error = 0;

        if (status == 0) {
            mp_hal_delay_us(10);
            continue;
        }

        return mp_obj_new_i2ctarget_i2c_target_request(self, address, is_read, is_restart);
    } while (forever || common_hal_time_monotonic_ms() < timeout_end);

    if (timeout_ms > 0) {
        mp_raise_OSError(MP_ETIMEDOUT);
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(i2ctarget_i2c_target_request_obj, 1, i2ctarget_i2c_target_request);

static const mp_rom_map_elem_t i2ctarget_i2c_target_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&i2ctarget_i2c_target_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&i2ctarget_i2c_target_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&default___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_request), MP_ROM_PTR(&i2ctarget_i2c_target_request_obj) },

};

static MP_DEFINE_CONST_DICT(i2ctarget_i2c_target_locals_dict, i2ctarget_i2c_target_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    i2ctarget_i2c_target_type,
    MP_QSTR_I2CTarget,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, i2ctarget_i2c_target_make_new,
    locals_dict, &i2ctarget_i2c_target_locals_dict
    );

//| class I2CTargetRequest:
//|     def __init__(
//|         self, target: i2ctarget.I2CTarget, address: int, is_read: bool, is_restart: bool
//|     ) -> None:
//|         """Information about an I2C transfer request
//|         This cannot be instantiated directly, but is returned by :py:meth:`I2CTarget.request`.
//|
//|         :param target: The I2CTarget object receiving this request
//|         :param address: I2C address
//|         :param is_read: True if the main target is requesting data
//|         :param is_restart: Repeated Start Condition"""
//|
static mp_obj_t i2ctarget_i2c_target_request_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    mp_arg_check_num(n_args, n_kw, 4, 4, false);
    return mp_obj_new_i2ctarget_i2c_target_request(args[0], mp_obj_get_int(args[1]), mp_obj_is_true(args[2]), mp_obj_is_true(args[3]));
}

static void target_request_check_for_deinit(i2ctarget_i2c_target_request_obj_t *self) {
    if (self->target == NULL) {
        raise_deinited_error();
    }
    check_for_deinit(self->target);
}

//|     def deinit(self) -> None:
//|         """Disconnects from parent `I2CTarget`.
//|         Called by `__exit__()` to indicate the `I2CTargetRequest` is no longer useful."""
//|         ...
//|
static mp_obj_t i2ctarget_i2c_target_request_deinit(mp_obj_t self_in) {
    i2ctarget_i2c_target_request_obj_t *self = MP_OBJ_TO_PTR(self_in);
    target_request_check_for_deinit(self);

    i2ctarget_i2c_target_obj_t *target = self->target;

    // Deinit I2CTargetRequest first in case _close() fails.
    self->target = NULL;

    common_hal_i2ctarget_i2c_target_close(target);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(i2ctarget_i2c_target_request_deinit_obj, i2ctarget_i2c_target_request_deinit);

// The finaliser. Unlike deinit(), do nothing if the request or its target is already deinited:
// the target may have been finalised first.
static mp_obj_t i2ctarget_i2c_target_request_del(mp_obj_t self_in) {
    i2ctarget_i2c_target_request_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->target == NULL || common_hal_i2ctarget_i2c_target_deinited(self->target)) {
        self->target = NULL;
        return mp_const_none;
    }
    return i2ctarget_i2c_target_request_deinit(self_in);
}
static MP_DEFINE_CONST_FUN_OBJ_1(i2ctarget_i2c_target_request_del_obj, i2ctarget_i2c_target_request_del);

//|     address: int
//|     """The I2C address of the request."""
static mp_obj_t i2ctarget_i2c_target_request_get_address(mp_obj_t self_in) {
    i2ctarget_i2c_target_request_obj_t *self = MP_OBJ_TO_PTR(self_in);
    target_request_check_for_deinit(self);

    return mp_obj_new_int(self->address);
}
MP_DEFINE_CONST_PROP_GET(i2ctarget_i2c_target_request_address_obj, i2ctarget_i2c_target_request_get_address);

//|     is_read: bool
//|     """The I2C main controller is reading from this target."""
static mp_obj_t i2ctarget_i2c_target_request_get_is_read(mp_obj_t self_in) {
    i2ctarget_i2c_target_request_obj_t *self = MP_OBJ_TO_PTR(self_in);
    target_request_check_for_deinit(self);

    return mp_obj_new_bool(self->is_read);
}
MP_DEFINE_CONST_PROP_GET(i2ctarget_i2c_target_request_is_read_obj, i2ctarget_i2c_target_request_get_is_read);

//|     is_restart: bool
//|     """Is Repeated Start Condition."""
//|
static mp_obj_t i2ctarget_i2c_target_request_get_is_restart(mp_obj_t self_in) {
    i2ctarget_i2c_target_request_obj_t *self = MP_OBJ_TO_PTR(self_in);
    target_request_check_for_deinit(self);

    return mp_obj_new_bool(self->is_restart);
}
MP_DEFINE_CONST_PROP_GET(i2ctarget_i2c_target_request_is_restart_obj, i2ctarget_i2c_target_request_get_is_restart);

//|     def read(self, n: int = -1, ack: bool = True) -> bytearray:
//|         """Read data.
//|         If ack=False, the caller is responsible for calling :py:meth:`I2CTargetRequest.ack`.
//|
//|         :param n: Number of bytes to read (negative means all)
//|         :param ack: Whether or not to send an ACK after the n'th byte
//|         :return: Bytes read"""
//|         ...
//|
static mp_obj_t i2ctarget_i2c_target_request_read(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    i2ctarget_i2c_target_request_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    target_request_check_for_deinit(self);

    enum { ARG_n, ARG_ack };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_n,          MP_ARG_INT, {.u_int = -1} },
        { MP_QSTR_ack,        MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (self->is_read) {
        mp_raise_OSError(MP_EACCES);
    }

    int n = args[ARG_n].u_int;
    if (n == 0) {
        return mp_obj_new_bytearray(0, NULL);
    }
    bool ack = args[ARG_ack].u_bool;

    int i = 0;
    uint8_t *buffer = NULL;
    uint64_t timeout_end = common_hal_time_monotonic_ms() + 10 * 1000;
    while (common_hal_time_monotonic_ms() < timeout_end) {
        RUN_BACKGROUND_TASKS;
        if (mp_hal_is_interrupted()) {
            break;
        }

        uint8_t data;
        int num = common_hal_i2ctarget_i2c_target_read_byte(self->target, &data);
        if (num == 0) {
            break;
        }

        buffer = m_renew(uint8_t, buffer, i, i + 1);
        buffer[i++] = data;
        if (i == n) {
            if (ack) {
                common_hal_i2ctarget_i2c_target_ack(self->target, true);
            }
            break;
        }
        common_hal_i2ctarget_i2c_target_ack(self->target, true);
    }

    return mp_obj_new_bytearray(i, buffer);
}
MP_DEFINE_CONST_FUN_OBJ_KW(i2ctarget_i2c_target_request_read_obj, 1, i2ctarget_i2c_target_request_read);

//|     def write(self, buffer: ReadableBuffer) -> int:
//|         """Write the data contained in buffer.
//|
//|         :param ~circuitpython_typing.ReadableBuffer buffer: Write out the data in this buffer
//|         :return: Number of bytes written"""
//|         ...
//|
static mp_obj_t i2ctarget_i2c_target_request_write(mp_obj_t self_in, mp_obj_t buf_in) {
    i2ctarget_i2c_target_request_obj_t *self = MP_OBJ_TO_PTR(self_in);
    target_request_check_for_deinit(self);

    if (!self->is_read) {
        mp_raise_OSError(MP_EACCES);
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(buf_in, &bufinfo, MP_BUFFER_READ);

    for (size_t i = 0; i < bufinfo.len; i++) {
        RUN_BACKGROUND_TASKS;
        if (mp_hal_is_interrupted()) {
            break;
        }

        int num = common_hal_i2ctarget_i2c_target_write_byte(self->target, ((uint8_t *)(bufinfo.buf))[i]);
        if (num == 0) {
            return mp_obj_new_int(i);
        }
    }

    return mp_obj_new_int(bufinfo.len);
}
static MP_DEFINE_CONST_FUN_OBJ_2(i2ctarget_i2c_target_request_write_obj, i2ctarget_i2c_target_request_write);

//|     def ack(self, ack: bool = True) -> None:
//|         """Acknowledge or Not Acknowledge last byte received.
//|         Use together with :py:meth:`I2CTargetRequest.read` ack=False.
//|
//|         :param ack: Whether to send an ACK or NACK"""
//|         ...
//|
//|
static mp_obj_t i2ctarget_i2c_target_request_ack(uint n_args, const mp_obj_t *args) {
    i2ctarget_i2c_target_request_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    target_request_check_for_deinit(self);

    bool ack = (n_args == 1) ? true : mp_obj_is_true(args[1]);

    if (self->is_read) {
        mp_raise_OSError(MP_EACCES);
    }

    common_hal_i2ctarget_i2c_target_ack(self->target, ack);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(i2ctarget_i2c_target_request_ack_obj, 1, 2, i2ctarget_i2c_target_request_ack);

//|     def __enter__(self) -> I2CTargetRequest:
//|         """No-op used in Context Managers."""
//|         ...
//|
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Close and deinit the request."""
//|         ...
//|
//  Provided by context manager helper.

static const mp_rom_map_elem_t i2ctarget_i2c_target_request_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&default___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&i2ctarget_i2c_target_request_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&i2ctarget_i2c_target_request_del_obj) },
    { MP_ROM_QSTR(MP_QSTR_address), MP_ROM_PTR(&i2ctarget_i2c_target_request_address_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_read), MP_ROM_PTR(&i2ctarget_i2c_target_request_is_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_restart), MP_ROM_PTR(&i2ctarget_i2c_target_request_is_restart_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&i2ctarget_i2c_target_request_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&i2ctarget_i2c_target_request_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_ack), MP_ROM_PTR(&i2ctarget_i2c_target_request_ack_obj) },
};

static MP_DEFINE_CONST_DICT(i2ctarget_i2c_target_request_locals_dict, i2ctarget_i2c_target_request_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    i2ctarget_i2c_target_request_type,
    MP_QSTR_I2CTargetRequest,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, i2ctarget_i2c_target_request_make_new,
    locals_dict, &i2ctarget_i2c_target_request_locals_dict
    );
