#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "builtin.h"

#if MICROPY_ENABLE_MOD_COLLECTIONS

STATIC const mp_map_elem_t mp_module_collections_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR__collections) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_namedtuple), (mp_obj_t)&mp_namedtuple_obj },
};

STATIC const mp_obj_dict_t mp_module_collections_globals = {
    .base = {&mp_type_dict},
    .map = {
        .all_keys_are_qstrs = 1,
        .table_is_fixed_array = 1,
        .used = ARRAY_SIZE(mp_module_collections_globals_table),
        .alloc = ARRAY_SIZE(mp_module_collections_globals_table),
        .table = (mp_map_elem_t*)mp_module_collections_globals_table,
    },
};

const mp_obj_module_t mp_module_collections = {
    .base = { &mp_type_module },
    .name = MP_QSTR__collections,
    .globals = (mp_obj_dict_t*)&mp_module_collections_globals,
};

#endif // MICROPY_ENABLE_MOD_COLLECTIONS
