/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/********************************************************
 * The virtual string file
 ********************************************************/

#include "../../gfx.h"

#if GFX_USE_GFILE && GFILE_NEED_STRINGS

#include "gfile_fs.h"

#include <string.h>

// Special String VMT
static int StringRead(GFILE *f, void *buf, int size) {
	int		res;
	char	*p;

	p = ((char *)f->obj) + f->pos;
	for(res = 0; res < size && *p; res++, p++, buf = ((char *)buf)+1)
		((char *)buf)[0] = *p;
	return res;
}
static int StringWrite(GFILE *f, const void *buf, int size) {
	if ((f->flags & GFILEFLG_APPEND)) {
		while(((char *)f->obj)[f->pos])
			f->pos++;
	}
	memcpy(((char *)f->obj)+f->pos, buf, size);
	((char *)f->obj)[f->pos+size] = 0;
	return size;
}

static const GFILEVMT StringVMT = {
	0,								// flags
	'_',							// prefix
	0, 0, 0, 0,
	0, 0, StringRead, StringWrite,
	0, 0, 0,
	0, 0, 0,
	#if GFILE_NEED_FILELISTS
		0, 0, 0,
	#endif
};

static void gfileOpenStringFromStaticGFILE(GFILE *f, char *str) {
	if ((f->flags & GFILEFLG_TRUNC))
		str[0] = 0;
	f->vmt = &StringVMT;
	f->obj = str;
	f->pos = 0;
	f->flags |= GFILEFLG_OPEN|GFILEFLG_CANSEEK;
}

GFILE *gfileOpenString(char *str, const char *mode) {
	GFILE	*f;

	// Get an empty file and set the flags
	if (!(f = _gfileFindSlot(mode)))
		return 0;

	// File is open - fill in all the details
	gfileOpenStringFromStaticGFILE(f, str);
	return f;
}

#if GFILE_NEED_PRINTG
	int snprintg(char *buf, int maxlen, const char *fmt, ...) {
		int		res;
		GFILE	f;
		va_list	ap;

		if (maxlen <= 1) {
			if (maxlen == 1) {
				*buf = 0;
				return 0;
			}
			maxlen += 1;
		}

		f.flags = GFILEFLG_WRITE|GFILEFLG_TRUNC;
		gfileOpenStringFromStaticGFILE(&f, buf);

		va_start(ap, fmt);
		res = vfnprintg(&f, maxlen-1, fmt, ap);
		va_end(ap);
		return res;
	}
	int vsnprintg(char *buf, int maxlen, const char *fmt, va_list arg) {
		GFILE	f;

		if (maxlen <= 1) {
			if (maxlen == 1) {
				*buf = 0;
				return 0;
			}
			maxlen += 1;
		}

		f.flags = GFILEFLG_WRITE|GFILEFLG_TRUNC;
		gfileOpenStringFromStaticGFILE(&f, buf);

		return vfnprintg(&f, maxlen-1, fmt, arg);
	}
#endif

#if GFILE_NEED_SCANG
	int sscang(const char *buf, const char *fmt, ...) {
		int		res;
		GFILE	f;
		va_list	ap;

		f.flags = GFILEFLG_READ;
		gfileOpenStringFromStaticGFILE(&f, (char *)buf);

		va_start(ap, fmt);
		res = vfscang(&f, fmt, ap);
		va_end(ap);
		return res;
	}

	int vsscang(const char *buf, const char *fmt, va_list arg) {
		GFILE	f;

		f.flags = GFILEFLG_READ;
		gfileOpenStringFromStaticGFILE(&f, (char *)buf);

		return vfscang(&f, fmt, arg);
	}
#endif

#endif //GFX_USE_GFILE && GFILE_NEED_STRINGS
