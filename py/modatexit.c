/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2024 Andrew Leech
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

#include <assert.h>
#include <string.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "py/modatexit.h"
#include "shared/runtime/pyexec.h"

#if MICROPY_PY_ATEXIT

mp_obj_t mp_atexit_register(mp_obj_t function) {
    if (!mp_obj_is_callable(function)) {
        mp_raise_ValueError(MP_ERROR_TEXT("function not callable"));
    }
    if (MP_STATE_VM(atexit) == NULL) {
        MP_STATE_VM(atexit) = mp_obj_new_list(0, mp_const_none);
    }
    mp_obj_list_append(MP_STATE_VM(atexit), function);
    // return the passed in function so this can be used as a decorator
    return function;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mp_atexit_register_obj, mp_atexit_register);

#if MICROPY_PY_ATEXIT_UNREGISTER
mp_obj_t mp_atexit_unregister(mp_obj_t function) {
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        while (MP_STATE_VM(atexit) != NULL) {
            mp_obj_list_remove(MP_STATE_VM(atexit), function);
        }
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(mp_atexit_unregister_obj, mp_atexit_unregister);
#endif

static const mp_rom_map_elem_t mp_module_atexit_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_atexit) },
    { MP_ROM_QSTR(MP_QSTR_register), MP_ROM_PTR(&mp_atexit_register_obj) },
    #if MICROPY_PY_ATEXIT_UNREGISTER
    { MP_ROM_QSTR(MP_QSTR_unregister), MP_ROM_PTR(&mp_atexit_unregister_obj) },
    #endif
};
static MP_DEFINE_CONST_DICT(mp_module_atexit_globals, mp_module_atexit_globals_table);

const mp_obj_module_t mp_module_atexit = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_atexit_globals,
};

MP_REGISTER_ROOT_POINTER(mp_obj_list_t * atexit);
MP_REGISTER_MODULE(MP_QSTR_atexit, mp_module_atexit);

int mp_atexit_execute(void) {
    int exit_code = 0;
    if (MP_STATE_VM(atexit) != NULL) {
        mp_obj_list_t *list = MP_STATE_VM(atexit);
        for (size_t i = 0; i < list->len; i++) {
            mp_obj_t function = list->items[i];

            nlr_buf_t nlr;
            if (nlr_push(&nlr) == 0) {
                mp_call_function_0(function);
            } else {
                exit_code = pyexec_handle_uncaught_exception(nlr.ret_val);
            }
        }
    }
    return exit_code;
}

#endif // MICROPY_PY_ATEXIT
