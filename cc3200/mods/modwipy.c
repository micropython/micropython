#include "py/mpconfig.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "mperror.h"


/******************************************************************************/
// Micro Python bindings

STATIC mp_obj_t mod_wipy_heartbeat (mp_uint_t n_args, const mp_obj_t *args) {
    if (n_args) {
        mperror_enable_heartbeat (mp_obj_is_true(args[0]));
        return mp_const_none;
    } else {
        return mp_obj_new_bool(mperror_is_heartbeat_enabled());
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_wipy_heartbeat_obj, 0, 1, mod_wipy_heartbeat);

STATIC const mp_map_elem_t wipy_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__),        MP_OBJ_NEW_QSTR(MP_QSTR_wipy) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_heartbeat),       (mp_obj_t)&mod_wipy_heartbeat_obj },
};

STATIC MP_DEFINE_CONST_DICT(wipy_module_globals, wipy_module_globals_table);

const mp_obj_module_t wipy_module = {
    .base = { &mp_type_module },
    .name = MP_QSTR_wipy,
    .globals = (mp_obj_dict_t*)&wipy_module_globals,
};
