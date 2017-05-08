#include "py/mpconfig.h"
#include "py/nlr.h"
#include "py/misc.h"
#include "py/qstr.h"
#include "py/obj.h"
#include "py/runtime.h"

STATIC mp_obj_t cbyte_hello(void) {
    printf("Hello world from C!\n");
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(cbyte_hello_obj, cbyte_hello);

STATIC const mp_map_elem_t mymodule_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_cbyte) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_hello), (mp_obj_t)&cbyte_hello_obj },

};

STATIC MP_DEFINE_CONST_DICT(mp_module_mymodule_globals, mymodule_globals_table);

const mp_obj_module_t mp_module_cbyte = {
    .base = { &mp_type_module },
   // .name = MP_QSTR_cbyte,
    .globals = (mp_obj_dict_t*)&mp_module_mymodule_globals,
};

