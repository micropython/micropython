// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2016 Scott Shawcroft
//
// SPDX-License-Identifier: MIT

// This file contains all of the Python API definitions for the
// bitbangio.SPI class.

#include <string.h>

#include "shared-bindings/bitbangio/SPI.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"

#include "shared/runtime/buffer_helper.h"
#include "shared/runtime/context_manager_helpers.h"
#include "py/binary.h"
#include "py/mperrno.h"
#include "py/runtime.h"

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
//|     def __init__(
//|         self,
//|         clock: microcontroller.Pin,
//|         MOSI: Optional[microcontroller.Pin] = None,
//|         MISO: Optional[microcontroller.Pin] = None,
//|     ) -> None:
//|         """Construct an SPI object on the given pins.
//|
//|         .. seealso:: Using this class directly requires careful lock management.
//|             Instead, use :class:`~adafruit_bus_device.spi_device.SPIDevice` to
//|             manage locks.
//|
//|         .. seealso:: Using this class to directly read registers requires manual
//|             bit unpacking. Instead, use an existing driver or make one with
//|             :ref:`Register <register-module-reference>` data descriptors.
//|
//|
//|         :param ~microcontroller.Pin clock: the pin to use for the clock.
//|         :param ~microcontroller.Pin MOSI: the Main Out Selected In pin.
//|         :param ~microcontroller.Pin MISO: the Main In Selected Out pin."""
//|         ...

// TODO(tannewt): Support LSB SPI.
static mp_obj_t bitbangio_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_clock, ARG_MOSI, ARG_MISO, ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits, ARG_firstbit };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_clock, MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_MOSI, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_MISO, MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mcu_pin_obj_t *clock = validate_obj_is_free_pin(args[ARG_clock].u_obj, MP_QSTR_clock);
    const mcu_pin_obj_t *mosi = validate_obj_is_free_pin_or_none(args[ARG_MOSI].u_obj, MP_QSTR_mosi);
    const mcu_pin_obj_t *miso = validate_obj_is_free_pin_or_none(args[ARG_MISO].u_obj, MP_QSTR_miso);

    bitbangio_spi_obj_t *self = mp_obj_malloc(bitbangio_spi_obj_t, &bitbangio_spi_type);
    shared_module_bitbangio_spi_construct(self, clock, mosi, miso);
    return (mp_obj_t)self;
}

//|     def deinit(self) -> None:
//|         """Turn off the SPI bus."""
//|         ...
static mp_obj_t bitbangio_spi_obj_deinit(mp_obj_t self_in) {
    bitbangio_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    shared_module_bitbangio_spi_deinit(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(bitbangio_spi_deinit_obj, bitbangio_spi_obj_deinit);

static void check_for_deinit(bitbangio_spi_obj_t *self) {
    if (shared_module_bitbangio_spi_deinited(self)) {
        raise_deinited_error();
    }
}

//|     def __enter__(self) -> SPI:
//|         """No-op used by Context Managers."""
//|         ...
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
static mp_obj_t bitbangio_spi_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    shared_module_bitbangio_spi_deinit(args[0]);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(bitbangio_spi_obj___exit___obj, 4, 4, bitbangio_spi_obj___exit__);


static void check_lock(bitbangio_spi_obj_t *self) {
    if (!shared_module_bitbangio_spi_has_lock(self)) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Function requires lock"));
    }
}

//|     def configure(
//|         self, *, baudrate: int = 100000, polarity: int = 0, phase: int = 0, bits: int = 8
//|     ) -> None:
//|         """Configures the SPI bus. Only valid when locked.
//|
//|         :param int baudrate: the clock rate in Hertz
//|         :param int polarity: the base state of the clock line (0 or 1)
//|         :param int phase: the edge of the clock that data is captured. First (0)
//|           or second (1). Rising or falling depends on clock polarity.
//|         :param int bits: the number of bits per word"""
//|         ...
static mp_obj_t bitbangio_spi_configure(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_baudrate, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 100000} },
        { MP_QSTR_polarity, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_phase, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_bits, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 8} },
    };
    bitbangio_spi_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    check_lock(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    uint8_t polarity = (uint8_t)mp_arg_validate_int_range(args[ARG_polarity].u_int, 0, 1, MP_QSTR_polarity);
    uint8_t phase = (uint8_t)mp_arg_validate_int_range(args[ARG_phase].u_int, 0, 1, MP_QSTR_phase);
    uint8_t bits = (uint8_t)mp_arg_validate_int_range(args[ARG_bits].u_int, 8, 9, MP_QSTR_bits);


    shared_module_bitbangio_spi_configure(self, args[ARG_baudrate].u_int, polarity, phase, bits);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(bitbangio_spi_configure_obj, 1, bitbangio_spi_configure);

//|     def try_lock(self) -> bool:
//|         """Attempts to grab the SPI lock. Returns True on success.
//|
//|         :return: True when lock has been grabbed
//|         :rtype: bool"""
//|         ...
static mp_obj_t bitbangio_spi_obj_try_lock(mp_obj_t self_in) {
    bitbangio_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    return mp_obj_new_bool(shared_module_bitbangio_spi_try_lock(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(bitbangio_spi_try_lock_obj, bitbangio_spi_obj_try_lock);

//|     def unlock(self) -> None:
//|         """Releases the SPI lock."""
//|         ...
static mp_obj_t bitbangio_spi_obj_unlock(mp_obj_t self_in) {
    bitbangio_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);
    shared_module_bitbangio_spi_unlock(self);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(bitbangio_spi_unlock_obj, bitbangio_spi_obj_unlock);

//|     import sys
//|     def write(self, buf: ReadableBuffer, *, start: int = 0, end: int = sys.maxsize) -> None:
//|         """Write the data contained in ``buf``. Requires the SPI being locked.
//|         If the buffer is empty, nothing happens.
//|
//|         If ``start`` or ``end`` is provided, then the buffer will be sliced
//|         as if ``buffer[start:end]`` were passed, but without copying the data.
//|         The number of bytes written will be the length of ``buffer[start:end]``.
//|
//|         :param ReadableBuffer buffer: buffer containing the bytes to write
//|         :param int start: beginning of buffer slice
//|         :param int end: end of buffer slice; if not specified, use ``len(buffer)``
//|         """
//|         ...
static mp_obj_t bitbangio_spi_write(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer, ARG_start, ARG_end };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_start,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_end,        MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
    };
    bitbangio_spi_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    check_lock(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_READ);
    // Compute bounds in terms of elements, not bytes.
    int stride_in_bytes = mp_binary_get_size('@', bufinfo.typecode, NULL);
    int32_t start = args[ARG_start].u_int;
    size_t length = bufinfo.len / stride_in_bytes;
    normalize_buffer_bounds(&start, args[ARG_end].u_int, &length);

    // Treat start and length in terms of bytes from now on.
    start *= stride_in_bytes;
    length *= stride_in_bytes;

    if (length == 0) {
        return mp_const_none;
    }

    bool ok = shared_module_bitbangio_spi_write(self, ((uint8_t *)bufinfo.buf) + start, length);
    if (!ok) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(bitbangio_spi_write_obj, 1, bitbangio_spi_write);


//|     import sys
//|     def readinto(
//|         self,
//|         buffer: WriteableBuffer,
//|         *,
//|         start: int = 0,
//|         end: int = sys.maxsize,
//|         write_value: int = 0
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
//|         :param int end: end of buffer slice; if not specified, use ``len(buffer)``
//|         :param int write_value: value to write while reading
//|         """
//|         ...
static mp_obj_t bitbangio_spi_readinto(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer, ARG_start, ARG_end, ARG_write_value };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_start,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_end,        MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
        { MP_QSTR_write_value, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
    };
    bitbangio_spi_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    check_lock(self);
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_WRITE);
    int stride_in_bytes = mp_binary_get_size('@', bufinfo.typecode, NULL);
    // Compute bounds in terms of elements, not bytes.
    int32_t start = args[ARG_start].u_int;
    size_t length = bufinfo.len / stride_in_bytes;
    normalize_buffer_bounds(&start, args[ARG_end].u_int, &length);

    // Treat start and length in terms of bytes from now on.
    start *= stride_in_bytes;
    length *= stride_in_bytes;

    if (length == 0) {
        return mp_const_none;
    }

    bool ok = shared_module_bitbangio_spi_read(self, ((uint8_t *)bufinfo.buf) + start, length, args[ARG_write_value].u_int);
    if (!ok) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(bitbangio_spi_readinto_obj, 1, bitbangio_spi_readinto);

//|     import sys
//|     def write_readinto(
//|         self,
//|         out_buffer: ReadableBuffer,
//|         in_buffer: WriteableBuffer,
//|         *,
//|         out_start: int = 0,
//|         out_end: int = sys.maxsize,
//|         in_start: int = 0,
//|         in_end: int = sys.maxsize
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
static mp_obj_t bitbangio_spi_write_readinto(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_out_buffer, ARG_in_buffer, ARG_out_start, ARG_out_end, ARG_in_start, ARG_in_end };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_out_buffer,    MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_in_buffer,     MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_out_start,     MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_out_end,       MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
        { MP_QSTR_in_start,      MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_in_end,        MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = INT_MAX} },
    };
    bitbangio_spi_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);
    check_for_deinit(self);
    check_lock(self);

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t buf_out_info;
    mp_get_buffer_raise(args[ARG_out_buffer].u_obj, &buf_out_info, MP_BUFFER_READ);
    int out_stride_in_bytes = mp_binary_get_size('@', buf_out_info.typecode, NULL);
    int32_t out_start = args[ARG_out_start].u_int;
    size_t out_length = buf_out_info.len / out_stride_in_bytes;
    normalize_buffer_bounds(&out_start, args[ARG_out_end].u_int, &out_length);

    mp_buffer_info_t buf_in_info;
    mp_get_buffer_raise(args[ARG_in_buffer].u_obj, &buf_in_info, MP_BUFFER_WRITE);
    int in_stride_in_bytes = mp_binary_get_size('@', buf_in_info.typecode, NULL);
    int32_t in_start = args[ARG_in_start].u_int;
    size_t in_length = buf_in_info.len / in_stride_in_bytes;
    normalize_buffer_bounds(&in_start, args[ARG_in_end].u_int, &in_length);

    // Treat start and length in terms of bytes from now on.
    out_start *= out_stride_in_bytes;
    out_length *= out_stride_in_bytes;
    in_start *= in_stride_in_bytes;
    in_length *= in_stride_in_bytes;

    if (out_length != in_length) {
        mp_raise_ValueError(MP_ERROR_TEXT("buffer slices must be of equal length"));
    }

    if (out_length == 0) {
        return mp_const_none;
    }

    bool ok = shared_module_bitbangio_spi_transfer(self,
        ((uint8_t *)buf_out_info.buf) + out_start,
        ((uint8_t *)buf_in_info.buf) + in_start,
        out_length);
    if (!ok) {
        mp_raise_OSError(MP_EIO);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(bitbangio_spi_write_readinto_obj, 1, bitbangio_spi_write_readinto);

static const mp_rom_map_elem_t bitbangio_spi_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&bitbangio_spi_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&bitbangio_spi_obj___exit___obj) },

    { MP_ROM_QSTR(MP_QSTR_configure), MP_ROM_PTR(&bitbangio_spi_configure_obj) },
    { MP_ROM_QSTR(MP_QSTR_try_lock), MP_ROM_PTR(&bitbangio_spi_try_lock_obj) },
    { MP_ROM_QSTR(MP_QSTR_unlock), MP_ROM_PTR(&bitbangio_spi_unlock_obj) },

    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&bitbangio_spi_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&bitbangio_spi_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_readinto), MP_ROM_PTR(&bitbangio_spi_write_readinto_obj) },
};
static MP_DEFINE_CONST_DICT(bitbangio_spi_locals_dict, bitbangio_spi_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    bitbangio_spi_type,
    MP_QSTR_SPI,
    MP_TYPE_FLAG_NONE,
    make_new, bitbangio_spi_make_new,
    locals_dict, &bitbangio_spi_locals_dict
    );
