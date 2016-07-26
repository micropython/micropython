/*
 * nonos.c - CC31xx/CC32xx Host Driver Implementation
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


#ifndef SL_PLATFORM_MULTI_THREADED

#include "nonos.h"


_SlNonOsCB_t g__SlNonOsCB = {0};


_SlNonOsRetVal_t _SlNonOsSemSet(_SlNonOsSemObj_t* pSemObj , _SlNonOsSemObj_t Value)
{
    *pSemObj = Value;
    return NONOS_RET_OK;
}

_SlNonOsRetVal_t _SlNonOsSemGet(_SlNonOsSemObj_t* pSyncObj, _SlNonOsSemObj_t WaitValue, _SlNonOsSemObj_t SetValue, _SlNonOsTime_t Timeout)
{
#if (!defined (SL_TINY)) && (defined(sl_GetTimestamp))
      _SlTimeoutParams_t      TimeoutInfo={0};
#endif

	 /* If timeout 0 configured, just detect the value and return */
      if ((Timeout ==0) && (WaitValue == *pSyncObj))
	  {
		  *pSyncObj = SetValue;
		  return NONOS_RET_OK;
	  }

#if (!defined (SL_TINY)) && (defined(sl_GetTimestamp))
      if ((Timeout != NONOS_WAIT_FOREVER) && (Timeout != NONOS_NO_WAIT))
      {
    	  _SlDrvStartMeasureTimeout(&TimeoutInfo, Timeout);
      }
#endif

#ifdef _SlSyncWaitLoopCallback
    _SlNonOsTime_t timeOutRequest = Timeout; 
#endif
    while (Timeout>0)
    {
        if (WaitValue == *pSyncObj)
        {
            *pSyncObj = SetValue;
            break;
        }
#if (!defined (sl_GetTimestamp)) ||  (defined (SL_TINY_EXT))
        if (Timeout != NONOS_WAIT_FOREVER)
        {		
            Timeout--;
        }
#else        
        if ((Timeout != NONOS_WAIT_FOREVER) && (Timeout != NONOS_NO_WAIT))
        {
            if (_SlDrvIsTimeoutExpired(&TimeoutInfo))
            {
            	return (_SlNonOsRetVal_t)NONOS_RET_ERR;
            }

        }
 #endif       

        /* If we are in cmd context and waiting for its cmd response
         * do not handle spawn async events as the global lock was already taken */
        if (FALSE == g_pCB->IsCmdRespWaited)
        {
        (void)_SlNonOsMainLoopTask();
        }
#ifdef _SlSyncWaitLoopCallback
        if( (__NON_OS_SYNC_OBJ_SIGNAL_VALUE == WaitValue) && (timeOutRequest != NONOS_NO_WAIT) )
        {
            if (WaitValue == *pSyncObj)
            {
                *pSyncObj = SetValue;
                break;
            }
            _SlSyncWaitLoopCallback();
        }
#endif
    }

    if (0 == Timeout)
    {
        return NONOS_RET_ERR;
    }
    else
    {
        return NONOS_RET_OK;
    }
}


_SlNonOsRetVal_t _SlNonOsSpawn(_SlSpawnEntryFunc_t pEntry , void* pValue , _u32 flags)
{
	 _i8 i = 0;

     (void)flags;
    
#ifndef SL_TINY_EXT 	
	for (i=0 ; i<NONOS_MAX_SPAWN_ENTRIES ; i++)
#endif     
	{
		_SlNonOsSpawnEntry_t* pE = &g__SlNonOsCB.SpawnEntries[i];
	
		if (pE->IsAllocated == FALSE)
		{
			pE->pValue = pValue;
			pE->pEntry = pEntry;
			pE->IsAllocated = TRUE;
#ifndef SL_TINY_EXT 	                        
			break;
#endif                        
		}
	}
        
        
        return NONOS_RET_OK;
}


_SlNonOsRetVal_t _SlNonOsMainLoopTask(void)
{
	_i8 i=0;
	void*  pValue;

#ifndef SL_TINY_EXT
	for (i=0 ; i<NONOS_MAX_SPAWN_ENTRIES ; i++)
#endif
	{
		_SlNonOsSpawnEntry_t* pE = &g__SlNonOsCB.SpawnEntries[i];
		

		if (pE->IsAllocated == TRUE)
		{
			_SlSpawnEntryFunc_t  pF = pE->pEntry;
			pValue = pE->pValue;


			/* Clear the entry */
			pE->pEntry = NULL;
			pE->pValue = NULL;
			pE->IsAllocated = FALSE;

			/* execute the spawn function */
            pF(pValue);
		}
	}
        
        return NONOS_RET_OK;
}

    
#endif /*(SL_PLATFORM != SL_PLATFORM_NON_OS)*/
