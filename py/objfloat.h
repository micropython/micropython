/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Ihor Nehrutsa
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

#if MICROPY_PY_BUILTINS_FLOAT

#if MICROPY_OBJ_REPR != MICROPY_OBJ_REPR_C && MICROPY_OBJ_REPR != MICROPY_OBJ_REPR_D

// M_E and M_PI are not part of the math.h standard and may not be defined
#ifndef M_E
#define M_E (2.7182818284590452354)
#endif
#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

typedef struct _mp_obj_float_t {
    mp_obj_base_t base;
    mp_float_t value;
} mp_obj_float_t;

#endif

#define MICROPY_FLOAT_ZERO MICROPY_FLOAT_CONST(0.0)

#define MP_DEFINE_FLOAT_OBJ(obj_name, f) mp_obj_float_t obj_name = {{&mp_type_float}, f} 

#endif // MICROPY_PY_BUILTINS_FLOAT
