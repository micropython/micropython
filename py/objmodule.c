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

#include "mpconfig.h"
#include "nlr.h"
#include "misc.h"
#include "qstr.h"
#include "obj.h"
#include "objmodule.h"
#include "runtime.h"
#include "builtintables.h"

STATIC mp_map_t mp_loaded_modules_map; // TODO: expose as sys.modules

STATIC void module_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
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
    if (value == MP_OBJ_NULL) {
        // delete attribute
        mp_obj_dict_delete(self->globals, MP_OBJ_NEW_QSTR(attr));
    } else {
        // store attribute
        // TODO CPython allows STORE_ATTR to a module, but is this the correct implementation?
        mp_obj_dict_store(self->globals, MP_OBJ_NEW_QSTR(attr), value);
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
    mp_map_elem_t *el = mp_map_lookup(&mp_loaded_modules_map, MP_OBJ_NEW_QSTR(module_name), MP_MAP_LOOKUP_ADD_IF_NOT_FOUND);
    // We could error out if module already exists, but let C extensions
    // add new members to existing modules.
    if (el->value != MP_OBJ_NULL) {
        return el->value;
    }

    // create new module object
    mp_obj_module_t *o = m_new_obj(mp_obj_module_t);
    o->base.type = &mp_type_module;
    o->name = module_name;
    o->globals = mp_obj_new_dict(1);

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

void mp_module_init(void) {
    mp_map_init(&mp_loaded_modules_map, 3);
}

void mp_module_deinit(void) {
    mp_map_deinit(&mp_loaded_modules_map);
}

// returns MP_OBJ_NULL if not found
mp_obj_t mp_module_get(qstr module_name) {
    // lookup module
    mp_map_elem_t *el = mp_map_lookup(&mp_loaded_modules_map, MP_OBJ_NEW_QSTR(module_name), MP_MAP_LOOKUP);

    if (el == NULL) {
        // module not found, look for builtin module names
        el = mp_map_lookup((mp_map_t*)&mp_builtin_module_dict_obj.map, MP_OBJ_NEW_QSTR(module_name), MP_MAP_LOOKUP);
        if (el == NULL) {
            return MP_OBJ_NULL;
        }
    }

    // module found, return it
    return el->value;
}

void mp_module_register(qstr qstr, mp_obj_t module) {
    mp_map_lookup(&mp_loaded_modules_map, MP_OBJ_NEW_QSTR(qstr), MP_MAP_LOOKUP_ADD_IF_NOT_FOUND)->value = module;
}
