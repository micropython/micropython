#include "analouge_readings.h"

// Register the functions - MP_DEFINE_CONST_FUN_OBJ_x, x=0 for 0 args, x=1 for 1 args etc 
STATIC MP_DEFINE_CONST_FUN_OBJ_1(trigger_readings_obj, trigger_readings);
STATIC MP_DEFINE_CONST_FUN_OBJ_2(read_batch_obj, read_batch);

// Define all attributes of the module.
// Table entries are key/value pairs of the attribute name (a string)
// and the MicroPython object reference.
// All identifiers and strings are written as MP_QSTR_xxx and will be
// optimized to word-sized integers by the build system (interned strings).
STATIC const mp_rom_map_elem_t analogue_readings_module_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_analogue_readings)},
    {MP_ROM_QSTR(MP_QSTR_trigger_readings), MP_ROM_PTR(&trigger_readings_obj)}, // need to expose it in the module
    {MP_ROM_QSTR(MP_QSTR_read_batch), MP_ROM_PTR(&read_batch_obj)} // need to expose it in the module
};
STATIC MP_DEFINE_CONST_DICT(analogue_readings_module_globals, analogue_readings_module_globals_table);

// Define module object.
const mp_obj_module_t mp_module_analogue_readings = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&analogue_readings_module_globals,
};

// Register the module to make it available in Python.
MP_REGISTER_MODULE(MP_QSTR_analogue_readings, mp_module_analogue_readings);
