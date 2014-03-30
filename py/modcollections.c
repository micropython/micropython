#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "builtin.h"

STATIC const mp_map_elem_t mp_module_collections_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_collections) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_namedtuple), (mp_obj_t)&mp_namedtuple_obj },
};

STATIC const mp_map_t mp_module_collections_globals = {
    .all_keys_are_qstrs = 1,
    .table_is_fixed_array = 1,
    .used = sizeof(mp_module_collections_globals_table) / sizeof(mp_map_elem_t),
    .alloc = sizeof(mp_module_collections_globals_table) / sizeof(mp_map_elem_t),
    .table = (mp_map_elem_t*)mp_module_collections_globals_table,
};

const mp_obj_module_t mp_module_collections = {
    .base = { &mp_type_module },
    .name = MP_QSTR_collections,
    .globals = (mp_map_t*)&mp_module_collections_globals,
};
