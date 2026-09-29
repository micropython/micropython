// An implementation of sqrtf for AArch64 using hardware fsqrt instruction.

#include <math.h>

float sqrtf(float x) {
    float ret;
    __asm__ volatile (
        "fsqrt %s0, %s1\n"
        : "=w" (ret)
        : "w" (x));
    return ret;
}
