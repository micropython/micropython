#include <math.h>

double tanh(double x) {
    int sign = 0;
    if (x < 0) {
        sign = 1;
        x = -x;
    }
    x = expm1(-2 * x);
    x = x / (x + 2);
    return sign ? x : -x;
}
