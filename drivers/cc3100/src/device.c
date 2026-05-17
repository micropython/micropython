/*
* device.c - CC31xx/CC32xx Host Driver Implementation
*
* Copyright (C) 2014 Texas Instruments Incorporated - http://www.ti.com/ 
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


/*****************************************************************************/
/* Internal functions                                                        */
/*****************************************************************************/

const _i8 StartResponseLUT[8] = 
{
    ROLE_UNKNOWN_ERR,
    ROLE_STA,
    ROLE_STA_ERR,
    ROLE_AP,
    ROLE_AP_ERR,
    ROLE_P2P,
    ROLE_P2P_ERR,
    ROLE_UNKNOWN_ERR    
};



_i16 _sl_GetStartResponseConvert(_u32 Status)
{
    return (_i16)StartResponseLUT[Status & 0x7];
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
    _SlTaskEntry();
#endif
}
#endif

/*****************************************************************************/
/* sl_Start                                                                  */
/*****************************************************************************/
#if _SL_INCLUDE_FUNC(sl_Start)
_i16 sl_Start(const void* pIfHdl, _i8*  pDevName, const P_INIT_CALLBACK pInitCallBack)
{
    _i16 ObjIdx = MAX_CONCURRENT_ACTIONS;
    InitComplete_t  AsyncRsp;

    /* Perform any preprocessing before enable networking services */
    sl_DeviceEnablePreamble();

    /* ControlBlock init */
    _SlDrvDriverCBInit();

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
            _SlDrvSyncObjWaitForever(&g_pCB->ObjPool[ObjIdx].SyncObj);

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
void _sl_HandleAsync_InitComplete(void *pVoidBuf)
{
    InitComplete_t     *pMsgArgs   = (InitComplete_t *)_SL_RESP_ARGS_START(pVoidBuf);

    _SlDrvProtectionObjLockWaitForever();

    if(g_pCB->pInitCallback)
    {
        g_pCB->pInitCallback(_sl_GetStartResponseConvert(pMsgArgs->Status));
    }
    else
    {
        sl_Memcpy(g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].pRespArgs, pMsgArgs, sizeof(InitComplete_t));
        _SlDrvSyncObjSignal(&g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].SyncObj);
    }
    
   _SlDrvProtectionObjUnLock();
   
    if(g_pCB->pInitCallback)
    {
        _SlDrvReleasePoolObj(g_pCB->FunctionParams.AsyncExt.ActionIndex);
    }

}

/***************************************************************************
_sl_HandleAsync_Stop - handles stop signalling to 
a waiting object
****************************************************************************/
void _sl_HandleAsync_Stop(void *pVoidBuf)
{
    _BasicResponse_t     *pMsgArgs   = (_BasicResponse_t *)_SL_RESP_ARGS_START(pVoidBuf);

    VERIFY_SOCKET_CB(NULL != g_pCB->StopCB.pAsyncRsp);

    _SlDrvProtectionObjLockWaitForever();

    sl_Memcpy(g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].pRespArgs, pMsgArgs, sizeof(_BasicResponse_t));

    _SlDrvSyncObjSignal(&g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].SyncObj);
    _SlDrvProtectionObjUnLock();
    
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

const _SlCmdCtrl_t _SlStopCmdCtrl =
{
    SL_OPCODE_DEVICE_STOP_COMMAND,
    sizeof(_DevStopCommand_t),
    sizeof(_BasicResponse_t)
};

#if _SL_INCLUDE_FUNC(sl_Stop)
_i16 sl_Stop(const _u16 timeout)
{
    _i16 RetVal=0;
    _SlStopMsg_u      Msg;
    _BasicResponse_t  AsyncRsp;
    _i8 ObjIdx = MAX_CONCURRENT_ACTIONS;
    /* if timeout is 0 the shutdown is forced immediately */
    if( 0 == timeout ) 
    {
        sl_IfRegIntHdlr(NULL, NULL);
        sl_DeviceDisable();
        RetVal = sl_IfClose(g_pCB->FD);

    }
    else
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
         _SlDrvSyncObjWaitForever(&g_pCB->ObjPool[ObjIdx].SyncObj);
         Msg.Rsp.status = AsyncRsp.status;
         RetVal = Msg.Rsp.status;
      }

      _SlDrvReleasePoolObj(ObjIdx);
      sl_IfRegIntHdlr(NULL, NULL);
      sl_DeviceDisable();
      sl_IfClose(g_pCB->FD);
    }
    _SlDrvDriverCBDeinit();

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

const _SlCmdCtrl_t _SlEventMaskSetCmdCtrl =
{
    SL_OPCODE_DEVICE_EVENTMASKSET,
    sizeof(_DevMaskEventSetCommand_t),
    sizeof(_BasicResponse_t)
};


_i16 sl_EventMaskSet(const _u8 EventClass ,const _u32 Mask)
{
    _SlEventMaskSetMsg_u Msg;

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

const _SlCmdCtrl_t _SlEventMaskGetCmdCtrl =
{
    SL_OPCODE_DEVICE_EVENTMASKGET,
    sizeof(_DevMaskEventGetCommand_t),
    sizeof(_DevMaskEventGetResponse_t)
};


_i16 sl_EventMaskGet(const _u8 EventClass,_u32 *pMask)
{
    _SlEventMaskGetMsg_u Msg;

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

const _SlCmdCtrl_t _SlDeviceGetCmdCtrl =
{
    SL_OPCODE_DEVICE_DEVICEGET,
    sizeof(_DeviceSetGet_t),
    sizeof(_DeviceSetGet_t)
};

_i32 sl_DevGet(const _u8 DeviceGetId,_u8 *pOption,_u8 *pConfigLen, _u8 *pValues)
{
    _SlDeviceMsgGet_u         Msg;
    _SlCmdExt_t               CmdExt;

    if (*pConfigLen == 0)
    {
        return SL_EZEROLEN;
    }

    if( pOption )
    {

      _SlDrvResetCmdExt(&CmdExt);
        CmdExt.RxPayloadLen = *pConfigLen;
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

const _SlCmdCtrl_t _SlDeviceSetCmdCtrl =
{
    SL_OPCODE_DEVICE_DEVICESET,
    sizeof(_DeviceSetGet_t),
    sizeof(_BasicResponse_t)
};

_i32 sl_DevSet(const _u8 DeviceSetId ,const _u8 Option,const _u8 ConfigLen,const _u8 *pValues)
{
    _SlDeviceMsgSet_u         Msg;
    _SlCmdExt_t               CmdExt;


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
void _SlDrvDeviceEventHandler(void* pArgs)
{
    _SlResponseHeader_t      *pHdr       = (_SlResponseHeader_t *)pArgs;

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
#if defined (sl_GeneralEvtHdlr) || defined(EXT_LIB_REGISTERED_GENERAL_EVENTS)
				SlDeviceEvent_t      devHandler;
				devHandler.Event = SL_DEVICE_ABORT_ERROR_EVENT;	
				devHandler.EventData.deviceReport.AbortType = *((_u32*)pArgs + 2);
				devHandler.EventData.deviceReport.AbortData = *((_u32*)pArgs + 3);
				_SlDrvHandleGeneralEvents(&devHandler);
#endif		
			}
        break;

    case  SL_OPCODE_DEVICE_DEVICEASYNCFATALERROR:
#if defined (sl_GeneralEvtHdlr) || defined(EXT_LIB_REGISTERED_GENERAL_EVENTS)
        {
            _BasicResponse_t     *pMsgArgs   = (_BasicResponse_t *)_SL_RESP_ARGS_START(pHdr);
            SlDeviceEvent_t      devHandler;
            devHandler.Event = SL_DEVICE_FATAL_ERROR_EVENT;
            devHandler.EventData.deviceEvent.status = pMsgArgs->status & 0xFF;
            devHandler.EventData.deviceEvent.sender = (SlErrorSender_e)((pMsgArgs->status >> 8) & 0xFF);
            _SlDrvHandleGeneralEvents(&devHandler);
        }
#endif
        break;
    default:
        SL_ERROR_TRACE2(MSG_306, "ASSERT: _SlDrvDeviceEventHandler : invalid opcode = 0x%x = %1", pHdr->GenHeader.Opcode, pHdr->GenHeader.Opcode);
    }
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
    sizeof(_DevUartSetModeCommand_t),
    sizeof(_DevUartSetModeResponse_t)
};

_i16 sl_UartSetMode(const SlUartIfParams_t* pUartParams)
{
    _SlUartSetModeMsg_u Msg;
    _u32 magicCode = 0xFFFFFFFF;

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


