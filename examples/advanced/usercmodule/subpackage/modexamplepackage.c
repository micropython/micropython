// Include MicroPython API.
#include "py/runtime.h"

// Define example_package.foo.bar.f()
static mp_obj_t example_package_foo_bar_f(void) {
    mp_printf(&mp_plat_print, "example_package.foo.bar.f\n");
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(example_package_foo_bar_f_obj, example_package_foo_bar_f);

// Define all attributes of the second-level sub-package (example_package.foo.bar).
static const mp_rom_map_elem_t example_package_foo_bar_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_example_package_dot_foo_dot_bar) },
    { MP_ROM_QSTR(MP_QSTR_f), MP_ROM_PTR(&example_package_foo_bar_f_obj) },
};
static MP_DEFINE_CONST_DICT(example_package_foo_bar_globals, example_package_foo_bar_globals_table);

// Define example_package.foo.bar module object.
const mp_obj_module_t example_package_foo_bar_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&example_package_foo_bar_globals,
};

// Define example_package.foo.f()
static mp_obj_t example_package_foo_f(void) {
    mp_printf(&mp_plat_print, "example_package.foo.f\n");
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(example_package_foo_f_obj, example_package_foo_f);

// Define all attributes of the first-level sub-package (example_package.foo).
static const mp_rom_map_elem_t example_package_foo_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_example_package_dot_foo) },
    { MP_ROM_QSTR(MP_QSTR_bar), MP_ROM_PTR(&example_package_foo_bar_user_cmodule) },
    { MP_ROM_QSTR(MP_QSTR_f), MP_ROM_PTR(&example_package_foo_f_obj) },
};
static MP_DEFINE_CONST_DICT(example_package_foo_globals, example_package_foo_globals_table);

// Define example_package.foo module object.
const mp_obj_module_t example_package_foo_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&example_package_foo_globals,
};

// Define example_package.f()
static mp_obj_t example_package_f(void) {
    mp_printf(&mp_plat_print, "example_package.f\n");
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(example_package_f_obj, example_package_f);

static mp_obj_t example_package___init__(void) {
    if (!MP_STATE_VM(example_package_initialised)) {
        // __init__ for builtins is called each time the module is imported,
        //   so ensure that initialisation only happens once.
        MP_STATE_VM(example_package_initialised) = true;
        mp_printf(&mp_plat_print, "example_package.__init__\n");
    }
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(example_package___init___obj, example_package___init__);

// The "initialised" state is stored on mp_state so that it is cleared on soft
// reset.
MP_REGISTER_ROOT_POINTER(int example_package_initialised);

// Define all attributes of the top-level package (example_package).
static const mp_rom_map_elem_t example_package_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_example_package) },
    { MP_ROM_QSTR(MP_QSTR___init__), MP_ROM_PTR(&example_package___init___obj) },
    { MP_ROM_QSTR(MP_QSTR_foo), MP_ROM_PTR(&example_package_foo_user_cmodule) },
    { MP_ROM_QSTR(MP_QSTR_f), MP_ROM_PTR(&example_package_f_obj) },
};
static MP_DEFINE_CONST_DICT(example_package_globals, example_package_globals_table);

// Define module object.
const mp_obj_module_t example_package_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&example_package_globals,
};

// Register the module to make it available in Python.
// Note: subpackages should not be registered with MP_REGISTER_MODULE.
MP_REGISTER_MODULE(MP_QSTR_example_package, example_package_user_cmodule);
