/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/********************************************************
 * Printg Routines
 ********************************************************/

#include "../../gfx.h"

#if GFX_USE_GFILE && GFILE_NEED_PRINTG

#include <stdarg.h>

#define MAX_FILLER		11
#define FLOAT_PRECISION 100000

int fnprintg(GFILE *f, int maxlen, const char *fmt, ...) {
	int		res;
	va_list	ap;

	va_start(ap, fmt);
	res = vfnprintg(f, maxlen, fmt, ap);
	va_end(ap);
	return res;
}

static char *ltoa_wd(char *p, long num, unsigned radix, long divisor) {
	int		i;
	char *	q;

	if (!divisor) divisor = num;

	q = p + MAX_FILLER;
	do {
		i = (int)(num % radix);
		i += '0';
		if (i > '9')
		  i += 'A' - '0' - 10;
		*--q = i;
		num /= radix;
	} while ((divisor /= radix) != 0);

	i = (int)(p + MAX_FILLER - q);
	do {
		*p++ = *q++;
	} while (--i);

	return p;
}

int vfnprintg(GFILE *f, int maxlen, const char *fmt, va_list arg) {
	int		ret;
	char	*p, *s, c, filler;
	int		i, precision, width;
	bool_t	is_long, left_align;
	long	l;
	#if GFILE_ALLOW_FLOATS
		float	fpv;
		char	tmpbuf[2*MAX_FILLER + 1];
	#else
		char	tmpbuf[MAX_FILLER + 1];
	#endif

	ret = 0;
	if (maxlen < 0)
		return 0;
	if (!maxlen)
		maxlen = -1;

	while (*fmt) {
		if (*fmt != '%') {
			gfileWrite(f, fmt, 1);
			ret++; if (!--maxlen) return ret;
			fmt++;
			continue;
		}
		fmt++;

		p = s = tmpbuf;
		left_align = FALSE;
		filler = ' ';
		width = 0;
		precision = 0;

		if (*fmt == '-') {
			fmt++;
			left_align = TRUE;
		}
		if (*fmt == '0') {
			fmt++;
			filler = '0';
		}

		while (1) {
			c = *fmt++;
			if (c >= '0' && c <= '9') {
				c -= '0';
				width = width * 10 + c;
			} else if (c == '*')
				width = va_arg(arg, int);
			else
				break;
		}
		if (c == '.') {
			while (1) {
				c = *fmt++;
				if (c >= '0' && c <= '9') {
					c -= '0';
					precision = precision * 10 + c;
				} else if (c == '*')
					precision = va_arg(arg, int);
				else
					break;
			}
		}
		/* Long modifier.*/
		if (c == 'l' || c == 'L') {
			is_long = TRUE;
			if (*fmt)
				c = *fmt++;
		}
		else
			is_long = (c >= 'A') && (c <= 'Z');

		/* Command decoding.*/
		switch (c) {
		case 0:
			return ret;
		case 'c':
			filler = ' ';
			*p++ = va_arg(arg, int);
			break;
		case 's':
			filler = ' ';
			if ((s = va_arg(arg, char *)) == 0)
				s = "(null)";
			if (precision == 0)
				precision = 32767;
			for (p = s; *p && (--precision >= 0); p++);
			break;
		case 'D':
		case 'd':
			if (is_long)
				l = va_arg(arg, long);
			else
				l = va_arg(arg, int);
			if (l < 0) {
				*p++ = '-';
				l = -l;
			}
			p = ltoa_wd(p, l, 10, 0);
			break;
		#if GFILE_ALLOW_FLOATS
			case 'f':
				fpv = (float) va_arg(arg, double);
				if (fpv < 0) {
					*p++ = '-';
					fpv = -fpv;
				}
				l = fpv;
				p = ltoa_wd(p, l, 10, 0);
				*p++ = '.';
				l = (fpv - l) * FLOAT_PRECISION;
				p = ltoa_wd(p, l, 10, FLOAT_PRECISION / 10);
				break;
		#endif
		case 'X':
		case 'x':
			c = 16;
			goto unsigned_common;
		case 'U':
		case 'u':
			c = 10;
			goto unsigned_common;
		case 'O':
		case 'o':
			c = 8;
		unsigned_common:
			if (is_long)
				l = va_arg(arg, long);
			else
				l = va_arg(arg, int);
			p = ltoa_wd(p, l, c, 0);
			break;
		default:
			*p++ = c;
			break;
		}

		i = (int)(p - s);
		if ((width -= i) < 0)
			width = 0;
		if (left_align == FALSE)
			width = -width;
		if (width < 0) {
			if (*s == '-' && filler == '0') {
				gfileWrite(f, s++, 1);
				ret++; if (!--maxlen) return ret;
				i--;
			}
			do {
				gfileWrite(f, &filler, 1);
				ret++; if (!--maxlen) return ret;
			} while (++width != 0);
		}
		while (--i >= 0) {
			gfileWrite(f, s++, 1);
			ret++; if (!--maxlen) return ret;
		}
		while (width) {
			gfileWrite(f, &filler, 1);
			ret++; if (!--maxlen) return ret;
			width--;
		}
	}
	return ret;
}

#endif //GFX_USE_GFILE && GFILE_NEED_PRINTG
