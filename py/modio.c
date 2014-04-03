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
};

STATIC const mp_map_t mp_module_io_globals = {
    .all_keys_are_qstrs = 1,
    .table_is_fixed_array = 1,
    .used = sizeof(mp_module_io_globals_table) / sizeof(mp_map_elem_t),
    .alloc = sizeof(mp_module_io_globals_table) / sizeof(mp_map_elem_t),
    .table = (mp_map_elem_t*)mp_module_io_globals_table,
};

const mp_obj_module_t mp_module_io = {
    .base = { &mp_type_module },
    .name = MP_QSTR_io,
    .globals = (mp_map_t*)&mp_module_io_globals,
};

#endif
