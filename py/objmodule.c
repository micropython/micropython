/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
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

#include <stdlib.h>
#include <assert.h>

#include "py/mpstate.h"
#include "py/nlr.h"
#include "py/objmodule.h"
#include "py/runtime.h"
#include "py/builtin.h"

STATIC void module_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_module_t *self = self_in;
    const char *name = qstr_str(self->name);

#if MICROPY_PY___FILE__
    // If we store __file__ to imported modules then try to lookup this
    // symbol to give more information about the module.
    mp_map_elem_t *elem = mp_map_lookup(&self->globals->map, MP_OBJ_NEW_QSTR(MP_QSTR___file__), MP_MAP_LOOKUP);
    if (elem != NULL) {
        print(env, "<module '%s' from '%s'>", name, mp_obj_str_get_str(elem->value));
        return;
    }
#endif

    print(env, "<module '%s'>", name);
}

STATIC void module_load_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    mp_obj_module_t *self = self_in;
    mp_map_elem_t *elem = mp_map_lookup(&self->globals->map, MP_OBJ_NEW_QSTR(attr), MP_MAP_LOOKUP);
    if (elem != NULL) {
        dest[0] = elem->value;
    }
}

STATIC bool module_store_attr(mp_obj_t self_in, qstr attr, mp_obj_t value) {
    mp_obj_module_t *self = self_in;
    mp_obj_dict_t *dict = self->globals;
    if (dict->map.table_is_fixed_array) {
        #if MICROPY_CAN_OVERRIDE_BUILTINS
        if (dict == &mp_module_builtins_globals) {
            if (MP_STATE_VM(mp_module_builtins_override_dict) == NULL) {
                MP_STATE_VM(mp_module_builtins_override_dict) = mp_obj_new_dict(1);
            }
            dict = MP_STATE_VM(mp_module_builtins_override_dict);
        } else
        #endif
        {
            // can't delete or store to fixed map
            return false;
        }
    }
    if (value == MP_OBJ_NULL) {
        // delete attribute
        mp_obj_dict_delete(dict, MP_OBJ_NEW_QSTR(attr));
    } else {
        // store attribute
        // TODO CPython allows STORE_ATTR to a module, but is this the correct implementation?
        mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(attr), value);
    }
    return true;
}

const mp_obj_type_t mp_type_module = {
    { &mp_type_type },
    .name = MP_QSTR_module,
    .print = module_print,
    .load_attr = module_load_attr,
    .store_attr = module_store_attr,
};

mp_obj_t mp_obj_new_module(qstr module_name) {
    mp_map_elem_t *el = mp_map_lookup(&MP_STATE_VM(mp_loaded_modules_map), MP_OBJ_NEW_QSTR(module_name), MP_MAP_LOOKUP_ADD_IF_NOT_FOUND);
    // We could error out if module already exists, but let C extensions
    // add new members to existing modules.
    if (el->value != MP_OBJ_NULL) {
        return el->value;
    }

    // create new module object
    mp_obj_module_t *o = m_new_obj(mp_obj_module_t);
    o->base.type = &mp_type_module;
    o->name = module_name;
    o->globals = mp_obj_new_dict(MICROPY_MODULE_DICT_SIZE);

    // store __name__ entry in the module
    mp_obj_dict_store(o->globals, MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(module_name));

    // store the new module into the slot in the global dict holding all modules
    el->value = o;

    // return the new module
    return o;
}

mp_obj_dict_t *mp_obj_module_get_globals(mp_obj_t self_in) {
    assert(MP_OBJ_IS_TYPE(self_in, &mp_type_module));
    mp_obj_module_t *self = self_in;
    return self->globals;
}

/******************************************************************************/
// Global module table and related functions

STATIC const mp_map_elem_t mp_builtin_module_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___main__), (mp_obj_t)&mp_module___main__ },
    { MP_OBJ_NEW_QSTR(MP_QSTR_builtins), (mp_obj_t)&mp_module_builtins },
    { MP_OBJ_NEW_QSTR(MP_QSTR_micropython), (mp_obj_t)&mp_module_micropython },

#if MICROPY_PY_ARRAY
    { MP_OBJ_NEW_QSTR(MP_QSTR_array), (mp_obj_t)&mp_module_array },
#endif
#if MICROPY_PY_IO
    { MP_OBJ_NEW_QSTR(MP_QSTR__io), (mp_obj_t)&mp_module_io },
#endif
#if MICROPY_PY_COLLECTIONS
    { MP_OBJ_NEW_QSTR(MP_QSTR__collections), (mp_obj_t)&mp_module_collections },
#endif
#if MICROPY_PY_STRUCT
    { MP_OBJ_NEW_QSTR(MP_QSTR_struct), (mp_obj_t)&mp_module_struct },
#endif

#if MICROPY_PY_BUILTINS_FLOAT
#if MICROPY_PY_MATH
    { MP_OBJ_NEW_QSTR(MP_QSTR_math), (mp_obj_t)&mp_module_math },
#endif
#if MICROPY_PY_CMATH
    { MP_OBJ_NEW_QSTR(MP_QSTR_cmath), (mp_obj_t)&mp_module_cmath },
#endif
#endif
#if MICROPY_PY_SYS
    { MP_OBJ_NEW_QSTR(MP_QSTR_sys), (mp_obj_t)&mp_module_sys },
#endif
#if MICROPY_PY_GC && MICROPY_ENABLE_GC
    { MP_OBJ_NEW_QSTR(MP_QSTR_gc), (mp_obj_t)&mp_module_gc },
#endif

    // extmod modules

#if MICROPY_PY_UCTYPES
    { MP_OBJ_NEW_QSTR(MP_QSTR_uctypes), (mp_obj_t)&mp_module_uctypes },
#endif
#if MICROPY_PY_UZLIB
    { MP_OBJ_NEW_QSTR(MP_QSTR_uzlib), (mp_obj_t)&mp_module_uzlib },
#endif
#if MICROPY_PY_UJSON
    { MP_OBJ_NEW_QSTR(MP_QSTR_ujson), (mp_obj_t)&mp_module_ujson },
#endif
#if MICROPY_PY_URE
    { MP_OBJ_NEW_QSTR(MP_QSTR_ure), (mp_obj_t)&mp_module_ure },
#endif
#if MICROPY_PY_UHEAPQ
    { MP_OBJ_NEW_QSTR(MP_QSTR_uheapq), (mp_obj_t)&mp_module_uheapq },
#endif
#if MICROPY_PY_UHASHLIB
    { MP_OBJ_NEW_QSTR(MP_QSTR_uhashlib), (mp_obj_t)&mp_module_uhashlib },
#endif
#if MICROPY_PY_UBINASCII
    { MP_OBJ_NEW_QSTR(MP_QSTR_ubinascii), (mp_obj_t)&mp_module_ubinascii },
#endif

    // extra builtin modules as defined by a port
    MICROPY_PORT_BUILTIN_MODULES
};

STATIC MP_DEFINE_CONST_MAP(mp_builtin_module_map, mp_builtin_module_table);

void mp_module_init(void) {
    mp_map_init(&MP_STATE_VM(mp_loaded_modules_map), 3);
}

void mp_module_deinit(void) {
    mp_map_deinit(&MP_STATE_VM(mp_loaded_modules_map));
}

// returns MP_OBJ_NULL if not found
mp_obj_t mp_module_get(qstr module_name) {
    // lookup module
    mp_map_elem_t *el = mp_map_lookup(&MP_STATE_VM(mp_loaded_modules_map), MP_OBJ_NEW_QSTR(module_name), MP_MAP_LOOKUP);

    if (el == NULL) {
        // module not found, look for builtin module names
        el = mp_map_lookup((mp_map_t*)&mp_builtin_module_map, MP_OBJ_NEW_QSTR(module_name), MP_MAP_LOOKUP);
        if (el == NULL) {
            return MP_OBJ_NULL;
        }
    }

    // module found, return it
    return el->value;
}

void mp_module_register(qstr qst, mp_obj_t module) {
    mp_map_lookup(&MP_STATE_VM(mp_loaded_modules_map), MP_OBJ_NEW_QSTR(qst), MP_MAP_LOOKUP_ADD_IF_NOT_FOUND)->value = module;
}
