/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2013, 2014 Damien P. George
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "py/builtin.h"
#include "py/nlr.h"

#if MICROPY_PY_BUILTINS_FLOAT && MICROPY_PY_MATH

#include <math.h>

// M_PI is not part of the math.h standard and may not be defined
// And by defining our own we can ensure it uses the correct const format.
#define MP_PI MICROPY_FLOAT_CONST(3.14159265358979323846)

/// \module math - mathematical functions
///
/// The `math` module provides some basic mathematical funtions for
/// working with floating-point numbers.

STATIC NORETURN void math_error(void) {
    nlr_raise(mp_obj_new_exception_msg_varg(&mp_type_ValueError, "math domain error"));
}

#define MATH_FUN_1(py_name, c_name) \
    STATIC mp_obj_t mp_math_ ## py_name(mp_obj_t x_obj) { return mp_obj_new_float(MICROPY_FLOAT_C_FUN(c_name)(mp_obj_get_float(x_obj))); } \
    STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_math_## py_name ## _obj, mp_math_ ## py_name);

#define MATH_FUN_2(py_name, c_name) \
    STATIC mp_obj_t mp_math_ ## py_name(mp_obj_t x_obj, mp_obj_t y_obj) { return mp_obj_new_float(MICROPY_FLOAT_C_FUN(c_name)(mp_obj_get_float(x_obj), mp_obj_get_float(y_obj))); } \
    STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_math_## py_name ## _obj, mp_math_ ## py_name);

#define MATH_FUN_1_TO_BOOL(py_name, c_name) \
    STATIC mp_obj_t mp_math_ ## py_name(mp_obj_t x_obj) { return mp_obj_new_bool(c_name(mp_obj_get_float(x_obj))); } \
    STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_math_## py_name ## _obj, mp_math_ ## py_name);

#define MATH_FUN_1_TO_INT(py_name, c_name) \
    STATIC mp_obj_t mp_math_ ## py_name(mp_obj_t x_obj) { return mp_obj_new_int_from_float(MICROPY_FLOAT_C_FUN(c_name)(mp_obj_get_float(x_obj))); } \
    STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_math_## py_name ## _obj, mp_math_ ## py_name);

#define MATH_FUN_1_ERRCOND(py_name, c_name, error_condition) \
    STATIC mp_obj_t mp_math_ ## py_name(mp_obj_t x_obj) { \
        mp_float_t x = mp_obj_get_float(x_obj); \
        if (error_condition) { \
            math_error(); \
        } \
        return mp_obj_new_float(MICROPY_FLOAT_C_FUN(c_name)(x)); \
    } \
    STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_math_## py_name ## _obj, mp_math_ ## py_name);

#if MP_NEED_LOG2
// 1.442695040888963407354163704 is 1/_M_LN2
#define log2(x) (log(x) * 1.442695040888963407354163704)
#endif

/// \function sqrt(x)
/// Returns the square root of `x`.
MATH_FUN_1_ERRCOND(sqrt, sqrt, (x < (mp_float_t)0.0))
/// \function pow(x, y)
/// Returns `x` to the power of `y`.
MATH_FUN_2(pow, pow)
/// \function exp(x)
MATH_FUN_1(exp, exp)
#if MICROPY_PY_MATH_SPECIAL_FUNCTIONS
/// \function expm1(x)
MATH_FUN_1(expm1, expm1)
/// \function log2(x)
MATH_FUN_1_ERRCOND(log2, log2, (x <= (mp_float_t)0.0))
/// \function log10(x)
MATH_FUN_1_ERRCOND(log10, log10, (x <= (mp_float_t)0.0))
/// \function cosh(x)
MATH_FUN_1(cosh, cosh)
/// \function sinh(x)
MATH_FUN_1(sinh, sinh)
/// \function tanh(x)
MATH_FUN_1(tanh, tanh)
/// \function acosh(x)
MATH_FUN_1(acosh, acosh)
/// \function asinh(x)
MATH_FUN_1(asinh, asinh)
/// \function atanh(x)
MATH_FUN_1(atanh, atanh)
#endif
/// \function cos(x)
MATH_FUN_1(cos, cos)
/// \function sin(x)
MATH_FUN_1(sin, sin)
/// \function tan(x)
MATH_FUN_1(tan, tan)
/// \function acos(x)
MATH_FUN_1(acos, acos)
/// \function asin(x)
MATH_FUN_1(asin, asin)
/// \function atan(x)
MATH_FUN_1(atan, atan)
/// \function atan2(y, x)
MATH_FUN_2(atan2, atan2)
/// \function ceil(x)
MATH_FUN_1_TO_INT(ceil, ceil)
/// \function copysign(x, y)
MATH_FUN_2(copysign, copysign)
/// \function fabs(x)
MATH_FUN_1(fabs, fabs)
/// \function floor(x)
MATH_FUN_1_TO_INT(floor, floor) //TODO: delegate to x.__floor__() if x is not a float
/// \function fmod(x, y)
MATH_FUN_2(fmod, fmod)
/// \function isfinite(x)
MATH_FUN_1_TO_BOOL(isfinite, isfinite)
/// \function isinf(x)
MATH_FUN_1_TO_BOOL(isinf, isinf)
/// \function isnan(x)
MATH_FUN_1_TO_BOOL(isnan, isnan)
/// \function trunc(x)
MATH_FUN_1_TO_INT(trunc, trunc)
/// \function ldexp(x, exp)
MATH_FUN_2(ldexp, ldexp)
#if MICROPY_PY_MATH_SPECIAL_FUNCTIONS
/// \function erf(x)
/// Return the error function of `x`.
MATH_FUN_1(erf, erf)
/// \function erfc(x)
/// Return the complementary error function of `x`.
MATH_FUN_1(erfc, erfc)
/// \function gamma(x)
/// Return the gamma function of `x`.
MATH_FUN_1(gamma, tgamma)
/// \function lgamma(x)
/// return the natural logarithm of the gamma function of `x`.
MATH_FUN_1(lgamma, lgamma)
#endif
//TODO: factorial, fsum

// Function that takes a variable number of arguments

// log(x[, base])
STATIC mp_obj_t mp_math_log(size_t n_args, const mp_obj_t *args) {
    mp_float_t x = mp_obj_get_float(args[0]);
    if (x <= (mp_float_t)0.0) {
        math_error();
    }
    mp_float_t l = MICROPY_FLOAT_C_FUN(log)(x);
    if (n_args == 1) {
        return mp_obj_new_float(l);
    } else {
        mp_float_t base = mp_obj_get_float(args[1]);
        if (base <= (mp_float_t)0.0) {
            math_error();
        }
        return mp_obj_new_float(l / MICROPY_FLOAT_C_FUN(log)(base));
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_math_log_obj, 1, 2, mp_math_log);

// Functions that return a tuple

/// \function frexp(x)
/// Converts a floating-point number to fractional and integral components.
STATIC mp_obj_t mp_math_frexp(mp_obj_t x_obj) {
    int int_exponent = 0;
    mp_float_t significand = MICROPY_FLOAT_C_FUN(frexp)(mp_obj_get_float(x_obj), &int_exponent);
    mp_obj_t tuple[2];
    tuple[0] = mp_obj_new_float(significand);
    tuple[1] = mp_obj_new_int(int_exponent);
    return mp_obj_new_tuple(2, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_math_frexp_obj, mp_math_frexp);

/// \function modf(x)
STATIC mp_obj_t mp_math_modf(mp_obj_t x_obj) {
    mp_float_t int_part = 0.0;
    mp_float_t fractional_part = MICROPY_FLOAT_C_FUN(modf)(mp_obj_get_float(x_obj), &int_part);
    mp_obj_t tuple[2];
    tuple[0] = mp_obj_new_float(fractional_part);
    tuple[1] = mp_obj_new_float(int_part);
    return mp_obj_new_tuple(2, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_math_modf_obj, mp_math_modf);

// Angular conversions

/// \function radians(x)
STATIC mp_obj_t mp_math_radians(mp_obj_t x_obj) {
    return mp_obj_new_float(mp_obj_get_float(x_obj) * (MP_PI / MICROPY_FLOAT_CONST(180.0)));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_math_radians_obj, mp_math_radians);

/// \function degrees(x)
STATIC mp_obj_t mp_math_degrees(mp_obj_t x_obj) {
    return mp_obj_new_float(mp_obj_get_float(x_obj) * (MICROPY_FLOAT_CONST(180.0) / MP_PI));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_math_degrees_obj, mp_math_degrees);

STATIC const mp_rom_map_elem_t mp_module_math_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_math) },
    { MP_ROM_QSTR(MP_QSTR_e), mp_const_float_e },
    { MP_ROM_QSTR(MP_QSTR_pi), mp_const_float_pi },
    { MP_ROM_QSTR(MP_QSTR_sqrt), MP_ROM_PTR(&mp_math_sqrt_obj) },
    { MP_ROM_QSTR(MP_QSTR_pow), MP_ROM_PTR(&mp_math_pow_obj) },
    { MP_ROM_QSTR(MP_QSTR_exp), MP_ROM_PTR(&mp_math_exp_obj) },
    #if MICROPY_PY_MATH_SPECIAL_FUNCTIONS
    { MP_ROM_QSTR(MP_QSTR_expm1), MP_ROM_PTR(&mp_math_expm1_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_log), MP_ROM_PTR(&mp_math_log_obj) },
    #if MICROPY_PY_MATH_SPECIAL_FUNCTIONS
    { MP_ROM_QSTR(MP_QSTR_log2), MP_ROM_PTR(&mp_math_log2_obj) },
    { MP_ROM_QSTR(MP_QSTR_log10), MP_ROM_PTR(&mp_math_log10_obj) },
    { MP_ROM_QSTR(MP_QSTR_cosh), MP_ROM_PTR(&mp_math_cosh_obj) },
    { MP_ROM_QSTR(MP_QSTR_sinh), MP_ROM_PTR(&mp_math_sinh_obj) },
    { MP_ROM_QSTR(MP_QSTR_tanh), MP_ROM_PTR(&mp_math_tanh_obj) },
    { MP_ROM_QSTR(MP_QSTR_acosh), MP_ROM_PTR(&mp_math_acosh_obj) },
    { MP_ROM_QSTR(MP_QSTR_asinh), MP_ROM_PTR(&mp_math_asinh_obj) },
    { MP_ROM_QSTR(MP_QSTR_atanh), MP_ROM_PTR(&mp_math_atanh_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_cos), MP_ROM_PTR(&mp_math_cos_obj) },
    { MP_ROM_QSTR(MP_QSTR_sin), MP_ROM_PTR(&mp_math_sin_obj) },
    { MP_ROM_QSTR(MP_QSTR_tan), MP_ROM_PTR(&mp_math_tan_obj) },
    { MP_ROM_QSTR(MP_QSTR_acos), MP_ROM_PTR(&mp_math_acos_obj) },
    { MP_ROM_QSTR(MP_QSTR_asin), MP_ROM_PTR(&mp_math_asin_obj) },
    { MP_ROM_QSTR(MP_QSTR_atan), MP_ROM_PTR(&mp_math_atan_obj) },
    { MP_ROM_QSTR(MP_QSTR_atan2), MP_ROM_PTR(&mp_math_atan2_obj) },
    { MP_ROM_QSTR(MP_QSTR_ceil), MP_ROM_PTR(&mp_math_ceil_obj) },
    { MP_ROM_QSTR(MP_QSTR_copysign), MP_ROM_PTR(&mp_math_copysign_obj) },
    { MP_ROM_QSTR(MP_QSTR_fabs), MP_ROM_PTR(&mp_math_fabs_obj) },
    { MP_ROM_QSTR(MP_QSTR_floor), MP_ROM_PTR(&mp_math_floor_obj) },
    { MP_ROM_QSTR(MP_QSTR_fmod), MP_ROM_PTR(&mp_math_fmod_obj) },
    { MP_ROM_QSTR(MP_QSTR_frexp), MP_ROM_PTR(&mp_math_frexp_obj) },
    { MP_ROM_QSTR(MP_QSTR_ldexp), MP_ROM_PTR(&mp_math_ldexp_obj) },
    { MP_ROM_QSTR(MP_QSTR_modf), MP_ROM_PTR(&mp_math_modf_obj) },
    { MP_ROM_QSTR(MP_QSTR_isfinite), MP_ROM_PTR(&mp_math_isfinite_obj) },
    { MP_ROM_QSTR(MP_QSTR_isinf), MP_ROM_PTR(&mp_math_isinf_obj) },
    { MP_ROM_QSTR(MP_QSTR_isnan), MP_ROM_PTR(&mp_math_isnan_obj) },
    { MP_ROM_QSTR(MP_QSTR_trunc), MP_ROM_PTR(&mp_math_trunc_obj) },
    { MP_ROM_QSTR(MP_QSTR_radians), MP_ROM_PTR(&mp_math_radians_obj) },
    { MP_ROM_QSTR(MP_QSTR_degrees), MP_ROM_PTR(&mp_math_degrees_obj) },
    #if MICROPY_PY_MATH_SPECIAL_FUNCTIONS
    { MP_ROM_QSTR(MP_QSTR_erf), MP_ROM_PTR(&mp_math_erf_obj) },
    { MP_ROM_QSTR(MP_QSTR_erfc), MP_ROM_PTR(&mp_math_erfc_obj) },
    { MP_ROM_QSTR(MP_QSTR_gamma), MP_ROM_PTR(&mp_math_gamma_obj) },
    { MP_ROM_QSTR(MP_QSTR_lgamma), MP_ROM_PTR(&mp_math_lgamma_obj) },
    #endif
};

STATIC MP_DEFINE_CONST_DICT(mp_module_math_globals, mp_module_math_globals_table);

const mp_obj_module_t mp_module_math = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_math_globals,
};

#endif // MICROPY_PY_BUILTINS_FLOAT && MICROPY_PY_MATH
