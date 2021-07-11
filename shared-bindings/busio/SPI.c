/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Scott Shawcroft
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
// busio.SPI class.

#include <string.h>

#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/util.h"

#include "lib/utils/buffer_helper.h"
#include "lib/utils/context_manager_helpers.h"
#include "py/mperrno.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "supervisor/shared/translate.h"


//| class SPI:
//|     """A 3-4 wire serial protocol
//|
//|     SPI is a serial protocol that has exclusive pins for data in and out of the
//|     main device.  It is typically faster than :py:class:`~bitbangio.I2C` because a
//|     separate pin is used to select a device rather than a transmitted
//|     address. This class only manages three of the four SPI lines: `!clock`,
//|     `!MOSI`, `!MISO`. Its up to the client to manage the appropriate
//|     select line, often abbreviated `!CS` or `!SS`. (This is common because
//|     multiple secondaries can share the `!clock`, `!MOSI` and `!MISO` lines
//|     and therefore the hardware.)"""
//|
//|     def __init__(self, clock: microcontroller.Pin, MOSI: Optional[microcontroller.Pin] = None, MISO: Optional[microcontroller.Pin] = None) -> None:
//|
//|         """Construct an SPI object on the given pins.
//|
//|         .. note:: The SPI peripherals allocated in order of desirability, if possible,
//|            such as highest speed and not shared use first. For instance, on the nRF52840,
//|            there is a single 32MHz SPI peripheral, and multiple 8MHz peripherals,
//|            some of which may also be used for I2C. The 32MHz SPI peripheral is returned
//|            first, then the exclusive 8MHz SPI peripheral, and finally the shared 8MHz
//|            peripherals.
//|
//|         .. seealso:: Using this class directly requires careful lock management.
//|             Instead, use :class:`~adafruit_bus_device.SPIDevice` to
//|             manage locks.
//|
//|         .. seealso:: Using this class to directly read registers requires manual
//|             bit unpacking. Instead, use an existing driver or make one with
//|             :ref:`Register <register-module-reference>` data descriptors.
//|
//|         :param ~microcontroller.Pin clock: the pin to use for the clock.
//|         :param ~microcontroller.Pin MOSI: the Main Out Selected In pin.
//|         :param ~microcontroller.Pin MISO: the Main In Selected Out pin."""
//|         ...
//|


// TODO(tannewt): Support LSB SPI.
STATIC mp_obj_t busio_spi_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    #if CIRCUITPY_BUSIO_SPI
    busio_spi_obj_t *self = m_new_obj(busio_spi_obj_t);
    self->base.type = &busio_spi_type;
    enum { ARG_clock, ARG_MOSI, ARG_MISO };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_clock, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_MOSI, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_MISO, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mcu_pin_obj_t *clock = validate_obj_is_free_pin(args[ARG_clock].u_obj);
    const mcu_pin_obj_t *mosi = validate_obj_is_free_pin_or_none(args[ARG_MOSI].u_obj);
    const mcu_pin_obj_t *miso = validate_obj_is_free_pin_or_none(args[ARG_MISO].u_obj);

    if (!miso && !mosi) {
        mp_raise_ValueError(translate("Must provide MISO or MOSI pin"));
    }

    common_hal_busio_spi_construct(self, clock, mosi, miso);
    return MP_OBJ_FROM_PTR(self);
    #else
    mp_raise_NotImplementedError(NULL);
    #endif // CIRCUITPY_BUSIO_SPI
}

#if CIRCUITPY_BUSIO_SPI
//|     def deinit(self) -> None:
//|         """Turn off the SPI bus."""
//|         ...
//|
STATIC mp_obj_t busio_spi_obj_deinit(mp_obj_t self_in) {
    busio_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_busio_spi_deinit(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(busio_spi_deinit_obj, busio_spi_obj_deinit);

//|     def __enter__(self) -> SPI:
//|         """No-op used by Context Managers.
//|         Provided by context manager helper."""
//|         ...
//|

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
STATIC mp_obj_t busio_spi_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_busio_spi_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(busio_spi_obj___exit___obj, 4, 4, busio_spi_obj___exit__);

STATIC void check_lock(busio_spi_obj_t *self) {
    asm ("");
    if (!common_hal_busio_spi_has_lock(self)) {
        mp_raise_RuntimeError(translate("Function requires lock"));
    }
}

STATIC void check_for_deinit(busio_spi_obj_t *self) {
    if (common_hal_busio_spi_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def configure(self, *, baudrate: int = 100000, polarity: int = 0, phase: int = 0, bits: int = 8) -> None:
//|         """Configures the SPI bus. The SPI object must be locked.
//|
//|         :param int baudrate: the desired clock rate in Hertz. The actual clock rate may be higher or lower
//|           due to the granularity of available clock settings.
//|           Check the `frequency` attribute for the actual clock rate.
//|         :param int polarity: the base state of the clock line (0 or 1)
//|         :param int phase: the edge of the clock that data is captured. First (0)
//|           or second (1). Rising or falling depends on clock polarity.
//|         :param int bits: the number of bits per word
//|
//|         .. note:: On the SAMD21, it is possible to set the baudrate to 24 MHz, but that
//|            speed is not guaranteed to work. 12 MHz is the next available lower speed, and is
//|            within spec for the SAMD21.
//|
//|         .. note:: On the nRF52840, these baudrates are available: 125kHz, 250kHz, 1MHz, 2MHz, 4MHz,
//|           and 8MHz.
//|           If you pick a a baudrate other than one of these, the nearest lower
//|           baudrate will be chosen, with a minimum of 125kHz.
//|           Two SPI objects may be created, except on the Circuit Playground Bluefruit,
//|           which allows only one (to allow for an additional I2C object)."""
//|         ...
//|

STATIC mp_obj_t busio_spi_configure(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 100000} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_phase, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_bits, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} },
    };
    busio_spi_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    check_lock(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint8_t polarity = args[ARG_polarity].u_int;
    if (polarity != 0 && polarity != 1) {
        mp_raise_ValueError(translate("Invalid polarity"));
    }
    uint8_t phase = args[ARG_phase].u_int;
    if (phase != 0 && phase != 1) {
        mp_raise_ValueError(translate("Invalid phase"));
    }
    uint8_t bits = args[ARG_bits].u_int;
    if (bits != 8 && bits != 9) {
        mp_raise_ValueError(translate("Invalid number of bits"));
    }

    if (!common_hal_busio_spi_configure(self, args[ARG_baudrate].u_int,
        polarity, phase, bits)) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(busio_spi_configure_obj, 1, busio_spi_configure);

//|     def try_lock(self) -> bool:
//|         """Attempts to grab the SPI lock. Returns True on success.
//|
//|         :return: True when lock has been grabbed
//|         :rtype: bool"""
//|         ...
//|

STATIC mp_obj_t busio_spi_obj_try_lock(mp_obj_t self_in) {
    busio_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(common_hal_busio_spi_try_lock(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(busio_spi_try_lock_obj, busio_spi_obj_try_lock);

//|     def unlock(self) -> None:
//|         """Releases the SPI lock."""
//|         ...
//|

STATIC mp_obj_t busio_spi_obj_unlock(mp_obj_t self_in) {
    busio_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_busio_spi_unlock(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(busio_spi_unlock_obj, busio_spi_obj_unlock);

//|     def write(self, buffer: ReadableBuffer, *, start: int = 0, end: Optional[int] = None) -> None:
//|         """Write the data contained in ``buffer``. The SPI object must be locked.
//|         If the buffer is empty, nothing happens.
//|
//|         :param ~_typing.ReadableBuffer buffer: Write out the data in this buffer
//|         :param int start: Start of the slice of ``buffer`` to write out: ``buffer[start:end]``
//|         :param int end: End of the slice; this index is not included. Defaults to ``len(buffer)``"""
//|         ...
//|

STATIC mp_obj_t busio_spi_write(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer, ARG_start, ARG_end };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_start,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_end,        MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
    };
    busio_spi_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    check_lock(self);
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

    bool ok = common_hal_busio_spi_write(self, ((uint8_t *)bufinfo.buf) + start, length);
    if (!ok) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(busio_spi_write_obj, 2, busio_spi_write);


//|     def readinto(self, buffer: WriteableBuffer, *, start: int = 0, end: Optional[int] = None, write_value: int = 0) -> None:
//|         """Read into ``buffer`` while writing ``write_value`` for each byte read.
//|         The SPI object must be locked.
//|         If the number of bytes to read is 0, nothing happens.
//|
//|         :param ~_typing.WriteableBuffer buffer: Read data into this buffer
//|         :param int start: Start of the slice of ``buffer`` to read into: ``buffer[start:end]``
//|         :param int end: End of the slice; this index is not included. Defaults to ``len(buffer)``
//|         :param int write_value: Value to write while reading. (Usually ignored.)"""
//|         ...
//|

STATIC mp_obj_t busio_spi_readinto(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer, ARG_start, ARG_end, ARG_write_value };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_start,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_end,        MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
        { MP_QSTR_write_value,MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };
    busio_spi_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    check_lock(self);
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

    bool ok = common_hal_busio_spi_read(self, ((uint8_t *)bufinfo.buf) + start, length, args[ARG_write_value].u_int);
    if (!ok) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(busio_spi_readinto_obj, 2, busio_spi_readinto);

//|     def write_readinto(self, buffer_out: ReadableBuffer, buffer_in: WriteableBuffer, *, out_start: int = 0, out_end: Optional[int] = None, in_start: int = 0, in_end: Optional[int] = None) -> None:
//|         """Write out the data in ``buffer_out`` while simultaneously reading data into ``buffer_in``.
//|         The SPI object must be locked.
//|         The lengths of the slices defined by ``buffer_out[out_start:out_end]`` and ``buffer_in[in_start:in_end]``
//|         must be equal.
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

STATIC mp_obj_t busio_spi_write_readinto(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer_out, ARG_buffer_in, ARG_out_start, ARG_out_end, ARG_in_start, ARG_in_end };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer_out,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_buffer_in,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_out_start,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_out_end,       MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
        { MP_QSTR_in_start,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_in_end,        MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
    };
    busio_spi_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    check_lock(self);
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

    if (out_length != in_length) {
        mp_raise_ValueError(translate("buffer slices must be of equal length"));
    }

    if (out_length == 0) {
        return mp_const_none;
    }

    bool ok = common_hal_busio_spi_transfer(self,
        ((uint8_t *)buf_out_info.buf) + out_start,
        ((uint8_t *)buf_in_info.buf) + in_start,
        out_length);
    if (!ok) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(busio_spi_write_readinto_obj, 2, busio_spi_write_readinto);

//|     frequency: int
//|     """The actual SPI bus frequency. This may not match the frequency requested
//|     due to internal limitations."""
//|

STATIC mp_obj_t busio_spi_obj_get_frequency(mp_obj_t self_in) {
    busio_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_busio_spi_get_frequency(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(busio_spi_get_frequency_obj, busio_spi_obj_get_frequency);

const mp_obj_property_t busio_spi_frequency_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&busio_spi_get_frequency_obj,
              MP_ROM_NONE,
              MP_ROM_NONE},
};
#endif // CIRCUITPY_BUSIO_SPI


STATIC const mp_rom_map_elem_t busio_spi_locals_dict_table[] = {
    #if CIRCUITPY_BUSIO_SPI
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&busio_spi_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&busio_spi_obj___exit___obj) },

    { MP_ROM_QSTR(MP_QSTR_configure), MP_ROM_PTR(&busio_spi_configure_obj) },
    { MP_ROM_QSTR(MP_QSTR_try_lock), MP_ROM_PTR(&busio_spi_try_lock_obj) },
    { MP_ROM_QSTR(MP_QSTR_unlock), MP_ROM_PTR(&busio_spi_unlock_obj) },

    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&busio_spi_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&busio_spi_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_readinto), MP_ROM_PTR(&busio_spi_write_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_frequency), MP_ROM_PTR(&busio_spi_frequency_obj) }
    #endif // CIRCUITPY_BUSIO_SPI
};
STATIC MP_DEFINE_CONST_DICT(busio_spi_locals_dict, busio_spi_locals_dict_table);

const mp_obj_type_t busio_spi_type = {
    { &mp_type_type },
    .name = MP_QSTR_SPI,
    .make_new = busio_spi_make_new,
    .locals_dict = (mp_obj_dict_t *)&busio_spi_locals_dict,
};

busio_spi_obj_t *validate_obj_is_spi_bus(mp_obj_t obj) {
    if (!mp_obj_is_type(obj, &busio_spi_type)) {
        mp_raise_TypeError_varg(translate("Expected a %q"), busio_spi_type.name);
    }
    return MP_OBJ_TO_PTR(obj);
}
