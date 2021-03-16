/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Paul Sokolovsky
 * Copyright (c) 2016 Scott Shawcroft for Adafruit Industries
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

#include <assert.h>
#include <string.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "shared-bindings/random/__init__.h"
#include "supervisor/shared/translate.h"

//| """pseudo-random numbers and choices
//|
//| The `random` module is a strict subset of the CPython `cpython:random`
//| module. So, code written in CircuitPython will work in CPython but not
//| necessarily the other way around.
//|
//| Like its CPython cousin, CircuitPython's random seeds itself on first use
//| with a true random from os.urandom() when available or the uptime otherwise.
//| Once seeded, it will be deterministic, which is why its bad for cryptography.
//|
//| .. warning:: Numbers from this module are not cryptographically strong! Use
//|   bytes from `os.urandom` directly for true randomness."""
//|
//| from typing import TypeVar
//| _T = TypeVar('_T')
//|

//| def seed(seed: int) -> None:
//|     """Sets the starting seed of the random  number generation. Further calls to
//|     `random` will return deterministic results afterwards."""
//|     ...
//|
STATIC mp_obj_t random_seed(mp_obj_t seed_in) {
    mp_uint_t seed = mp_obj_get_int_truncated(seed_in);
    shared_modules_random_seed(seed);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(random_seed_obj, random_seed);

//| def getrandbits(k: int) -> int:
//|     """Returns an integer with *k* random bits."""
//|     ...
//|
STATIC mp_obj_t random_getrandbits(mp_obj_t num_in) {
    int n = mp_obj_get_int(num_in);
    if (n > 32 || n == 0) {
        mp_raise_ValueError(NULL);
    }
    return mp_obj_new_int_from_uint(shared_modules_random_getrandbits((uint8_t)n));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(random_getrandbits_obj, random_getrandbits);

//| def randrange(stop: Tuple[int, int, int]) -> int:
//|     """Returns a randomly selected integer from ``range(start, stop, step)``."""
//|     ...
//|
STATIC mp_obj_t random_randrange(size_t n_args, const mp_obj_t *args) {
    mp_int_t start = 0;
    mp_int_t stop = mp_obj_get_int(args[0]);
    mp_int_t step = 1;
    if (n_args == 1) {
        // range(stop)
        if (stop <= 0) {
            mp_raise_ValueError(translate("stop not reachable from start"));
        }
    } else {
        start = stop;
        stop = mp_obj_get_int(args[1]);
        if (n_args == 2) {
            // range(start, stop)
            if (start >= stop) {
                mp_raise_ValueError(translate("stop not reachable from start"));
            }
        } else {
            // range(start, stop, step)
            step = mp_obj_get_int(args[2]);
            mp_int_t n;
            if (step > 0) {
                n = (stop - start + step - 1) / step;
            } else if (step < 0) {
                n = (stop - start + step + 1) / step;
            } else {
                mp_raise_ValueError(translate("step must be non-zero"));
            }
            if (n <= 0) {
                mp_raise_ValueError(translate("invalid step"));
            }
        }
    }

    return mp_obj_new_int(shared_modules_random_randrange(start, stop, step));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(random_randrange_obj, 1, 3, random_randrange);

//| def randint(a: int, b: int) -> int:
//|     """Returns a randomly selected integer between a and b inclusive. Equivalent
//|     to ``randrange(a, b + 1, 1)``"""
//|     ...
//|
STATIC mp_obj_t random_randint(mp_obj_t a_in, mp_obj_t b_in) {
    mp_int_t a = mp_obj_get_int(a_in);
    mp_int_t b = mp_obj_get_int(b_in);
    if (a > b) {
        mp_raise_ValueError(NULL);
    }
    return mp_obj_new_int(shared_modules_random_randrange(a, b + 1, 1));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(random_randint_obj, random_randint);

//| def choice(seq: Sequence[_T]) -> _T:
//|     """Returns a randomly selected element from the given sequence. Raises
//|     IndexError when the sequence is empty."""
//|     ...
//|
STATIC mp_obj_t random_choice(mp_obj_t seq) {
    mp_int_t len = mp_obj_get_int(mp_obj_len(seq));
    if (len == 0) {
        mp_raise_IndexError(translate("empty sequence"));
    }
    return mp_obj_subscr(seq, mp_obj_new_int(shared_modules_random_randrange(0, len, 1)), MP_OBJ_SENTINEL);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(random_choice_obj, random_choice);

//| def random() -> float:
//|     """Returns a random float between 0 and 1.0."""
//|     ...
//|
STATIC mp_obj_t random_random(void) {
    return mp_obj_new_float(shared_modules_random_random());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(random_random_obj, random_random);

//| def uniform(a: float, b: float) -> float:
//|     """Returns a random float between a and b. It may or may not be inclusive
//|     depending on float rounding."""
//|     ...
//|
STATIC mp_obj_t random_uniform(mp_obj_t a_in, mp_obj_t b_in) {
    mp_float_t a = mp_obj_get_float(a_in);
    mp_float_t b = mp_obj_get_float(b_in);
    return mp_obj_new_float(shared_modules_random_uniform(a, b));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(random_uniform_obj, random_uniform);

STATIC const mp_rom_map_elem_t mp_module_random_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_random) },
    { MP_ROM_QSTR(MP_QSTR_seed), MP_ROM_PTR(&random_seed_obj) },
    { MP_ROM_QSTR(MP_QSTR_getrandbits), MP_ROM_PTR(&random_getrandbits_obj) },
    { MP_ROM_QSTR(MP_QSTR_randrange), MP_ROM_PTR(&random_randrange_obj) },
    { MP_ROM_QSTR(MP_QSTR_randint), MP_ROM_PTR(&random_randint_obj) },
    { MP_ROM_QSTR(MP_QSTR_choice), MP_ROM_PTR(&random_choice_obj) },
    { MP_ROM_QSTR(MP_QSTR_random), MP_ROM_PTR(&random_random_obj) },
    { MP_ROM_QSTR(MP_QSTR_uniform), MP_ROM_PTR(&random_uniform_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_random_globals, mp_module_random_globals_table);

const mp_obj_module_t random_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_random_globals,
};
