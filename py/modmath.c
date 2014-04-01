#include <math.h>

#include "misc.h"
#include "mpconfig.h"
#include "qstr.h"
#include "obj.h"
#include "builtin.h"

#if MICROPY_ENABLE_FLOAT

//TODO: Change macros to check for overflow and raise OverflowError or RangeError
#define MATH_FUN_1(py_name, c_name) \
    mp_obj_t mp_math_ ## py_name(mp_obj_t x_obj) { return mp_obj_new_float(MICROPY_FLOAT_C_FUN(c_name)(mp_obj_get_float(x_obj))); } \
    STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_math_## py_name ## _obj, mp_math_ ## py_name);

#define MATH_FUN_2(py_name, c_name) \
    mp_obj_t mp_math_ ## py_name(mp_obj_t x_obj, mp_obj_t y_obj) { return mp_obj_new_float(MICROPY_FLOAT_C_FUN(c_name)(mp_obj_get_float(x_obj), mp_obj_get_float(y_obj))); } \
    STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_math_## py_name ## _obj, mp_math_ ## py_name);

#define MATH_FUN_1_TO_BOOL(py_name, c_name) \
    mp_obj_t mp_math_ ## py_name(mp_obj_t x_obj) { return MP_BOOL(c_name(mp_obj_get_float(x_obj))); } \
    STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_math_## py_name ## _obj, mp_math_ ## py_name);

#define MATH_FUN_1_TO_INT(py_name, c_name) \
    mp_obj_t mp_math_ ## py_name(mp_obj_t x_obj) { return mp_obj_new_int((machine_int_t)MICROPY_FLOAT_C_FUN(c_name)(mp_obj_get_float(x_obj))); } \
    STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_math_## py_name ## _obj, mp_math_ ## py_name);

STATIC const mp_obj_float_t mp_math_e_obj = {{&mp_type_float}, M_E};
STATIC const mp_obj_float_t mp_math_pi_obj = {{&mp_type_float}, M_PI};

MATH_FUN_1(sqrt, sqrt)
MATH_FUN_2(pow, pow)
MATH_FUN_1(exp, exp)
MATH_FUN_1(expm1, expm1)
MATH_FUN_1(log, log)
MATH_FUN_1(log2, log2)
MATH_FUN_1(log10, log10)
MATH_FUN_1(cosh, cosh)
MATH_FUN_1(sinh, sinh)
MATH_FUN_1(tanh, tanh)
MATH_FUN_1(acosh, acosh)
MATH_FUN_1(asinh, asinh)
MATH_FUN_1(atanh, atanh)
MATH_FUN_1(cos, cos)
MATH_FUN_1(sin, sin)
MATH_FUN_1(tan, tan)
MATH_FUN_1(acos, acos)
MATH_FUN_1(asin, asin)
MATH_FUN_1(atan, atan)
MATH_FUN_2(atan2, atan2)
MATH_FUN_1_TO_INT(ceil, ceil)
MATH_FUN_2(copysign, copysign)
MATH_FUN_1(fabs, fabs)
MATH_FUN_1_TO_INT(floor, floor) //TODO: delegate to x.__floor__() if x is not a float
MATH_FUN_2(fmod, fmod)
MATH_FUN_1_TO_BOOL(isfinite, isfinite)
MATH_FUN_1_TO_BOOL(isinf, isinf)
MATH_FUN_1_TO_BOOL(isnan, isnan)
MATH_FUN_1_TO_INT(trunc, trunc)
MATH_FUN_2(ldexp, ldexp)
MATH_FUN_1(erf, erf)
MATH_FUN_1(erfc, erfc)
MATH_FUN_1(gamma, tgamma)
MATH_FUN_1(lgamma, lgamma)
//TODO: factorial, fsum

// Functions that return a tuple
mp_obj_t mp_math_frexp(mp_obj_t x_obj) {
    int int_exponent = 0;
    mp_float_t significand = MICROPY_FLOAT_C_FUN(frexp)(mp_obj_get_float(x_obj), &int_exponent);
    mp_obj_t tuple[2];
    tuple[0] = mp_obj_new_float(significand);
    tuple[1] = mp_obj_new_int(int_exponent);
    return mp_obj_new_tuple(2, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_math_frexp_obj, mp_math_frexp);

mp_obj_t mp_math_modf(mp_obj_t x_obj) {
    mp_float_t int_part = 0.0;
    mp_float_t fractional_part = MICROPY_FLOAT_C_FUN(modf)(mp_obj_get_float(x_obj), &int_part);
    mp_obj_t tuple[2];
    tuple[0] = mp_obj_new_float(fractional_part);
    tuple[1] = mp_obj_new_float(int_part);
    return mp_obj_new_tuple(2, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_math_modf_obj, mp_math_modf);

// Angular conversions
mp_obj_t mp_math_radians(mp_obj_t x_obj) {
    return mp_obj_new_float(mp_obj_get_float(x_obj) * M_PI / 180.0);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_math_radians_obj, mp_math_radians);

mp_obj_t mp_math_degrees(mp_obj_t x_obj) {
    return mp_obj_new_float(mp_obj_get_float(x_obj) * 180.0 / M_PI);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_math_degrees_obj, mp_math_degrees);

STATIC const mp_map_elem_t mp_module_math_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_math) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_e), (mp_obj_t)&mp_math_e_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pi), (mp_obj_t)&mp_math_pi_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sqrt), (mp_obj_t)&mp_math_sqrt_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pow), (mp_obj_t)&mp_math_pow_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_exp), (mp_obj_t)&mp_math_exp_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_expm1), (mp_obj_t)&mp_math_expm1_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_log), (mp_obj_t)&mp_math_log_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_log2), (mp_obj_t)&mp_math_log2_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_log10), (mp_obj_t)&mp_math_log10_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_cosh), (mp_obj_t)&mp_math_cosh_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sinh), (mp_obj_t)&mp_math_sinh_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_tanh), (mp_obj_t)&mp_math_tanh_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_acosh), (mp_obj_t)&mp_math_acosh_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_asinh), (mp_obj_t)&mp_math_asinh_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_atanh), (mp_obj_t)&mp_math_atanh_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_cos), (mp_obj_t)&mp_math_cos_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sin), (mp_obj_t)&mp_math_sin_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_tan), (mp_obj_t)&mp_math_tan_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_acos), (mp_obj_t)&mp_math_acos_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_asin), (mp_obj_t)&mp_math_asin_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_atan), (mp_obj_t)&mp_math_atan_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_atan2), (mp_obj_t)&mp_math_atan2_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ceil), (mp_obj_t)&mp_math_ceil_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_copysign), (mp_obj_t)&mp_math_copysign_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_fabs), (mp_obj_t)&mp_math_fabs_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_floor), (mp_obj_t)&mp_math_floor_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_frexp), (mp_obj_t)&mp_math_frexp_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ldexp), (mp_obj_t)&mp_math_ldexp_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_modf), (mp_obj_t)&mp_math_modf_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_isfinite), (mp_obj_t)&mp_math_isfinite_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_isinf), (mp_obj_t)&mp_math_isinf_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_isnan), (mp_obj_t)&mp_math_isnan_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_trunc), (mp_obj_t)&mp_math_trunc_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_radians), (mp_obj_t)&mp_math_radians_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_degrees), (mp_obj_t)&mp_math_degrees_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_erf), (mp_obj_t)&mp_math_erf_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_erfc), (mp_obj_t)&mp_math_erfc_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_gamma), (mp_obj_t)&mp_math_gamma_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_lgamma), (mp_obj_t)&mp_math_lgamma_obj },
};

STATIC const mp_map_t mp_module_math_globals = {
    .all_keys_are_qstrs = 1,
    .table_is_fixed_array = 1,
    .used = sizeof(mp_module_math_globals_table) / sizeof(mp_map_elem_t),
    .alloc = sizeof(mp_module_math_globals_table) / sizeof(mp_map_elem_t),
    .table = (mp_map_elem_t*)mp_module_math_globals_table,
};

const mp_obj_module_t mp_module_math = {
    .base = { &mp_type_module },
    .name = MP_QSTR_math,
    .globals = (mp_map_t*)&mp_module_math_globals,
};

#endif // MICROPY_ENABLE_FLOAT
