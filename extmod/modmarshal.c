/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Damien P. George
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
 *
 */

#include "py/objcode.h"
#include "py/objfun.h"
#include "py/persistentcode.h"
#include "py/runtime.h"

#if MICROPY_PY_MARSHAL

static mp_obj_t marshal_dumps(mp_obj_t value_in) {
    if (mp_obj_is_type(value_in, &mp_type_code)) {
        mp_obj_code_t *code = MP_OBJ_TO_PTR(value_in);
        const void *proto_fun = mp_code_get_proto_fun(code);
        const uint8_t *bytecode;
        if (mp_proto_fun_is_bytecode(proto_fun)) {
            bytecode = proto_fun;
        } else {
            const mp_raw_code_t *rc = proto_fun;
            if (!(rc->kind == MP_CODE_BYTECODE && rc->children == NULL)) {
                mp_raise_ValueError(MP_ERROR_TEXT("function must be bytecode with no children"));
            }
            bytecode = rc->fun_data;
        }
        return mp_raw_code_save_fun_to_bytes(mp_code_get_constants(code), bytecode);
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("unmarshallable object"));
    }
}
static MP_DEFINE_CONST_FUN_OBJ_1(marshal_dumps_obj, marshal_dumps);

static mp_obj_t marshal_loads(mp_obj_t data_in) {
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data_in, &bufinfo, MP_BUFFER_READ);
    mp_module_context_t ctx;
    ctx.module.globals = mp_globals_get();
    mp_compiled_module_t cm = { .context = &ctx };
    mp_raw_code_load_mem(bufinfo.buf, bufinfo.len, &cm);
    #if MICROPY_PY_BUILTINS_CODE <= MICROPY_PY_BUILTINS_CODE_BASIC
    return mp_obj_new_code(ctx.constants, cm.rc);
    #else
    mp_module_context_t *ctx_ptr = m_new_obj(mp_module_context_t);
    *ctx_ptr = ctx;
    return mp_obj_new_code(ctx_ptr, cm.rc, true);
    #endif
}
static MP_DEFINE_CONST_FUN_OBJ_1(marshal_loads_obj, marshal_loads);

static const mp_rom_map_elem_t mod_marshal_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_marshal) },
    { MP_ROM_QSTR(MP_QSTR_dumps), MP_ROM_PTR(&marshal_dumps_obj) },
    { MP_ROM_QSTR(MP_QSTR_loads), MP_ROM_PTR(&marshal_loads_obj) },
};

static MP_DEFINE_CONST_DICT(mod_marshal_globals, mod_marshal_globals_table);

const mp_obj_module_t mp_module_marshal = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mod_marshal_globals,
};

MP_REGISTER_MODULE(MP_QSTR_marshal, mp_module_marshal);

#endif // MICROPY_PY_MARSHAL
