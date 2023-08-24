#include <analogue_readings.hpp>

// Register the functions - has 0 arguments so using MP_DEFINE_CONST_FUN_OBJ_0
STATIC MP_DEFINE_CONST_FUN_OBJ_0(make_analogue_readings_obj, make_analogue_readings);

// Define all attributes of the module.
// Table entries are key/value pairs of the attribute name (a string)
// and the MicroPython object reference.
// All identifiers and strings are written as MP_QSTR_xxx and will be
// optimized to word-sized integers by the build system (interned strings).
STATIC const mp_rom_map_elem_t analogue_readings_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ultrasound_readings) },
    { MP_ROM_QSTR(MP_QSTR_make_analogue_readings), MP_ROM_PTR(&make_analogue_readings_obj) } // need to expose it in the module
};
STATIC MP_DEFINE_CONST_DICT(analogue_readings_module_globals, analogue_readings_module_globals_table);

// Define module object.
const mp_obj_module_t analogue_readings_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&analogue_readings_module_globals,
};

// Register the module to make it available in Python.
MP_REGISTER_MODULE(MP_QSTR_analogue_readings, analogue_readings_module);
