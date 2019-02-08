/*
 * This file is part of the MicroPython project, http://micropython.org/
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

#include "py/objmodule.h"
#include "py/runtime.h"
#include "py/builtin.h"

#include "genhdr/moduledefs.h"

STATIC void module_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind) {
    (void)kind;
    mp_obj_module_t *self = MP_OBJ_TO_PTR(self_in);

    const char *module_name = "";
    mp_map_elem_t *elem = mp_map_lookup(&self->globals->map, MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_MAP_LOOKUP);
    if (elem != NULL) {
        module_name = mp_obj_str_get_str(elem->value);
    }

#if MICROPY_PY___FILE__
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

STATIC void module_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    mp_obj_module_t *self = MP_OBJ_TO_PTR(self_in);
    if (dest[0] == MP_OBJ_NULL) {
        // load attribute
        mp_map_elem_t *elem = mp_map_lookup(&self->globals->map, MP_OBJ_NEW_QSTR(attr), MP_MAP_LOOKUP);
        if (elem != NULL) {
            dest[0] = elem->value;
        #if MICROPY_MODULE_GETATTR
        } else if (attr != MP_QSTR___getattr__) {
            elem = mp_map_lookup(&self->globals->map, MP_OBJ_NEW_QSTR(MP_QSTR___getattr__), MP_MAP_LOOKUP);
            if (elem != NULL) {
                dest[0] = mp_call_function_1(elem->value, MP_OBJ_NEW_QSTR(attr));
            }
        #endif
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

const mp_obj_type_t mp_type_module = {
    { &mp_type_type },
    .name = MP_QSTR_module,
    .print = module_print,
    .attr = module_attr,
};

mp_obj_t mp_obj_new_module(qstr module_name) {
    mp_map_t *mp_loaded_modules_map = &MP_STATE_VM(mp_loaded_modules_dict).map;
    mp_map_elem_t *el = mp_map_lookup(mp_loaded_modules_map, MP_OBJ_NEW_QSTR(module_name), MP_MAP_LOOKUP_ADD_IF_NOT_FOUND);
    // We could error out if module already exists, but let C extensions
    // add new members to existing modules.
    if (el->value != MP_OBJ_NULL) {
        return el->value;
    }

    // create new module object
    mp_obj_module_t *o = m_new_obj(mp_obj_module_t);
    o->base.type = &mp_type_module;
    o->globals = MP_OBJ_TO_PTR(mp_obj_new_dict(MICROPY_MODULE_DICT_SIZE));

    // store __name__ entry in the module
    mp_obj_dict_store(MP_OBJ_FROM_PTR(o->globals), MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(module_name));

    // store the new module into the slot in the global dict holding all modules
    el->value = MP_OBJ_FROM_PTR(o);

    // return the new module
    return MP_OBJ_FROM_PTR(o);
}

/******************************************************************************/
// Global module table and related functions

#if MICROPY_PY_IO
#define MODULE_UIO MP_BUILTIN_MODULE(uio),
#else
#define MODULE_UIO
#endif

#if MICROPY_PY_COLLECTIONS
#define MODULE_UCOLLECTIONS MP_BUILTIN_MODULE(ucollections),
#else
#define MODULE_UCOLLECTIONS
#endif

#if MICROPY_PY_STRUCT
#define MODULE_USTRUCT MP_BUILTIN_MODULE(ustruct),
#else
#define MODULE_USTRUCT
#endif

#if MICROPY_PY_BUILTINS_FLOAT
#if MICROPY_PY_MATH
#define MODULE_MATH MP_BUILTIN_MODULE(math),
#endif
#if MICROPY_PY_BUILTINS_COMPLEX && MICROPY_PY_CMATH
#define MODULE_CMATH MP_BUILTIN_MODULE(cmath),
#endif
#endif

#ifndef MODULE_MATH
#define MODULE_MATH
#endif

#ifndef MODULE_CMATH
#define MODULE_CMATH
#endif

#if MICROPY_PY_SYS
#define MODULE_SYS MP_BUILTIN_MODULE(sys),
#else
#define MODULE_SYS
#endif

#if MICROPY_PY_GC && MICROPY_ENABLE_GC
#define MODULE_GC MP_BUILTIN_MODULE(gc),
#else
#define MODULE_GC
#endif

#if MICROPY_PY_THREAD
#define MODULE__THERAD MP_BUILTIN_MODULE(_thread),
#else
#define MODULE__THERAD
#endif

// extmod modules

#if MICROPY_PY_UERRNO
#define MODULE_ERRNO MP_BUILTIN_MODULE(uerrno),
#else
#define MODULE_ERRNO
#endif

#if MICROPY_PY_UCTYPES
#define MODULE_CTYPES MP_BUILTIN_MODULE(uctypes),
#else
#define MODULE_CTYPES
#endif

#if MICROPY_PY_UZLIB
#define MODULE_UZLIB MP_BUILTIN_MODULE(uzlib),
#else
#define MODULE_UZLIB
#endif

#if MICROPY_PY_UJSON
#define MODULE_UJSON MP_BUILTIN_MODULE(ujson),
#else
#define MODULE_UJSON
#endif

#if MICROPY_PY_URE
#define MODULE_URE MP_BUILTIN_MODULE(ure),
#else
#define MODULE_URE
#endif

#if MICROPY_PY_UHEAPQ
#define MODULE_UHEAPQ MP_BUILTIN_MODULE(uheapq),
#else
#define MODULE_UHEAPQ
#endif

#if MICROPY_PY_UTIMEQ
#define MODULE_UTIMEQ MP_BUILTIN_MODULE(utimeq),
#else
#define MODULE_UTIMEQ
#endif

#if MICROPY_PY_UHASHLIB
#define MODULE_UHASHLIB MP_BUILTIN_MODULE(uhashlib),
#else
#define MODULE_UHASHLIB
#endif

#if MICROPY_PY_UCRYPTOLIB
#define MODULE_UCRYPTOLIB MP_BUILTIN_MODULE(ucryptolib),
#else
#define MODULE_UCRYPTOLIB
#endif

#if MICROPY_PY_UBINASCII
#define MODULE_UBINASCII MP_BUILTIN_MODULE(ubinascii),
#else
#define MODULE_UBINASCII
#endif

#if MICROPY_PY_URANDOM
#define MODULE_URANDOM MP_BUILTIN_MODULE(urandom),
#else
#define MODULE_URANDOM
#endif

#if MICROPY_PY_USELECT
#define MODULE_USELECT MP_BUILTIN_MODULE(uselect),
#else
#define MODULE_USELECT
#endif

#if MICROPY_PY_USSL
#define MODULE_USSL MP_BUILTIN_MODULE(ussl),
#else
#define MODULE_USSL
#endif

#if MICROPY_PY_LWIP
#define MODULE_LWIP MP_BUILTIN_MODULE(lwip),
#else
#define MODULE_LWIP
#endif

#if MICROPY_PY_UWEBSOCKET
#define MODULE_UWEBSOCKET MP_BUILTIN_MODULE(uwebsocket),
#else
#define MODULE_UWEBSOCKET
#endif

#if MICROPY_PY_WEBREPL
#define MODULE__WEBREPL MP_BUILTIN_MODULE(_webrepl),
#else
#define MODULE__WEBREPL
#endif

#if MICROPY_PY_FRAMEBUF
#define MODULE_FRAMEBUF MP_BUILTIN_MODULE(framebuf),
#else
#define MODULE_FRAMEBUF
#endif

#if MICROPY_PY_BTREE
#define MODULE_BTREE MP_BUILTIN_MODULE(btree),
#else
#define MODULE_BTREE
#endif


#ifdef MICROPY_REGISTERED_MODULES
// builtin modules declared with MP_REGISTER_MODULE()
#define REGISTERED_MODULES MICROPY_REGISTERED_MODULES
#else
#define REGISTERED_MODULES
#endif

#define BUILTIN_MODULES \
    MP_BUILTIN_MODULE(__main__), \
    MP_BUILTIN_MODULE(builtins), \
    MP_BUILTIN_MODULE(micropython), \
    MODULE_UIO \
    MODULE_UCOLLECTIONS \
    MODULE_USTRUCT \
    MODULE_MATH \
    MODULE_CMATH \
    MODULE_SYS \
    MODULE_GC \
    MODULE__THERAD \
    MODULE_ERRNO \
    MODULE_CTYPES \
    MODULE_UZLIB \
    MODULE_UJSON \
    MODULE_URE \
    MODULE_UHEAPQ \
    MODULE_UTIMEQ \
    MODULE_UHASHLIB \
    MODULE_UCRYPTOLIB \
    MODULE_UBINASCII \
    MODULE_URANDOM \
    MODULE_USELECT \
    MODULE_USSL \
    MODULE_LWIP \
    MODULE_UWEBSOCKET \
    MODULE__WEBREPL \
    MODULE_FRAMEBUF \
    MODULE_BTREE \

#define ALL_BUILTIN_MODULES \
    BUILTIN_MODULES \
    REGISTERED_MODULES \
    MICROPY_PORT_BUILTIN_MODULES // extra builtin modules as defined by a port

STATIC const mp_rom_obj_t mp_builtin_modules[] = {
#define MP_BUILTIN_MODULE(name) MP_ROM_PTR(&mp_module_##name)
#define MP_BUILTIN_MODULE2(name, objname) MP_ROM_PTR(&mp_module_##objname)
    ALL_BUILTIN_MODULES
#undef MP_BUILTIN_MODULE
#undef MP_BUILTIN_MODULE2
};

#define MP_BUILTIN_MODULE(name) MP_ROM_QSTR(MP_QSTR_##name)
#define MP_BUILTIN_MODULE2(name, objname) MP_ROM_QSTR(MP_QSTR_##name)


#if MICROPY_PY_BUILTINS_HELP && MICROPY_PY_BUILTINS_HELP_MODULES
const size_t mp_builtin_module_names_len = MP_ARRAY_SIZE(mp_builtin_modules);
#endif

const mp_rom_obj_t mp_builtin_module_names[] = {
    ALL_BUILTIN_MODULES
};

#undef MP_BUILTIN_MODULE
#undef MP_BUILTIN_MODULE2

#if MICROPY_MODULE_WEAK_LINKS
STATIC const mp_rom_map_elem_t mp_builtin_module_weak_links_table[] = {
    MICROPY_PORT_BUILTIN_MODULE_WEAK_LINKS
};

MP_DEFINE_CONST_MAP(mp_builtin_module_weak_links_map, mp_builtin_module_weak_links_table);
#endif

// returns MP_OBJ_NULL if not found
mp_obj_t mp_module_get(qstr module_name) {
    mp_map_t *mp_loaded_modules_map = &MP_STATE_VM(mp_loaded_modules_dict).map;
    const mp_obj_t module_name_obj = MP_OBJ_NEW_QSTR(module_name);
    // lookup module
    mp_map_elem_t *el = mp_map_lookup(mp_loaded_modules_map, module_name_obj, MP_MAP_LOOKUP);

    // module found, return it
    if (el != NULL) {
        return el->value;
    }

    // module not found, look for builtin module names

    // nicer accessors to prevent repetitive castings from `mp_rom_obj_t`.
    mp_obj_t *names = (mp_obj_t*)mp_builtin_module_names;
    mp_obj_t *modules = (mp_obj_t*)mp_builtin_modules;

    for (size_t i = 0; i < MP_ARRAY_SIZE(mp_builtin_modules); ++i) {
        if (module_name_obj == names[i]) {
            mp_module_call_init(module_name, modules[i]);
            return modules[i];
        }
    }

    return MP_OBJ_NULL;
}

void mp_module_register(qstr qst, mp_obj_t module) {
    mp_map_t *mp_loaded_modules_map = &MP_STATE_VM(mp_loaded_modules_dict).map;
    mp_map_lookup(mp_loaded_modules_map, MP_OBJ_NEW_QSTR(qst), MP_MAP_LOOKUP_ADD_IF_NOT_FOUND)->value = module;
}

#if MICROPY_MODULE_BUILTIN_INIT
void mp_module_call_init(qstr module_name, mp_obj_t module_obj) {
    // Look for __init__ and call it if it exists
    mp_obj_t dest[2];
    mp_load_method_maybe(module_obj, MP_QSTR___init__, dest);
    if (dest[0] != MP_OBJ_NULL) {
        mp_call_method_n_kw(0, 0, dest);
        // Register module so __init__ is not called again.
        // If a module can be referenced by more than one name (eg due to weak links)
        // then __init__ will still be called for each distinct import, and it's then
        // up to the particular module to make sure it's __init__ code only runs once.
        mp_module_register(module_name, module_obj);
    }
}
#endif
