/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 microDev
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

#include "shared-bindings/ota/__init__.h"

//| """OTA Module
//|
//| The `ota` module implements over-the-air update.
//|
//| There are two identical ota partitions ota_0/1, these
//| contain different firmware versions.
//| Having two partitions enables rollback functionality.
//|
//| The two partitions are defined as boot partition and
//| next-update partition. Calling `ota.flash()` writes the
//| next-update partition.
//|
//| After the next-update partition is written a validation
//| check is performed and on a successful validation this
//| partition is set as the boot partition. On next reset,
//| firmware will be loaded from this partition.
//|
//| Here is the sequence of commands to follow:
//|
//| .. code-block:: python
//|
//|     import ota
//|
//|     ota.flash(buffer, offset)
//|     ota.finish()
//| """
//| ...
//|

//| def switch() -> None:
//|     """Switches the boot partition. On next reset,
//|        firmware will be loaded from the partition
//|        just switched over to."""
//|     ...
//|
STATIC mp_obj_t ota_switch(void) {
    common_hal_ota_switch();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(ota_switch_obj, ota_switch);

//| def finish() -> None:
//|     """Validates flashed firmware, sets next boot partition.
//|         **Must be called** after the firmware has been
//|         completely written into the flash using `ota.flash()`.
//|
//|         This is to be called only once per update."""
//|     ...
//|
STATIC mp_obj_t ota_finish(void) {
    common_hal_ota_finish();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(ota_finish_obj, ota_finish);

//| def flash(*buffer: ReadableBuffer, offset: int=0) -> None:
//|     """Writes one of two OTA partitions at the given offset.
//|
//|     The default offset is 0. It is necessary to provide the
//|     offset when writing in discontinous chunks.
//|
//|     This can be called multiple times when flashing the firmware
//|     in small chunks."""
//|     ...
//|
STATIC mp_obj_t ota_flash(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer, ARG_offset };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_offset, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = -1} },
    };

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_offset].u_int < -1) {
        mp_raise_ValueError(translate("offset must be >= 0"));
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_READ);

    common_hal_ota_flash(bufinfo.buf, bufinfo.len, args[ARG_offset].u_int);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(ota_flash_obj, 0, ota_flash);

STATIC const mp_rom_map_elem_t ota_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ota) },
    { MP_ROM_QSTR(MP_QSTR_switch), MP_ROM_PTR(&ota_switch_obj) },
    { MP_ROM_QSTR(MP_QSTR_finish), MP_ROM_PTR(&ota_finish_obj) },
    { MP_ROM_QSTR(MP_QSTR_flash), MP_ROM_PTR(&ota_flash_obj) },
};
STATIC MP_DEFINE_CONST_DICT(ota_module_globals, ota_module_globals_table);

const mp_obj_module_t ota_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&ota_module_globals,
};
