// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2026 Vladimir Smitka
//
// SPDX-License-Identifier: MIT

#include <stdint.h>

#include "supervisor/shared/awaitable.h"
#include "py/objproperty.h"
#include "py/runtime.h"

#include "shared/runtime/buffer_helper.h"
#include "shared/runtime/context_manager_helpers.h"
#include "shared-bindings/async_spi/SPI.h"
#include "shared-bindings/busio/SPI.h"

#if !CIRCUITPY_BUSIO_SPI
#error "async_spi uses busio.SPI's argument tables"
#endif
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"

//| class SPI:
//|     """An SPI bus whose transfers are awaited
//|
//|     ``write``, ``readinto`` and ``write_readinto`` return awaitables: the transfer runs while
//|     other asyncio tasks run. Short transfers, and transfers that cannot use DMA (for example
//|     from a buffer in flash), block other tasks until they finish. A transfer's arguments are
//|     checked when the result is awaited. A transfer or ``configure()`` needs the bus's `lock`
//|     held, so that a transaction of several transfers is not interleaved with another task's."""
//|
//|     def __init__(
//|         self,
//|         clock: microcontroller.Pin,
//|         MOSI: Optional[microcontroller.Pin] = None,
//|         MISO: Optional[microcontroller.Pin] = None,
//|     ) -> None:
//|         """Construct an SPI object on the given pins.
//|
//|         :param ~microcontroller.Pin clock: the pin to use for the clock.
//|         :param ~microcontroller.Pin MOSI: the Main Out Selected In pin.
//|         :param ~microcontroller.Pin MISO: the Main In Selected Out pin."""
//|         ...
//|

static mp_obj_t async_spi_spi_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    enum { ARG_clock, ARG_MOSI, ARG_MISO };
    mp_arg_val_t args[3];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, 3, busio_spi_make_new_args, args);

    const mcu_pin_obj_t *clock = validate_obj_is_free_pin(args[ARG_clock].u_obj, MP_QSTR_clock);
    const mcu_pin_obj_t *mosi = validate_obj_is_free_pin_or_none(args[ARG_MOSI].u_obj, MP_QSTR_MOSI);
    const mcu_pin_obj_t *miso = validate_obj_is_free_pin_or_none(args[ARG_MISO].u_obj, MP_QSTR_MISO);
    if (!miso && !mosi) {
        mp_raise_ValueError(MP_ERROR_TEXT("Must provide MISO or MOSI pin"));
    }

    async_spi_spi_obj_t *self = mp_obj_malloc_with_finaliser(async_spi_spi_obj_t, &async_spi_spi_type);
    common_hal_async_spi_spi_construct(self, clock, mosi, miso);
    return MP_OBJ_FROM_PTR(self);
}

static MP_NOINLINE async_spi_spi_obj_t *checked_spi(mp_obj_t self_in) {
    async_spi_spi_obj_t *self = MP_OBJ_TO_PTR(mp_arg_validate_type(self_in, &async_spi_spi_type, MP_QSTR_self));
    if (common_hal_async_spi_spi_deinited(self)) {
        raise_deinited_error();
    }
    return self;
}

// For the calls that use the bus.
static MP_NOINLINE async_spi_spi_obj_t *locked_spi(mp_obj_t self_in) {
    async_spi_spi_obj_t *self = checked_spi(self_in);
    if (!circuitpy_async_lock_held(common_hal_async_spi_spi_get_lock(self))) {
        mp_raise_RuntimeError(MP_ERROR_TEXT("Function requires lock"));
    }
    return self;
}

//|     def deinit(self) -> None:
//|         """Turn off the SPI bus. A transfer still running is stopped."""
//|         ...
//|
static mp_obj_t async_spi_spi_obj_deinit(mp_obj_t self_in) {
    async_spi_spi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_async_spi_spi_deinit(self);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(async_spi_spi_deinit_obj, async_spi_spi_obj_deinit);

//|     def __enter__(self) -> SPI:
//|         """No-op used by Context Managers."""
//|         ...
//|
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes the hardware when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
//|
//  Provided by context manager helper.

// The awaitable passes the object as mp_obj_t; the common-hal functions take it typed.
static mp_obj_t async_spi_spi_transfer_end(mp_obj_t self_in, void *context, circuitpy_async_flag_t *done) {
    return common_hal_async_spi_spi_transfer_end(MP_OBJ_TO_PTR(self_in), context, done);
}
static void async_spi_spi_transfer_cancel(mp_obj_t self_in, void *context, circuitpy_async_flag_t *done) {
    common_hal_async_spi_spi_transfer_cancel(MP_OBJ_TO_PTR(self_in), context, done);
}

//|     def configure(
//|         self, *, baudrate: int = 100000, polarity: int = 0, phase: int = 0, bits: int = 8
//|     ) -> None:
//|         """Configure the SPI bus, as `busio.SPI.configure`. Needs the `lock`."""
//|         ...
//|
static mp_obj_t async_spi_spi_configure(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_baudrate, ARG_polarity, ARG_phase, ARG_bits };
    async_spi_spi_obj_t *self = locked_spi(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(busio_spi_configure_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(busio_spi_configure_args), busio_spi_configure_args, args);

    uint8_t polarity = (uint8_t)mp_arg_validate_int_range(args[ARG_polarity].u_int, 0, 1, MP_QSTR_polarity);
    uint8_t phase = (uint8_t)mp_arg_validate_int_range(args[ARG_phase].u_int, 0, 1, MP_QSTR_phase);
    uint8_t bits = (uint8_t)mp_arg_validate_int_range(args[ARG_bits].u_int, 8, 9, MP_QSTR_bits);

    common_hal_async_spi_spi_configure(self, args[ARG_baudrate].u_int, polarity, phase, bits);
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_KW(async_spi_spi_configure_obj, 1, async_spi_spi_configure);

//|     import sys
//|
//|     async def write(
//|         self, buffer: ReadableBuffer, *, start: int = 0, end: int = sys.maxsize
//|     ) -> None:
//|         """Write the data in ``buffer[start:end]``.
//|
//|         :param ~circuitpython_typing.ReadableBuffer buffer: write out the data in this buffer
//|         :param int start: beginning of buffer slice
//|         :param int end: end of buffer slice; if not specified, use ``len(buffer)``"""
//|         ...
//|
static void *async_spi_spi_write(circuitpy_async_flag_t *done, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer, ARG_start, ARG_end };
    async_spi_spi_obj_t *self = locked_spi(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(busio_spi_write_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(busio_spi_write_args), busio_spi_write_args, args);

    size_t len;
    uint8_t *data = buffer_slice(args[ARG_buffer].u_obj, args[ARG_start].u_int, args[ARG_end].u_int,
        MP_BUFFER_READ, &len);
    return common_hal_async_spi_spi_write_start(self, data, len, done);
}
static CIRCUITPY_DEFINE_ASYNC_FUN_OBJ_KW(async_spi_spi_write_obj, 1, async_spi_spi_write,
    async_spi_spi_transfer_end, async_spi_spi_transfer_cancel);

//|     async def readinto(
//|         self,
//|         buffer: WriteableBuffer,
//|         *,
//|         start: int = 0,
//|         end: int = sys.maxsize,
//|         write_value: int = 0,
//|     ) -> None:
//|         """Read into ``buffer[start:end]`` while writing ``write_value`` for each byte read.
//|
//|         :param ~circuitpython_typing.WriteableBuffer buffer: read data into this buffer
//|         :param int start: beginning of buffer slice
//|         :param int end: end of buffer slice; if not specified, use ``len(buffer)``
//|         :param int write_value: value to write while reading"""
//|         ...
//|
static void *async_spi_spi_readinto(circuitpy_async_flag_t *done, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer, ARG_start, ARG_end, ARG_write_value };
    async_spi_spi_obj_t *self = locked_spi(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(busio_spi_readinto_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(busio_spi_readinto_args), busio_spi_readinto_args, args);

    size_t len;
    uint8_t *data = buffer_slice(args[ARG_buffer].u_obj, args[ARG_start].u_int, args[ARG_end].u_int,
        MP_BUFFER_WRITE, &len);
    return common_hal_async_spi_spi_readinto_start(self, data, len, (uint8_t)args[ARG_write_value].u_int, done);
}
static CIRCUITPY_DEFINE_ASYNC_FUN_OBJ_KW(async_spi_spi_readinto_obj, 1, async_spi_spi_readinto,
    async_spi_spi_transfer_end, async_spi_spi_transfer_cancel);

//|     async def write_readinto(
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
//|         The two slices must have the same length.
//|
//|         :param ~circuitpython_typing.ReadableBuffer out_buffer: write out the data in this buffer
//|         :param ~circuitpython_typing.WriteableBuffer in_buffer: read data into this buffer
//|         :param int out_start: beginning of ``out_buffer`` slice
//|         :param int out_end: end of ``out_buffer`` slice; if not specified, use ``len(out_buffer)``
//|         :param int in_start: beginning of ``in_buffer`` slice
//|         :param int in_end: end of ``in_buffer`` slice; if not specified, use ``len(in_buffer)``"""
//|         ...
//|
static void *async_spi_spi_write_readinto(circuitpy_async_flag_t *done, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_out_buffer, ARG_in_buffer, ARG_out_start, ARG_out_end, ARG_in_start, ARG_in_end };
    async_spi_spi_obj_t *self = locked_spi(pos_args[0]);
    mp_arg_val_t args[MP_ARRAY_SIZE(busio_spi_write_readinto_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(busio_spi_write_readinto_args), busio_spi_write_readinto_args, args);

    size_t out_len, in_len;
    const uint8_t *data_out = buffer_slice(args[ARG_out_buffer].u_obj, args[ARG_out_start].u_int,
        args[ARG_out_end].u_int, MP_BUFFER_READ, &out_len);
    uint8_t *data_in = buffer_slice(args[ARG_in_buffer].u_obj, args[ARG_in_start].u_int,
        args[ARG_in_end].u_int, MP_BUFFER_WRITE, &in_len);
    if (out_len != in_len) {
        mp_raise_ValueError(MP_ERROR_TEXT("buffer slices must be of equal length"));
    }
    return common_hal_async_spi_spi_write_readinto_start(self, data_out, data_in, out_len, done);
}
static CIRCUITPY_DEFINE_ASYNC_FUN_OBJ_KW(async_spi_spi_write_readinto_obj, 2, async_spi_spi_write_readinto,
    async_spi_spi_transfer_end, async_spi_spi_transfer_cancel);

//|     frequency: int
//|     """The actual SPI bus frequency. This may not match the frequency requested
//|     due to internal limitations."""
//|
//|
static mp_obj_t async_spi_spi_get_frequency(mp_obj_t self_in) {
    async_spi_spi_obj_t *self = checked_spi(self_in);
    return MP_OBJ_NEW_SMALL_INT(common_hal_async_spi_spi_get_frequency(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(async_spi_spi_get_frequency_obj, async_spi_spi_get_frequency);

MP_PROPERTY_GETTER(async_spi_spi_frequency_obj,
    (mp_obj_t)&async_spi_spi_get_frequency_obj);

//|     lock: Lock
//|     """The bus's lock. Hold it around a transaction.
//|
//|     For example::
//|
//|       async with spi.lock:
//|           spi.configure(baudrate=8_000_000)
//|           await spi.write(command)
//|           await spi.readinto(reply)
//|     """
//|
static mp_obj_t async_spi_spi_get_lock(mp_obj_t self_in) {
    async_spi_spi_obj_t *self = checked_spi(self_in);
    return MP_OBJ_FROM_PTR(common_hal_async_spi_spi_get_lock(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(async_spi_spi_get_lock_obj, async_spi_spi_get_lock);

MP_PROPERTY_GETTER(async_spi_spi_lock_obj,
    (mp_obj_t)&async_spi_spi_get_lock_obj);

static const mp_rom_map_elem_t async_spi_spi_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&async_spi_spi_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&async_spi_spi_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&default___exit___obj) },

    { MP_ROM_QSTR(MP_QSTR_configure), MP_ROM_PTR(&async_spi_spi_configure_obj) },

    { MP_ROM_QSTR(MP_QSTR_write), MP_ROM_PTR(&async_spi_spi_write_obj) },
    { MP_ROM_QSTR(MP_QSTR_readinto), MP_ROM_PTR(&async_spi_spi_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_write_readinto), MP_ROM_PTR(&async_spi_spi_write_readinto_obj) },

    { MP_ROM_QSTR(MP_QSTR_frequency), MP_ROM_PTR(&async_spi_spi_frequency_obj) },
    { MP_ROM_QSTR(MP_QSTR_lock), MP_ROM_PTR(&async_spi_spi_lock_obj) },
};
static MP_DEFINE_CONST_DICT(async_spi_spi_locals_dict, async_spi_spi_locals_dict_table);

MP_DEFINE_CONST_OBJ_TYPE(
    async_spi_spi_type,
    MP_QSTR_SPI,
    MP_TYPE_FLAG_HAS_SPECIAL_ACCESSORS,
    make_new, async_spi_spi_make_new,
    locals_dict, &async_spi_spi_locals_dict
    );
