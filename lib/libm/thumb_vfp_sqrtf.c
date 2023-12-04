// an implementation of sqrtf for Thumb using hardware VFP instructions

#include "libm.h"

#define sqrtf LIBM_FUNC_WRAP(sqrtf)

float sqrtf(float x) {
    __asm__ volatile (
            "vsqrt.f32  %[r], %[x]\n"
            : [r] "=t" (x)
            : [x] "t"  (x));
    return x;
}
