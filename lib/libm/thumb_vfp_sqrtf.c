// an implementation of sqrtf for Thumb using hardware VFP instructions

#include <math.h>

float sqrtf(float x) {
    __asm__ volatile (
            "vsqrt.f32  %[r], %[x]\n"
            : [r] "=t" (x)
            : [x] "t"  (x));
    return x;
}
