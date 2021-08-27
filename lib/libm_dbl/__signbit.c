#include "libm.h"

int __signbitd(double x)
{
	union {
		double d;
		uint64_t i;
	} y = { x };
	return y.i>>63;
}
