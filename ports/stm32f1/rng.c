#include "rng.h"

// For MCUs that don't have an RNG we still need to provide a rng_get() function,
// eg for mbedtls.  A pseudo-RNG is not really ideal but we go with it for now.  We
// don't want to use urandom's pRNG because then the user won't see a reproducible
// random stream.

// Yasmarang random number generator by Ilya Levin
// http://www.literatecode.com/yasmarang
STATIC uint32_t pyb_rng_yasmarang(void) {
    static uint32_t pad = 0xeda4baba, n = 69, d = 233;
    static uint8_t dat = 0;

    pad += dat + d * n;
    pad = (pad << 3) + (pad >> 29);
    n = pad | 2;
    d ^= (pad << 31) + (pad >> 1);
    dat ^= (char)pad ^ (d >> 8) ^ 1;

    return pad ^ (d << 5) ^ (pad >> 18) ^ (dat << 1);
}

uint32_t rng_get(void) {
    return pyb_rng_yasmarang();
}

STATIC mp_obj_t pyb_rng_get(void) {
    return mp_obj_new_int(rng_get() >> 2);
}

MP_DEFINE_CONST_FUN_OBJ_0(pyb_rng_get_obj, pyb_rng_get);