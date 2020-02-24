// Stub header for libhydrogen use
#include "py/mphal.h"


#include "rng.h"

static inline uint32_t HAL_RNG_GetRandomNumber() {
    return rng_get();
}