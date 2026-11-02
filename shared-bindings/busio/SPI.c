// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

// This file contains all of the Python API definitions for the
// busio.SPI class.

#include <string.h>

#include "py/mperrno.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/busio/SPI.h"
#include "shared-bindings/util.h"
#include "shared/runtime/buffer_helper.h"
#include "shared/runtime/context_manager_helpers.h"
#include "shared/runtime/interrupt_char.h"
#include "supervisor/shared/tick.h"

//| class SPI:
//|     """A 3-4 wire serial protocol
//|
//|     SPI is a serial protocol that has exclusive pins for data in and out of the
//|     main device.  It is typically faster than :py:class:`~bitbangio.I2C` because a
//|     separate pin is used to select a device rather than a transmitted
//|     address. This class only manages three of the four SPI lines: `!clock`,
//|     `!MOSI`, `!MISO`. It is up to the client to manage the appropriate
//|     select line, often abbreviated `!CS` or `!SS`. (This is common because
//|     multiple secondaries can share the `!clock`, `!MOSI` and `!MISO` lines
//|     and therefore the hardware.)
//|
//|     .. raw:: html
//|
//|         <p>
//|         <details>
//|         <summary>Available on these boards</summary>
//|         <ul>
//|         {% for board in support_matrix_reverse["busio.SPI"] %}
//|         <li> {{ board }}
//|         {% endfor %}
//|         </ul>
//|         </details>
//|         </p>
//|
//|     .. seealso:: This class acts as an SPI main (controller).
//|         To act as an SPI secondary (target), use `spitarget.SPITarget`.
//|     """
//|
//|     def __init__(
//|         self,
//|         clock: microcontroller.Pin,
//|         MOSI: Optional[microcontroller.Pin] = None,
//|         MISO: Optional[microcontroller.Pin] = None,
//|         half_duplex: bool = False,
//|     ) -> None:
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
//|         :param ~microcontroller.Pin MISO: the Main In Selected Out pin.
//|         :param bool half_duplex: True when MOSI is used for bidirectional data. False when SPI is full-duplex or simplex.
//|
//|         **Limitations:** ``half_duplex`` is available only on STM; other chips do not have the hardware support.
//|         """
//|         ...
//|


#if CIRCUITPY_BUSIO_SPI
// async_spi.SPI takes the same arguments and uses these tables too.
const mp_arg_t busio_spi_make_new_args[] = {
    { MP_QSTR_clock, MP_ARG_REQUIRED | MP_ARG_OBJ },
    { MP_QSTR_MOSI, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    { MP_QSTR_MISO, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    { MP_QSTR_half_duplex, MP_ARG_BOOL | MP_ARG_KW_ONLY, {.u_bool = false} },
};
#endif

// TODO(tannewt): Support LSB SPI.
static mp_obj_t busio_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    #if CIRCUITPY_BUSIO_SPI
    enum { ARG_clock, ARG_MOSI, ARG_MISO, ARG_half_duplex };
    mp_arg_val_t args[MP_ARRAY_SIZE(busio_spi_make_new_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(busio_spi_make_new_args), busio_spi_make_new_args, args);

    const mcu_pin_obj_t *clock = validate_obj_is_free_pin(args[ARG_clock].u_obj, MP_QSTR_clock);
    const mcu_pin_obj_t *mosi = validate_obj_is_free_pin_or_none(args[ARG_MOSI].u_obj, MP_QSTR_mosi);
    const mcu_pin_obj_t *miso = validate_obj_is_free_pin_or_none(args[ARG_MISO].u_obj, MP_QSTR_miso);

    if (!miso && !mosi) {
        mp_raise_ValueError(MP_ERROR_TEXT("Must provide MISO or MOSI pin"));
    }

    busio_spi_obj_t *self = mp_obj_malloc_with_finaliser(busio_spi_obj_t, &busio_spi_type);
    common_hal_busio_spi_construct(self, clock, mosi, miso, args[ARG_half_duplex].u_bool);
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
static mp_obj_t busio_spi_obj_deinit(mp_obj_t self_in) {
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
//  Provided by context manager helper.

static void check_lock(busio_spi_obj_t *self) {
    __asm__ ("");
    if (!common_hal_busio_spi_has_lock(self)) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Function requires lock"));
    }
}

static void check_for_deinit(busio_spi_obj_t *self) {
    if (common_hal_busio_spi_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def configure(
//|         self, *, baudrate: int = 100000, polarity: int = 0, phase: int = 0, bits: int = 8
//|     ) -> None:
//|         """Configures the SPI bus. The SPI object must be locked.
//|
//|         :param int baudrate: the desired clock rate in Hertz. The value is treated as a ceiling:
//|           the actual clock rate may be lower due to the granularity of available clock settings,
//|           but it will not exceed the given value.
//|           Check the `frequency` attribute for the actual clock rate.
//|           **Limitations**: On Zephyr, the ceiling behavior may be violated, and
//|           the `frequency` value does not reflect the actual baudrate.
//|         :param int polarity: the base state of the clock line (0 or 1)
//|         :param int phase: the edge of the clock that data is captured. First (0)
//|           or second (1). Rising or falling depends on clock polarity.
//|         :param int bits: the number of bits per word
//|
//|         .. note:: On the SAMD21, it is possible to set the baudrate to 24 MHz, but that
//|            speed is not guaranteed to work. 12 MHz is the next available lower speed, and is
//|            within spec for the SAMD21.
//|         """
//|         ...
//|

const mp_arg_t busio_spi_configure_args[] = {
    { MP_QSTR_baudrate, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 100000} },
    { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_phase, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_bits, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} },
};

static mp_obj_t busio_spi_configure(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits };
    busio_spi_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    check_lock(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(busio_spi_configure_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(busio_spi_configure_args), busio_spi_configure_args, args);

    uint8_t polarity = (uint8_t)mp_arg_validate_int_range(args[ARG_polarity].u_int, 0, 1, MP_QSTR_polarity);
    uint8_t phase = (uint8_t)mp_arg_validate_int_range(args[ARG_phase].u_int, 0, 1, MP_QSTR_phase);
    uint8_t bits = (uint8_t)mp_arg_validate_int_range(args[ARG_bits].u_int, 8, 9, MP_QSTR_bits);

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

static mp_obj_t busio_spi_obj_try_lock(mp_obj_t self_in) {
    busio_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_bool(common_hal_busio_spi_try_lock(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(busio_spi_try_lock_obj, busio_spi_obj_try_lock);

//|     def unlock(self) -> None:
//|         """Releases the SPI lock."""
//|         ...
//|

static mp_obj_t busio_spi_obj_unlock(mp_obj_t self_in) {
    busio_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    common_hal_busio_spi_unlock(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(busio_spi_unlock_obj, busio_spi_obj_unlock);

//|     import sys
//|
//|     def write(self, buffer: ReadableBuffer, *, start: int = 0, end: int = sys.maxsize) -> None:
//|         """Write the data contained in ``buffer``. The SPI object must be locked.
//|         If the buffer is empty, nothing happens.
//|
//|         If ``start`` or ``end`` is provided, then the buffer will be sliced
//|         as if ``buffer[start:end]`` were passed, but without copying the data.
//|         The number of bytes written will be the length of ``buffer[start:end]``.
//|
//|         :param ReadableBuffer buffer: write out bytes from this buffer
//|         :param int start: beginning of buffer slice
//|         :param int end: end of buffer slice; if not specified, use ``len(buffer)``
//|         """
//|         ...
//|

const mp_arg_t busio_spi_write_args[] = {
    { MP_QSTR_buffer,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_start,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_end,        MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
};

static mp_obj_t busio_spi_write(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer, ARG_start, ARG_end };
    busio_spi_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    check_lock(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(busio_spi_write_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(busio_spi_write_args), busio_spi_write_args, args);

    size_t length;
    uint8_t *data = buffer_slice(args[ARG_buffer].u_obj, args[ARG_start].u_int, args[ARG_end].u_int,
        MP_BUFFER_READ, &length);
    if (length == 0) {
        return mp_const_none;
    }

    bool ok = common_hal_busio_spi_write(self, data, length);
    if (!ok) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(busio_spi_write_obj, 1, busio_spi_write);


//|     import sys
//|
//|     def readinto(
//|         self,
//|         buffer: WriteableBuffer,
//|         *,
//|         start: int = 0,
//|         end: int = sys.maxsize,
//|         write_value: int = 0,
//|     ) -> None:
//|         """Read into ``buffer`` while writing ``write_value`` for each byte read.
//|         The SPI object must be locked.
//|         If the number of bytes to read is 0, nothing happens.
//|
//|         If ``start`` or ``end`` is provided, then the buffer will be sliced
//|         as if ``buffer[start:end]`` were passed.
//|         The number of bytes read will be the length of ``buffer[start:end]``.
//|
//|         :param WriteableBuffer buffer: read bytes into this buffer
//|         :param int start: beginning of buffer slice
//|         :param int end: end of buffer slice; if not specified, it will be the equivalent value
//|             of ``len(buffer)`` and for any value provided it will take the value of
//|             ``min(end, len(buffer))``
//|         :param int write_value: value to write while reading
//|         """
//|         ...
//|

const mp_arg_t busio_spi_readinto_args[] = {
    { MP_QSTR_buffer,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_start,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_end,        MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
    { MP_QSTR_write_value, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
};

static mp_obj_t busio_spi_readinto(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer, ARG_start, ARG_end, ARG_write_value };
    busio_spi_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    check_lock(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(busio_spi_readinto_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(busio_spi_readinto_args), busio_spi_readinto_args, args);

    size_t length;
    uint8_t *data = buffer_slice(args[ARG_buffer].u_obj, args[ARG_start].u_int, args[ARG_end].u_int,
        MP_BUFFER_WRITE, &length);
    if (length == 0) {
        return mp_const_none;
    }

    bool ok = common_hal_busio_spi_read(self, data, length, args[ARG_write_value].u_int);
    if (!ok) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(busio_spi_readinto_obj, 1, busio_spi_readinto);

//|     import sys
//|
//|     def write_readinto(
//|         self,
//|         out_buffer: ReadableBuffer,
//|         in_buffer: WriteableBuffer,
//|         *,
//|         out_start: int = 0,
//|         out_end: int = sys.maxsize,
//|         in_start: int = 0,
//|         in_end: int = sys.maxsize,
//|     ) -> None:
//|         """Write out the data in ``out_buffer`` while simultaneously reading data into ``in_buffer``.
//|         The SPI object must be locked.
//|
//|         If ``out_start`` or ``out_end`` is provided, then the buffer will be sliced
//|         as if ``out_buffer[out_start:out_end]`` were passed, but without copying the data.
//|         The number of bytes written will be the length of ``out_buffer[out_start:out_end]``.
//|
//|         If ``in_start`` or ``in_end`` is provided, then the input buffer will be sliced
//|         as if ``in_buffer[in_start:in_end]`` were passed,
//|         The number of bytes read will be the length of ``out_buffer[in_start:in_end]``.
//|
//|         The lengths of the slices defined by ``out_buffer[out_start:out_end]``
//|         and ``in_buffer[in_start:in_end]`` must be equal.
//|         If buffer slice lengths are both 0, nothing happens.
//|
//|         :param ReadableBuffer out_buffer: write out bytes from this buffer
//|         :param WriteableBuffer in_buffer: read bytes into this buffer
//|         :param int out_start: beginning of ``out_buffer`` slice
//|         :param int out_end: end of ``out_buffer`` slice; if not specified, use ``len(out_buffer)``
//|         :param int in_start: beginning of ``in_buffer`` slice
//|         :param int in_end: end of ``in_buffer slice``; if not specified, use ``len(in_buffer)``
//|         """
//|         ...
//|

const mp_arg_t busio_spi_write_readinto_args[] = {
    { MP_QSTR_out_buffer,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_in_buffer,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    { MP_QSTR_out_start,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_out_end,       MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
    { MP_QSTR_in_start,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    { MP_QSTR_in_end,        MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
};

static mp_obj_t busio_spi_write_readinto(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_out_buffer, ARG_in_buffer, ARG_out_start, ARG_out_end, ARG_in_start, ARG_in_end };
    busio_spi_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    check_lock(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(busio_spi_write_readinto_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(busio_spi_write_readinto_args), busio_spi_write_readinto_args, args);

    size_t out_length, in_length;
    const uint8_t *data_out = buffer_slice(args[ARG_out_buffer].u_obj, args[ARG_out_start].u_int,
        args[ARG_out_end].u_int, MP_BUFFER_READ, &out_length);
    uint8_t *data_in = buffer_slice(args[ARG_in_buffer].u_obj, args[ARG_in_start].u_int,
        args[ARG_in_end].u_int, MP_BUFFER_WRITE, &in_length);

    if (out_length != in_length) {
        mp_raise_ValueError(MP_ERROR_TEXT("buffer slices must be of equal length"));
    }

    if (out_length == 0) {
        return mp_const_none;
    }

    bool ok = common_hal_busio_spi_transfer(self, data_out, data_in, out_length);
    if (!ok) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(busio_spi_write_readinto_obj, 1, busio_spi_write_readinto);

//|     frequency: int
//|     """The actual SPI bus frequency. This may be lower than the frequency requested
//|     due to internal limitations, but it will not be higher.
//|
//|     **Limitations**: On Zephyr, the returned value does not reflect the actual baudrate,
//|     because there is `no way to fetch the actual chosen frequency
//|     <https://github.com/zephyrproject-rtos/zephyr/issues/77922>`_.
//|     In addition, on Zephyr, the ceiling behavior described in `configure` may be violated.
//|     """
//|

static mp_obj_t busio_spi_obj_get_frequency(mp_obj_t self_in) {
    busio_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return MP_OBJ_NEW_SMALL_INT(common_hal_busio_spi_get_frequency(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(busio_spi_get_frequency_obj, busio_spi_obj_get_frequency);

MP_PROPERTY_GETTER(busio_spi_frequency_obj,
    (mp_obj_t)&busio_spi_get_frequency_obj);

#endif // CIRCUITPY_BUSIO_SPI


static const mp_rom_map_elem_t busio_spi_locals_dict_table[] = {
    #if CIRCUITPY_BUSIO_SPI
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&busio_spi_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&busio_spi_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&default___exit___obj) },

    { MP_ROM_QSTR(MP_QSTR_configure), MP_ROM_PTR(&busio_spi_configure_obj) },
    { MP_ROM_QSTR(MP_QSTR_try_lock), MP_ROM_PTR(&busio_spi_try_lock_obj) },
    { MP_ROM_QSTR(MP_QSTR_unlock), MP_ROM_PTR(&busio_spi_unlock_obj) },

    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&busio_spi_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&busio_spi_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_readinto), MP_ROM_PTR(&busio_spi_write_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_frequency), MP_ROM_PTR(&busio_spi_frequency_obj) }

    #endif // CIRCUITPY_BUSIO_SPI
};
static MP_DEFINE_CONST_DICT(busio_spi_locals_dict, busio_spi_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    busio_spi_type,
    MP_QSTR_SPI,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, busio_spi_make_new,
    locals_dict, &busio_spi_locals_dict
    );

busio_spi_obj_t *validate_obj_is_spi_bus(mp_obj_t obj, qstr arg_name) {
    return mp_arg_validate_type(obj, &busio_spi_type, arg_name);
}

// Wait as long as needed for the lock. This is used by SD card access from USB.
// The default implementation is to busy-wait while running the background tasks. espressif is different.
bool common_hal_busio_spi_wait_for_lock(busio_spi_obj_t *self, uint32_t timeout_ms) {
    uint64_t deadline = supervisor_ticks_ms64() + timeout_ms;
    while (supervisor_ticks_ms64() < deadline &&
           !mp_hal_is_interrupted()) {
        if (common_hal_busio_spi_try_lock(self)) {
            return true;
        }
        RUN_BACKGROUND_TASKS;
    }
    return false;
}
