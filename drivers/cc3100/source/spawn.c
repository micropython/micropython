/*
 * spawn.c - CC31xx/CC32xx Host Driver Implementation
 *
 * Copyright (C) 2015 Texas Instruments Incorporated - http://www.ti.com/ 
 * 
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/



/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/
#include "simplelink.h"
#include "protocol.h"
#include "driver.h"


#if (defined (SL_PLATFORM_MULTI_THREADED)) && (!defined (SL_PLATFORM_EXTERNAL_SPAWN))

#define _SL_MAX_INTERNAL_SPAWN_ENTRIES      10

typedef struct _SlInternalSpawnEntry_t
{
	_SlSpawnEntryFunc_t 		        pEntry;
	void* 						        pValue;
    struct _SlInternalSpawnEntry_t*     pNext;
}_SlInternalSpawnEntry_t;

typedef struct
{
	_SlInternalSpawnEntry_t     SpawnEntries[_SL_MAX_INTERNAL_SPAWN_ENTRIES];
    _SlInternalSpawnEntry_t*    pFree;
    _SlInternalSpawnEntry_t*    pWaitForExe;
    _SlInternalSpawnEntry_t*    pLastInWaitList;
    _SlSyncObj_t                SyncObj;
    _SlLockObj_t                LockObj;
	_u8							IrqWriteCnt;
	_u8							IrqReadCnt;
	void*						pIrqFuncValue;
}_SlInternalSpawnCB_t;

_SlInternalSpawnCB_t g_SlInternalSpawnCB;


void _SlInternalSpawnTaskEntry() 
{
    _i16                         i;
    _SlInternalSpawnEntry_t*    pEntry;
    _u8                         LastEntry;

    /* create and lock the locking object. lock in order to avoid race condition 
        on the first creation */
    sl_LockObjCreate(&g_SlInternalSpawnCB.LockObj,"SlSpawnProtect");
    sl_LockObjLock(&g_SlInternalSpawnCB.LockObj,SL_OS_NO_WAIT);

    /* create and clear the sync object */
    sl_SyncObjCreate(&g_SlInternalSpawnCB.SyncObj,"SlSpawnSync");
    sl_SyncObjWait(&g_SlInternalSpawnCB.SyncObj,SL_OS_NO_WAIT);

    g_SlInternalSpawnCB.pFree = &g_SlInternalSpawnCB.SpawnEntries[0];
    g_SlInternalSpawnCB.pWaitForExe = NULL;
    g_SlInternalSpawnCB.pLastInWaitList = NULL;

    /* create the link list between the entries */
    for (i=0 ; i<_SL_MAX_INTERNAL_SPAWN_ENTRIES - 1 ; i++)
    {
        g_SlInternalSpawnCB.SpawnEntries[i].pNext = &g_SlInternalSpawnCB.SpawnEntries[i+1];
        g_SlInternalSpawnCB.SpawnEntries[i].pEntry = NULL;
    }
    g_SlInternalSpawnCB.SpawnEntries[i].pNext = NULL;

	g_SlInternalSpawnCB.IrqWriteCnt =0;
	g_SlInternalSpawnCB.IrqReadCnt = 0;
	g_SlInternalSpawnCB.pIrqFuncValue = NULL;

    SL_DRV_OBJ_UNLOCK(&g_SlInternalSpawnCB.LockObj);

    /* here we ready to execute entries */

    while (TRUE)
    {
        sl_SyncObjWait(&g_SlInternalSpawnCB.SyncObj,SL_OS_WAIT_FOREVER);

        /* handle IRQ requests */
        while (g_SlInternalSpawnCB.IrqWriteCnt != g_SlInternalSpawnCB.IrqReadCnt)
		{
			/* handle the ones that came from ISR context*/
			_SlDrvMsgReadSpawnCtx(g_SlInternalSpawnCB.pIrqFuncValue);
			g_SlInternalSpawnCB.IrqReadCnt++;
		}

        /* go over all entries that already waiting for execution */
        LastEntry = FALSE;

        do
        {
            /* get entry to execute */
            SL_DRV_OBJ_LOCK_FOREVER(&g_SlInternalSpawnCB.LockObj);

            pEntry = g_SlInternalSpawnCB.pWaitForExe;
            if ( NULL == pEntry )
            {
               SL_DRV_OBJ_UNLOCK(&g_SlInternalSpawnCB.LockObj);
               break;
            }
            g_SlInternalSpawnCB.pWaitForExe = pEntry->pNext;
            if (pEntry == g_SlInternalSpawnCB.pLastInWaitList)
            {
                g_SlInternalSpawnCB.pLastInWaitList = NULL;
                LastEntry = TRUE;
            }

            SL_DRV_OBJ_UNLOCK(&g_SlInternalSpawnCB.LockObj);

            /* pEntry could be null in case that the sync was already set by some
               of the entries during execution of earlier entry */
            if (NULL != pEntry)
            {
                pEntry->pEntry(pEntry->pValue);
                /* free the entry */

                SL_DRV_OBJ_LOCK_FOREVER(&g_SlInternalSpawnCB.LockObj);
                
                pEntry->pNext = g_SlInternalSpawnCB.pFree;
                g_SlInternalSpawnCB.pFree = pEntry;


                if (NULL != g_SlInternalSpawnCB.pWaitForExe)
                {
                    /* new entry received meanwhile */
                    LastEntry = FALSE;
                }

                SL_DRV_OBJ_UNLOCK(&g_SlInternalSpawnCB.LockObj);

            }

        }while (!LastEntry);
    }
}


_i16 _SlInternalSpawn(_SlSpawnEntryFunc_t pEntry , void* pValue , _u32 flags)
{
    _i16                         Res = 0;
    _SlInternalSpawnEntry_t*    pSpawnEntry;


	/*	Increment the counter that specifies that async event has recived 
		from interrupt context and should be handled by the internal spawn task */
	if (flags & SL_SPAWN_FLAG_FROM_SL_IRQ_HANDLER)
	{
		g_SlInternalSpawnCB.IrqWriteCnt++;
		g_SlInternalSpawnCB.pIrqFuncValue = pValue;
		SL_DRV_SYNC_OBJ_SIGNAL(&g_SlInternalSpawnCB.SyncObj);
		return Res;
	}


    if (NULL == pEntry || (g_SlInternalSpawnCB.pFree == NULL))
    {
        Res = -1;
    }
    else
    {
        SL_DRV_OBJ_LOCK_FOREVER(&g_SlInternalSpawnCB.LockObj);

        pSpawnEntry = g_SlInternalSpawnCB.pFree;
        g_SlInternalSpawnCB.pFree = pSpawnEntry->pNext;

        pSpawnEntry->pEntry = pEntry;
        pSpawnEntry->pValue = pValue;
        pSpawnEntry->pNext = NULL;

        if (NULL == g_SlInternalSpawnCB.pWaitForExe)
        {
            g_SlInternalSpawnCB.pWaitForExe = pSpawnEntry;
            g_SlInternalSpawnCB.pLastInWaitList = pSpawnEntry;
        }
        else
        {
            g_SlInternalSpawnCB.pLastInWaitList->pNext = pSpawnEntry;
            g_SlInternalSpawnCB.pLastInWaitList = pSpawnEntry;
        }

        SL_DRV_OBJ_UNLOCK(&g_SlInternalSpawnCB.LockObj);
        
        /* this sync is called after releasing the lock object to avoid unnecessary context switches */
        SL_DRV_SYNC_OBJ_SIGNAL(&g_SlInternalSpawnCB.SyncObj);
    }

    return Res;
}





#endif
