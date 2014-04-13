#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "builtin.h"
#include "runtime.h"
#include "objlist.h"
#include "objtuple.h"

#if MICROPY_ENABLE_MOD_SYS

// These should be implemented by ports, specific types don't matter,
// only addresses.
struct _dummy_t;
extern struct _dummy_t mp_sys_stdin_obj;
extern struct _dummy_t mp_sys_stdout_obj;
extern struct _dummy_t mp_sys_stderr_obj;

mp_obj_list_t mp_sys_path_obj;
mp_obj_list_t mp_sys_argv_obj;
#define I(n) MP_OBJ_NEW_SMALL_INT(n)
// TODO: CPython is now at 5-element array, but save 2 els so far...
STATIC const mp_obj_tuple_t mp_sys_version_info_obj = {{&mp_type_tuple}, 3, {I(3), I(3), I(5)}};
#undef I

STATIC const mp_map_elem_t mp_module_sys_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_sys) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_path), (mp_obj_t)&mp_sys_path_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_argv), (mp_obj_t)&mp_sys_argv_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_version_info), (mp_obj_t)&mp_sys_version_info_obj },

#if MICROPY_MOD_SYS_STDFILES
    { MP_OBJ_NEW_QSTR(MP_QSTR_stdin), (mp_obj_t)&mp_sys_stdin_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_stdout), (mp_obj_t)&mp_sys_stdout_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_stderr), (mp_obj_t)&mp_sys_stderr_obj },
#endif
};

STATIC const mp_obj_dict_t mp_module_sys_globals = {
    .base = {&mp_type_dict},
    .map = {
        .all_keys_are_qstrs = 1,
        .table_is_fixed_array = 1,
        .used = sizeof(mp_module_sys_globals_table) / sizeof(mp_map_elem_t),
        .alloc = sizeof(mp_module_sys_globals_table) / sizeof(mp_map_elem_t),
        .table = (mp_map_elem_t*)mp_module_sys_globals_table,
    },
};

const mp_obj_module_t mp_module_sys = {
    .base = { &mp_type_module },
    .name = MP_QSTR_sys,
    .globals = (mp_obj_dict_t*)&mp_module_sys_globals,
};

#endif
