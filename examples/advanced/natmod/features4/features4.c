/*
  This example extends on features0 but demonstrates how to define a class,
  and a custom exception.

  The Factorial class constructor takes an integer, and then the calculate
  method can be called to get the factorial.

  >>> import features4
  >>> f = features4.Factorial(4)
  >>> f.calculate()
  24

  If the argument to the Factorial class constructor is less than zero, a
  FactorialError is raised.
*/

// Include the header file to get access to the MicroPython API
#include "py/dynruntime.h"

// This is type(Factorial)
mp_obj_full_type_t mp_type_factorial;

// This is the internal state of a Factorial instance.
typedef struct {
    mp_obj_base_t base;
    mp_int_t n;
} mp_obj_factorial_t;

mp_obj_full_type_t mp_type_FactorialError;

// Essentially Factorial.__new__ (but also kind of __init__).
// Takes a single argument (the number to find the factorial of)
static mp_obj_t factorial_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args_in) {
    mp_arg_check_num(n_args, n_kw, 1, 1, false);

    mp_obj_factorial_t *o = mp_obj_malloc(mp_obj_factorial_t, type);
    o->n = mp_obj_get_int(args_in[0]);

    if (o->n < 0) {
        mp_raise_msg((mp_obj_type_t *)&mp_type_FactorialError, "argument must be zero or above");
    }

    return MP_OBJ_FROM_PTR(o);
}

static mp_int_t factorial_helper(mp_int_t x) {
    if (x == 0) {
        return 1;
    }
    return x * factorial_helper(x - 1);
}

// Implements Factorial.calculate()
static mp_obj_t factorial_calculate(mp_obj_t self_in) {
    mp_obj_factorial_t *self = MP_OBJ_TO_PTR(self_in);
    return mp_obj_new_int(factorial_helper(self->n));
}
static MP_DEFINE_CONST_FUN_OBJ_1(factorial_calculate_obj, factorial_calculate);

// Locals dict for the Factorial type (will have a single method, calculate,
// added in mpy_init).
mp_map_elem_t factorial_locals_dict_table[1];
static MP_DEFINE_CONST_DICT(factorial_locals_dict, factorial_locals_dict_table);

// This is the entry point and is called when the module is imported
mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    // This must be first, it sets up the globals dict and other things
    MP_DYNRUNTIME_INIT_ENTRY

    // Initialise the type.
    mp_type_factorial.base.type = (void*)&mp_type_type;
    mp_type_factorial.flags = MP_TYPE_FLAG_NONE;
    mp_type_factorial.name = MP_QSTR_Factorial;
    MP_OBJ_TYPE_SET_SLOT(&mp_type_factorial, make_new, factorial_make_new, 0);
    factorial_locals_dict_table[0] = (mp_map_elem_t){ MP_OBJ_NEW_QSTR(MP_QSTR_calculate), MP_OBJ_FROM_PTR(&factorial_calculate_obj) };
    MP_OBJ_TYPE_SET_SLOT(&mp_type_factorial, locals_dict, (void*)&factorial_locals_dict, 1);

    // Make the Factorial type available on the module.
    mp_store_global(MP_QSTR_Factorial, MP_OBJ_FROM_PTR(&mp_type_factorial));

    // Initialise the exception type.
    mp_obj_exception_init(&mp_type_FactorialError, MP_QSTR_FactorialError, &mp_type_Exception);

    // Make the FactorialError type available on the module.
    mp_store_global(MP_QSTR_FactorialError, MP_OBJ_FROM_PTR(&mp_type_FactorialError));

    // This must be last, it restores the globals dict
    MP_DYNRUNTIME_INIT_EXIT
}
