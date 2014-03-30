#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "builtin.h"

// Various builtins specific to MicroPython runtime,
// living in micropython module

#if MICROPY_MEM_STATS
STATIC mp_obj_t mp_micropython_mem_total() {
    return MP_OBJ_NEW_SMALL_INT((machine_int_t)m_get_total_bytes_allocated());
}

STATIC mp_obj_t mp_micropython_mem_current() {
    return MP_OBJ_NEW_SMALL_INT((machine_int_t)m_get_current_bytes_allocated());
}

STATIC mp_obj_t mp_micropython_mem_peak() {
    return MP_OBJ_NEW_SMALL_INT((machine_int_t)m_get_peak_bytes_allocated());
}

STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_micropython_mem_total_obj, mp_micropython_mem_total);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_micropython_mem_current_obj, mp_micropython_mem_current);
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mp_micropython_mem_peak_obj, mp_micropython_mem_peak);
#endif

STATIC const mp_map_elem_t mp_module_micropython_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_micropython) },
#if MICROPY_MEM_STATS
    { MP_OBJ_NEW_QSTR(MP_QSTR_mem_total), (mp_obj_t)&mp_micropython_mem_total_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_mem_current), (mp_obj_t)&mp_micropython_mem_current_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_mem_peak), (mp_obj_t)&mp_micropython_mem_peak_obj },
#endif
};

STATIC const mp_map_t mp_module_micropython_globals = {
    .all_keys_are_qstrs = 1,
    .table_is_fixed_array = 1,
    .used = sizeof(mp_module_micropython_globals_table) / sizeof(mp_map_elem_t),
    .alloc = sizeof(mp_module_micropython_globals_table) / sizeof(mp_map_elem_t),
    .table = (mp_map_elem_t*)mp_module_micropython_globals_table,
};

const mp_obj_module_t mp_module_micropython = {
    .base = { &mp_type_module },
    .name = MP_QSTR_micropython,
    .globals = (mp_map_t*)&mp_module_micropython_globals,
};
