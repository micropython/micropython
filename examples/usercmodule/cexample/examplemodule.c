// Include MicroPython API.
#include "py/runtime.h"

// Used to get the time in the Timer class example.
#include "py/mphal.h"

// This is the function which will be called from Python as cexample.add_ints(a, b).
static mp_obj_t example_add_ints(mp_obj_t a_obj, mp_obj_t b_obj) {
    // Extract the ints from the micropython input objects.
    int a = mp_obj_get_int(a_obj);
    int b = mp_obj_get_int(b_obj);

    // Calculate the addition and convert to MicroPython object.
    return mp_obj_new_int(a + b);
}
// Define a Python reference to the function above.
static MP_DEFINE_CONST_FUN_OBJ_2(example_add_ints_obj, example_add_ints);

// This structure represents Timer instance objects.
typedef struct _example_Timer_obj_t {
    // All objects start with the base.
    mp_obj_base_t base;
    // Everything below can be thought of as instance attributes, but they
    // cannot be accessed by MicroPython code directly. In this example we
    // store the time at which the object was created.
    mp_uint_t start_time;
} example_Timer_obj_t;

// This is the Timer.time() method. After creating a Timer object, this
// can be called to get the time elapsed since creating the Timer.
static mp_obj_t example_Timer_time(mp_obj_t self_in) {
    // The first argument is self. It is cast to the *example_Timer_obj_t
    // type so we can read its attributes.
    example_Timer_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // Get the elapsed time and return it as a MicroPython integer.
    mp_uint_t elapsed = mp_hal_ticks_ms() - self->start_time;
    return mp_obj_new_int_from_uint(elapsed);
}
static MP_DEFINE_CONST_FUN_OBJ_1(example_Timer_time_obj, example_Timer_time);

// This represents Timer.__new__ and Timer.__init__, which is called when
// the user instantiates a Timer object.
static mp_obj_t example_Timer_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args) {
    // Allocates the new object and sets the type.
    example_Timer_obj_t *self = mp_obj_malloc(example_Timer_obj_t, type);

    // Initializes the time for this Timer instance.
    self->start_time = mp_hal_ticks_ms();

    // The make_new function always returns self.
    return MP_OBJ_FROM_PTR(self);
}

// This collects all methods and other static class attributes of the Timer.
// The table structure is similar to the module table, as detailed below.
static const mp_rom_map_elem_t example_Timer_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_time), MP_ROM_PTR(&example_Timer_time_obj) },
};
static MP_DEFINE_CONST_DICT(example_Timer_locals_dict, example_Timer_locals_dict_table);

// This defines the type(Timer) object.
MP_DEFINE_CONST_OBJ_TYPE(
    example_type_Timer,
    MP_QSTR_Timer,
    MP_TYPE_FLAG_NONE,
    make_new, example_Timer_make_new,
    locals_dict, &example_Timer_locals_dict
    );

// Define all attributes of the module.
// Table entries are key/value pairs of the attribute name (a string)
// and the MicroPython object reference.
// All identifiers and strings are written as MP_QSTR_xxx and will be
// optimized to word-sized integers by the build system (interned strings).
static const mp_rom_map_elem_t example_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_cexample) },
    { MP_ROM_QSTR(MP_QSTR_add_ints), MP_ROM_PTR(&example_add_ints_obj) },
    { MP_ROM_QSTR(MP_QSTR_Timer),    MP_ROM_PTR(&example_type_Timer) },
};
static MP_DEFINE_CONST_DICT(example_module_globals, example_module_globals_table);

// Define module object.
const mp_obj_module_t example_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&example_module_globals,
};

// Register the module to make it available in Python.
MP_REGISTER_MODULE(MP_QSTR_cexample, example_user_cmodule);
