/*
 * device.c - CC31xx/CC32xx Host Driver Implementation
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
#include "flowcont.h"
#include "driver.h"
#include "wlan.h"


/*****************************************************************************/
/* Internal functions                                                        */
/*****************************************************************************/

static SlWlanMode_e _sl_GetStartResponseConvert(_i32 Status);


static const SlWlanMode_e StartResponseLUT[8] = 
{
    ROLE_UNKNOWN,
    ROLE_STA,
    ROLE_STA_ERR,
    ROLE_AP,
    ROLE_AP_ERR,
    ROLE_P2P,
    ROLE_P2P_ERR,
    INIT_CALIB_FAIL
};


static SlWlanMode_e _sl_GetStartResponseConvert(_i32 Status)
{
    return StartResponseLUT[Status & 0x7];
}


/*****************************************************************************/
/* API Functions                                                             */
/*****************************************************************************/



/*****************************************************************************/
/* sl_Task                                                                   */
/*****************************************************************************/
#if _SL_INCLUDE_FUNC(sl_Task)
void sl_Task(void)
{
#ifdef _SlTaskEntry
    (void)_SlTaskEntry();
#endif
}
#endif

/*****************************************************************************/
/* sl_Start                                                                  */
/*****************************************************************************/
#if _SL_INCLUDE_FUNC(sl_Start)
_i16 sl_Start(const void* pIfHdl, _i8*  pDevName, const P_INIT_CALLBACK pInitCallBack)
{
    _u8 ObjIdx = MAX_CONCURRENT_ACTIONS;
    InitComplete_t  AsyncRsp;

    /* verify no erorr handling in progress. if in progress than
    ignore the API execution and return immediately with an error */
    VERIFY_NO_ERROR_HANDLING_IN_PROGRESS();
    /* Perform any preprocessing before enable networking services */
#ifdef sl_DeviceEnablePreamble
    sl_DeviceEnablePreamble();
#endif

    /* ControlBlock init */
    (void)_SlDrvDriverCBInit();

    /* open the interface: usually SPI or UART */
    if (NULL == pIfHdl)
    {
        g_pCB->FD = sl_IfOpen((void *)pDevName, 0);
    }
    else
    {
        g_pCB->FD = (_SlFd_t)pIfHdl;
    }
    
    ObjIdx = _SlDrvProtectAsyncRespSetting((_u8 *)&AsyncRsp, START_STOP_ID, SL_MAX_SOCKETS);

    if (MAX_CONCURRENT_ACTIONS == ObjIdx)
    {
        return SL_POOL_IS_EMPTY;
    }

    if( g_pCB->FD >= (_SlFd_t)0)
    {
        sl_DeviceDisable();

        sl_IfRegIntHdlr((SL_P_EVENT_HANDLER)_SlDrvRxIrqHandler, NULL);

        g_pCB->pInitCallback = pInitCallBack;
        
        sl_DeviceEnable();
        
        if (NULL == pInitCallBack)
        {
#ifdef SL_TINY_EXT
            _SlDrvSyncObjWaitForever(&g_pCB->ObjPool[ObjIdx].SyncObj);
#else
        	_SlReturnVal_t retVal;
            retVal = _SlDrvSyncObjWaitTimeout(&g_pCB->ObjPool[ObjIdx].SyncObj,
                                              INIT_COMPLETE_TIMEOUT,
                                              SL_DRIVER_API_DEVICE_SL_START);
            if (retVal)
            {
                return SL_API_ABORTED;
            }
#endif            


            /* release Pool Object */
            _SlDrvReleasePoolObj(g_pCB->FunctionParams.AsyncExt.ActionIndex);
	         return _sl_GetStartResponseConvert(AsyncRsp.Status);
        }
        else
        {
            return SL_RET_CODE_OK;
        }
    }
    return SL_BAD_INTERFACE;
}
#endif

/***************************************************************************
_sl_HandleAsync_InitComplete - handles init complete signalling to 
a waiting object
****************************************************************************/
_SlReturnVal_t _sl_HandleAsync_InitComplete(void *pVoidBuf)
{
    InitComplete_t     *pMsgArgs   = (InitComplete_t *)_SL_RESP_ARGS_START(pVoidBuf);

    SL_DRV_PROTECTION_OBJ_LOCK_FOREVER();
    
    if(g_pCB->pInitCallback)
    {
        g_pCB->pInitCallback(_sl_GetStartResponseConvert(pMsgArgs->Status));
    }
    else
    {
        sl_Memcpy(g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].pRespArgs, pMsgArgs, sizeof(InitComplete_t));
        SL_DRV_SYNC_OBJ_SIGNAL(&g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].SyncObj);
    }
    
    SL_DRV_PROTECTION_OBJ_UNLOCK();
   
    if(g_pCB->pInitCallback)
    {
        _SlDrvReleasePoolObj(g_pCB->FunctionParams.AsyncExt.ActionIndex);
    }


	return SL_RET_CODE_OK;
}

/***************************************************************************
_sl_HandleAsync_Stop - handles stop signalling to 
a waiting object
****************************************************************************/
void _sl_HandleAsync_Stop(void *pVoidBuf)
{
    _BasicResponse_t     *pMsgArgs   = (_BasicResponse_t *)_SL_RESP_ARGS_START(pVoidBuf);

    VERIFY_SOCKET_CB(NULL != g_pCB->StopCB.pAsyncRsp);

    SL_DRV_PROTECTION_OBJ_LOCK_FOREVER();

    sl_Memcpy(g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].pRespArgs, pMsgArgs, sizeof(_BasicResponse_t));

    SL_DRV_SYNC_OBJ_SIGNAL(&g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].SyncObj);
    SL_DRV_PROTECTION_OBJ_UNLOCK();
    
    return;
}


/*****************************************************************************
sl_stop
******************************************************************************/
typedef union
{
    _DevStopCommand_t  Cmd;
    _BasicResponse_t   Rsp;    
}_SlStopMsg_u;

static const _SlCmdCtrl_t _SlStopCmdCtrl =
{
    SL_OPCODE_DEVICE_STOP_COMMAND,
    (_SlArgSize_t)sizeof(_DevStopCommand_t),
    (_SlArgSize_t)sizeof(_BasicResponse_t)
};

#if _SL_INCLUDE_FUNC(sl_Stop)
_i16 sl_Stop(const _u16 timeout)
{
    _i16 RetVal=0;
    _SlStopMsg_u      Msg;
    _BasicResponse_t  AsyncRsp;
    _u8 ObjIdx = MAX_CONCURRENT_ACTIONS;

    /* If we are in the middle of assert handling then ignore stopping
     * the device with timeout and force immediate shutdown as we would like
     * to avoid any additional commands to the NWP */
    if( (timeout != 0) 
#ifndef SL_TINY_EXT  
       && ((_u8)FALSE == g_bDeviceRestartIsRequired)
#endif 
    )      
    {
    	/* let the device make the shutdown using the defined timeout */
        Msg.Cmd.Timeout = timeout;

        ObjIdx = _SlDrvProtectAsyncRespSetting((_u8 *)&AsyncRsp, START_STOP_ID, SL_MAX_SOCKETS);
        if (MAX_CONCURRENT_ACTIONS == ObjIdx)
        {
          return SL_POOL_IS_EMPTY;
        }

        VERIFY_RET_OK(_SlDrvCmdOp((_SlCmdCtrl_t *)&_SlStopCmdCtrl, &Msg, NULL));

        if(SL_OS_RET_CODE_OK == (_i16)Msg.Rsp.status)
        {

#ifdef SL_TINY_EXT        
         _SlDrvSyncObjWaitForever(&g_pCB->ObjPool[ObjIdx].SyncObj);
                /* Wait for sync object to be signaled */
#else                
         SL_DRV_SYNC_OBJ_WAIT_TIMEOUT(&g_pCB->ObjPool[ObjIdx].SyncObj,
                                      STOP_DEVICE_TIMEOUT,
                                      SL_DRIVER_API_DEVICE_SL_STOP);
#endif

         Msg.Rsp.status = AsyncRsp.status;
         RetVal = Msg.Rsp.status;
        }
        _SlDrvReleasePoolObj(ObjIdx);
    }

    sl_IfRegIntHdlr(NULL, NULL);
    sl_DeviceDisable();
    RetVal = sl_IfClose(g_pCB->FD);

    (void)_SlDrvDriverCBDeinit();

#ifndef SL_TINY_EXT
    /* Clear the restart device flag  */
    g_bDeviceRestartIsRequired = FALSE;
#endif

    return RetVal;
}
#endif


/*****************************************************************************
sl_EventMaskSet
*****************************************************************************/
typedef union
{
    _DevMaskEventSetCommand_t	    Cmd;
    _BasicResponse_t	            Rsp;
}_SlEventMaskSetMsg_u;




#if _SL_INCLUDE_FUNC(sl_EventMaskSet)

static const _SlCmdCtrl_t _SlEventMaskSetCmdCtrl =
{
    SL_OPCODE_DEVICE_EVENTMASKSET,
    (_SlArgSize_t)sizeof(_DevMaskEventSetCommand_t),
    (_SlArgSize_t)sizeof(_BasicResponse_t)
};


_i16 sl_EventMaskSet(const _u8 EventClass ,const _u32 Mask)
{
    _SlEventMaskSetMsg_u Msg;

    /* verify no erorr handling in progress. if in progress than
    ignore the API execution and return immediately with an error */
    VERIFY_NO_ERROR_HANDLING_IN_PROGRESS();

    Msg.Cmd.group = EventClass;
    Msg.Cmd.mask = Mask;

    VERIFY_RET_OK(_SlDrvCmdOp((_SlCmdCtrl_t *)&_SlEventMaskSetCmdCtrl, &Msg, NULL));

    return (_i16)Msg.Rsp.status;
}
#endif

/******************************************************************************
sl_EventMaskGet
******************************************************************************/
typedef union
{
    _DevMaskEventGetCommand_t	    Cmd;
    _DevMaskEventGetResponse_t      Rsp;
}_SlEventMaskGetMsg_u;



#if _SL_INCLUDE_FUNC(sl_EventMaskGet)

static const _SlCmdCtrl_t _SlEventMaskGetCmdCtrl =
{
    SL_OPCODE_DEVICE_EVENTMASKGET,
    (_SlArgSize_t)sizeof(_DevMaskEventGetCommand_t),
    (_SlArgSize_t)sizeof(_DevMaskEventGetResponse_t)
};


_i16 sl_EventMaskGet(const _u8 EventClass,_u32 *pMask)
{
    _SlEventMaskGetMsg_u Msg;

    /* verify no erorr handling in progress. if in progress than
    ignore the API execution and return immediately with an error */
    VERIFY_NO_ERROR_HANDLING_IN_PROGRESS();
    Msg.Cmd.group = EventClass;

    VERIFY_RET_OK(_SlDrvCmdOp((_SlCmdCtrl_t *)&_SlEventMaskGetCmdCtrl, &Msg, NULL));

    *pMask = Msg.Rsp.mask;
    return SL_RET_CODE_OK;
}
#endif



/******************************************************************************
sl_DevGet
******************************************************************************/

typedef union
{
    _DeviceSetGet_t	    Cmd;
    _DeviceSetGet_t	    Rsp;
}_SlDeviceMsgGet_u;



#if _SL_INCLUDE_FUNC(sl_DevGet)

static const _SlCmdCtrl_t _SlDeviceGetCmdCtrl =
{
    SL_OPCODE_DEVICE_DEVICEGET,
    (_SlArgSize_t)sizeof(_DeviceSetGet_t),
    (_SlArgSize_t)sizeof(_DeviceSetGet_t)
};

_i32 sl_DevGet(const _u8 DeviceGetId,_u8 *pOption,_u8 *pConfigLen, _u8 *pValues)
{
    _SlDeviceMsgGet_u         Msg;
    _SlCmdExt_t               CmdExt;
    /* verify no erorr handling in progress. if in progress than
    ignore the API execution and return immediately with an error */
    VERIFY_NO_ERROR_HANDLING_IN_PROGRESS();

    if (*pConfigLen == 0)
    {
        return SL_EZEROLEN;
    }

    if( pOption )
    {

        _SlDrvResetCmdExt(&CmdExt);
        CmdExt.RxPayloadLen = (_i16)*pConfigLen;
        CmdExt.pRxPayload = (_u8 *)pValues;

        Msg.Cmd.DeviceSetId = DeviceGetId;

        Msg.Cmd.Option   = (_u16)*pOption;

        VERIFY_RET_OK(_SlDrvCmdOp((_SlCmdCtrl_t *)&_SlDeviceGetCmdCtrl, &Msg, &CmdExt));

        if( pOption )
        {
            *pOption = (_u8)Msg.Rsp.Option;
        }

        if (CmdExt.RxPayloadLen < CmdExt.ActualRxPayloadLen) 
        {
            *pConfigLen = (_u8)CmdExt.RxPayloadLen;
            return SL_ESMALLBUF;
        }
        else
        {
            *pConfigLen = (_u8)CmdExt.ActualRxPayloadLen;
        }

        return (_i16)Msg.Rsp.Status;
    }
    else
    {
        return -1;
    }
}
#endif

/******************************************************************************
sl_DevSet
******************************************************************************/
typedef union
{
    _DeviceSetGet_t    Cmd;
    _BasicResponse_t   Rsp;
}_SlDeviceMsgSet_u;



#if _SL_INCLUDE_FUNC(sl_DevSet)

static const _SlCmdCtrl_t _SlDeviceSetCmdCtrl =
{
    SL_OPCODE_DEVICE_DEVICESET,
    (_SlArgSize_t)sizeof(_DeviceSetGet_t),
    (_SlArgSize_t)sizeof(_BasicResponse_t)
};

_i32 sl_DevSet(const _u8 DeviceSetId ,const _u8 Option,const _u8 ConfigLen,const _u8 *pValues)
{
    _SlDeviceMsgSet_u         Msg;
    _SlCmdExt_t               CmdExt;

    /* verify no erorr handling in progress. if in progress than
    ignore the API execution and return immediately with an error */
    VERIFY_NO_ERROR_HANDLING_IN_PROGRESS();

    _SlDrvResetCmdExt(&CmdExt);

    CmdExt.TxPayloadLen = (ConfigLen+3) & (~3);
    CmdExt.pTxPayload = (_u8 *)pValues;

    Msg.Cmd.DeviceSetId    = DeviceSetId;
    Msg.Cmd.ConfigLen   = ConfigLen;
    Msg.Cmd.Option   = Option;

    VERIFY_RET_OK(_SlDrvCmdOp((_SlCmdCtrl_t *)&_SlDeviceSetCmdCtrl, &Msg, &CmdExt));

    return (_i16)Msg.Rsp.status;
}
#endif


/******************************************************************************
_SlDrvDeviceEventHandler - handles internally device async events
******************************************************************************/
_SlReturnVal_t _SlDrvDeviceEventHandler(void* pEventInfo)
{
    DeviceEventInfo_t*    pInfo = (DeviceEventInfo_t*)pEventInfo;
    _SlResponseHeader_t*  pHdr  = (_SlResponseHeader_t *)pInfo->pAsyncMsgBuff;
    _BasicResponse_t     *pMsgArgs   = (_BasicResponse_t *)_SL_RESP_ARGS_START(pHdr);

    switch(pHdr->GenHeader.Opcode)
    {
    case SL_OPCODE_DEVICE_INITCOMPLETE:
        _sl_HandleAsync_InitComplete(pHdr);
        break;
    case SL_OPCODE_DEVICE_STOP_ASYNC_RESPONSE:
        _sl_HandleAsync_Stop(pHdr);
        break;


	case SL_OPCODE_DEVICE_ABORT:
        {
        /* release global lock of cmd context */
        if (pInfo->bInCmdContext == TRUE)
        {
            SL_DRV_LOCK_GLOBAL_UNLOCK();	
        }

#ifndef SL_TINY_EXT
        _SlDriverHandleError(SL_DEVICE_ABORT_ERROR_EVENT,
                             *((_u32*)pMsgArgs),       /* Abort type */
                             *((_u32*)pMsgArgs + 1));  /* Abort data */
#endif        
	}
        break;

    case  SL_OPCODE_DEVICE_ASYNC_GENERAL_ERROR:
        {
            SlDeviceEvent_t      devHandler;
            devHandler.Event = SL_DEVICE_GENERAL_ERROR_EVENT;
            devHandler.EventData.deviceEvent.status = pMsgArgs->status & 0xFF;
            devHandler.EventData.deviceEvent.sender = (SlErrorSender_e)((pMsgArgs->status >> 8) & 0xFF);
            _SlDrvHandleGeneralEvents(&devHandler);
        }

        break;
    default:
        SL_ERROR_TRACE2(MSG_306, "ASSERT: _SlDrvDeviceEventHandler : invalid opcode = 0x%x = %1", pHdr->GenHeader.Opcode, pHdr->GenHeader.Opcode);
    }

    return SL_OS_RET_CODE_OK;
}


/******************************************************************************
sl_UartSetMode 
******************************************************************************/
#ifdef SL_IF_TYPE_UART
typedef union
{
    _DevUartSetModeCommand_t	  Cmd;
    _DevUartSetModeResponse_t     Rsp;
}_SlUartSetModeMsg_u;


#if _SL_INCLUDE_FUNC(sl_UartSetMode)


const _SlCmdCtrl_t _SlUartSetModeCmdCtrl =
{
    SL_OPCODE_DEVICE_SETUARTMODECOMMAND,
    (_SlArgSize_t)sizeof(_DevUartSetModeCommand_t),
    (_SlArgSize_t)sizeof(_DevUartSetModeResponse_t)
};

_i16 sl_UartSetMode(const SlUartIfParams_t* pUartParams)
{
    _SlUartSetModeMsg_u Msg;
    _u32 magicCode = (_u32)0xFFFFFFFF;

    Msg.Cmd.BaudRate = pUartParams->BaudRate;
    Msg.Cmd.FlowControlEnable = pUartParams->FlowControlEnable;


    VERIFY_RET_OK(_SlDrvCmdOp((_SlCmdCtrl_t *)&_SlUartSetModeCmdCtrl, &Msg, NULL));

    /* cmd response OK, we can continue with the handshake */
    if (SL_RET_CODE_OK == Msg.Rsp.status)
    {
        sl_IfMaskIntHdlr();

        /* Close the comm port */
        sl_IfClose(g_pCB->FD);

        /* Re-open the comm port */
        sl_IfOpen((void * )pUartParams, UART_IF_OPEN_FLAG_RE_OPEN);

        sl_IfUnMaskIntHdlr();

        /* send the magic code and wait for the response */
        sl_IfWrite(g_pCB->FD, (_u8* )&magicCode, 4);

        magicCode = UART_SET_MODE_MAGIC_CODE;
        sl_IfWrite(g_pCB->FD, (_u8* )&magicCode, 4);

        /* clear magic code */
        magicCode = 0;

        /* wait (blocking) till the magic code to be returned from device */
        sl_IfRead(g_pCB->FD, (_u8* )&magicCode, 4);

        /* check for the received magic code matching */
        if (UART_SET_MODE_MAGIC_CODE != magicCode)
        {
            _SL_ASSERT(0);
        }
    }

    return (_i16)Msg.Rsp.status;
}
#endif
#endif


