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

#if MICROPY_PY_BUILTINS_FLOAT && MICROPY_PY_CMATH

#include <math.h>

/// \module cmath - mathematical functions for complex numbers
///
/// The `cmath` module provides some basic mathematical funtions for
/// working with complex numbers.

// These are defined in modmath.c
/// \constant e - base of the natural logarithm
extern const mp_obj_float_t mp_math_e_obj;
/// \constant pi - the ratio of a circle's circumference to its diameter
extern const mp_obj_float_t mp_math_pi_obj;

/// \function phase(z)
/// Returns the phase of the number `z`, in the range (-pi, +pi].
STATIC mp_obj_t mp_cmath_phase(mp_obj_t z_obj) {
    mp_float_t real, imag;
    mp_obj_get_complex(z_obj, &real, &imag);
    return mp_obj_new_float(MICROPY_FLOAT_C_FUN(atan2)(imag, real));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_cmath_phase_obj, mp_cmath_phase);

/// \function polar(z)
/// Returns, as a tuple, the polar form of `z`.
STATIC mp_obj_t mp_cmath_polar(mp_obj_t z_obj) {
    mp_float_t real, imag;
    mp_obj_get_complex(z_obj, &real, &imag);
    mp_obj_t tuple[2] = {
        mp_obj_new_float(MICROPY_FLOAT_C_FUN(sqrt)(real*real + imag*imag)),
        mp_obj_new_float(MICROPY_FLOAT_C_FUN(atan2)(imag, real)),
    };
    return mp_obj_new_tuple(2, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_cmath_polar_obj, mp_cmath_polar);

/// \function rect(r, phi)
/// Returns the complex number with modulus `r` and phase `phi`.
STATIC mp_obj_t mp_cmath_rect(mp_obj_t r_obj, mp_obj_t phi_obj) {
    mp_float_t r = mp_obj_get_float(r_obj);
    mp_float_t phi = mp_obj_get_float(phi_obj);
    return mp_obj_new_complex(r * MICROPY_FLOAT_C_FUN(cos)(phi), r * MICROPY_FLOAT_C_FUN(sin)(phi));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_cmath_rect_obj, mp_cmath_rect);

/// \function exp(z)
/// Return the exponential of `z`.
STATIC mp_obj_t mp_cmath_exp(mp_obj_t z_obj) {
    mp_float_t real, imag;
    mp_obj_get_complex(z_obj, &real, &imag);
    mp_float_t exp_real = MICROPY_FLOAT_C_FUN(exp)(real);
    return mp_obj_new_complex(exp_real * MICROPY_FLOAT_C_FUN(cos)(imag), exp_real * MICROPY_FLOAT_C_FUN(sin)(imag));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_cmath_exp_obj, mp_cmath_exp);

/// \function log(z)
/// Return the natural logarithm of `z`.  The branch cut is along the negative real axis.
// TODO can take second argument, being the base
STATIC mp_obj_t mp_cmath_log(mp_obj_t z_obj) {
    mp_float_t real, imag;
    mp_obj_get_complex(z_obj, &real, &imag);
    return mp_obj_new_complex(0.5 * MICROPY_FLOAT_C_FUN(log)(real*real + imag*imag), MICROPY_FLOAT_C_FUN(atan2)(imag, real));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_cmath_log_obj, mp_cmath_log);

/// \function log10(z)
/// Return the base-10 logarithm of `z`.  The branch cut is along the negative real axis.
STATIC mp_obj_t mp_cmath_log10(mp_obj_t z_obj) {
    mp_float_t real, imag;
    mp_obj_get_complex(z_obj, &real, &imag);
    return mp_obj_new_complex(0.5 * MICROPY_FLOAT_C_FUN(log10)(real*real + imag*imag), 0.4342944819032518 * MICROPY_FLOAT_C_FUN(atan2)(imag, real));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_cmath_log10_obj, mp_cmath_log10);

/// \function sqrt(z)
/// Return the square-root of `z`.
STATIC mp_obj_t mp_cmath_sqrt(mp_obj_t z_obj) {
    mp_float_t real, imag;
    mp_obj_get_complex(z_obj, &real, &imag);
    mp_float_t sqrt_abs = MICROPY_FLOAT_C_FUN(pow)(real*real + imag*imag, 0.25);
    mp_float_t theta = 0.5 * MICROPY_FLOAT_C_FUN(atan2)(imag, real);
    return mp_obj_new_complex(sqrt_abs * MICROPY_FLOAT_C_FUN(cos)(theta), sqrt_abs * MICROPY_FLOAT_C_FUN(sin)(theta));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_cmath_sqrt_obj, mp_cmath_sqrt);

/// \function cos(z)
/// Return the cosine of `z`.
STATIC mp_obj_t mp_cmath_cos(mp_obj_t z_obj) {
    mp_float_t real, imag;
    mp_obj_get_complex(z_obj, &real, &imag);
    return mp_obj_new_complex(MICROPY_FLOAT_C_FUN(cos)(real) * MICROPY_FLOAT_C_FUN(cosh)(imag), -MICROPY_FLOAT_C_FUN(sin)(real) * MICROPY_FLOAT_C_FUN(sinh)(imag));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_cmath_cos_obj, mp_cmath_cos);

/// \function sin(z)
/// Return the sine of `z`.
STATIC mp_obj_t mp_cmath_sin(mp_obj_t z_obj) {
    mp_float_t real, imag;
    mp_obj_get_complex(z_obj, &real, &imag);
    return mp_obj_new_complex(MICROPY_FLOAT_C_FUN(sin)(real) * MICROPY_FLOAT_C_FUN(cosh)(imag), MICROPY_FLOAT_C_FUN(cos)(real) * MICROPY_FLOAT_C_FUN(sinh)(imag));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_cmath_sin_obj, mp_cmath_sin);

STATIC const mp_map_elem_t mp_module_cmath_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_OBJ_NEW_QSTR(MP_QSTR_cmath) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_e), (mp_obj_t)&mp_math_e_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_pi), (mp_obj_t)&mp_math_pi_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_phase), (mp_obj_t)&mp_cmath_phase_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_polar), (mp_obj_t)&mp_cmath_polar_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_rect), (mp_obj_t)&mp_cmath_rect_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_exp), (mp_obj_t)&mp_cmath_exp_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_log), (mp_obj_t)&mp_cmath_log_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_log10), (mp_obj_t)&mp_cmath_log10_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sqrt), (mp_obj_t)&mp_cmath_sqrt_obj },
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_acos), (mp_obj_t)&mp_cmath_acos_obj },
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_asin), (mp_obj_t)&mp_cmath_asin_obj },
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_atan), (mp_obj_t)&mp_cmath_atan_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_cos), (mp_obj_t)&mp_cmath_cos_obj },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sin), (mp_obj_t)&mp_cmath_sin_obj },
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_tan), (mp_obj_t)&mp_cmath_tan_obj },
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_acosh), (mp_obj_t)&mp_cmath_acosh_obj },
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_asinh), (mp_obj_t)&mp_cmath_asinh_obj },
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_atanh), (mp_obj_t)&mp_cmath_atanh_obj },
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_cosh), (mp_obj_t)&mp_cmath_cosh_obj },
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_sinh), (mp_obj_t)&mp_cmath_sinh_obj },
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_tanh), (mp_obj_t)&mp_cmath_tanh_obj },
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_isfinite), (mp_obj_t)&mp_cmath_isfinite_obj },
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_isinf), (mp_obj_t)&mp_cmath_isinf_obj },
    //{ MP_OBJ_NEW_QSTR(MP_QSTR_isnan), (mp_obj_t)&mp_cmath_isnan_obj },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_cmath_globals, mp_module_cmath_globals_table);

const mp_obj_module_t mp_module_cmath = {
    .base = { &mp_type_module },
    .name = MP_QSTR_cmath,
    .globals = (mp_obj_dict_t*)&mp_module_cmath_globals,
};

#endif // MICROPY_PY_BUILTINS_FLOAT && MICROPY_PY_CMATH
