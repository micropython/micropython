// adapted from the rintf() function from musl-1.1.16

#include "libm.h"

float nearbyintf(float x)
{
	union {float f; uint32_t i;} u = {x};
	int e = u.i>>23 & 0xff;
	int s = u.i>>31;
	float_t y;

	if (e >= 0x7f+23)
		return x;
	if (s)
		y = x - 0x1p23f + 0x1p23f;
	else
		y = x + 0x1p23f - 0x1p23f;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
	if (y == 0)
		return s ? -0.0f : 0.0f;
#pragma GCC diagnostic pop
	return y;
}
