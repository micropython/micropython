/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2025 Koudai Aono
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

#include "py/runtime.h"
#include "py/objmodule.h"

#if MICROPY_PY_TSTRINGS

// Function to register string.templatelib in sys.modules
static void register_string_templatelib(void) {
    mp_obj_t sys_module = mp_module_get_builtin(MP_QSTR_sys, false);
    if (sys_module != MP_OBJ_NULL) {
        mp_obj_t modules_dict = mp_load_attr(sys_module, MP_QSTR_modules);
        // Register as "string.templatelib" for import to work
        mp_obj_t key = mp_obj_new_str("string.templatelib", 18);
        mp_obj_dict_store(modules_dict, key, MP_OBJ_FROM_PTR(&mp_module_templatelib));
    }
}

// External references
extern const mp_obj_type_t mp_type_template;
extern const mp_obj_type_t mp_type_interpolation;

// templatelib submodule
static const mp_rom_map_elem_t mp_module_templatelib_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_templatelib) },
    { MP_ROM_QSTR(MP_QSTR_Template), MP_ROM_PTR(&mp_type_template) },
    { MP_ROM_QSTR(MP_QSTR_Interpolation), MP_ROM_PTR(&mp_type_interpolation) },
};

static MP_DEFINE_CONST_DICT(
    mp_module_templatelib_globals,
    mp_module_templatelib_globals_table
    );

const mp_obj_module_t mp_module_templatelib = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_templatelib_globals,
};

// string module
static const mp_rom_map_elem_t mp_module_string_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_string) },
    { MP_ROM_QSTR(MP_QSTR_templatelib), MP_ROM_PTR(&mp_module_templatelib) },
};

static MP_DEFINE_CONST_DICT(
    mp_module_string_globals,
    mp_module_string_globals_table
    );

// Custom module type with attribute handler
static void string_module_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    // Register string.templatelib in sys.modules on first access
    static bool registered = false;
    if (!registered) {
        register_string_templatelib();
        registered = true;
    }

    // Standard attribute lookup
    if (dest[0] == MP_OBJ_NULL) {
        // Load attribute
        mp_obj_module_t *self = MP_OBJ_TO_PTR(self_in);
        mp_map_elem_t *elem = mp_map_lookup(&self->globals->map, MP_OBJ_NEW_QSTR(attr), MP_MAP_LOOKUP);
        if (elem != NULL) {
            dest[0] = elem->value;
        }
    }
}

MP_DEFINE_CONST_OBJ_TYPE(
    mp_type_string_module,
    MP_QSTR_module,
    MP_TYPE_FLAG_NONE,
    attr, string_module_attr
    );

const mp_obj_module_t mp_module_string = {
    .base = { &mp_type_string_module },
    .globals = (mp_obj_dict_t *)&mp_module_string_globals,
};

MP_REGISTER_MODULE(MP_QSTR_string, mp_module_string);

// Also register templatelib directly for import compatibility
// This allows "import templatelib" to work
MP_REGISTER_MODULE(MP_QSTR_templatelib, mp_module_templatelib);

#endif // MICROPY_PY_TSTRINGS
