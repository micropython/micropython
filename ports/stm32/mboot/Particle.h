// Header for libhydrogen use only.  Act like a Particle board for the random
// implementation.  This code is not actually called when just decrypting and
// verifying a signature, so call a function that does not exist in order to
// trigger a link error if it is unintentionally used.

#include <stdint.h>

uint32_t invalid_function_used_to_prevent_code_being_used(void);

static inline uint32_t HAL_RNG_GetRandomNumber(void) {
    return invalid_function_used_to_prevent_code_being_used();
}
