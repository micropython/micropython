#include "py/nlr.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/binary.h"

#include <stdio.h>

// forward declare emscripten stuff
void emscripten_run_script(const char *);
int emscripten_run_script_int(const char *);

STATIC mp_obj_t js_eval(mp_obj_t js_string) {
    emscripten_run_script(mp_obj_str_get_str(js_string));
    return mp_const_none;
}

STATIC mp_obj_t js_evali(mp_obj_t js_string) {
    int res = emscripten_run_script_int(mp_obj_str_get_str(js_string));
    return mp_obj_new_int(res);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(js_eval_obj, js_eval);
STATIC MP_DEFINE_CONST_FUN_OBJ_1(js_evali_obj, js_evali);

STATIC const mp_map_elem_t js_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_js) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_eval), (mp_obj_t)&js_eval_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_evali), (mp_obj_t)&js_evali_obj },
};

STATIC MP_DEFINE_CONST_DICT (
    mp_module_js_globals,
    js_globals_table
);

const mp_obj_module_t mp_module_js = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_js_globals,
};
