/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 microDev
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

#include "shared-bindings/util.h"
#include "shared-bindings/coproc/__init__.h"
#include "shared-bindings/coproc/Coproc.h"
#include "shared-bindings/coproc/CoprocMemory.h"

#include "py/runtime.h"

//| """COPROC Module
//|
//| The `coproc` module adds ability to load and run
//| programs on a co-processor or a different cpu core.
//|
//| .. code-block:: python
//|
//|     import coproc
//|
//|     shared_mem = coproc.CoprocMemory(address=0x500007fc, length=1024)
//|
//|     with open("program.bin", "rb") as f:
//|         program = coproc.Coproc(buffer=f.read(), memory=shared_mem)
//|
//|     coproc.run(program)
//|     print(coproc.memory(program)[0])
//|     # coproc.halt(program)
//| """
//| ...
//|

STATIC coproc_coproc_obj_t *get_coproc_obj(mp_obj_t *self_in) {
    if (!mp_obj_is_type(*self_in, &coproc_coproc_type)) {
        mp_raise_TypeError_varg(translate("Expected a %q"), MP_QSTR_Coproc);
    }
    coproc_coproc_obj_t *self = MP_OBJ_TO_PTR(*self_in);
    if (common_hal_coproc_coproc_deinited(self)) {
        raise_deinited_error();
    }
    return self;
}

//| def run(*coproc: Coproc) -> None:
//|     """Runs the loaded program."""
//|     ...
//|
STATIC mp_obj_t coproc_run(mp_obj_t self_in) {
    common_hal_coproc_run(get_coproc_obj(&self_in));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(coproc_run_obj, coproc_run);

//| def halt(*coproc: Coproc) -> None:
//|     """Halts the loaded program."""
//|     ...
//|
STATIC mp_obj_t coproc_halt(mp_obj_t self_in) {
    common_hal_coproc_halt(get_coproc_obj(&self_in));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(coproc_halt_obj, coproc_halt);

//| def memory(*coproc: Coproc) -> CoprocMemory:
//|     """Returns the shared memory as a bytearray."""
//|     ...
//|
STATIC mp_obj_t coproc_memory(mp_obj_t self_in) {
    return common_hal_coproc_memory(get_coproc_obj(&self_in));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(coproc_memory_obj, coproc_memory);

STATIC const mp_rom_map_elem_t coproc_module_globals_table[] = {
    // module name
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_coproc) },

    // module functions
    { MP_ROM_QSTR(MP_QSTR_run), MP_ROM_PTR(&coproc_run_obj) },
    { MP_ROM_QSTR(MP_QSTR_halt), MP_ROM_PTR(&coproc_halt_obj) },
    { MP_ROM_QSTR(MP_QSTR_memory), MP_ROM_PTR(&coproc_memory_obj) },

    // module classes
    { MP_ROM_QSTR(MP_QSTR_Coproc), MP_OBJ_FROM_PTR(&coproc_coproc_type) },
    { MP_ROM_QSTR(MP_QSTR_CoprocMemory), MP_OBJ_FROM_PTR(&coproc_memory_type) },
};
STATIC MP_DEFINE_CONST_DICT(coproc_module_globals, coproc_module_globals_table);

const mp_obj_module_t coproc_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&coproc_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_coproc, coproc_module, CIRCUITPY_COPROC);
