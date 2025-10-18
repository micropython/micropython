/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013-2019 Damien P. George
 * Copyright (c) 2014-2015 Paul Sokolovsky
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
#include <string.h>
#include <assert.h>

#include "py/bc.h"
#include "py/objmodule.h"
#include "py/runtime.h"
#include "py/builtin.h"

static void module_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_module_t *self = MP_OBJ_TO_PTR(self_in);

    const char *module_name = "";
    mp_map_elem_t *elem = mp_map_lookup(&self->globals->map, MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_MAP_LOOKUP);
    if (elem != NULL) {
        module_name = mp_obj_str_get_str(elem->value);
    }

    #if MICROPY_MODULE___FILE__
    // If we store __file__ to imported modules then try to lookup this
    // symbol to give more information about the module.
    elem = mp_map_lookup(&self->globals->map, MP_OBJ_NEW_QSTR(MP_QSTR___file__), MP_MAP_LOOKUP);
    if (elem != NULL) {
        mp_printf(print, "<module '%s' from '%s'>", module_name, mp_obj_str_get_str(elem->value));
        return;
    }
    #endif

    mp_printf(print, "<module '%s'>", module_name);
}

static void module_attr_try_delegation(mp_obj_t self_in, qstr attr, mp_obj_t *dest);

static void module_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    mp_obj_module_t *self = MP_OBJ_TO_PTR(self_in);
    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        mp_map_elem_t *elem = mp_map_lookup(&self->globals->map, MP_OBJ_NEW_QSTR(attr), MP_MAP_LOOKUP);
        if (elem != NULL) {
            dest[0] = elem->value;
        #if MICROPY_CPYTHON_COMPAT
        } else if (attr == MP_QSTR___dict__) {
            dest[0] = MP_OBJ_FROM_PTR(self->globals);
        #endif
        #if MICROPY_MODULE_GETATTR
        } else if (attr != MP_QSTR___getattr__) {
            elem = mp_map_lookup(&self->globals->map, MP_OBJ_NEW_QSTR(MP_QSTR___getattr__), MP_MAP_LOOKUP);
            if (elem != NULL) {
                dest[0] = mp_call_function_1(elem->value, MP_OBJ_NEW_QSTR(attr));
            } else {
                module_attr_try_delegation(self_in, attr, dest);
            }
        #endif
        } else {
            module_attr_try_delegation(self_in, attr, dest);
        }
    } else {
        // delete/store attribute
        mp_obj_dict_t *dict = self->globals;
        if (dict->map.is_fixed) {
            #if MICROPY_CAN_OVERRIDE_BUILTINS
            if (dict == &mp_module_builtins_globals) {
                if (MP_STATE_VM(mp_module_builtins_override_dict) == NULL) {
                    MP_STATE_VM(mp_module_builtins_override_dict) = MP_OBJ_TO_PTR(mp_obj_new_dict(1));
                }
                dict = MP_STATE_VM(mp_module_builtins_override_dict);
            } else
            #endif
            {
                // can't delete or store to fixed map
                module_attr_try_delegation(self_in, attr, dest);
                return;
            }
        }
        if (dest[1] == MP_OBJ_NULL) {
            // delete attribute
            mp_obj_dict_delete(MP_OBJ_FROM_PTR(dict), MP_OBJ_NEW_QSTR(attr));
        } else {
            // store attribute
            mp_obj_dict_store(MP_OBJ_FROM_PTR(dict), MP_OBJ_NEW_QSTR(attr), dest[1]);
        }
        dest[0] = MP_OBJ_NULL; // indicate success
    }
}

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_module,
    MP_QSTR_module,
    MP_TYPE_FLAG_NONE,
    print, module_print,
    attr, module_attr
    );

mp_obj_t mp_obj_new_module(qstr module_name) {
    mp_map_t *mp_loaded_modules_map = &MP_STATE_VM(mp_loaded_modules_dict).map;
    mp_map_elem_t *el = mp_map_lookup(mp_loaded_modules_map, MP_OBJ_NEW_QSTR(module_name), MP_MAP_LOOKUP_ADD_IF_NOT_FOUND);
    // We could error out if module already exists, but let C extensions
    // add new members to existing modules.
    if (el->value != MP_OBJ_NULL) {
        return el->value;
    }

    // create new module object
    mp_module_context_t *o = m_new_obj(mp_module_context_t);
    o->module.base.type = &mp_type_module;
    o->module.globals = MP_OBJ_TO_PTR(mp_obj_new_dict(MICROPY_MODULE_DICT_SIZE));

    // store __name__ entry in the module
    mp_obj_dict_store(MP_OBJ_FROM_PTR(o->module.globals), MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(module_name));

    // store the new module into the slot in the global dict holding all modules
    el->value = MP_OBJ_FROM_PTR(o);

    // return the new module
    return MP_OBJ_FROM_PTR(o);
}

/******************************************************************************/
// Global module table and related functions

static const mp_rom_map_elem_t mp_builtin_module_table[] = {
    // built-in modules declared with MP_REGISTER_MODULE()
    MICROPY_REGISTERED_MODULES
};
MP_DEFINE_CONST_MAP(mp_builtin_module_map, mp_builtin_module_table);

#if MICROPY_HAVE_REGISTERED_EXTENSIBLE_MODULES
static const mp_rom_map_elem_t mp_builtin_extensible_module_table[] = {
    // built-in modules declared with MP_REGISTER_EXTENSIBLE_MODULE()
    MICROPY_REGISTERED_EXTENSIBLE_MODULES
};
MP_DEFINE_CONST_MAP(mp_builtin_extensible_module_map, mp_builtin_extensible_module_table);
#endif

#if MICROPY_MODULE_ATTR_DELEGATION && defined(MICROPY_MODULE_DELEGATIONS)
typedef struct _mp_module_delegation_entry_t {
    mp_rom_obj_t mod;
    mp_attr_fun_t fun;
} mp_module_delegation_entry_t;

static const mp_module_delegation_entry_t mp_builtin_module_delegation_table[] = {
    // delegation entries declared with MP_REGISTER_MODULE_DELEGATION()
    MICROPY_MODULE_DELEGATIONS
};
#endif

// Attempts to find (and initialise) a built-in, otherwise returns
// MP_OBJ_NULL.
mp_obj_t mp_module_get_builtin(qstr module_name, bool extensible) {
    #if MICROPY_HAVE_REGISTERED_EXTENSIBLE_MODULES
    const mp_map_t *map = extensible ? &mp_builtin_extensible_module_map : &mp_builtin_module_map;
    #else
    const mp_map_t *map = &mp_builtin_module_map;
    #endif
    mp_map_elem_t *elem = mp_map_lookup((mp_map_t *)map, MP_OBJ_NEW_QSTR(module_name), MP_MAP_LOOKUP);

    if (!elem) {
        #if MICROPY_PY_SYS
        // Special case for sys, which isn't extensible but can always be
        // imported with the alias `usys`.
        if (module_name == MP_QSTR_usys) {
            return MP_OBJ_FROM_PTR(&mp_module_sys);
        }
        #endif

        #if MICROPY_HAVE_REGISTERED_EXTENSIBLE_MODULES
        if (extensible) {
            // At this point we've already tried non-extensible built-ins, the
            // filesystem, and now extensible built-ins. No match, so fail
            // the import.
            return MP_OBJ_NULL;
        }

        // We're trying to match a non-extensible built-in (i.e. before trying
        // the filesystem), but if the user is importing `ufoo`, _and_ `foo`
        // is an extensible module, then allow it as a way of forcing the
        // built-in. Essentially, this makes it as if all the extensible
        // built-ins also had non-extensible aliases named `ufoo`. Newer code
        // should be using sys.path to force the built-in, but this retains
        // the old behaviour of the u-prefix being used to force a built-in
        // import.
        size_t module_name_len;
        const char *module_name_str = (const char *)qstr_data(module_name, &module_name_len);
        if (module_name_str[0] != 'u') {
            return MP_OBJ_NULL;
        }
        elem = mp_map_lookup((mp_map_t *)&mp_builtin_extensible_module_map, MP_OBJ_NEW_QSTR(qstr_find_strn(module_name_str + 1, module_name_len - 1)), MP_MAP_LOOKUP);
        #endif

        if (!elem) {
            return MP_OBJ_NULL;
        }
    }

    #if MICROPY_MODULE_BUILTIN_INIT
    // If found, it's a newly loaded built-in, so init it. This can run
    // multiple times, so the module must ensure that it handles being
    // initialised multiple times.
    mp_obj_t dest[2];
    mp_load_method_maybe(elem->value, MP_QSTR___init__, dest);
    if (dest[0] != MP_OBJ_NULL) {
        mp_call_method_n_kw(0, 0, dest);
    }
    #endif

    return elem->value;
}

static void module_attr_try_delegation(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    #if MICROPY_MODULE_ATTR_DELEGATION && defined(MICROPY_MODULE_DELEGATIONS)
    // Delegate lookup to a module's custom attr method.
    size_t n = MP_ARRAY_SIZE(mp_builtin_module_delegation_table);
    for (size_t i = 0; i < n; ++i) {
        if (*(mp_obj_t *)(&mp_builtin_module_delegation_table[i].mod) == self_in) {
            mp_builtin_module_delegation_table[i].fun(self_in, attr, dest);
            break;
        }
    }
    #else
    (void)self_in;
    (void)attr;
    (void)dest;
    #endif
}

void mp_module_generic_attr(qstr attr, mp_obj_t *dest, const uint16_t *keys, mp_obj_t *values) {
    for (size_t i = 0; keys[i] != MP_QSTRnull; ++i) {
        if (attr == keys[i]) {
            if (dest[0] == MP_OBJ_NULL) {
                // load attribute (MP_OBJ_NULL returned for deleted items)
                dest[0] = values[i];
            } else {
                // delete or store (delete stores MP_OBJ_NULL)
                values[i] = dest[1];
                dest[0] = MP_OBJ_NULL; // indicate success
            }
            return;
        }
    }
}
