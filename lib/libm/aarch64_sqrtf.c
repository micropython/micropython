// An implementation of sqrtf for AArch64 using hardware fsqrt instruction.

#include <math.h>

float sqrtf(float x) {
    __asm__ volatile (
        "fsqrt %s0, %s1\n"
        : "=w" (x)
        : "w" (x));
    return x;
}
