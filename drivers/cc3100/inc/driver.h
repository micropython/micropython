/*
 * driver.h - CC31xx/CC32xx Host Driver Implementation
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
 
#ifndef __DRIVER_INT_H__
#define __DRIVER_INT_H__


/*****************************************************************************/
/* Macro declarations                                                        */
/*****************************************************************************/

#ifndef CPU_FREQ_IN_MHZ
 #define CPU_FREQ_IN_MHZ         (200)
#endif
#define USEC_DELAY              (50)

/*****************************************************************************/
/* Structure/Enum declarations                                               */
/*****************************************************************************/

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

extern _volatile _u8           RxIrqCnt;

extern _SlDriverCb_t* g_pCB;
extern P_SL_DEV_PING_CALLBACK  pPingCallBackFunc;

/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/
extern void _SlDrvDriverCBInit(void);
extern void _SlDrvDriverCBDeinit(void);
extern void _SlDrvRxIrqHandler(void *pValue);
extern _SlReturnVal_t  _SlDrvCmdOp(_SlCmdCtrl_t *pCmdCtrl , void* pTxRxDescBuff , _SlCmdExt_t* pCmdExt);
extern _SlReturnVal_t  _SlDrvCmdSend(_SlCmdCtrl_t *pCmdCtrl , void* pTxRxDescBuff , _SlCmdExt_t* pCmdExt);
extern _SlReturnVal_t  _SlDrvDataReadOp(_SlSd_t Sd, _SlCmdCtrl_t *pCmdCtrl , void* pTxRxDescBuff , _SlCmdExt_t* pCmdExt);
extern _SlReturnVal_t  _SlDrvDataWriteOp(_SlSd_t Sd, _SlCmdCtrl_t *pCmdCtrl , void* pTxRxDescBuff , _SlCmdExt_t* pCmdExt);
extern void _sl_HandleAsync_InitComplete(void *pVoidBuf);
extern void _sl_HandleAsync_Connect(void *pVoidBuf);


#ifndef SL_TINY_EXT
extern _i16  _SlDrvBasicCmd(_SlOpcode_t Opcode);
extern void _sl_HandleAsync_Accept(void *pVoidBuf);
extern void _sl_HandleAsync_DnsGetHostByService(void *pVoidBuf);
extern void _sl_HandleAsync_Select(void *pVoidBuf);
#endif


extern void _sl_HandleAsync_DnsGetHostByName(void *pVoidBuf);
extern void _sl_HandleAsync_DnsGetHostByAddr(void *pVoidBuf);
extern void _sl_HandleAsync_PingResponse(void *pVoidBuf);
extern void _SlDrvNetAppEventHandler(void* pArgs);
extern void _SlDrvDeviceEventHandler(void* pArgs);
extern void _sl_HandleAsync_Stop(void *pVoidBuf);
extern _u8  _SlDrvWaitForPoolObj(_u8 ActionID, _u8 SocketID);
extern void _SlDrvReleasePoolObj(_u8 pObj);
extern _u16 _SlDrvAlignSize(_u16 msgLen); 
extern _u8  _SlDrvProtectAsyncRespSetting(_u8 *pAsyncRsp, _u8 ActionID, _u8 SocketID);


extern void  _SlDrvSyncObjWaitForever(_SlSyncObj_t *pSyncObj);
extern void  _SlDrvSyncObjSignal(_SlSyncObj_t *pSyncObj);
extern void  _SlDrvObjLock(_SlLockObj_t *pLockObj, _SlTime_t Timeout);
extern void  _SlDrvObjLockWaitForever(_SlLockObj_t *pLockObj);
extern void  _SlDrvProtectionObjLockWaitForever();
extern void  _SlDrvObjUnLock(_SlLockObj_t *pLockObj);
extern void  _SlDrvProtectionObjUnLock();

extern void  _SlDrvMemZero(void* Addr, _u16 size);
extern void  _SlDrvResetCmdExt(_SlCmdExt_t* pCmdExt);



#define _SL_PROTOCOL_ALIGN_SIZE(msgLen)             (((msgLen)+3) & (~3))
#define _SL_IS_PROTOCOL_ALIGNED_SIZE(msgLen)        (!((msgLen) & 3))


#define _SL_PROTOCOL_CALC_LEN(pCmdCtrl,pCmdExt)     ((pCmdExt) ? \
                                                     (_SL_PROTOCOL_ALIGN_SIZE(pCmdCtrl->TxDescLen) + _SL_PROTOCOL_ALIGN_SIZE(pCmdExt->TxPayloadLen)) : \
                                                     (_SL_PROTOCOL_ALIGN_SIZE(pCmdCtrl->TxDescLen)))
#endif /* __DRIVER_INT_H__ */
