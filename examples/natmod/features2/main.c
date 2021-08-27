/* This example demonstrates the following features in a native module:
    - using floats
    - defining additional code in Python (see test.py)
    - have extra C code in a separate file (see prod.c)
*/

// Include the header file to get access to the MicroPython API
#include "py/dynruntime.h"

// Include the header for auxiliary C code for this module
#include "prod.h"

// Automatically detect if this module should include double-precision code.
// If double precision is supported by the target architecture then it can
// be used in native module regardless of what float setting the target
// MicroPython runtime uses (being none, float or double).
#if defined(__i386__) || defined(__x86_64__) || (defined(__ARM_FP) && (__ARM_FP & 8))
#define USE_DOUBLE 1
#else
#define USE_DOUBLE 0
#endif

// A function that uses the default float type configured for the current target
// This default can be overridden by specifying MICROPY_FLOAT_IMPL at the make level
STATIC mp_obj_t add(mp_obj_t x, mp_obj_t y) {
    return mp_obj_new_float(mp_obj_get_float(x) + mp_obj_get_float(y));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(add_obj, add);

// A function that explicitly uses single precision floats
STATIC mp_obj_t add_f(mp_obj_t x, mp_obj_t y) {
    return mp_obj_new_float_from_f(mp_obj_get_float_to_f(x) + mp_obj_get_float_to_f(y));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(add_f_obj, add_f);

#if USE_DOUBLE
// A function that explicitly uses double precision floats
STATIC mp_obj_t add_d(mp_obj_t x, mp_obj_t y) {
    return mp_obj_new_float_from_d(mp_obj_get_float_to_d(x) + mp_obj_get_float_to_d(y));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(add_d_obj, add_d);
#endif

// A function that computes the product of floats in an array.
// This function uses the most general C argument interface, which is more difficult
// to use but has access to the globals dict of the module via self->globals.
STATIC mp_obj_t productf(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    // Check number of arguments is valid
    mp_arg_check_num_mp(n_args, n_kw, 1, 1, false);

    // Extract buffer pointer and verify typecode
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[0], &bufinfo, MP_BUFFER_RW);
    if (bufinfo.typecode != 'f') {
        mp_raise_ValueError(MP_ERROR_TEXT("expecting float array"));
    }

    // Compute product, store result back in first element of array
    float *ptr = bufinfo.buf;
    float prod = prod_array(bufinfo.len / sizeof(*ptr), ptr);
    ptr[0] = prod;

    return mp_const_none;
}

// This is the entry point and is called when the module is imported
mp_obj_t mpy_init(mp_obj_fun_bc_t *self, size_t n_args, size_t n_kw, mp_obj_t *args) {
    // This must be first, it sets up the globals dict and other things
    MP_DYNRUNTIME_INIT_ENTRY

    // Make the functions available in the module's namespace
    mp_store_global(MP_QSTR_add, MP_OBJ_FROM_PTR(&add_obj));
    mp_store_global(MP_QSTR_add_f, MP_OBJ_FROM_PTR(&add_f_obj));
    #if USE_DOUBLE
    mp_store_global(MP_QSTR_add_d, MP_OBJ_FROM_PTR(&add_d_obj));
    #endif

    // The productf function uses the most general C argument interface
    mp_store_global(MP_QSTR_productf, MP_DYNRUNTIME_MAKE_FUNCTION(productf));

    // This must be last, it restores the globals dict
    MP_DYNRUNTIME_INIT_EXIT
}
