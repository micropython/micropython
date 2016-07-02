/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

/********************************************************
 * The RAM file-system
 ********************************************************/

#include "../../gfx.h"

#if GFX_USE_GFILE && GFILE_NEED_RAMFS

#include "gfile_fs.h"

#error "GFILE: RAMFS Not implemented yet"

const GFILEVMT FsRAMVMT = {
	0,								// flags
	'R',							// prefix
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0,
	0, 0, 0,
	#if GFILE_NEED_FILELISTS
		0, 0, 0,
	#endif
};

#endif //GFX_USE_GFILE && GFILE_NEED_RAMFS
