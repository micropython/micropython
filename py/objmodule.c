#include <stdlib.h>
#include <assert.h>

#include "nlr.h"
#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "objmodule.h"
#include "runtime.h"
#include "map.h"
#include "builtintables.h"

STATIC mp_map_t mp_loaded_modules_map; // TODO: expose as sys.modules

STATIC void module_print(void (*print)(void *env, const char *fmt, ...), void *env, mp_obj_t self_in, mp_print_kind_t kind) {
    mp_obj_module_t *self = self_in;
    print(env, "<module '%s' from '-unknown-file-'>", qstr_str(self->name));
}

STATIC void module_load_attr(mp_obj_t self_in, qstr attr, mp_obj_t *dest) {
    mp_obj_module_t *self = self_in;
    mp_map_elem_t *elem = mp_map_lookup(self->globals, MP_OBJ_NEW_QSTR(attr), MP_MAP_LOOKUP);
    if (elem != NULL) {
        dest[0] = elem->value;
    }
}

STATIC bool module_store_attr(mp_obj_t self_in, qstr attr, mp_obj_t value) {
    mp_obj_module_t *self = self_in;
    // TODO CPython allows STORE_ATTR to a module, but is this the correct implementation?
    mp_map_lookup(self->globals, MP_OBJ_NEW_QSTR(attr), MP_MAP_LOOKUP_ADD_IF_NOT_FOUND)->value = value;
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
    o->globals = mp_map_new(1);

    // store __name__ entry in the module
    mp_map_lookup(o->globals, MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_MAP_LOOKUP_ADD_IF_NOT_FOUND)->value = MP_OBJ_NEW_QSTR(module_name);

    // store the new module into the slot in the global dict holding all modules
    el->value = o;

    // return the new module
    return o;
}

mp_map_t *mp_obj_module_get_globals(mp_obj_t self_in) {
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

    // module found, return it
    if (el != NULL) {
        return el->value;
    }

    // module not found, look for builtin module names
    // it will return MP_OBJ_NULL if nothing found
    return mp_builtin_tables_lookup_module(module_name);
}

void mp_module_register(qstr qstr, mp_obj_t module) {
    mp_map_lookup(&mp_loaded_modules_map, MP_OBJ_NEW_QSTR(qstr), MP_MAP_LOOKUP_ADD_IF_NOT_FOUND)->value = module;
}
