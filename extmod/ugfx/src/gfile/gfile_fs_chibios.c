/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/********************************************************
 * The ChibiOS FileStream file-system
 ********************************************************/

#include "../../gfx.h"

#if GFX_USE_GFILE && GFILE_NEED_CHIBIOSFS && GFX_USE_OS_CHIBIOS

#include "gfile_fs.h"

static void ChibiOSBFSClose(GFILE *f);
static int ChibiOSBFSRead(GFILE *f, void *buf, int size);
static int ChibiOSBFSWrite(GFILE *f, const void *buf, int size);
static bool_t ChibiOSBFSSetpos(GFILE *f, long int pos);
static long int ChibiOSBFSGetsize(GFILE *f);
static bool_t ChibiOSBFSEof(GFILE *f);

static const GFILEVMT FsCHIBIOSVMT = {
	GFSFLG_SEEKABLE|GFSFLG_WRITEABLE,					// flags
	0,													// prefix
	0, 0, 0, 0,
	0, ChibiOSBFSClose, ChibiOSBFSRead, ChibiOSBFSWrite,
	ChibiOSBFSSetpos, ChibiOSBFSGetsize, ChibiOSBFSEof,
	0, 0, 0,
	#if GFILE_NEED_FILELISTS
		0, 0, 0,
	#endif
};

#if CH_KERNEL_MAJOR == 2
	#define FileStream			BaseFileStream
	#define fileStreamClose		chFileStreamClose
	#define fileStreamRead		chSequentialStreamRead
	#define fileStreamWrite		chSequentialStreamWrite
	#define fileStreamSeek		chFileStreamSeek
	#define fileStreamGetSize	chFileStreamGetSize
#endif

static void ChibiOSBFSClose(GFILE *f) {
	fileStreamClose(((FileStream *)f->obj));
}
static int ChibiOSBFSRead(GFILE *f, void *buf, int size) {
	return fileStreamRead(((FileStream *)f->obj), (uint8_t *)buf, size);
}
static int ChibiOSBFSWrite(GFILE *f, const void *buf, int size) {
	return fileStreamWrite(((FileStream *)f->obj), (uint8_t *)buf, size);
}
static bool_t ChibiOSBFSSetpos(GFILE *f, long int pos) {
	fileStreamSeek(((FileStream *)f->obj), pos);
	return TRUE;
}
static long int ChibiOSBFSGetsize(GFILE *f) { return fileStreamGetSize(((FileStream *)f->obj)); }
static bool_t ChibiOSBFSEof(GFILE *f) { return f->pos >= fileStreamGetSize(((FileStream *)f->obj)); }

GFILE *		gfileOpenChibiOSFileStream(void *FileStreamPtr, const char *mode) {
	GFILE *			f;

	// Get an empty file and set the flags
	if (!(f = _gfileFindSlot(mode)))
		return 0;

	// File is open - fill in all the details
	f->vmt = &FsCHIBIOSVMT;
	f->obj = FileStreamPtr;
	f->pos = 0;
	f->flags |= GFILEFLG_OPEN|GFILEFLG_CANSEEK;
	return f;
}

#endif //GFX_USE_GFILE && GFILE_NEED_CHIBIOSFS && GFX_USE_OS_CHIBIOS
