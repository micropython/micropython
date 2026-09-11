double sqrt(double x) {
    double ret;
    __asm__ volatile (
            "fsqrt %d0, %d1\n"
            : "=w" (ret)
            : "w"  (x));
    return ret;
}
