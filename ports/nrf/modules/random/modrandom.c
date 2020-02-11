/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016 Paul Sokolovsky
 * Copyright (c) 2016 Damien P. George
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

#include "py/runtime.h"

#if MICROPY_PY_RANDOM_HW_RNG

#include "nrf_rng.h"
#include "modrandom.h"

#if BLUETOOTH_SD
#include "nrf_soc.h"
#include "ble_drv.h"
#define BLUETOOTH_STACK_ENABLED() (ble_drv_stack_enabled())
#endif

static inline uint32_t generate_hw_random(void) {
    uint32_t retval = 0;
    uint8_t * p_retval = (uint8_t *)&retval;

    nrf_rng_event_clear(NRF_RNG_EVENT_VALRDY);
    nrf_rng_task_trigger(NRF_RNG_TASK_START);

    for (uint16_t i = 0; i < 4; i++) {
        while (!nrf_rng_event_get(NRF_RNG_EVENT_VALRDY)) {
            ;
        }

        nrf_rng_event_clear(NRF_RNG_EVENT_VALRDY);
        p_retval[i] = nrf_rng_random_value_get();
    }

    nrf_rng_task_trigger(NRF_RNG_TASK_STOP);

    return retval;
}

uint32_t machine_rng_generate_random_word(void) {

#if BLUETOOTH_SD
    if (BLUETOOTH_STACK_ENABLED() == 1) {
        uint32_t retval = 0;
        uint32_t status;
        do {
            status = sd_rand_application_vector_get((uint8_t *)&retval, 4); // Extract 4 bytes
        } while (status != 0);

        return retval;
    }
#endif

    return generate_hw_random();
}

static inline int rand30(void) {
    uint32_t val = machine_rng_generate_random_word();
    return (val & 0x3fffffff); // binary mask b00111111111111111111111111111111
}

static inline int randbelow(int n) {
    return rand30() % n;
}

STATIC mp_obj_t mod_random_getrandbits(mp_obj_t num_in) {
    int n = mp_obj_get_int(num_in);
    if (n > 30 || n == 0) {
        mp_raise_ValueError(NULL);
    }
    uint32_t mask = ~0;
    // Beware of C undefined behavior when shifting by >= than bit size
    mask >>= (32 - n);
    return mp_obj_new_int_from_uint(rand30() & mask);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_random_getrandbits_obj, mod_random_getrandbits);

STATIC mp_obj_t mod_random_randrange(size_t n_args, const mp_obj_t *args) {
    mp_int_t start = mp_obj_get_int(args[0]);
    if (n_args == 1) {
        // range(stop)
        if (start > 0) {
            return mp_obj_new_int(randbelow(start));
        } else {
            mp_raise_ValueError(NULL);
        }
    } else {
        mp_int_t stop = mp_obj_get_int(args[1]);
        if (n_args == 2) {
            // range(start, stop)
            if (start < stop) {
                return mp_obj_new_int(start + randbelow(stop - start));
            } else {
                mp_raise_ValueError(NULL);
            }
        } else {
            // range(start, stop, step)
            mp_int_t step = mp_obj_get_int(args[2]);
            mp_int_t n;
            if (step > 0) {
                n = (stop - start + step - 1) / step;
            } else if (step < 0) {
                n = (stop - start + step + 1) / step;
            } else {
                mp_raise_ValueError(NULL);
            }
            if (n > 0) {
                return mp_obj_new_int(start + step * randbelow(n));
            } else {
                mp_raise_ValueError(NULL);
            }
        }
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mod_random_randrange_obj, 1, 3, mod_random_randrange);

STATIC mp_obj_t mod_random_randint(mp_obj_t a_in, mp_obj_t b_in) {
    mp_int_t a = mp_obj_get_int(a_in);
    mp_int_t b = mp_obj_get_int(b_in);
    if (a <= b) {
        return mp_obj_new_int(a + randbelow(b - a + 1));
    } else {
        mp_raise_ValueError(NULL);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_random_randint_obj, mod_random_randint);

STATIC mp_obj_t mod_random_choice(mp_obj_t seq) {
    mp_int_t len = mp_obj_get_int(mp_obj_len(seq));
    if (len > 0) {
        return mp_obj_subscr(seq, mp_obj_new_int(randbelow(len)), MP_OBJ_SENTINEL);
    } else {
        mp_raise_type(&mp_type_IndexError);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(mod_random_choice_obj, mod_random_choice);

#if MICROPY_PY_BUILTINS_FLOAT

// returns a number in the range [0..1) using RNG to fill in the fraction bits
STATIC mp_float_t randfloat(void) {
    #if MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_DOUBLE
    typedef uint64_t mp_float_int_t;
    #elif MICROPY_FLOAT_IMPL == MICROPY_FLOAT_IMPL_FLOAT
    typedef uint32_t mp_float_int_t;
    #endif
    union {
        mp_float_t f;
        #if MP_ENDIANNESS_LITTLE
        struct { mp_float_int_t frc:MP_FLOAT_FRAC_BITS, exp:MP_FLOAT_EXP_BITS, sgn:1; } p;
        #else
        struct { mp_float_int_t sgn:1, exp:MP_FLOAT_EXP_BITS, frc:MP_FLOAT_FRAC_BITS; } p;
        #endif
    } u;
    u.p.sgn = 0;
    u.p.exp = (1 << (MP_FLOAT_EXP_BITS - 1)) - 1;
    if (MP_FLOAT_FRAC_BITS <= 30) {
        u.p.frc = rand30();
    } else {
        u.p.frc = ((uint64_t)rand30() << 30) | (uint64_t)rand30();
    }
    return u.f - 1;
}

STATIC mp_obj_t mod_random_random(void) {
    return mp_obj_new_float(randfloat());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(mod_random_random_obj, mod_random_random);

STATIC mp_obj_t mod_random_uniform(mp_obj_t a_in, mp_obj_t b_in) {
    mp_float_t a = mp_obj_get_float(a_in);
    mp_float_t b = mp_obj_get_float(b_in);
    return mp_obj_new_float(a + (b - a) * randfloat());
}
STATIC MP_DEFINE_CONST_FUN_OBJ_2(mod_random_uniform_obj, mod_random_uniform);

#endif

STATIC const mp_rom_map_elem_t mp_module_random_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_random) },
    { MP_ROM_QSTR(MP_QSTR_getrandbits), MP_ROM_PTR(&mod_random_getrandbits_obj) },
    { MP_ROM_QSTR(MP_QSTR_randrange), MP_ROM_PTR(&mod_random_randrange_obj) },
    { MP_ROM_QSTR(MP_QSTR_randint), MP_ROM_PTR(&mod_random_randint_obj) },
    { MP_ROM_QSTR(MP_QSTR_choice), MP_ROM_PTR(&mod_random_choice_obj) },
#if MICROPY_PY_BUILTINS_FLOAT
    { MP_ROM_QSTR(MP_QSTR_random), MP_ROM_PTR(&mod_random_random_obj) },
    { MP_ROM_QSTR(MP_QSTR_uniform), MP_ROM_PTR(&mod_random_uniform_obj) },
#endif
};

STATIC MP_DEFINE_CONST_DICT(mp_module_random_globals, mp_module_random_globals_table);

const mp_obj_module_t random_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&mp_module_random_globals,
};

#endif // MICROPY_PY_RANDOM_HW_RNG
