/*
 * driver.h - CC31xx/CC32xx Host Driver Implementation
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
 
#ifndef __DRIVER_INT_H__
#define __DRIVER_INT_H__

#define TIMEOUT_STATE_EXPIRY      (0)
#define TIMEOUT_ONE_MORE_SHOT     (1)
#define TIMEOUT_STATE_INIT_VAL    (2)


/* Timeouts for the sync objects  */
#ifndef SL_DRIVER_TIMEOUT_SHORT
#define SL_DRIVER_TIMEOUT_SHORT        (1000) /* msec units */
#endif
#ifndef SL_DRIVER_TIMEOUT_LONG
#define SL_DRIVER_TIMEOUT_LONG         (65535) /* msec units */
#endif

#define INIT_COMPLETE_TIMEOUT          SL_DRIVER_TIMEOUT_LONG
#define STOP_DEVICE_TIMEOUT            SL_DRIVER_TIMEOUT_LONG
#ifndef SYNC_PATTERN_TIMEOUT_IN_MSEC
#define SYNC_PATTERN_TIMEOUT_IN_MSEC   (50) /* the sync patttern timeout in milliseconds units */
#endif
/*****************************************************************************/
/* Macro declarations                                                        */
/*****************************************************************************/

#ifndef CPU_FREQ_IN_MHZ
 #define CPU_FREQ_IN_MHZ         (200)
#endif
#define USEC_DELAY              (50)

#define SL_DRV_PROTECTION_OBJ_UNLOCK()       (void)_SlDrvProtectionObjUnLock();
#define SL_DRV_PROTECTION_OBJ_LOCK_FOREVER() (void)_SlDrvProtectionObjLockWaitForever();
#define SL_DRV_OBJ_UNLOCK(pObj)              (void)_SlDrvObjUnLock(pObj);
#define SL_DRV_OBJ_LOCK_FOREVER(pObj)        (void)_SlDrvObjLockWaitForever(pObj);
#define SL_DRV_SYNC_OBJ_SIGNAL(pObj)         (void)_SlDrvSyncObjSignal(pObj);
#define SL_DRV_SYNC_OBJ_CLEAR(pObj)          (void)sl_SyncObjWait(pObj,SL_OS_NO_WAIT);


#ifdef SL_TINY_EXT
#define SL_DRV_SYNC_OBJ_WAIT_FOREVER(SyncObj) (void)_SlDrvSyncObjWaitForever(SyncObj);
#define SL_DRV_LOCK_GLOBAL_LOCK_FOREVER()     (void)_SlDrvObjGlobalLockWaitForever();
#define SL_DRV_LOCK_GLOBAL_UNLOCK()           (void)_SlDrvGlobalObjUnLock();
#else
#define SL_DRV_SYNC_OBJ_WAIT_FOREVER(SyncObj) { \
if (SL_API_ABORTED == _SlDrvSyncObjWaitForever(SyncObj)) \
{ \
	return SL_API_ABORTED; \
} \
}

#define SL_DRV_SYNC_OBJ_WAIT_TIMEOUT(SyncObj, timeoutVal, apiIdx) { \
if (SL_API_ABORTED == _SlDrvSyncObjWaitTimeout(SyncObj, timeoutVal, apiIdx)) \
{ \
	return SL_API_ABORTED; \
} \
}
#define SL_DRV_LOCK_GLOBAL_LOCK_FOREVER() { \
_SlReturnVal_t retVal;                     \
                                           \
retVal = _SlDrvObjGlobalLockWaitForever(); \
if (retVal)         \
{                   \
	return retVal;  \
}                   \
}

#define SL_DRV_LOCK_GLOBAL_UNLOCK() { \
_SlReturnVal_t retVal;                \
                                      \
retVal = _SlDrvGlobalObjUnLock();    \
if (retVal)         \
{                   \
    return retVal;  \
}                   \
}
#endif


/*****************************************************************************/
/* Structure/Enum declarations                                               */
/*****************************************************************************/

typedef struct
{
	_u32  TSPrev;
	_u32  TSCurr;
	_u32  DeltaTicks;
	_u32  DeltaTicksReminder;
	_i32  Total10MSecUnits;
} _SlTimeoutParams_t;

typedef struct
{
	_u8 *pAsyncMsgBuff;
	_u8  bInCmdContext;
} DeviceEventInfo_t;

typedef struct
{
    _SlOpcode_t      Opcode;
    _SlArgSize_t     TxDescLen;
    _SlArgSize_t     RxDescLen;
}_SlCmdCtrl_t;

typedef struct
{
    _u16  TxPayloadLen;
    _i16  RxPayloadLen;
	_i16  ActualRxPayloadLen;
    _u8   *pTxPayload;
    _u8   *pRxPayload;
}_SlCmdExt_t;


typedef struct _SlArgsData_t
{
    _u8	 *pArgs;
	_u8    *pData;
} _SlArgsData_t;


typedef struct _SlPoolObj_t
{
    _SlSyncObj_t	      SyncObj;
	 _u8                *pRespArgs;
	_u8			      ActionID; 
	_u8			      AdditionalData; /* use for socketID and one bit which indicate supprt IPV6 or not (1=support, 0 otherwise) */
    _u8				  NextIndex;  

} _SlPoolObj_t;


typedef enum
{
	SOCKET_0,
	SOCKET_1,
	SOCKET_2,
	SOCKET_3,
	SOCKET_4,
	SOCKET_5,
	SOCKET_6,
	SOCKET_7,
	MAX_SOCKET_ENUM_IDX,
#ifndef SL_TINY_EXT    
    ACCEPT_ID = MAX_SOCKET_ENUM_IDX,
    CONNECT_ID,
#else
    CONNECT_ID = MAX_SOCKET_ENUM_IDX,
#endif
#ifndef SL_TINY_EXT    
	SELECT_ID,
#endif
	GETHOSYBYNAME_ID,
#ifndef SL_TINY_EXT    
	GETHOSYBYSERVICE_ID,
	PING_ID,
#endif	
    START_STOP_ID,
	RECV_ID
}_SlActionID_e;

typedef struct _SlActionLookup_t
{
    _u8					    ActionID;
    _u16				    ActionAsyncOpcode;
	_SlSpawnEntryFunc_t		AsyncEventHandler; 

} _SlActionLookup_t;


typedef struct
{
    _u8             TxPoolCnt;
    _SlLockObj_t    TxLockObj;
    _SlSyncObj_t    TxSyncObj;
}_SlFlowContCB_t;

typedef enum
{
    RECV_RESP_CLASS,
    CMD_RESP_CLASS,
    ASYNC_EVT_CLASS,
    DUMMY_MSG_CLASS
}_SlRxMsgClass_e;

typedef struct
{
    _u8                     *pAsyncBuf;         /* place to write pointer to buffer with CmdResp's Header + Arguments */
	_u8					    ActionIndex; 
    _SlSpawnEntryFunc_t     AsyncEvtHandler;    /* place to write pointer to AsyncEvent handler (calc-ed by Opcode)   */
    _SlRxMsgClass_e         RxMsgClass;         /* type of Rx message                                                 */
} AsyncExt_t;

typedef _u8 _SlSd_t;

typedef struct
{
	_SlCmdCtrl_t         *pCmdCtrl;
	_u8                  *pTxRxDescBuff;
	_SlCmdExt_t          *pCmdExt;
    AsyncExt_t            AsyncExt;
}_SlFunctionParams_t;


typedef struct
{
    _SlFd_t                          FD;
    _SlLockObj_t                     GlobalLockObj;
    _SlCommandHeader_t               TempProtocolHeader;
    P_INIT_CALLBACK                  pInitCallback;

    _SlPoolObj_t                    ObjPool[MAX_CONCURRENT_ACTIONS];
	_u8					    FreePoolIdx;
	_u8					    PendingPoolIdx;
	_u8					    ActivePoolIdx;
	_u32					ActiveActionsBitmap;
	_SlLockObj_t                    ProtectionLockObj;

    _SlSyncObj_t                     CmdSyncObj;  
    _u8                     IsCmdRespWaited;
    _SlFlowContCB_t          FlowContCB;
    _u8                     TxSeqNum;
    _u8                     RxDoneCnt;
    _u8                     SocketNonBlocking;
	_u8                     SocketTXFailure;
    /* for stack reduction the parameters are globals */
    _SlFunctionParams_t              FunctionParams;

    _u8 ActionIndex;
}_SlDriverCb_t;

extern _volatile _u8     RxIrqCnt;

#ifndef SL_TINY_EXT
extern _volatile _u8	 g_bDeviceRestartIsRequired;
#endif

extern _SlDriverCb_t* g_pCB;
extern P_SL_DEV_PING_CALLBACK  pPingCallBackFunc;

/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/

extern _SlReturnVal_t _SlDrvDriverCBInit(void);
extern _SlReturnVal_t _SlDrvDriverCBDeinit(void);
extern _SlReturnVal_t _SlDrvRxIrqHandler(void *pValue);
extern _SlReturnVal_t  _SlDrvCmdOp(_SlCmdCtrl_t *pCmdCtrl , void* pTxRxDescBuff , _SlCmdExt_t* pCmdExt);
extern _SlReturnVal_t  _SlDrvCmdSend(_SlCmdCtrl_t *pCmdCtrl , void* pTxRxDescBuff , _SlCmdExt_t* pCmdExt);
extern _SlReturnVal_t  _SlDrvDataReadOp(_SlSd_t Sd, _SlCmdCtrl_t *pCmdCtrl , void* pTxRxDescBuff , _SlCmdExt_t* pCmdExt);
extern _SlReturnVal_t  _SlDrvDataWriteOp(_SlSd_t Sd, _SlCmdCtrl_t *pCmdCtrl , void* pTxRxDescBuff , _SlCmdExt_t* pCmdExt);
extern _SlReturnVal_t _sl_HandleAsync_InitComplete(void *pVoidBuf); 
extern _SlReturnVal_t _sl_HandleAsync_Connect(void *pVoidBuf);
extern _SlReturnVal_t _SlDrvGlobalObjUnLock(void);
extern _SlReturnVal_t _SlDrvMsgReadSpawnCtx(void *pValue);


#ifndef SL_TINY_EXT
extern _i16  _SlDrvBasicCmd(_SlOpcode_t Opcode);
extern _SlReturnVal_t _sl_HandleAsync_Accept(void *pVoidBuf);
extern _SlReturnVal_t _sl_HandleAsync_DnsGetHostByService(void *pVoidBuf);
extern _SlReturnVal_t _sl_HandleAsync_Select(void *pVoidBuf);
#ifdef sl_GetTimestamp
extern void _SlDrvStartMeasureTimeout(_SlTimeoutParams_t *pTimeoutInfo, _u32 TimeoutInMsec);
extern _u8 _SlDrvIsTimeoutExpired(_SlTimeoutParams_t *pTimeoutInfo);
#endif

#endif
extern _SlReturnVal_t _sl_HandleAsync_DnsGetHostByName(void *pVoidBuf);
extern _SlReturnVal_t _sl_HandleAsync_DnsGetHostByAddr(void *pVoidBuf);
extern _SlReturnVal_t _sl_HandleAsync_PingResponse(void *pVoidBuf);
extern _SlReturnVal_t _SlDrvNetAppEventHandler(void* pArgs);

#if defined(sl_HttpServerCallback) || defined(EXT_LIB_REGISTERED_HTTP_SERVER_EVENTS)
extern void _SlDrvDispatchHttpServerEvents(SlHttpServerEvent_t *slHttpServerEvent, SlHttpServerResponse_t *slHttpServerResponse);
#endif

extern void _sl_HandleAsync_Stop(void *pVoidBuf);
extern _u8 _SlDrvWaitForPoolObj(_u8 ActionID, _u8 SocketID);
extern void _SlDrvReleasePoolObj(_u8 pObj);
extern _u16 _SlDrvAlignSize(_u16 msgLen); 
extern _u8  _SlDrvProtectAsyncRespSetting(_u8 *pAsyncRsp, _SlActionID_e ActionID, _u8 SocketID);



extern _SlReturnVal_t _SlDrvDeviceEventHandler(void* pEventInfo);
extern _SlReturnVal_t  _SlDrvSyncObjWaitForever(_SlSyncObj_t *pSyncObj);
extern _SlReturnVal_t  _SlDrvObjLockWaitForever(_SlLockObj_t *pLockObj);
extern _SlReturnVal_t  _SlDrvSyncObjWaitTimeout(_SlSyncObj_t *pSyncObj,
		                                _u32 timeoutVal,
		                                SlDriverAPIWithTimeout_e apiIdx);

extern _SlReturnVal_t  _SlDrvSyncObjSignal(_SlSyncObj_t *pSyncObj);
extern _SlReturnVal_t  _SlDrvObjLock(_SlLockObj_t *pLockObj, _SlTime_t Timeout);
extern _SlReturnVal_t  _SlDrvProtectionObjLockWaitForever(void);
extern _SlReturnVal_t  _SlDrvObjUnLock(_SlLockObj_t *pLockObj);
extern _SlReturnVal_t  _SlDrvProtectionObjUnLock(void);

extern void  _SlDrvMemZero(void* Addr, _u16 size);
extern void  _SlDrvResetCmdExt(_SlCmdExt_t* pCmdExt);

#ifndef SL_TINY_EXT
extern void _SlDriverHandleError(SlDeviceDriverError_e eError, _u32 info1, _u32 info2);
#endif

#define _SL_PROTOCOL_ALIGN_SIZE(msgLen)             (((msgLen)+3) & (~3))
#define _SL_IS_PROTOCOL_ALIGNED_SIZE(msgLen)        (!((msgLen) & 3))


#define _SL_PROTOCOL_CALC_LEN(pCmdCtrl,pCmdExt)     ((pCmdExt) ? \
                                                     (_SL_PROTOCOL_ALIGN_SIZE(pCmdCtrl->TxDescLen) + _SL_PROTOCOL_ALIGN_SIZE(pCmdExt->TxPayloadLen)) : \
                                                     (_SL_PROTOCOL_ALIGN_SIZE(pCmdCtrl->TxDescLen)))
#endif /* __DRIVER_INT_H__ */
