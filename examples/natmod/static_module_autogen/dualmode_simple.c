/*
 * Simple dual-mode module example
 * 
 * This module can be compiled as either:
 * - User C module: make USER_C_MODULES=path/to/this/dir
 * - Native module: python3 ../../tools/mpy_ld.py --arch x64 dualmode_simple.c -o dualmode_simple.mpy
 */

#include "mp_dualmode.h"

// Ensure QSTRs are defined
MP_DUALMODE_QSTR_DEF(dualmode_simple)
MP_DUALMODE_QSTR_DEF(square)
MP_DUALMODE_QSTR_DEF(factorial)
MP_DUALMODE_QSTR_DEF(concat)
MP_DUALMODE_QSTR_DEF(PI)
MP_DUALMODE_QSTR_DEF(E)

// Function: square a number
static mp_obj_t simple_square(mp_obj_t x_obj) {
    mp_int_t x = mp_obj_get_int(x_obj);
    return mp_obj_new_int(x * x);
}
static MP_DEFINE_CONST_FUN_OBJ_1(simple_square_obj, simple_square);

// Function: factorial (recursive)
static mp_int_t factorial_helper(mp_int_t n) {
    if (n <= 1) return 1;
    return n * factorial_helper(n - 1);
}

static mp_obj_t simple_factorial(mp_obj_t n_obj) {
    mp_int_t n = mp_obj_get_int(n_obj);
    if (n < 0) {
        mp_raise_ValueError(MP_ERROR_TEXT("factorial of negative number"));
    }
    if (n > 12) {  // Prevent overflow on 32-bit systems
        mp_raise_ValueError(MP_ERROR_TEXT("factorial too large"));
    }
    return mp_obj_new_int(factorial_helper(n));
}
static MP_DEFINE_CONST_FUN_OBJ_1(simple_factorial_obj, simple_factorial);

// Function: concatenate strings
static mp_obj_t simple_concat(mp_obj_t str1_obj, mp_obj_t str2_obj) {
    // Get string data
    size_t len1, len2;
    const char *str1 = mp_obj_str_get_data(str1_obj, &len1);
    const char *str2 = mp_obj_str_get_data(str2_obj, &len2);
    
    // Allocate new string
    vstr_t vstr;
    vstr_init(&vstr, len1 + len2);
    vstr_add_strn(&vstr, str1, len1);
    vstr_add_strn(&vstr, str2, len2);
    
    // Return new string
    return mp_obj_new_str_from_vstr(&vstr);
}
static MP_DEFINE_CONST_FUN_OBJ_2(simple_concat_obj, simple_concat);

// Module definition using dual-mode macros
MP_DUALMODE_MODULE_GLOBALS_TABLE(dualmode_simple) = {
    // Module name
    MP_DUALMODE_MODULE_ATTR(MP_QSTR___name__, MP_DUALMODE_ROM_QSTR(MP_QSTR_dualmode_simple))
    
    // Functions
    MP_DUALMODE_MODULE_ATTR(MP_QSTR_square, MP_DUALMODE_ROM_PTR(&simple_square_obj))
    MP_DUALMODE_MODULE_ATTR(MP_QSTR_factorial, MP_DUALMODE_ROM_PTR(&simple_factorial_obj))
    MP_DUALMODE_MODULE_ATTR(MP_QSTR_concat, MP_DUALMODE_ROM_PTR(&simple_concat_obj))
    
    // Constants
    MP_DUALMODE_MODULE_ATTR(MP_QSTR_PI, MP_DUALMODE_ROM_INT(31416))  // PI * 10000
    MP_DUALMODE_MODULE_ATTR(MP_QSTR_E, MP_DUALMODE_ROM_INT(27183))   // E * 10000
MP_DUALMODE_MODULE_END()

// Generate init function for native module
MP_DUALMODE_INIT_MODULE(dualmode_simple)

// Define module for user C module
MP_DUALMODE_MODULE_FINALIZE(dualmode_simple, MP_QSTR_dualmode_simple)