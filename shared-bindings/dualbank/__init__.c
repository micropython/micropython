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

#include "shared-bindings/dualbank/__init__.h"

#if CIRCUITPY_STORAGE_EXTEND
#include "supervisor/flash.h"
#endif

//| """Dualbank Module
//|
//| The `dualbank` module adds ability to update and switch between the
//| two identical app partitions, which can contain different firmware versions.
//|
//| Having two partitions enables rollback functionality.
//|
//| The two partitions are defined as the boot partition and the next-update partition.
//| Calling `dualbank.flash()` writes the next-update partition.
//|
//| After the next-update partition is written a validation check is performed
//| and on a successful validation this partition is set as the boot partition.
//| On next reset, firmware will be loaded from this partition.
//|
//| Use cases:
//|     * Can be used for ``OTA`` Over-The-Air updates.
//|     * Can be used for ``dual-boot`` of different firmware versions or platforms.
//|
//| .. note::
//|
//|     Boards with flash ``=2MB``:
//|         This module is unavailable as the flash is only large enough for one app partition.
//|
//|     Boards with flash ``>2MB``:
//|         This module is enabled/disabled at runtime based on whether the ``CIRCUITPY`` drive
//|         is extended or not. See `storage.erase_filesystem()` for more information.
//|
//| .. code-block:: python
//|
//|     import dualbank
//|
//|     dualbank.flash(buffer, offset)
//|     dualbank.switch()
//| """
//| ...
//|

#if CIRCUITPY_STORAGE_EXTEND
STATIC void raise_error_if_storage_extended(void) {
    if (supervisor_flash_get_extended()) {
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("%q is %q"), MP_QSTR_storage, MP_QSTR_extended);
    }
}
#endif

//| def flash(buffer: ReadableBuffer, offset: int = 0) -> None:
//|     """Writes one of the two app partitions at the given offset.
//|
//|     This can be called multiple times when flashing the firmware in smaller chunks.
//|
//|     :param ReadableBuffer buffer: The entire firmware or a partial chunk.
//|     :param int offset: Start writing at this offset in the app partition.
//|     """
//|     ...
//|
STATIC mp_obj_t dualbank_flash(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_buffer, ARG_offset };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_buffer, MP_ARG_OBJ | MP_ARG_REQUIRED },
        { MP_QSTR_offset, MP_ARG_INT | MP_ARG_KW_ONLY, {.u_int = 0} },
    };

    #if CIRCUITPY_STORAGE_EXTEND
    raise_error_if_storage_extended();
    #endif

    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    if (args[ARG_offset].u_int < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("offset must be >= 0"));
    }

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[ARG_buffer].u_obj, &bufinfo, MP_BUFFER_READ);

    common_hal_dualbank_flash(bufinfo.buf, bufinfo.len, args[ARG_offset].u_int);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(dualbank_flash_obj, 0, dualbank_flash);

//| def switch() -> None:
//|     """Switches to the next-update partition.
//|
//|     On next reset, firmware will be loaded from the partition just switched over to.
//|     """
//|     ...
//|
STATIC mp_obj_t dualbank_switch(void) {
    #if CIRCUITPY_STORAGE_EXTEND
    raise_error_if_storage_extended();
    #endif
    common_hal_dualbank_switch();
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(dualbank_switch_obj, dualbank_switch);

STATIC const mp_rom_map_elem_t dualbank_module_globals_table[] = {
    // module name
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_dualbank) },
    // module functions
    { MP_ROM_QSTR(MP_QSTR_flash), MP_ROM_PTR(&dualbank_flash_obj) },
    { MP_ROM_QSTR(MP_QSTR_switch), MP_ROM_PTR(&dualbank_switch_obj) },
};
STATIC MP_DEFINE_CONST_DICT(dualbank_module_globals, dualbank_module_globals_table);

const mp_obj_module_t dualbank_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&dualbank_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_dualbank, dualbank_module);
