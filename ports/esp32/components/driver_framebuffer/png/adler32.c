#include <stdint.h>
#include <stddef.h>

#include "adler32.h"

uint32_t
lib_adler32(const uint8_t *buf, size_t buf_len, uint32_t adler)
{
	uint32_t adler_a = adler & 0xffff;
	uint32_t adler_b = adler >> 16;

	while (buf_len > 5552)
	{
		int sub_len = 5552;
		while (sub_len-- > 0)
		{
			adler_a += *buf++;
			adler_b += adler_a;
		}
		buf_len -= 5552;
		adler_a %= 65521;
		adler_b %= 65521;
	}

	while (buf_len-- > 0)
	{
		adler_a += *buf++;
		adler_b += adler_a;
	}

	adler_a %= 65521;
	adler_b %= 65521;

	return (adler_b << 16) | adler_a;
}
