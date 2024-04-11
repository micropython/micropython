#include <examplemodule.h>

// Define a Python reference to the function we'll make available.
// See example.cpp for the definition.
static MP_DEFINE_CONST_FUN_OBJ_2(cppfunc_obj, cppfunc);

// Define all attributes of the module.
// Table entries are key/value pairs of the attribute name (a string)
// and the MicroPython object reference.
// All identifiers and strings are written as MP_QSTR_xxx and will be
// optimized to word-sized integers by the build system (interned strings).
static const mp_rom_map_elem_t cppexample_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_cppexample) },
    { MP_ROM_QSTR(MP_QSTR_cppfunc), MP_ROM_PTR(&cppfunc_obj) },
};
static MP_DEFINE_CONST_DICT(cppexample_module_globals, cppexample_module_globals_table);

// Define module object.
const mp_obj_module_t cppexample_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&cppexample_module_globals,
};

// Register the module to make it available in Python.
MP_REGISTER_MODULE(MP_QSTR_cppexample, cppexample_user_cmodule);
