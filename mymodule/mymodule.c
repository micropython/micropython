#include "py/runtime.h"

// 1. 文字列を返すC言語の関数
static mp_obj_t mymodule_hello(void) {
    return mp_obj_new_str("Hello from Pico 2!", 18);
}
static MP_DEFINE_CONST_FUN_OBJ_0(mymodule_hello_obj, mymodule_hello);

// 2. 関数をモジュール内に登録
static const mp_rom_map_elem_t mymodule_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_mymodule) },
    { MP_ROM_QSTR(MP_QSTR_hello), MP_ROM_PTR(&mymodule_hello_obj) },
};
static MP_DEFINE_CONST_DICT(mymodule_globals, mymodule_globals_table);

// 3. モジュール自体をMicroPythonに登録
const mp_obj_module_t mymodule_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mymodule_globals,
};

MP_REGISTER_MODULE(MP_QSTR_mymodule, mymodule_user_cmodule);
