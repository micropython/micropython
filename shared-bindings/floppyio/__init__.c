// This file is part of the CircuitPython project: https://circuitpython.org
//
// SPDX-FileCopyrightText: Copyright (c) 2022 Jeff Epler for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include "shared-bindings/floppyio/__init__.h"
#if CIRCUITPY_DIGITALIO
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "common-hal/floppyio/__init__.h"
#else
#define FLOPPYIO_SAMPLERATE (24000000)
#endif

#include <math.h>
#include <stdint.h>

#include "py/binary.h"
#include "py/obj.h"
#include "py/runtime.h"

//| def flux_readinto(
//|     buffer: WriteableBuffer,
//|     data: digitalio.DigitalInOut,
//|     index: digitalio.DigitalInOut,
//|     index_wait=0.220,
//| ) -> int:
//|     """Read flux transition information into the buffer.
//|
//|     The function returns when the buffer has filled, or when the index input
//|     indicates that one full revolution of data has been recorded.  Due to
//|     technical limitations, this process may not be interruptible by
//|     KeyboardInterrupt.
//|
//|     :param buffer: Read data into this buffer.  Each element represents the time between successive zero-to-one transitions.
//|     :param data: Pin on which the flux data appears
//|     :param index: Pin on which the index pulse appears
//|     :param index_wait: Time to wait, in seconds, for the index pulse
//|     :return: The actual number of bytes of read
//|     """
//|     ...
//|
static mp_obj_t floppyio_flux_readinto(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    #if CIRCUITPY_DIGITALIO
    enum { ARG_buffer, ARG_data, ARG_index, ARG_index_wait };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_data, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_index, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_index_wait, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_WRITE);

    digitalio_digitalinout_obj_t *data = assert_digitalinout(args[ARG_data].u_obj);
    digitalio_digitalinout_obj_t *index = assert_digitalinout(args[ARG_index].u_obj);

    mp_int_t index_wait_ms = args[ARG_index_wait].u_obj ?
        MICROPY_FLOAT_C_FUN(round)(mp_arg_validate_type_float(args[ARG_index_wait].u_obj, MP_QSTR_index_wait) * 1000) :
        220;

    return MP_OBJ_NEW_SMALL_INT(common_hal_floppyio_flux_readinto(bufinfo.buf, bufinfo.len, data, index, index_wait_ms));
    #else
    mp_raise_NotImplementedError(NULL);
    #endif
}
MP_DEFINE_CONST_FUN_OBJ_KW(floppyio_flux_readinto_obj, 0, floppyio_flux_readinto);

//| def mfm_readinto(
//|     buffer: WriteableBuffer,
//|     flux: ReadableBuffer,
//|     flux_t2_max: int,
//|     flux_t3_max: int,
//|     validity: bytearray | None = None,
//|     clear_validity: bool = True,
//| ) -> int:
//|     """Decode MFM flux into the buffer
//|
//|     The track is assumed to consist of 512-byte sectors.
//|
//|     The function returns the number of sectors successfully read. In addition,
//|     it updates the ``validity`` buffer with information about which sectors
//|     were read.
//|
//|     MFM encoding uses pulses of 3 widths, "T2", "T3" and "T4".
//|     A 1440KiB disk in standard MFM format has "T2" pulses of 2000ns, "T3" pulses of 3000ns,
//|     and "T4" pulses of 4000ns.
//|
//|     Parameters ``t2_max`` and ``t3_max`` are used to distinguish these pulses.
//|     A pulse with ``p <= t2_max`` is a "T2" pulse,
//|     a pulse with ``t2_max < p <= t3_max`` is a "T3" pulse,
//|     and a pulse with ``t3_max < p`` is a "T4" pulse.
//|
//|     The following code can convert a number in nanoseconds to a number of samples
//|     for a given sample rate:
//|
//|     .. code-block:: py
//|
//|         def ns_to_count(ns, samplerate):
//|             return round(ns * samplerate * 1e-9)
//|
//|      This means the following typical values are a good starting place for a 1.44MB floppy:
//|
//|     .. code-block:: py
//|
//|         t2_max = ns_to_count(2500, samplerate) # Mid way between T2 and T3 length
//|         t3_max = ns_to_count(3500, samplerate) # Mid way between T2 and T3 length
//|
//|     :param buffer: Read data into this buffer.  Byte length must be a multiple of 512.
//|     :param flux: Flux data from a previous `flux_readinto` call
//|     :param t2_max: Maximum time of a flux cell in counts.
//|     :param t3_max: Nominal time of a flux cell in counts.
//|     :param validity: Optional bytearray. For each sector successfully read, the corresponding validity entry is set to ``1`` and previously valid sectors are not decoded.
//|     :param clear_validity: If `True`, clear the validity information before decoding and attempt to decode all sectors.
//|     :return: The actual number of sectors read
//|     """
//|     ...
//|
static mp_obj_t floppyio_mfm_readinto(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer, ARG_flux, ARG_t2_max, ARG_t3_max, ARG_validity, ARG_clear_validity };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_flux, MP_ARG_REQUIRED | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_flux_t2_max, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_flux_t3_max, MP_ARG_REQUIRED | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_validity, MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_clear_validity, MP_ARG_BOOL, {.u_bool = true } },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_WRITE);
    if (bufinfo.len % 512 != 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("Buffer must be a multiple of 512 bytes"));
    }
    size_t n_sectors = bufinfo.len / 512;

    mp_buffer_info_t bufinfo_flux;
    mp_get_buffer_raise(args[ARG_flux].u_obj, &bufinfo_flux, MP_BUFFER_READ);

    mp_buffer_info_t bufinfo_validity;
    uint8_t validity_buf[n_sectors];
    if (args[ARG_validity].u_obj) {
        mp_get_buffer_raise(args[ARG_validity].u_obj, &bufinfo_validity, MP_BUFFER_READ);
        mp_arg_validate_length_min(bufinfo_validity.len, n_sectors, MP_QSTR_validity);
        if (args[ARG_clear_validity].u_bool) {
            memset(validity_buf, 0, sizeof(validity_buf));
        }
    } else {
        bufinfo_validity.buf = &validity_buf;
        bufinfo_validity.len = n_sectors;
        memset(validity_buf, 0, sizeof(validity_buf));
    }

    return MP_OBJ_NEW_SMALL_INT(common_hal_floppyio_mfm_readinto(&bufinfo, &bufinfo_flux, bufinfo_validity.buf, args[ARG_t2_max].u_int, args[ARG_t3_max].u_int));
}
MP_DEFINE_CONST_FUN_OBJ_KW(floppyio_mfm_readinto_obj, 0, floppyio_mfm_readinto);

//| samplerate: int
//| """The approximate sample rate in Hz used by flux_readinto."""

static const mp_rom_map_elem_t floppyio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_floppyio) },
    { MP_ROM_QSTR(MP_QSTR_flux_readinto), MP_ROM_PTR(&floppyio_flux_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_mfm_readinto), MP_ROM_PTR(&floppyio_mfm_readinto_obj) },
    { MP_ROM_QSTR(MP_QSTR_samplerate), MP_ROM_INT(FLOPPYIO_SAMPLERATE) },
};
static MP_DEFINE_CONST_DICT(floppyio_module_globals, floppyio_module_globals_table);

const mp_obj_module_t floppyio_module = {
    .base = {&mp_type_module },
    .globals = (mp_obj_dict_t *)&floppyio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_floppyio, floppyio_module);
