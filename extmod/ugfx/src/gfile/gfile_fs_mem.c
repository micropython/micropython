/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/********************************************************
 * The virtual memory file-system
 ********************************************************/

#include "../../gfx.h"

#if GFX_USE_GFILE && GFILE_NEED_MEMFS

#include "gfile_fs.h"

#include <string.h>

static int MEMRead(GFILE *f, void *buf, int size);
static int MEMWrite(GFILE *f, const void *buf, int size);
static bool_t MEMSetpos(GFILE *f, long int pos);

static const GFILEVMT FsMemVMT = {
	GFSFLG_SEEKABLE|GFSFLG_WRITEABLE,					// flags
	0,													// prefix
	0, 0, 0, 0,
	0, 0, MEMRead, MEMWrite,
	MEMSetpos, 0, 0,
	0, 0, 0,
	#if GFILE_NEED_FILELISTS
		0, 0, 0,
	#endif
};

static int MEMRead(GFILE *f, void *buf, int size) {
	memcpy(buf, ((char *)f->obj)+f->pos, size);
	return size;
}
static int MEMWrite(GFILE *f, const void *buf, int size) {
	memcpy(((char *)f->obj)+f->pos, buf, size);
	return size;
}
static bool_t MEMSetpos(GFILE *f, long int pos) {
	(void) f;
	(void) pos;
	return TRUE;
}

GFILE *	gfileOpenMemory(void *memptr, const char *mode) {
	GFILE	*f;

	// Get an empty file and set the flags
	if (!(f = _gfileFindSlot(mode)))
		return 0;

	// File is open - fill in all the details
	f->vmt = &FsMemVMT;
	f->obj = memptr;
	f->pos = 0;
	f->flags |= GFILEFLG_OPEN|GFILEFLG_CANSEEK;
	return f;
}

#endif //GFX_USE_GFILE && GFILE_NEED_MEMFS
