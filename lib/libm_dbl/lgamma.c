#include <math.h>

double __lgamma_r(double, int*);

double lgamma(double x) {
    int sign;
    return __lgamma_r(x, &sign);
}
