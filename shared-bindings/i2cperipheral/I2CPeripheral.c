/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Noralf Trønnes
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

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/i2cperipheral/I2CPeripheral.h"
#include "shared-bindings/time/__init__.h"
#include "shared-bindings/util.h"

#include "lib/utils/buffer_helper.h"
#include "lib/utils/context_manager_helpers.h"
#include "lib/utils/interrupt_char.h"

#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/obj.h"
#include "py/objproperty.h"
#include "py/runtime.h"

STATIC mp_obj_t mp_obj_new_i2cperipheral_i2c_peripheral_request(i2cperipheral_i2c_peripheral_obj_t *peripheral, uint8_t address, bool is_read, bool is_restart) {
    i2cperipheral_i2c_peripheral_request_obj_t *self = m_new_obj(i2cperipheral_i2c_peripheral_request_obj_t);
    self->base.type = &i2cperipheral_i2c_peripheral_request_type;
    self->peripheral = peripheral;
    self->address = address;
    self->is_read = is_read;
    self->is_restart = is_restart;
    return (mp_obj_t)self;
}

//| class I2CPeripheral:
//|     """Two wire serial protocol peripheral"""
//|
//|     def __init__(self, scl: microcontroller.Pin, sda: microcontroller.Pin, addresses: Sequence[int], smbus: bool = False) -> None:
//|         """I2C is a two-wire protocol for communicating between devices.
//|         This implements the peripheral (sensor, secondary) side.
//|
//|         :param ~microcontroller.Pin scl: The clock pin
//|         :param ~microcontroller.Pin sda: The data pin
//|         :param addresses: The I2C addresses to respond to (how many is hw dependent).
//|         :type addresses: list[int]
//|         :param bool smbus: Use SMBUS timings if the hardware supports it"""
//|         ...
//|
STATIC mp_obj_t i2cperipheral_i2c_peripheral_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    i2cperipheral_i2c_peripheral_obj_t *self = m_new_obj(i2cperipheral_i2c_peripheral_obj_t);
    self->base.type = &i2cperipheral_i2c_peripheral_type;
    enum { ARG_scl, ARG_sda, ARG_addresses, ARG_smbus };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_scl, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_sda, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_addresses, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_smbus, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mcu_pin_obj_t *scl = validate_obj_is_free_pin(args[ARG_scl].u_obj);
    const mcu_pin_obj_t *sda = validate_obj_is_free_pin(args[ARG_sda].u_obj);

    mp_obj_iter_buf_t iter_buf;
    mp_obj_t iterable = mp_getiter(args[ARG_addresses].u_obj, &iter_buf);
    mp_obj_t item;
    uint8_t *addresses = NULL;
    unsigned int i = 0;
    while ((item = mp_iternext(iterable)) != MP_OBJ_STOP_ITERATION) {
        mp_int_t value;
        if (!mp_obj_get_int_maybe(item, &value)) {
            mp_raise_TypeError_varg(translate("can't convert %q to %q"), MP_QSTR_address, MP_QSTR_int);
        }
        if (value < 0x00 || value > 0x7f) {
            mp_raise_ValueError(translate("address out of bounds"));
        }
        addresses = m_renew(uint8_t, addresses, i, i + 1);
        addresses[i++] = value;
    }
    if (i == 0) {
        mp_raise_ValueError(translate("addresses is empty"));
    }

    common_hal_i2cperipheral_i2c_peripheral_construct(self, scl, sda, addresses, i, args[ARG_smbus].u_bool);
    return (mp_obj_t)self;
}

//|     def deinit(self) -> None:
//|         """Releases control of the underlying hardware so other classes can use it."""
//|         ...
//|
STATIC mp_obj_t i2cperipheral_i2c_peripheral_obj_deinit(mp_obj_t self_in) {
    mp_check_self(mp_obj_is_type(self_in, &i2cperipheral_i2c_peripheral_type));
    i2cperipheral_i2c_peripheral_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_i2cperipheral_i2c_peripheral_deinit(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(i2cperipheral_i2c_peripheral_deinit_obj, i2cperipheral_i2c_peripheral_obj_deinit);

//|     def __enter__(self) -> I2CPeripheral:
//|         """No-op used in Context Managers."""
//|         ...
//|
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware on context exit. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
STATIC mp_obj_t i2cperipheral_i2c_peripheral_obj___exit__(size_t n_args, const mp_obj_t *args) {
    mp_check_self(mp_obj_is_type(args[0], &i2cperipheral_i2c_peripheral_type));
    i2cperipheral_i2c_peripheral_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    common_hal_i2cperipheral_i2c_peripheral_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(i2cperipheral_i2c_peripheral___exit___obj, 4, 4, i2cperipheral_i2c_peripheral_obj___exit__);

//|     def request(self, timeout: float = -1) -> I2CPeripheralRequest:
//|         """Wait for an I2C request.
//|
//|         :param float timeout: Timeout in seconds. Zero means wait forever, a negative value means check once
//|         :return: I2C Slave Request or None if timeout=-1 and there's no request
//|         :rtype: ~i2cperipheral.I2CPeripheralRequest"""
//|
STATIC mp_obj_t i2cperipheral_i2c_peripheral_request(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_check_self(mp_obj_is_type(pos_args[0], &i2cperipheral_i2c_peripheral_type));
    i2cperipheral_i2c_peripheral_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    if (common_hal_i2cperipheral_i2c_peripheral_deinited(self)) {
        raise_deinited_error();
    }
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

        int status = common_hal_i2cperipheral_i2c_peripheral_is_addressed(self, &address, &is_read, &is_restart);
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

        return mp_obj_new_i2cperipheral_i2c_peripheral_request(self, address, is_read, is_restart);
    } while (forever || common_hal_time_monotonic_ms() < timeout_end);

    if (timeout_ms > 0) {
        mp_raise_OSError(MP_ETIMEDOUT);
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(i2cperipheral_i2c_peripheral_request_obj, 1, i2cperipheral_i2c_peripheral_request);

STATIC const mp_rom_map_elem_t i2cperipheral_i2c_peripheral_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&i2cperipheral_i2c_peripheral_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&i2cperipheral_i2c_peripheral___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_request), MP_ROM_PTR(&i2cperipheral_i2c_peripheral_request_obj) },

};

STATIC MP_DEFINE_CONST_DICT(i2cperipheral_i2c_peripheral_locals_dict, i2cperipheral_i2c_peripheral_locals_dict_table);

const mp_obj_type_t i2cperipheral_i2c_peripheral_type = {
    { &mp_type_type },
    .name = MP_QSTR_I2CPeripheral,
    .make_new = i2cperipheral_i2c_peripheral_make_new,
    .locals_dict = (mp_obj_dict_t *)&i2cperipheral_i2c_peripheral_locals_dict,
};

//| class I2CPeripheralRequest:
//|
//|     def __init__(self, peripheral: i2cperipheral.I2CPeripheral, address: int, is_read: bool, is_restart: bool) -> None:
//|         """Information about an I2C transfer request
//|         This cannot be instantiated directly, but is returned by :py:meth:`I2CPeripheral.request`.
//|
//|         :param peripheral: The I2CPeripheral object receiving this request
//|         :param address: I2C address
//|         :param is_read: True if the main peripheral is requesting data
//|         :param is_restart: Repeated Start Condition"""
//|
STATIC mp_obj_t i2cperipheral_i2c_peripheral_request_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *args, mp_map_t *kw_args) {
    mp_arg_check_num(n_args, kw_args, 4, 4, false);
    return mp_obj_new_i2cperipheral_i2c_peripheral_request(args[0], mp_obj_get_int(args[1]), mp_obj_is_true(args[2]), mp_obj_is_true(args[3]));
}

//|     def __enter__(self) -> I2CPeripheralRequest:
//|         """No-op used in Context Managers."""
//|         ...
//|
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Close the request."""
//|         ...
//|
STATIC mp_obj_t i2cperipheral_i2c_peripheral_request_obj___exit__(size_t n_args, const mp_obj_t *args) {
    mp_check_self(mp_obj_is_type(args[0], &i2cperipheral_i2c_peripheral_request_type));
    i2cperipheral_i2c_peripheral_request_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    common_hal_i2cperipheral_i2c_peripheral_close(self->peripheral);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(i2cperipheral_i2c_peripheral_request___exit___obj, 4, 4, i2cperipheral_i2c_peripheral_request_obj___exit__);

//|     address: int
//|     """The I2C address of the request."""
//|
STATIC mp_obj_t i2cperipheral_i2c_peripheral_request_get_address(mp_obj_t self_in) {
    mp_check_self(mp_obj_is_type(self_in, &i2cperipheral_i2c_peripheral_request_type));
    i2cperipheral_i2c_peripheral_request_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(self->address);
}
MP_DEFINE_CONST_PROP_GET(i2cperipheral_i2c_peripheral_request_address_obj, i2cperipheral_i2c_peripheral_request_get_address);

//|     is_read: bool
//|     """The I2C main controller is reading from this peripheral."""
//|
STATIC mp_obj_t i2cperipheral_i2c_peripheral_request_get_is_read(mp_obj_t self_in) {
    mp_check_self(mp_obj_is_type(self_in, &i2cperipheral_i2c_peripheral_request_type));
    i2cperipheral_i2c_peripheral_request_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(self->is_read);
}
MP_DEFINE_CONST_PROP_GET(i2cperipheral_i2c_peripheral_request_is_read_obj, i2cperipheral_i2c_peripheral_request_get_is_read);

//|     is_restart: bool
//|     """Is Repeated Start Condition."""
//|
STATIC mp_obj_t i2cperipheral_i2c_peripheral_request_get_is_restart(mp_obj_t self_in) {
    mp_check_self(mp_obj_is_type(self_in, &i2cperipheral_i2c_peripheral_request_type));
    i2cperipheral_i2c_peripheral_request_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(self->is_restart);
}
MP_DEFINE_CONST_PROP_GET(i2cperipheral_i2c_peripheral_request_is_restart_obj, i2cperipheral_i2c_peripheral_request_get_is_restart);

//|     def read(self, n: int = -1, ack: bool = True) -> bytearray:
//|         """Read data.
//|         If ack=False, the caller is responsible for calling :py:meth:`I2CPeripheralRequest.ack`.
//|
//|         :param n: Number of bytes to read (negative means all)
//|         :param ack: Whether or not to send an ACK after the n'th byte
//|         :return: Bytes read"""
//|         ...
//|
STATIC mp_obj_t i2cperipheral_i2c_peripheral_request_read(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    mp_check_self(mp_obj_is_type(pos_args[0], &i2cperipheral_i2c_peripheral_request_type));
    i2cperipheral_i2c_peripheral_request_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
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
        int num = common_hal_i2cperipheral_i2c_peripheral_read_byte(self->peripheral, &data);
        if (num == 0) {
            break;
        }

        buffer = m_renew(uint8_t, buffer, i, i + 1);
        buffer[i++] = data;
        if (i == n) {
            if (ack) {
                common_hal_i2cperipheral_i2c_peripheral_ack(self->peripheral, true);
            }
            break;
        }
        common_hal_i2cperipheral_i2c_peripheral_ack(self->peripheral, true);
    }

    return mp_obj_new_bytearray(i, buffer);
}
MP_DEFINE_CONST_FUN_OBJ_KW(i2cperipheral_i2c_peripheral_request_read_obj, 1, i2cperipheral_i2c_peripheral_request_read);

//|     def write(self, buffer: ReadableBuffer) -> int:
//|         """Write the data contained in buffer.
//|
//|         :param ~_typing.ReadableBuffer buffer: Write out the data in this buffer
//|         :return: Number of bytes written"""
//|         ...
//|
STATIC mp_obj_t i2cperipheral_i2c_peripheral_request_write(mp_obj_t self_in, mp_obj_t buf_in) {
    mp_check_self(mp_obj_is_type(self_in, &i2cperipheral_i2c_peripheral_request_type));
    i2cperipheral_i2c_peripheral_request_obj_t *self = MP_OBJ_TO_PTR(self_in);

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

        int num = common_hal_i2cperipheral_i2c_peripheral_write_byte(self->peripheral, ((uint8_t *)(bufinfo.buf))[i]);
        if (num == 0) {
            return mp_obj_new_int(i);
        }
    }

    return mp_obj_new_int(bufinfo.len);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(i2cperipheral_i2c_peripheral_request_write_obj, i2cperipheral_i2c_peripheral_request_write);

//|     def ack(self, ack: bool = True) -> None:
//|         """Acknowledge or Not Acknowledge last byte received.
//|         Use together with :py:meth:`I2CPeripheralRequest.read` ack=False.
//|
//|         :param ack: Whether to send an ACK or NACK"""
//|         ...
//|
STATIC mp_obj_t i2cperipheral_i2c_peripheral_request_ack(uint n_args, const mp_obj_t *args) {
    mp_check_self(mp_obj_is_type(args[0], &i2cperipheral_i2c_peripheral_request_type));
    i2cperipheral_i2c_peripheral_request_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    bool ack = (n_args == 1) ? true : mp_obj_is_true(args[1]);

    if (self->is_read) {
        mp_raise_OSError(MP_EACCES);
    }

    common_hal_i2cperipheral_i2c_peripheral_ack(self->peripheral, ack);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(i2cperipheral_i2c_peripheral_request_ack_obj, 1, 2, i2cperipheral_i2c_peripheral_request_ack);

STATIC mp_obj_t i2cperipheral_i2c_peripheral_request_close(mp_obj_t self_in) {
    mp_check_self(mp_obj_is_type(self_in, &i2cperipheral_i2c_peripheral_request_type));
    i2cperipheral_i2c_peripheral_request_obj_t *self = MP_OBJ_TO_PTR(self_in);

    common_hal_i2cperipheral_i2c_peripheral_close(self->peripheral);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(i2cperipheral_i2c_peripheral_request_close_obj, i2cperipheral_i2c_peripheral_request_close);

STATIC const mp_rom_map_elem_t i2cperipheral_i2c_peripheral_request_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&i2cperipheral_i2c_peripheral_request___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_address), MP_ROM_PTR(&i2cperipheral_i2c_peripheral_request_address_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_read), MP_ROM_PTR(&i2cperipheral_i2c_peripheral_request_is_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_is_restart), MP_ROM_PTR(&i2cperipheral_i2c_peripheral_request_is_restart_obj) },
    { MP_ROM_QSTR(MP_QSTR_read), MP_ROM_PTR(&i2cperipheral_i2c_peripheral_request_read_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&i2cperipheral_i2c_peripheral_request_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_ack), MP_ROM_PTR(&i2cperipheral_i2c_peripheral_request_ack_obj) },
    { MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&i2cperipheral_i2c_peripheral_request_close_obj) },
};

STATIC MP_DEFINE_CONST_DICT(i2cperipheral_i2c_peripheral_request_locals_dict, i2cperipheral_i2c_peripheral_request_locals_dict_table);

const mp_obj_type_t i2cperipheral_i2c_peripheral_request_type = {
    { &mp_type_type },
    .name = MP_QSTR_I2CPeripheralRequest,
    .make_new = i2cperipheral_i2c_peripheral_request_make_new,
    .locals_dict = (mp_obj_dict_t *)&i2cperipheral_i2c_peripheral_request_locals_dict,
};
