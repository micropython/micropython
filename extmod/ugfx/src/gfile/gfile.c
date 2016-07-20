/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "../../gfx.h"

#if GFX_USE_GFILE

#include "gfile_fs.h"

/**
 * Define the VMT's for the file-systems we want to search for files.
 * Virtual file-systems that have special open() calls do not need to
 * be in this list.
 */
#if GFILE_NEED_ROMFS
	extern const GFILEVMT FsROMVMT;
#endif
#if GFILE_NEED_NATIVEFS
	extern const GFILEVMT FsNativeVMT;
#endif
#if GFILE_NEED_FATFS
	extern const GFILEVMT FsFatFSVMT;
#endif
#if GFILE_NEED_RAMFS
	extern const GFILEVMT FsRAMVMT;
#endif


/**
 * The order of the file-systems below determines the order
 * that they are searched to find a file.
 */
static const GFILEVMT const * FsArray[] = {
	#if GFILE_NEED_ROMFS
		&FsROMVMT,
	#endif
	#if GFILE_NEED_NATIVEFS
		&FsNativeVMT,
	#endif
	#if GFILE_NEED_FATFS
		&FsFatFSVMT,
	#endif
	#if GFILE_NEED_RAMFS
		&FsRAMVMT,
	#endif
};

/*
 * The table of GFILE's
 */
static GFILE gfileArr[GFILE_MAX_GFILES];
GFILE *gfileStdIn;
GFILE *gfileStdOut;
GFILE *gfileStdErr;

/**
 * The init routine
 */
void _gfileInit(void) {
	#if GFILE_NEED_NATIVEFS
		extern void _gfileNativeAssignStdio(void);
		_gfileNativeAssignStdio();
	#endif
}

void _gfileDeinit(void)
{
	GFILE *	f;
	for (f = gfileArr; f < &gfileArr[GFILE_MAX_GFILES]; f++) {
		if (f->flags & GFILEFLG_OPEN)
			gfileClose(f);
	}
}

/**
 * Internal routine to find an empty GFILE slot and interpret flags.
 */
GFILE *_gfileFindSlot(const char *mode) {
	GFILE *			f;

	// First find an available GFILE slot.
	for (f = gfileArr; f < &gfileArr[GFILE_MAX_GFILES]; f++) {
		if (!(f->flags & GFILEFLG_OPEN)) {
			// Get the flags
			switch(mode[0]) {
			case 'r':
				f->flags = GFILEFLG_READ|GFILEFLG_MUSTEXIST;
				while (*++mode) {
					switch(mode[0]) {
					case '+':	f->flags |= GFILEFLG_WRITE;			break;
					case 'b':	f->flags |= GFILEFLG_BINARY;		break;
					}
				}
				break;
			case 'w':
				f->flags = GFILEFLG_WRITE|GFILEFLG_TRUNC;
				while (*++mode) {
					switch(mode[0]) {
					case '+':	f->flags |= GFILEFLG_READ;			break;
					case 'b':	f->flags |= GFILEFLG_BINARY;		break;
					case 'x':	f->flags |= GFILEFLG_MUSTNOTEXIST;	break;
					}
				}
				break;
			case 'a':
				f->flags = GFILEFLG_WRITE|GFILEFLG_APPEND;
				while (*++mode) {
					switch(mode[0]) {
					case '+':	f->flags |= GFILEFLG_READ;			break;
					case 'b':	f->flags |= GFILEFLG_BINARY;		break;
					case 'x':	f->flags |= GFILEFLG_MUSTNOTEXIST;	break;
					}
				}
				break;
			default:
				return 0;
			}
			return f;
		}
	}
	return 0;
}

/********************************************************
 * IO routines
 ********************************************************/

bool_t gfileExists(const char *fname) {
	const GFILEVMT * const *p;

	#if GFILE_ALLOW_DEVICESPECIFIC
		if (fname[0] && fname[1] == '|') {
			for(p = FsArray; p < &FsArray[sizeof(FsArray)/sizeof(FsArray[0])]; p++) {
				if (p[0]->prefix == fname[0])
					return p[0]->exists && p[0]->exists(fname+2);
			}
			return FALSE;
		}
	#endif

	for(p = FsArray; p < &FsArray[sizeof(FsArray)/sizeof(FsArray[0])]; p++) {
		if (p[0]->exists && p[0]->exists(fname))
			return TRUE;
	}
	return FALSE;
}

bool_t gfileDelete(const char *fname) {
	const GFILEVMT **p;

	#if GFILE_ALLOW_DEVICESPECIFIC
		if (fname[0] && fname[1] == '|') {
			for(p = FsArray; p < &FsArray[sizeof(FsArray)/sizeof(FsArray[0])]; p++) {
				if (p[0]->prefix == fname[0])
					return p[0]->del && p[0]->del(fname+2);
			}
			return FALSE;
		}
	#endif

	for(p = FsArray; p < &FsArray[sizeof(FsArray)/sizeof(FsArray[0])]; p++) {
		if (p[0]->del && p[0]->del(fname))
			return TRUE;
	}
	return FALSE;
}

long int gfileGetFilesize(const char *fname) {
	const GFILEVMT * const *p;
	long int res;

	#if GFILE_ALLOW_DEVICESPECIFIC
		if (fname[0] && fname[1] == '|') {
			for(p = FsArray; p < &FsArray[sizeof(FsArray)/sizeof(FsArray[0])]; p++) {
				if (p[0]->prefix == fname[0])
					return p[0]->filesize ? p[0]->filesize(fname+2) : -1;
			}
			return -1;
		}
	#endif

	for(p = FsArray; p < &FsArray[sizeof(FsArray)/sizeof(FsArray[0])]; p++) {
		if (p[0]->filesize && (res = p[0]->filesize(fname)) != -1)
			return res;
	}
	return -1;
}

bool_t gfileRename(const char *oldname, const char *newname) {
	const GFILEVMT * const *p;

	#if GFILE_ALLOW_DEVICESPECIFIC
		if ((oldname[0] && oldname[1] == '|') || (newname[0] && newname[1] == '|')) {
			char ch;

			if (oldname[0] && oldname[1] == '|') {
				ch = oldname[0];
				oldname += 2;
				if (newname[0] && newname[1] == '|') {
					if (newname[0] != ch)
						// Both oldname and newname are fs specific but different ones.
						return FALSE;
					newname += 2;
				}
			} else {
				ch = newname[0];
				newname += 2;
			}
			for(p = FsArray; p < &FsArray[sizeof(FsArray)/sizeof(FsArray[0])]; p++) {
				if (p[0]->prefix == ch)
					return p[0]->ren && p[0]->ren(oldname, newname);
			}
			return FALSE;
		}
	#endif

	for(p = FsArray; p < &FsArray[sizeof(FsArray)/sizeof(FsArray[0])]; p++) {
		if (p[0]->ren && p[0]->ren(oldname,newname))
			return TRUE;
	}
	return FALSE;
}

static bool_t testopen(const GFILEVMT *p, GFILE *f, const char *fname) {
	// If we want write but the fs doesn't allow it then return
	if ((f->flags & GFILEFLG_WRITE) && !(p->flags & GFSFLG_WRITEABLE))
		return FALSE;

	// Try to open
	if (!p->open || !p->open(f, fname))
		return FALSE;

	// File is open - fill in all the details
	f->vmt = p;
	f->pos = 0;
	f->flags |= GFILEFLG_OPEN;
	if (p->flags & GFSFLG_SEEKABLE)
		f->flags |= GFILEFLG_CANSEEK;
	return TRUE;
}

GFILE *gfileOpen(const char *fname, const char *mode) {
	GFILE *			f;
	const GFILEVMT * const *p;

	// Get an empty file and set the flags
	if (!(f = _gfileFindSlot(mode)))
		return 0;

	#if GFILE_ALLOW_DEVICESPECIFIC
		if (fname[0] && fname[1] == '|') {
			for(p = FsArray; p < &FsArray[sizeof(FsArray)/sizeof(FsArray[0])]; p++) {
				if (p[0]->prefix == fname[0])
					return testopen(p[0], f, fname+2) ? f : 0;
			}

			// File not found
			return 0;
		}
	#endif

	for(p = FsArray; p < &FsArray[sizeof(FsArray)/sizeof(FsArray[0])]; p++) {
		if (testopen(p[0], f, fname))
			return f;
	}

	// File not found
	return 0;
}

void gfileClose(GFILE *f) {
	if (!f || !(f->flags & GFILEFLG_OPEN))
		return;
	if (f->vmt->close)
		f->vmt->close(f);
	f->flags = 0;
}

size_t gfileRead(GFILE *f, void *buf, size_t len) {
	size_t	res;

	if (!f || (f->flags & (GFILEFLG_OPEN|GFILEFLG_READ)) != (GFILEFLG_OPEN|GFILEFLG_READ))
		return 0;
	if (!f->vmt->read)
		return 0;
	if ((res = f->vmt->read(f, buf, len)) <= 0)
		return 0;
	f->pos += res;
	return res;
}

size_t gfileWrite(GFILE *f, const void *buf, size_t len) {
	size_t	res;

	if (!f || (f->flags & (GFILEFLG_OPEN|GFILEFLG_WRITE)) != (GFILEFLG_OPEN|GFILEFLG_WRITE))
		return 0;
	if (!f->vmt->write)
		return 0;
	if ((res = f->vmt->write(f, buf, len)) <= 0)
		return 0;
	f->pos += res;
	return res;
}

long int gfileGetPos(GFILE *f) {
	if (!f || !(f->flags & GFILEFLG_OPEN))
		return 0;
	return f->pos;
}

bool_t gfileSetPos(GFILE *f, long int pos) {
	if (!f || !(f->flags & GFILEFLG_OPEN))
		return FALSE;
	if (!f->vmt->setpos || !f->vmt->setpos(f, pos))
		return FALSE;
	f->pos = pos;
	return TRUE;
}

long int gfileGetSize(GFILE *f) {
	if (!f || !(f->flags & GFILEFLG_OPEN))
		return 0;
	if (!f->vmt->getsize)
		return 0;
	return f->vmt->getsize(f);
}

bool_t gfileEOF(GFILE *f) {
	if (!f || !(f->flags & GFILEFLG_OPEN))
		return TRUE;
	if (!f->vmt->eof)
		return FALSE;
	return f->vmt->eof(f);
}

bool_t gfileMount(char fs, const char* drive) {
	const GFILEVMT * const *p;

	// Find the correct VMT
	for(p = FsArray; p < &FsArray[sizeof(FsArray)/sizeof(FsArray[0])]; p++) {
		if (p[0]->prefix == fs) {
			if (!p[0]->mount)
				return FALSE;
			return p[0]->mount(drive);
		}
	}
	return FALSE;
}

bool_t gfileUnmount(char fs, const char* drive) {
	const GFILEVMT * const *p;

	// Find the correct VMT
	for(p = FsArray; p < &FsArray[sizeof(FsArray)/sizeof(FsArray[0])]; p++) {
		if (p[0]->prefix == fs) {
			if (!p[0]->mount)
				return FALSE;
			return p[0]->unmount(drive);
		}
	}
	return FALSE;
}

bool_t gfileSync(GFILE *f) {
	if (!f->vmt->sync)
		return FALSE;
	return f->vmt->sync(f);
}

#if GFILE_NEED_FILELISTS
	gfileList *gfileOpenFileList(char fs, const char *path, bool_t dirs) {
		const GFILEVMT * const *p;
		gfileList *		pfl;

		// Find the correct VMT
		for(p = FsArray; p < &FsArray[sizeof(FsArray)/sizeof(FsArray[0])]; p++) {
			if (p[0]->prefix == fs) {
				if (!p[0]->flopen)
					return 0;
				pfl = p[0]->flopen(path, dirs);
				if (pfl) {
					pfl->vmt = p[0];
					pfl->dirs = dirs;
				}
				return pfl;
			}
		}
		return 0;
	}

	const char *gfileReadFileList(gfileList *pfl) {
		return pfl->vmt->flread ? pfl->vmt->flread(pfl) : 0;
	}

	void gfileCloseFileList(gfileList *pfl) {
		if (pfl->vmt->flclose)
			pfl->vmt->flclose(pfl);
	}
#endif

#endif /* GFX_USE_GFILE */
