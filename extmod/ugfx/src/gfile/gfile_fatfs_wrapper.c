/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "../../gfx.h"

#if GFX_USE_GFILE && GFILE_NEED_FATFS && !GFILE_FATFS_EXTERNAL_LIB

#include "gfile_fatfs_wrapper.h"

// Include the source we want
#include "../../3rdparty/fatfs-0.10b/src/ff.c"
#include "../../3rdparty/fatfs-0.10b/src/option/unicode.c"

// Extra operating system support
#if _FS_REENTRANT
	/*------------------------------------------------------------------------*/
	/* Static array of Synchronization Objects                                */
	/*------------------------------------------------------------------------*/
	static gfxSem ff_sem[_VOLUMES];

	/*------------------------------------------------------------------------*/
	/* Create a Synchronization Object                                        */
	/*------------------------------------------------------------------------*/
	int ff_cre_syncobj(BYTE vol, _SYNC_t *sobj)
	{
		*sobj = ff_sem[vol];
		gfxSemInit(sobj, 1, MAX_SEMAPHORE_COUNT);

		return 1;
	}

	/*------------------------------------------------------------------------*/
	/* Delete a Synchronization Object                                        */
	/*------------------------------------------------------------------------*/
	int ff_del_syncobj(_SYNC_t sobj)
	{
		gfxSemDestroy( (gfxSem*)&sobj );

		return 1;
	}

	/*------------------------------------------------------------------------*/
	/* Request Grant to Access the Volume                                     */
	/*------------------------------------------------------------------------*/
	int ff_req_grant(_SYNC_t sobj)
	{
		if (gfxSemWait( (gfxSem*)&sobj, (delaytime_t)_FS_TIMEOUT) )
			return TRUE;
		return FALSE;
	}

	/*------------------------------------------------------------------------*/
	/* Release Grant to Access the Volume                                     */
	/*------------------------------------------------------------------------*/
	void ff_rel_grant(_SYNC_t sobj)
	{
		gfxSemSignal( (gfxSem*)&sobj );
	}
#endif /* _FS_REENTRANT */

#if _USE_LFN == 3	/* LFN with a working buffer on the heap */
	/*------------------------------------------------------------------------*/
	/* Allocate a memory block                                                */
	/*------------------------------------------------------------------------*/
	void *ff_memalloc(UINT size)
	{
		return gfxAlloc( (size_t)size );
	}

	/*------------------------------------------------------------------------*/
	/* Free a memory block                                                    */
	/*------------------------------------------------------------------------*/
	void ff_memfree(void *mblock)
	{
		gfxFree(mblock);
	}
#endif /* _USE_LFN == 3 */

#endif // GFX_USE_GFILE && GFILE_NEED_FATFS && !GFILE_FATFS_EXTERNAL_LIB
