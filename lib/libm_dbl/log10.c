#include <math.h>

static const double _M_LN10 = 2.302585092994046;

double log10(double x) {
    return log(x) / (double)_M_LN10;
}
