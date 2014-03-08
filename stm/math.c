#include <math.h>

// these 2 functions seem to actually work... no idea why
// replacing with libgcc does not work (probably due to wrong calling conventions)
double __aeabi_f2d(float x) {
    // TODO
    return 0.0;
}

float __aeabi_d2f(double x) {
    // TODO
    return 0.0;
}

/*
double sqrt(double x) {
    // TODO
    return 0.0;
}
*/

float sqrtf(float x) {
    asm volatile (
            "vsqrt.f32  %[r], %[x]\n"
            : [r] "=t" (x)
            : [x] "t"  (x));
    return x;
}

// TODO we need import these functions from some library (eg musl or newlib)
float powf(float x, float y) { return 0.0; }
float expf(float x) { return 0.0; }
float logf(float x) { return 0.0; }
float log2f(float x) { return 0.0; }
float log10f(float x) { return 0.0; }
float coshf(float x) { return 0.0; }
float sinhf(float x) { return 0.0; }
float tanhf(float x) { return 0.0; }
float acoshf(float x) { return 0.0; }
float asinhf(float x) { return 0.0; }
float atanhf(float x) { return 0.0; }
float cosf(float x) { return 0.0; }
float sinf(float x) { return 0.0; }
float tanf(float x) { return 0.0; }
float acosf(float x) { return 0.0; }
float asinf(float x) { return 0.0; }
float atanf(float x) { return 0.0; }
float atan2f(float x, float y) { return 0.0; }
