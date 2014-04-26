#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "builtin.h"

#if MICROPY_ENABLE_MOD_IO

STATIC const mp_map_elem_t mp_module_io_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_io) },
    // Note: mp_builtin_open_obj should be defined by port, it's not
    // part of the core.
    { MP_OBJ_NEW_QSTR(MP_QSTR_open), (mp_obj_t)&mp_builtin_open_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_BytesIO), (mp_obj_t)&mp_type_stringio },
    { MP_OBJ_NEW_QSTR(MP_QSTR_StringIO), (mp_obj_t)&mp_type_stringio },
};

STATIC const mp_obj_dict_t mp_module_io_globals = {
    .base = {&mp_type_dict},
    .map = {
        .all_keys_are_qstrs = 1,
        .table_is_fixed_array = 1,
        .used = ARRAY_SIZE(mp_module_io_globals_table),
        .alloc = ARRAY_SIZE(mp_module_io_globals_table),
        .table = (mp_map_elem_t*)mp_module_io_globals_table,
    },
};

const mp_obj_module_t mp_module_io = {
    .base = { &mp_type_module },
    .name = MP_QSTR_io,
    .globals = (mp_obj_dict_t*)&mp_module_io_globals,
};

#endif
