/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
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

#include "shared-bindings/busio/UART.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"

#include "shared/runtime/context_manager_helpers.h"
#include "shared/runtime/interrupt_char.h"

#include "py/stream.h"
#include "py/objproperty.h"
#include "py/objtype.h"
#include "py/runtime.h"
#include "py/stream.h"

#define STREAM_DEBUG(...) (void)0
// #define STREAM_DEBUG(...) mp_printf(&mp_plat_print __VA_OPT__(,) __VA_ARGS__)

//| class UART:
//|     """A bidirectional serial protocol
//|
//|     .. raw:: html
//|
//|         <p>
//|         <details>
//|         <summary>Available on these boards</summary>
//|         <ul>
//|         {% for board in support_matrix_reverse["busio.UART"] %}
//|         <li> {{ board }}
//|         {% endfor %}
//|         </ul>
//|         </details>
//|         </p>
//|
//|     """
//|
//|     def __init__(
//|         self,
//|         tx: Optional[microcontroller.Pin] = None,
//|         rx: Optional[microcontroller.Pin] = None,
//|         *,
//|         rts: Optional[microcontroller.Pin] = None,
//|         cts: Optional[microcontroller.Pin] = None,
//|         rs485_dir: Optional[microcontroller.Pin] = None,
//|         rs485_invert: bool = False,
//|         baudrate: int = 9600,
//|         bits: int = 8,
//|         parity: Optional[Parity] = None,
//|         stop: int = 1,
//|         timeout: float = 1,
//|         receiver_buffer_size: int = 64
//|     ) -> None:
//|         """A common bidirectional serial protocol that uses an an agreed upon speed
//|         rather than a shared clock line.
//|
//|         :param ~microcontroller.Pin tx: the pin to transmit with, or ``None`` if this ``UART`` is receive-only.
//|         :param ~microcontroller.Pin rx: the pin to receive on, or ``None`` if this ``UART`` is transmit-only.
//|         :param ~microcontroller.Pin rts: the pin for rts, or ``None`` if rts not in use.
//|         :param ~microcontroller.Pin cts: the pin for cts, or ``None`` if cts not in use.
//|         :param ~microcontroller.Pin rs485_dir: the output pin for rs485 direction setting, or ``None`` if rs485 not in use.
//|         :param bool rs485_invert: rs485_dir pin active high when set. Active low otherwise.
//|         :param int baudrate: the transmit and receive speed.
//|         :param int bits:  the number of bits per byte, 5 to 9.
//|         :param Parity parity:  the parity used for error checking.
//|         :param int stop:  the number of stop bits, 1 or 2.
//|         :param float timeout:  the timeout in seconds to wait for the first character and between subsequent characters when reading. Raises ``ValueError`` if timeout >100 seconds.
//|         :param int receiver_buffer_size: the character length of the read buffer (0 to disable). (When a character is 9 bits the buffer will be 2 * receiver_buffer_size bytes.)
//|
//|         ``tx`` and ``rx`` cannot both be ``None``.
//|
//|         *New in CircuitPython 4.0:* ``timeout`` has incompatibly changed units from milliseconds to seconds.
//|         The new upper limit on ``timeout`` is meant to catch mistaken use of milliseconds.
//|
//|         **Limitations:** RS485 is not supported on SAMD, nRF, Broadcom, Spresense, or STM.
//|         On i.MX and Raspberry Pi RP2040, RS485 support is implemented in software:
//|         The timing for the ``rs485_dir`` pin signal is done on a best-effort basis, and may not meet
//|         RS485 specifications intermittently.
//|         """
//|         ...
typedef struct {
    mp_obj_base_t base;
} busio_uart_parity_obj_t;
extern const busio_uart_parity_obj_t busio_uart_parity_even_obj;
extern const busio_uart_parity_obj_t busio_uart_parity_odd_obj;

#if CIRCUITPY_BUSIO_UART
STATIC void validate_timeout(mp_float_t timeout) {
    mp_arg_validate_int_range((int)timeout, 0, 100, MP_QSTR_timeout);
}
#endif  // CIRCUITPY_BUSIO_UART

STATIC mp_obj_t busio_uart_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    #if CIRCUITPY_BUSIO_UART
    enum { ARG_tx, ARG_rx, ARG_baudrate, ARG_bits, ARG_parity, ARG_stop, ARG_timeout, ARG_receiver_buffer_size,
           ARG_rts, ARG_cts, ARG_rs485_dir, ARG_rs485_invert};
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_tx, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_rx, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_baudrate, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 9600} },
        { MP_QSTR_bits, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} },
        { MP_QSTR_parity, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_stop, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 1} },
        { MP_QSTR_timeout, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NEW_SMALL_INT(1)} },
        { MP_QSTR_receiver_buffer_size, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 64} },
        { MP_QSTR_rts, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_cts, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_rs485_dir, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none } },
        { MP_QSTR_rs485_invert, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = false } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mcu_pin_obj_t *rx = validate_obj_is_free_pin_or_none(args[ARG_rx].u_obj, MP_QSTR_rx);
    const mcu_pin_obj_t *tx = validate_obj_is_free_pin_or_none(args[ARG_tx].u_obj, MP_QSTR_tx);
    const mcu_pin_obj_t *rts = validate_obj_is_free_pin_or_none(args[ARG_rts].u_obj, MP_QSTR_rts);
    const mcu_pin_obj_t *cts = validate_obj_is_free_pin_or_none(args[ARG_cts].u_obj, MP_QSTR_cts);
    const mcu_pin_obj_t *rs485_dir = validate_obj_is_free_pin_or_none(args[ARG_rs485_dir].u_obj, MP_QSTR_rs485_dir);
    if ((tx == NULL) && (rx == NULL)) {
        mp_raise_ValueError(translate("tx and rx cannot both be None"));
    }

    // Pins must be distinct.
    if ((tx != NULL && (tx == rx || tx == rts || tx == cts || tx == rs485_dir)) ||
        (rx != NULL && (rx == rts || rx == cts || rx == rs485_dir)) ||
        (rts != NULL && (rts == cts || rts == rs485_dir)) ||
        (cts != NULL && (cts == rs485_dir))) {
        raise_ValueError_invalid_pins();
    }

    uint8_t bits = (uint8_t)mp_arg_validate_int_range(args[ARG_bits].u_int, 5, 9, MP_QSTR_bits);

    busio_uart_parity_t parity = BUSIO_UART_PARITY_NONE;
    if (args[ARG_parity].u_obj == MP_OBJ_FROM_PTR(&busio_uart_parity_even_obj)) {
        parity = BUSIO_UART_PARITY_EVEN;
    } else if (args[ARG_parity].u_obj == MP_OBJ_FROM_PTR(&busio_uart_parity_odd_obj)) {
        parity = BUSIO_UART_PARITY_ODD;
    }

    uint8_t stop = (uint8_t)mp_arg_validate_int_range(args[ARG_stop].u_int, 1, 2, MP_QSTR_stop);

    mp_float_t timeout = mp_obj_get_float(args[ARG_timeout].u_obj);
    validate_timeout(timeout);

    const bool rs485_invert = args[ARG_rs485_invert].u_bool;

    busio_uart_obj_t *self = m_new_obj_with_finaliser(busio_uart_obj_t);
    self->base.type = &busio_uart_type;

    common_hal_busio_uart_construct(self, tx, rx, rts, cts, rs485_dir, rs485_invert,
        args[ARG_baudrate].u_int, bits, parity, stop, timeout,
        args[ARG_receiver_buffer_size].u_int, NULL, false);
    return (mp_obj_t)self;
    #else
    mp_raise_NotImplementedError(NULL);
    #endif  // CIRCUITPY_BUSIO_UART
}

#if CIRCUITPY_BUSIO_UART

// Helper to ensure we have the native super class instead of a subclass.
STATIC busio_uart_obj_t *native_uart(mp_obj_t uart_obj) {
    mp_obj_t native_uart = mp_obj_cast_to_native_base(uart_obj, MP_OBJ_FROM_PTR(&busio_uart_type));
    if (native_uart == MP_OBJ_NULL) {
        mp_raise_ValueError_varg(translate("Must be a %q subclass."), MP_QSTR_UART);
    }
    mp_obj_assert_native_inited(native_uart);
    return MP_OBJ_TO_PTR(native_uart);
}


//|     def deinit(self) -> None:
//|         """Deinitialises the UART and releases any hardware resources for reuse."""
//|         ...
STATIC mp_obj_t busio_uart_obj_deinit(mp_obj_t self_in) {
    busio_uart_obj_t *self = native_uart(self_in);
    common_hal_busio_uart_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(busio_uart_deinit_obj, busio_uart_obj_deinit);

STATIC void check_for_deinit(busio_uart_obj_t *self) {
    if (common_hal_busio_uart_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def __enter__(self) -> UART:
//|         """No-op used by Context Managers."""
//|         ...
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
STATIC mp_obj_t busio_uart_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_busio_uart_deinit(MP_OBJ_TO_PTR(args[0]));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(busio_uart___exit___obj, 4, 4, busio_uart_obj___exit__);

// These are standard stream methods. Code is in py/stream.c.
//
//|     def read(self, nbytes: Optional[int] = None) -> Optional[bytes]:
//|         """Read bytes.  If ``nbytes`` is specified then read at most that many
//|         bytes. Otherwise, read everything that arrives until the connection
//|         times out. Providing the number of bytes expected is highly recommended
//|         because it will be faster. If no bytes are read, return ``None``.
//|
//|         .. note:: When no bytes are read due to a timeout, this function returns ``None``.
//|           This matches the behavior of `io.RawIOBase.read` in Python 3, but
//|           differs from pyserial which returns ``b''`` in that situation.
//|
//|         :return: Data read
//|         :rtype: bytes or None"""
//|         ...

//|     def readinto(self, buf: WriteableBuffer) -> Optional[int]:
//|         """Read bytes into the ``buf``. Read at most ``len(buf)`` bytes.
//|
//|         :return: number of bytes read and stored into ``buf``
//|         :rtype: int or None (on a non-blocking error)
//|
//|         *New in CircuitPython 4.0:* No length parameter is permitted."""
//|         ...

//|     def readline(self) -> bytes:
//|         """Read a line, ending in a newline character, or
//|         return ``None`` if a timeout occurs sooner, or
//|         return everything readable if no newline is found and
//|         ``timeout=0``
//|
//|         :return: the line read
//|         :rtype: bytes or None"""
//|         ...

//|     def write(self, buf: ReadableBuffer) -> Optional[int]:
//|         """Write the buffer of bytes to the bus.
//|
//|         *New in CircuitPython 4.0:* ``buf`` must be bytes, not a string.
//|
//|           :return: the number of bytes written
//|           :rtype: int or None"""
//|         ...

// These three methods are used by the shared stream methods.
STATIC mp_uint_t busio_uart_read(mp_obj_t self_in, void *buf_in, mp_uint_t size, int *errcode) {
    STREAM_DEBUG("busio_uart_read stream %d\n", size);
    busio_uart_obj_t *self = native_uart(self_in);
    check_for_deinit(self);
    byte *buf = buf_in;

    // make sure we want at least 1 char
    if (size == 0) {
        return 0;
    }

    return common_hal_busio_uart_read(self, buf, size, errcode);
}

STATIC mp_uint_t busio_uart_write(mp_obj_t self_in, const void *buf_in, mp_uint_t size, int *errcode) {
    busio_uart_obj_t *self = native_uart(self_in);
    check_for_deinit(self);
    const byte *buf = buf_in;

    return common_hal_busio_uart_write(self, buf, size, errcode);
}

STATIC mp_uint_t busio_uart_ioctl(mp_obj_t self_in, mp_uint_t request, mp_uint_t arg, int *errcode) {
    busio_uart_obj_t *self = native_uart(self_in);
    check_for_deinit(self);
    mp_uint_t ret;
    if (request == MP_STREAM_POLL) {
        mp_uint_t flags = arg;
        ret = 0;
        if ((flags & MP_STREAM_POLL_RD) && common_hal_busio_uart_rx_characters_available(self) > 0) {
            ret |= MP_STREAM_POLL_RD;
        }
        if ((flags & MP_STREAM_POLL_WR) && common_hal_busio_uart_ready_to_tx(self)) {
            ret |= MP_STREAM_POLL_WR;
        }
    } else {
        *errcode = MP_EINVAL;
        ret = MP_STREAM_ERROR;
    }
    return ret;
}

//|     baudrate: int
//|     """The current baudrate."""
STATIC mp_obj_t busio_uart_obj_get_baudrate(mp_obj_t self_in) {
    busio_uart_obj_t *self = native_uart(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_busio_uart_get_baudrate(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(busio_uart_get_baudrate_obj, busio_uart_obj_get_baudrate);

STATIC mp_obj_t busio_uart_obj_set_baudrate(mp_obj_t self_in, mp_obj_t baudrate) {
    busio_uart_obj_t *self = native_uart(self_in);
    check_for_deinit(self);
    common_hal_busio_uart_set_baudrate(self, mp_obj_get_int(baudrate));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(busio_uart_set_baudrate_obj, busio_uart_obj_set_baudrate);


MP_PROPERTY_GETSET(busio_uart_baudrate_obj,
    (mp_obj_t)&busio_uart_get_baudrate_obj,
    (mp_obj_t)&busio_uart_set_baudrate_obj);

//|     in_waiting: int
//|     """The number of bytes in the input buffer, available to be read"""
STATIC mp_obj_t busio_uart_obj_get_in_waiting(mp_obj_t self_in) {
    busio_uart_obj_t *self = native_uart(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_busio_uart_rx_characters_available(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(busio_uart_get_in_waiting_obj, busio_uart_obj_get_in_waiting);

MP_PROPERTY_GETTER(busio_uart_in_waiting_obj,
    (mp_obj_t)&busio_uart_get_in_waiting_obj);

//|     timeout: float
//|     """The current timeout, in seconds (float)."""
STATIC mp_obj_t busio_uart_obj_get_timeout(mp_obj_t self_in) {
    busio_uart_obj_t *self = native_uart(self_in);
    check_for_deinit(self);
    return mp_obj_new_float(common_hal_busio_uart_get_timeout(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(busio_uart_get_timeout_obj, busio_uart_obj_get_timeout);

STATIC mp_obj_t busio_uart_obj_set_timeout(mp_obj_t self_in, mp_obj_t timeout) {
    busio_uart_obj_t *self = native_uart(self_in);
    check_for_deinit(self);
    mp_float_t timeout_float = mp_obj_get_float(timeout);
    validate_timeout(timeout_float);
    common_hal_busio_uart_set_timeout(self, timeout_float);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(busio_uart_set_timeout_obj, busio_uart_obj_set_timeout);


MP_PROPERTY_GETSET(busio_uart_timeout_obj,
    (mp_obj_t)&busio_uart_get_timeout_obj,
    (mp_obj_t)&busio_uart_set_timeout_obj);

//|     def reset_input_buffer(self) -> None:
//|         """Discard any unread characters in the input buffer."""
//|         ...
//|
STATIC mp_obj_t busio_uart_obj_reset_input_buffer(mp_obj_t self_in) {
    busio_uart_obj_t *self = native_uart(self_in);
    check_for_deinit(self);
    common_hal_busio_uart_clear_rx_buffer(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(busio_uart_reset_input_buffer_obj, busio_uart_obj_reset_input_buffer);
#endif  // CIRCUITPY_BUSIO_UART

//| class Parity:
//|     """Enum-like class to define the parity used to verify correct data transfer."""
//|
//|     ODD: int
//|     """Total number of ones should be odd."""
//|
//|     EVEN: int
//|     """Total number of ones should be even."""
//|
const mp_obj_type_t busio_uart_parity_type;

const busio_uart_parity_obj_t busio_uart_parity_odd_obj = {
    { &busio_uart_parity_type },
};

const busio_uart_parity_obj_t busio_uart_parity_even_obj = {
    { &busio_uart_parity_type },
};

STATIC const mp_rom_map_elem_t busio_uart_parity_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_ODD),    MP_ROM_PTR(&busio_uart_parity_odd_obj) },
    { MP_ROM_QSTR(MP_QSTR_EVEN),   MP_ROM_PTR(&busio_uart_parity_even_obj) },
};
STATIC MP_DEFINE_CONST_DICT(busio_uart_parity_locals_dict, busio_uart_parity_locals_dict_table);

STATIC void busio_uart_parity_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    qstr parity = MP_QSTR_ODD;
    if (self_in == MP_ROM_PTR(&busio_uart_parity_even_obj)) {
        parity = MP_QSTR_EVEN;
    }
    mp_printf(print, "%q.%q.%q.%q", MP_QSTR_busio, MP_QSTR_UART, MP_QSTR_Parity, parity);
}

MP_DEFINE_CONST_OBJ_TYPE(
    busio_uart_parity_type,
    MP_QSTR_Parity,
    MP_TYPE_FLAG_NONE,
    print, busio_uart_parity_print,
    locals_dict, &busio_uart_parity_locals_dict
    );

STATIC const mp_rom_map_elem_t busio_uart_locals_dict_table[] = {
    #if CIRCUITPY_BUSIO_UART
    { MP_ROM_QSTR(MP_QSTR___del__),      MP_ROM_PTR(&busio_uart_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit),       MP_ROM_PTR(&busio_uart_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__),    MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__),     MP_ROM_PTR(&busio_uart___exit___obj) },

    // Standard stream methods.
    { MP_OBJ_NEW_QSTR(MP_QSTR_read),     MP_ROM_PTR(&mp_stream_read_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readline), MP_ROM_PTR(&mp_stream_unbuffered_readline_obj)},
    { MP_OBJ_NEW_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&mp_stream_readinto_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_write),    MP_ROM_PTR(&mp_stream_write_obj) },

    { MP_OBJ_NEW_QSTR(MP_QSTR_reset_input_buffer), MP_ROM_PTR(&busio_uart_reset_input_buffer_obj) },

    // Properties
    { MP_ROM_QSTR(MP_QSTR_baudrate),     MP_ROM_PTR(&busio_uart_baudrate_obj) },
    { MP_ROM_QSTR(MP_QSTR_in_waiting),   MP_ROM_PTR(&busio_uart_in_waiting_obj) },
    { MP_ROM_QSTR(MP_QSTR_timeout),      MP_ROM_PTR(&busio_uart_timeout_obj) },
    #endif  // CIRCUITPY_BUSIO_UART

    // Nested Enum-like Classes.
    { MP_ROM_QSTR(MP_QSTR_Parity),       MP_ROM_PTR(&busio_uart_parity_type) },
};
STATIC MP_DEFINE_CONST_DICT(busio_uart_locals_dict, busio_uart_locals_dict_table);

#if CIRCUITPY_BUSIO_UART
STATIC const mp_stream_p_t uart_stream_p = {
    MP_PROTO_IMPLEMENT(MP_QSTR_protocol_stream)
    .read = busio_uart_read,
    .write = busio_uart_write,
    .ioctl = busio_uart_ioctl,
    .is_text = false,
    // Disallow optional length argument for .readinto()
    .pyserial_readinto_compatibility = true,
};

MP_DEFINE_CONST_OBJ_TYPE(
    busio_uart_type,
    MP_QSTR_UART,
    MP_TYPE_FLAG_ITER_IS_ITERNEXT,
    make_new, busio_uart_make_new,
    locals_dict, &busio_uart_locals_dict,
    iter, mp_stream_unbuffered_iter,
    protocol, &uart_stream_p
    );
#else
MP_DEFINE_CONST_OBJ_TYPE(
    busio_uart_type,
    MP_QSTR_UART,
    MP_TYPE_FLAG_NONE,
    make_new, busio_uart_make_new,
    locals_dict, &busio_uart_locals_dict
    );
#endif  // CIRCUITPY_BUSIO_UART
