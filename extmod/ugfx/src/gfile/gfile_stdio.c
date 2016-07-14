/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/********************************************************
 * Stdio Emulation Routines
 ********************************************************/

#include "../../gfx.h"

#if GFX_USE_GFILE && GFILE_NEED_STDIO && !defined(GFILE_NEED_STDIO_MUST_BE_OFF)

#include "gfile_fs.h"

size_t gstdioRead(void * ptr, size_t size, size_t count, GFILE *f) {
	return gfileRead(f, ptr, size*count)/size;
}

size_t gstdioWrite(const void * ptr, size_t size, size_t count, GFILE *f) {
	return gfileWrite(f, ptr, size*count)/size;
}

int gstdioSeek(GFILE *f, size_t offset, int origin) {
	switch(origin) {
	case SEEK_SET:
		break;
	case SEEK_CUR:
		offset += f->pos;
		break;
	case SEEK_END:
		offset += gfileGetSize(f);
		break;
	default:
		return -1;
	}
	return gfileSetPos(f, offset) ? 0 : -1;
}

int gstdioGetpos(GFILE *f, long int *pos) {
	if (!(f->flags & GFILEFLG_OPEN))
		return -1;
	*pos = f->pos;
	return 0;
}

#endif //GFX_USE_GFILE && GFILE_NEED_STDIO
