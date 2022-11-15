/*
 * This file is part of the MicroPython project, http://micropython.org/
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

#if MICROPY_PY_BUILTINS_FLOAT && MICROPY_PY_BUILTINS_COMPLEX && MICROPY_PY_CMATH

#include <math.h>

// phase(z): returns the phase of the number z in the range (-pi, +pi]
STATIC mp_obj_t mp_cmath_phase(mp_obj_t z_obj) {
    mp_float_t real, imag;
    mp_obj_get_complex(z_obj, &real, &imag);
    return mp_obj_new_float(MICROPY_FLOAT_C_FUN(atan2)(imag, real));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_cmath_phase_obj, mp_cmath_phase);

// polar(z): returns the polar form of z as a tuple
STATIC mp_obj_t mp_cmath_polar(mp_obj_t z_obj) {
    mp_float_t real, imag;
    mp_obj_get_complex(z_obj, &real, &imag);
    mp_obj_t tuple[2] = {
        mp_obj_new_float(MICROPY_FLOAT_C_FUN(sqrt)(real * real + imag * imag)),
        mp_obj_new_float(MICROPY_FLOAT_C_FUN(atan2)(imag, real)),
    };
    return mp_obj_new_tuple(2, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_cmath_polar_obj, mp_cmath_polar);

// rect(r, phi): returns the complex number with modulus r and phase phi
STATIC mp_obj_t mp_cmath_rect(mp_obj_t r_obj, mp_obj_t phi_obj) {
    mp_float_t r = mp_obj_get_float(r_obj);
    mp_float_t phi = mp_obj_get_float(phi_obj);
    return mp_obj_new_complex(r * MICROPY_FLOAT_C_FUN(cos)(phi), r * MICROPY_FLOAT_C_FUN(sin)(phi));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mp_cmath_rect_obj, mp_cmath_rect);

// exp(z): return the exponential of z
STATIC mp_obj_t mp_cmath_exp(mp_obj_t z_obj) {
    mp_float_t real, imag;
    mp_obj_get_complex(z_obj, &real, &imag);
    mp_float_t exp_real = MICROPY_FLOAT_C_FUN(exp)(real);
    return mp_obj_new_complex(exp_real * MICROPY_FLOAT_C_FUN(cos)(imag), exp_real * MICROPY_FLOAT_C_FUN(sin)(imag));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_cmath_exp_obj, mp_cmath_exp);

// log(z): return the natural logarithm of z, with branch cut along the negative real axis
// TODO can take second argument, being the base
STATIC mp_obj_t mp_cmath_log(mp_obj_t z_obj) {
    mp_float_t real, imag;
    mp_obj_get_complex(z_obj, &real, &imag);
    return mp_obj_new_complex(MICROPY_FLOAT_CONST(0.5) * MICROPY_FLOAT_C_FUN(log)(real * real + imag * imag), MICROPY_FLOAT_C_FUN(atan2)(imag, real));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_cmath_log_obj, mp_cmath_log);

#if MICROPY_PY_MATH_SPECIAL_FUNCTIONS
// log10(z): return the base-10 logarithm of z, with branch cut along the negative real axis
STATIC mp_obj_t mp_cmath_log10(mp_obj_t z_obj) {
    mp_float_t real, imag;
    mp_obj_get_complex(z_obj, &real, &imag);
    return mp_obj_new_complex(MICROPY_FLOAT_CONST(0.5) * MICROPY_FLOAT_C_FUN(log10)(real * real + imag * imag), MICROPY_FLOAT_CONST(0.4342944819032518) * MICROPY_FLOAT_C_FUN(atan2)(imag, real));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_cmath_log10_obj, mp_cmath_log10);
#endif

// sqrt(z): return the square-root of z
STATIC mp_obj_t mp_cmath_sqrt(mp_obj_t z_obj) {
    mp_float_t real, imag;
    mp_obj_get_complex(z_obj, &real, &imag);
    mp_float_t sqrt_abs = MICROPY_FLOAT_C_FUN(pow)(real * real + imag * imag, MICROPY_FLOAT_CONST(0.25));
    mp_float_t theta = MICROPY_FLOAT_CONST(0.5) * MICROPY_FLOAT_C_FUN(atan2)(imag, real);
    return mp_obj_new_complex(sqrt_abs * MICROPY_FLOAT_C_FUN(cos)(theta), sqrt_abs * MICROPY_FLOAT_C_FUN(sin)(theta));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_cmath_sqrt_obj, mp_cmath_sqrt);

// cos(z): return the cosine of z
STATIC mp_obj_t mp_cmath_cos(mp_obj_t z_obj) {
    mp_float_t real, imag;
    mp_obj_get_complex(z_obj, &real, &imag);
    return mp_obj_new_complex(MICROPY_FLOAT_C_FUN(cos)(real) * MICROPY_FLOAT_C_FUN(cosh)(imag), -MICROPY_FLOAT_C_FUN(sin)(real) * MICROPY_FLOAT_C_FUN(sinh)(imag));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_cmath_cos_obj, mp_cmath_cos);

// sin(z): return the sine of z
STATIC mp_obj_t mp_cmath_sin(mp_obj_t z_obj) {
    mp_float_t real, imag;
    mp_obj_get_complex(z_obj, &real, &imag);
    return mp_obj_new_complex(MICROPY_FLOAT_C_FUN(sin)(real) * MICROPY_FLOAT_C_FUN(cosh)(imag), MICROPY_FLOAT_C_FUN(cos)(real) * MICROPY_FLOAT_C_FUN(sinh)(imag));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mp_cmath_sin_obj, mp_cmath_sin);

STATIC const mp_rom_map_elem_t mp_module_cmath_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_cmath) },
    { MP_ROM_QSTR(MP_QSTR_e), mp_const_float_e },
    { MP_ROM_QSTR(MP_QSTR_pi), mp_const_float_pi },
    { MP_ROM_QSTR(MP_QSTR_phase), MP_ROM_PTR(&mp_cmath_phase_obj) },
    { MP_ROM_QSTR(MP_QSTR_polar), MP_ROM_PTR(&mp_cmath_polar_obj) },
    { MP_ROM_QSTR(MP_QSTR_rect), MP_ROM_PTR(&mp_cmath_rect_obj) },
    { MP_ROM_QSTR(MP_QSTR_exp), MP_ROM_PTR(&mp_cmath_exp_obj) },
    { MP_ROM_QSTR(MP_QSTR_log), MP_ROM_PTR(&mp_cmath_log_obj) },
    #if MICROPY_PY_MATH_SPECIAL_FUNCTIONS
    { MP_ROM_QSTR(MP_QSTR_log10), MP_ROM_PTR(&mp_cmath_log10_obj) },
    #endif
    { MP_ROM_QSTR(MP_QSTR_sqrt), MP_ROM_PTR(&mp_cmath_sqrt_obj) },
    // { MP_ROM_QSTR(MP_QSTR_acos), MP_ROM_PTR(&mp_cmath_acos_obj) },
    // { MP_ROM_QSTR(MP_QSTR_asin), MP_ROM_PTR(&mp_cmath_asin_obj) },
    // { MP_ROM_QSTR(MP_QSTR_atan), MP_ROM_PTR(&mp_cmath_atan_obj) },
    { MP_ROM_QSTR(MP_QSTR_cos), MP_ROM_PTR(&mp_cmath_cos_obj) },
    { MP_ROM_QSTR(MP_QSTR_sin), MP_ROM_PTR(&mp_cmath_sin_obj) },
    // { MP_ROM_QSTR(MP_QSTR_tan), MP_ROM_PTR(&mp_cmath_tan_obj) },
    // { MP_ROM_QSTR(MP_QSTR_acosh), MP_ROM_PTR(&mp_cmath_acosh_obj) },
    // { MP_ROM_QSTR(MP_QSTR_asinh), MP_ROM_PTR(&mp_cmath_asinh_obj) },
    // { MP_ROM_QSTR(MP_QSTR_atanh), MP_ROM_PTR(&mp_cmath_atanh_obj) },
    // { MP_ROM_QSTR(MP_QSTR_cosh), MP_ROM_PTR(&mp_cmath_cosh_obj) },
    // { MP_ROM_QSTR(MP_QSTR_sinh), MP_ROM_PTR(&mp_cmath_sinh_obj) },
    // { MP_ROM_QSTR(MP_QSTR_tanh), MP_ROM_PTR(&mp_cmath_tanh_obj) },
    // { MP_ROM_QSTR(MP_QSTR_isfinite), MP_ROM_PTR(&mp_cmath_isfinite_obj) },
    // { MP_ROM_QSTR(MP_QSTR_isinf), MP_ROM_PTR(&mp_cmath_isinf_obj) },
    // { MP_ROM_QSTR(MP_QSTR_isnan), MP_ROM_PTR(&mp_cmath_isnan_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_cmath_globals, mp_module_cmath_globals_table);

const mp_obj_module_t mp_module_cmath = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_cmath_globals,
};

MP_REGISTER_MODULE(MP_QSTR_cmath, mp_module_cmath);

#endif // MICROPY_PY_BUILTINS_FLOAT && MICROPY_PY_BUILTINS_COMPLEX && MICROPY_PY_CMATH
