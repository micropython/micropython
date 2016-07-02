/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/********************************************************
 * Scang Routines
 ********************************************************/

#include "../../gfx.h"

#if GFX_USE_GFILE && GFILE_NEED_SCANG

int fscang(GFILE *f, const char *fmt, ...) {
	int		res;
	va_list	ap;

	va_start(ap, fmt);
	res = vfscang(f, fmt, ap);
	va_end(ap);
	return res;
}

int vfscang(GFILE *f, const char *fmt, va_list arg) {
	int		res, width, size, base;
	unsigned long num;
	char	c;
	bool_t	assign, negate;
	char	*p;

	for(res = 0; *fmt; fmt++) {
		switch(*fmt) {
		case ' ': case '\t': case '\r': case '\n': case '\v': case '\f':
			break;

		case '%':
			fmt++;
			assign = TRUE;
			negate = FALSE;
			width = 0;
			size = 1;
			num = 0;

			if (*fmt == '*') {
				fmt++;
				assign = FALSE;
			}
			while(*fmt >= '0' && *fmt <= '9')
				width = width * 10 + (*fmt++ - '0');
			if (*fmt == 'h') {
				fmt++;
				size = 0;
			} else if (*fmt == 'l') {
				fmt++;
				size = 2;
			} else if (*fmt == 'L') {
				fmt++;
				size = 3;
			}
			switch(*fmt) {
			case 0:
				return res;
			case '%':
				goto matchchar;
			case 'c':
				if (!width) {
					while(1) {
						if (!gfileRead(f, &c, 1))			return res;
						switch(c) {
						case ' ': case '\t': case '\r':
						case '\n': case '\v': case '\f':	continue;
						}
						break;
					}
					width = 1;
				} else {
					if (!gfileRead(f, &c, 1)) 				return res;
				}
				if (assign) {
					p = va_arg(arg, char *);
					res++;
					*p++ = c;
				}
				while(--width) {
					if (!gfileRead(f, &c, 1)) 			return res;
					if (assign) *p++ = c;
				}
				break;
			case 's':
				while(1) {
					if (!gfileRead(f, &c, 1))			return res;
					switch(c) {
					case ' ': case '\t': case '\r':
					case '\n': case '\v': case '\f':	continue;
					}
					break;
				}
				if (assign) {
					p = va_arg(arg, char *);
					res++;
					*p++ = c;
				}
				if (width) {
					while(--width) {
						if (!gfileRead(f, &c, 1)) {
							if (assign) *((char *)p) = 0;
							return res;
						}
						if (assign) *p++ = c;
					}
				} else {
					while(1) {
						if (!gfileRead(f, &c, 1)) {
							if (assign) *((char *)p) = 0;
							return res;
						}
						switch(c) {
						case ' ': case '\t': case '\r':
						case '\n': case '\v': case '\f':	break;
						default:
							if (assign) *p++ = c;
							continue;
						}
						break;
					}
					//ungetch(c);
				}
				if (assign) *p = 0;
				break;
			case 'd':	base = 10;	goto getnum;
			case 'i':	base = -1;	goto getnum;
			case 'o':	base = 8;	goto getnum;
			case 'u':	base = 10;	goto getnum;
			case 'x':	base = 16;	goto getnum;
			case 'b':	base = 2;
			getnum:
				while(1) {
					if (!gfileRead(f, &c, 1))			return res;
					switch(c) {
					case ' ': case '\t': case '\r':
					case '\n': case '\v': case '\f':	continue;
					}
					break;
				}
				if (c == '-' && *fmt != 'u') {
					negate = TRUE;
					if ((width && !--width) || !gfileRead(f, &c, 1))			return res;
				}
				if (base == -1) {
					if (c == '0') {
						if ((width && !--width) || !gfileRead(f, &c, 1))		goto assignnum;
						switch(c) {
						case 'x': case 'X':
							base = 16;
							if ((width && !--width) || !gfileRead(f, &c, 1))	return res;
							break;
						case 'b': case 'B':
							base = 2;
							if ((width && !--width) || !gfileRead(f, &c, 1))	return res;
							break;
						default:
							base = 8;
							break;
						}
					} else
						base = 10;
				}
				while(1) {
					if (c >= '0' && c <= '9' && c - '0' < base)
						num = num * base + (c - '0');
					else if (c >= 'A' && c <= 'F' && base == 16)
						num = num * base + (c - ('A'-10));
					else if (c >= 'a' && c <= 'f' && base == 16)
						num = num * base + (c - ('a'-10));
					else {
						// ungetch(c)
						break;
					}
					if ((width && !--width) || !gfileRead(f, &c, 1))
						break;
				}

			assignnum:
				if (negate)
					num = -num;

				if (assign) {
					switch(size) {
					case 0:				// short
						p = (char *)va_arg(arg, short *);
						res++;
						*((short *)p) = (short)num;
					case 1:				// int
						p = (char *)va_arg(arg, int *);
						res++;
						*((int *)p) = (int)num;
					case 2: case 3:		// long
						p = (char *)va_arg(arg, long *);
						res++;
						*((long *)p) = (long)num;
					}
				}
				break;

			#if GFILE_ALLOW_FLOATS
				case 'e': case 'f': case 'g':
					// TODO
			#endif
			default:
				return res;
			}

			break;

		default:
		matchchar:
			while(1) {
				if (!gfileRead(f, &c, 1))			return res;
				switch(c) {
				case ' ': case '\t': case '\r':
				case '\n': case '\v': case '\f':	continue;
				}
				break;
			}
			if (c != *fmt) 							return res;
			break;
		}
	}
	return res;
}

#endif //GFX_USE_GFILE && GFILE_NEED_SCANG
