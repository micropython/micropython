// an implementation of sqrt for Thumb using hardware double-precision VFP instructions

double sqrt(double x) {
    double ret;
    __asm__ volatile (
            "vsqrt.f64  %P0, %P1\n"
            : "=w" (ret)
            : "w"  (x));
    return ret;
}
