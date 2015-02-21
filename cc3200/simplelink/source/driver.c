/*
* driver.c - CC31xx/CC32xx Host Driver Implementation
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
#include "driver.h"
#include "flowcont.h"

/*****************************************************************************/
/* Macro declarations                                                        */
/*****************************************************************************/

#define          _SL_PENDING_RX_MSG(pDriverCB)   ((pDriverCB)->RxIrqCnt != (pDriverCB)->RxDoneCnt)

/*  2 LSB of the N2H_SYNC_PATTERN are for sequence number 
only in SPI interface
support backward sync pattern */
#define N2H_SYNC_PATTERN_SEQ_NUM_BITS            ((_u32)0x00000003) /* Bits 0..1    - use the 2 LBS for seq num */
#define N2H_SYNC_PATTERN_SEQ_NUM_EXISTS          ((_u32)0x00000004) /* Bit  2       - sign that sequence number exists in the sync pattern */
#define N2H_SYNC_PATTERN_MASK                    ((_u32)0xFFFFFFF8) /* Bits 3..31   - constant SYNC PATTERN */
#define N2H_SYNC_SPI_BUGS_MASK                   ((_u32)0x7FFF7F7F) /* Bits 7,15,31 - ignore the SPI (8,16,32 bites bus) error bits  */
#define BUF_SYNC_SPIM(pBuf)                      ((*(_u32 *)(pBuf)) & N2H_SYNC_SPI_BUGS_MASK)
#define N2H_SYNC_SPIM                            (N2H_SYNC_PATTERN    & N2H_SYNC_SPI_BUGS_MASK)
#define N2H_SYNC_SPIM_WITH_SEQ(TxSeqNum)         ((N2H_SYNC_SPIM & N2H_SYNC_PATTERN_MASK) | N2H_SYNC_PATTERN_SEQ_NUM_EXISTS | ((TxSeqNum) & (N2H_SYNC_PATTERN_SEQ_NUM_BITS)))
#define MATCH_WOUT_SEQ_NUM(pBuf)                 ( BUF_SYNC_SPIM(pBuf) ==  N2H_SYNC_SPIM )
#define MATCH_WITH_SEQ_NUM(pBuf, TxSeqNum)       ( BUF_SYNC_SPIM(pBuf) == (N2H_SYNC_SPIM_WITH_SEQ(TxSeqNum)) )
#define N2H_SYNC_PATTERN_MATCH(pBuf, TxSeqNum) \
    ( \
    (  (*((_u32 *)pBuf) & N2H_SYNC_PATTERN_SEQ_NUM_EXISTS) && ( MATCH_WITH_SEQ_NUM(pBuf, TxSeqNum) ) )  || \
    ( !(*((_u32 *)pBuf) & N2H_SYNC_PATTERN_SEQ_NUM_EXISTS) && ( MATCH_WOUT_SEQ_NUM(pBuf          ) ) )     \
    )

#define OPCODE(_ptr)          (((_SlResponseHeader_t *)(_ptr))->GenHeader.Opcode)         
#define RSP_PAYLOAD_LEN(_ptr) (((_SlResponseHeader_t *)(_ptr))->GenHeader.Len - _SL_RESP_SPEC_HDR_SIZE)         
#define SD(_ptr)              (((_SocketAddrResponse_u *)(_ptr))->IpV4.sd)
/*  Actual size of Recv/Recvfrom response data  */
#define ACT_DATA_SIZE(_ptr)   (((_SocketAddrResponse_u *)(_ptr))->IpV4.statusOrLen)

#if (SL_MEMORY_MGMT == SL_MEMORY_MGMT_STATIC)
typedef struct
{
    _u32 Align;
    _SlDriverCb_t DriverCB;
    _u8 AsyncRespBuf[SL_ASYNC_MAX_MSG_LEN];
}_SlStatMem_t;

_SlStatMem_t g_StatMem;
#endif

/*****************************************************************************/
/* Variables                                                                 */
/*****************************************************************************/
const _SlSyncPattern_t g_H2NSyncPattern = H2N_SYNC_PATTERN;
const _SlSyncPattern_t g_H2NCnysPattern = H2N_CNYS_PATTERN;
const _SlActionLookup_t _SlActionLookupTable[7] = 
{
    {ACCEPT_ID, SL_OPCODE_SOCKET_ACCEPTASYNCRESPONSE, (_SlSpawnEntryFunc_t)_sl_HandleAsync_Accept},
    {CONNECT_ID, SL_OPCODE_SOCKET_CONNECTASYNCRESPONSE,(_SlSpawnEntryFunc_t)_sl_HandleAsync_Connect},
    {SELECT_ID, SL_OPCODE_SOCKET_SELECTASYNCRESPONSE,(_SlSpawnEntryFunc_t)_sl_HandleAsync_Select},
    {GETHOSYBYNAME_ID, SL_OPCODE_NETAPP_DNSGETHOSTBYNAMEASYNCRESPONSE,(_SlSpawnEntryFunc_t)_sl_HandleAsync_DnsGetHostByName},
    {GETHOSYBYSERVICE_ID, SL_OPCODE_NETAPP_MDNSGETHOSTBYSERVICEASYNCRESPONSE,(_SlSpawnEntryFunc_t)_sl_HandleAsync_DnsGetHostByService},
    {PING_ID, SL_OPCODE_NETAPP_PINGREPORTREQUESTRESPONSE, (_SlSpawnEntryFunc_t)_sl_HandleAsync_PingResponse},
    {START_STOP_ID, SL_OPCODE_DEVICE_STOP_ASYNC_RESPONSE,(_SlSpawnEntryFunc_t)_sl_HandleAsync_Stop}

};

_SlDriverCb_t* g_pCB = NULL;
P_SL_DEV_PING_CALLBACK  pPingCallBackFunc = NULL;
_u8 gFirstCmdMode = 0;

/*****************************************************************************/
/* Function prototypes                                                       */
/*****************************************************************************/
_SlReturnVal_t   _SlDrvMsgRead(void);
_SlReturnVal_t   _SlDrvMsgWrite(void);
_SlReturnVal_t   _SlDrvMsgReadCmdCtx(void);
_SlReturnVal_t   _SlDrvMsgReadSpawnCtx(void *pValue);
void             _SlDrvClassifyRxMsg(_SlOpcode_t Opcode );
_SlReturnVal_t   _SlDrvRxHdrRead(_u8 *pBuf, _u8 *pAlignSize);
void             _SlDrvShiftDWord(_u8 *pBuf);
void             _SlDrvDriverCBInit(void);
void             _SlAsyncEventGenericHandler(void);
_i16             _SlDrvWaitForPoolObj(_u32 ActionID, _u8 SocketID);
void             _SlDrvReleasePoolObj(_u8 pObj);
void             _SlDrvObjInit(void);
void             _SlDrvObjDeInit(void);
void             _SlRemoveFromList(_u8* ListIndex, _u8 ItemIndex);
_SlReturnVal_t   _SlFindAndSetActiveObj(_SlOpcode_t  Opcode, _u8 Sd);


/*****************************************************************************/
/* Internal functions                                                        */
/*****************************************************************************/



/*****************************************************************************
_SlDrvDriverCBInit - init Driver Control Block
*****************************************************************************/
void _SlDrvDriverCBInit(void)
{
    _u8          Idx;

#if (SL_MEMORY_MGMT == SL_MEMORY_MGMT_STATIC)
    g_pCB = &(g_StatMem.DriverCB);
#else
    g_pCB = sl_Malloc(sizeof(_SlDriverCb_t));
#endif
    MALLOC_OK_CHECK(g_pCB);

    sl_Memset((g_pCB), 0, sizeof(_SlDriverCb_t));

    OSI_RET_OK_CHECK( sl_SyncObjCreate(&g_pCB->CmdSyncObj, "CmdSyncObj") );
    sl_SyncObjClear(&g_pCB->CmdSyncObj);

    OSI_RET_OK_CHECK( sl_LockObjCreate(&g_pCB->GlobalLockObj, "GlobalLockObj") );

    OSI_RET_OK_CHECK( sl_LockObjCreate(&g_pCB->ProtectionLockObj, "ProtectionLockObj") );

    _SlDrvObjInit();

    for (Idx = 0; Idx < MAX_CONCURRENT_ACTIONS; Idx++)
    {
        OSI_RET_OK_CHECK( sl_SyncObjCreate(&g_pCB->ObjPool[Idx].SyncObj, "SyncObj") );  
        sl_SyncObjClear(&g_pCB->ObjPool[Idx].SyncObj);     
    }
    _SlDrvFlowContInit();
    gFirstCmdMode = 0;
}

/*****************************************************************************
_SlDrvDriverCBDeinit - De init Driver Control Block
*****************************************************************************/
void _SlDrvDriverCBDeinit()
{
    _u8        Idx;

    _SlDrvFlowContDeinit();

    OSI_RET_OK_CHECK( sl_SyncObjDelete(&g_pCB->CmdSyncObj) );

    OSI_RET_OK_CHECK( sl_LockObjDelete(&g_pCB->GlobalLockObj) );

    OSI_RET_OK_CHECK( sl_LockObjDelete(&g_pCB->ProtectionLockObj) );
    for (Idx = 0; Idx < MAX_CONCURRENT_ACTIONS; Idx++)
    {
        OSI_RET_OK_CHECK( sl_SyncObjDelete(&g_pCB->ObjPool[Idx].SyncObj) );   
    }

    _SlDrvObjDeInit();

#if (SL_MEMORY_MGMT == SL_MEMORY_MGMT_STATIC)
    g_pCB = NULL;
#else
    sl_Free(g_pCB);
#endif

    g_pCB = NULL;
}

/*****************************************************************************
_SlDrvRxIrqHandler - Interrupt handler 
*****************************************************************************/
void _SlDrvRxIrqHandler(void *pValue)
{
    sl_IfMaskIntHdlr();

    g_pCB->RxIrqCnt++;

    if (TRUE == g_pCB->IsCmdRespWaited)
    {
        OSI_RET_OK_CHECK( sl_SyncObjSignalFromIRQ(&g_pCB->CmdSyncObj) );
    }
    else
    {
        sl_Spawn((_SlSpawnEntryFunc_t)_SlDrvMsgReadSpawnCtx, NULL, 0);
    }
}

/*****************************************************************************
_SlDrvCmdOp
*****************************************************************************/
_SlReturnVal_t _SlDrvCmdOp(
    _SlCmdCtrl_t  *pCmdCtrl ,
    void          *pTxRxDescBuff ,
    _SlCmdExt_t   *pCmdExt)
{
    _SlReturnVal_t RetVal;

    OSI_RET_OK_CHECK(sl_LockObjLock(&g_pCB->GlobalLockObj, SL_OS_WAIT_FOREVER));
    g_pCB->IsCmdRespWaited = TRUE;

    SL_TRACE0(DBG_MSG, MSG_312, "_SlDrvCmdOp: call _SlDrvMsgWrite");
    /* send the message */
    g_pCB->FunctionParams.pCmdCtrl = pCmdCtrl;
    g_pCB->FunctionParams.pTxRxDescBuff = pTxRxDescBuff;
    g_pCB->FunctionParams.pCmdExt = pCmdExt;

    RetVal = _SlDrvMsgWrite();

    if(SL_OS_RET_CODE_OK == RetVal)
    {

#ifndef SL_IF_TYPE_UART    
        /* Waiting for SPI to stabilize after first command */
        if( 0 == gFirstCmdMode )
        {
            volatile _u32 CountVal = 0;
            gFirstCmdMode = 1;
            CountVal = CPU_FREQ_IN_MHZ*USEC_DELAY;
            while( CountVal-- );
        }   
#endif 
        /* wait for respond */
        RetVal = _SlDrvMsgReadCmdCtx(); /* will free global lock */
        SL_TRACE0(DBG_MSG, MSG_314, "_SlDrvCmdOp: exited _SlDrvMsgReadCmdCtx");

    }
    else
    {
        OSI_RET_OK_CHECK(sl_LockObjUnlock(&g_pCB->GlobalLockObj));
    }

    return RetVal;
}

/*****************************************************************************
_SlDrvCmdSend 
Send SL command without waiting for command response 
This function is unprotected and the caller should make 
sure global lock is active
*****************************************************************************/
_SlReturnVal_t _SlDrvCmdSend(
    _SlCmdCtrl_t  *pCmdCtrl ,
    void          *pTxRxDescBuff ,
    _SlCmdExt_t   *pCmdExt)
{
    _SlReturnVal_t RetVal;

    g_pCB->IsCmdRespWaited = FALSE;

    SL_TRACE0(DBG_MSG, MSG_312, "_SlDrvCmdSend: call _SlDrvMsgWrite");
    /*  send the message */
    g_pCB->FunctionParams.pCmdCtrl = pCmdCtrl;
    g_pCB->FunctionParams.pTxRxDescBuff = pTxRxDescBuff;
    g_pCB->FunctionParams.pCmdExt = pCmdExt;

    RetVal = _SlDrvMsgWrite();

    return RetVal;
}

/*****************************************************************************
_SlDrvDataReadOp
*****************************************************************************/
_SlReturnVal_t _SlDrvDataReadOp(
    _SlSd_t             Sd,
    _SlCmdCtrl_t        *pCmdCtrl ,
    void                *pTxRxDescBuff ,
    _SlCmdExt_t         *pCmdExt)
{
    _SlReturnVal_t RetVal;
    _u8 ObjIdx = MAX_CONCURRENT_ACTIONS;
    _SlArgsData_t pArgsData;

    /* Validate input arguments */
    VERIFY_PROTOCOL(NULL != pCmdExt->pRxPayload);

    /* If zero bytes is requested, return error. */
    /*  This allows us not to fill remote socket's IP address in return arguments */
    VERIFY_PROTOCOL(0 != pCmdExt->RxPayloadLen);

    /* Validate socket */
    if((Sd & BSD_SOCKET_ID_MASK) >= SL_MAX_SOCKETS)
    {
        return SL_EBADF;
    }

    /*Use Obj to issue the command, if not available try later*/
    ObjIdx = (_u8)_SlDrvWaitForPoolObj(RECV_ID, Sd & BSD_SOCKET_ID_MASK);

    if (MAX_CONCURRENT_ACTIONS == ObjIdx)
    {
        return SL_POOL_IS_EMPTY;
    }

    OSI_RET_OK_CHECK(sl_LockObjLock(&g_pCB->ProtectionLockObj, SL_OS_WAIT_FOREVER));

    pArgsData.pData = pCmdExt->pRxPayload;
    pArgsData.pArgs =  (_u8 *)pTxRxDescBuff;
    g_pCB->ObjPool[ObjIdx].pRespArgs =  (_u8 *)&pArgsData;
    OSI_RET_OK_CHECK(sl_LockObjUnlock(&g_pCB->ProtectionLockObj));


    /* Do Flow Control check/update for DataWrite operation */
    OSI_RET_OK_CHECK( sl_LockObjLock(&g_pCB->FlowContCB.TxLockObj, SL_OS_WAIT_FOREVER) );

    /* Clear SyncObj for the case it was signalled before TxPoolCnt */
    /* dropped below '1' (last Data buffer was taken)  */
    /* OSI_RET_OK_CHECK( sl_SyncObjClear(&g_pCB->FlowContCB.TxSyncObj) ); */
    sl_SyncObjClear(&g_pCB->FlowContCB.TxSyncObj);

    if(g_pCB->FlowContCB.TxPoolCnt <= FLOW_CONT_MIN)
    {

        /* If TxPoolCnt was increased by other thread at this moment,
        TxSyncObj won't wait here */
        OSI_RET_OK_CHECK( sl_SyncObjWait(&g_pCB->FlowContCB.TxSyncObj, SL_OS_WAIT_FOREVER) );
    }

    OSI_RET_OK_CHECK(sl_LockObjLock(&g_pCB->GlobalLockObj, SL_OS_WAIT_FOREVER));

    VERIFY_PROTOCOL(g_pCB->FlowContCB.TxPoolCnt > FLOW_CONT_MIN);
    g_pCB->FlowContCB.TxPoolCnt--;

    OSI_RET_OK_CHECK( sl_LockObjUnlock(&g_pCB->FlowContCB.TxLockObj) );

    /* send the message */
    g_pCB->TempProtocolHeader.Opcode    = pCmdCtrl->Opcode;
    g_pCB->TempProtocolHeader.Len   = _SL_PROTOCOL_CALC_LEN(pCmdCtrl,pCmdExt);
    g_pCB->FunctionParams.pCmdCtrl      = pCmdCtrl;
    g_pCB->FunctionParams.pTxRxDescBuff = (_u8 *)pTxRxDescBuff;
    g_pCB->FunctionParams.pCmdExt       = pCmdExt;
    RetVal =  _SlDrvMsgWrite();

    OSI_RET_OK_CHECK(sl_LockObjUnlock(&g_pCB->GlobalLockObj));

    if(SL_OS_RET_CODE_OK == RetVal)
    {
        /* Wait for response message. Will be signalled by _SlDrvMsgRead. */
        OSI_RET_OK_CHECK(sl_SyncObjWait(&g_pCB->ObjPool[ObjIdx].SyncObj, SL_OS_WAIT_FOREVER));
    }

    _SlDrvReleasePoolObj(ObjIdx);
    return RetVal;
}

/* ******************************************************************************/
/*   _SlDrvDataWriteOp                                                          */
/* ******************************************************************************/
_SlReturnVal_t _SlDrvDataWriteOp(
    _SlSd_t             Sd,
    _SlCmdCtrl_t  *pCmdCtrl ,
    void                *pTxRxDescBuff ,
    _SlCmdExt_t         *pCmdExt)
{
    _SlReturnVal_t  RetVal = SL_EAGAIN; /*  initiated as SL_EAGAIN for the non blocking mode */
    while( 1 )
    {
        /*  Do Flow Control check/update for DataWrite operation */
        OSI_RET_OK_CHECK( sl_LockObjLock(&g_pCB->FlowContCB.TxLockObj, SL_OS_WAIT_FOREVER) );

        /*  Clear SyncObj for the case it was signalled before TxPoolCnt */
        /*  dropped below '1' (last Data buffer was taken) */
        /* OSI_RET_OK_CHECK( sl_SyncObjClear(&g_pCB->FlowContCB.TxSyncObj) ); */
        sl_SyncObjClear(&g_pCB->FlowContCB.TxSyncObj);

        /*  we have indication that the last send has failed - socket is no longer valid for operations  */
        if(g_pCB->SocketTXFailure & (1<<(Sd & BSD_SOCKET_ID_MASK)))
        {
            OSI_RET_OK_CHECK( sl_LockObjUnlock(&g_pCB->FlowContCB.TxLockObj) );
            return SL_SOC_ERROR;
        }
        if(g_pCB->FlowContCB.TxPoolCnt <= FLOW_CONT_MIN + 1)
        {
            /*  we have indication that this socket is set as blocking and we try to  */
            /*  unblock it - return an error */
            if( g_pCB->SocketNonBlocking >> (Sd & BSD_SOCKET_ID_MASK) )
            {
                OSI_RET_OK_CHECK( sl_LockObjUnlock(&g_pCB->FlowContCB.TxLockObj) );
                return RetVal;
            }
            /*  If TxPoolCnt was increased by other thread at this moment, */
            /*  TxSyncObj won't wait here */
            OSI_RET_OK_CHECK( sl_SyncObjWait(&g_pCB->FlowContCB.TxSyncObj, SL_OS_WAIT_FOREVER) );
        }
        if(g_pCB->FlowContCB.TxPoolCnt > FLOW_CONT_MIN + 1 )
        {
            break;
        }
        else
        {
            OSI_RET_OK_CHECK( sl_LockObjUnlock(&g_pCB->FlowContCB.TxLockObj) );
        }
    }
    OSI_RET_OK_CHECK( sl_LockObjLock(&g_pCB->GlobalLockObj, SL_OS_WAIT_FOREVER) );

    VERIFY_PROTOCOL(g_pCB->FlowContCB.TxPoolCnt > FLOW_CONT_MIN + 1 );
    g_pCB->FlowContCB.TxPoolCnt--;

    OSI_RET_OK_CHECK( sl_LockObjUnlock(&g_pCB->FlowContCB.TxLockObj) );

    /*  send the message */
    g_pCB->TempProtocolHeader.Opcode    = pCmdCtrl->Opcode;
    g_pCB->TempProtocolHeader.Len   = _SL_PROTOCOL_CALC_LEN(pCmdCtrl,pCmdExt);

    g_pCB->FunctionParams.pCmdCtrl = pCmdCtrl;
    g_pCB->FunctionParams.pTxRxDescBuff = pTxRxDescBuff;
    g_pCB->FunctionParams.pCmdExt = pCmdExt;
    RetVal =  _SlDrvMsgWrite();

    OSI_RET_OK_CHECK( sl_LockObjUnlock(&g_pCB->GlobalLockObj) );

    return RetVal;
}

/* ******************************************************************************/
/*  _SlDrvMsgWrite */
/* ******************************************************************************/
_SlReturnVal_t _SlDrvMsgWrite(void)
{
    VERIFY_PROTOCOL(NULL != g_pCB->FunctionParams.pCmdCtrl);

    g_pCB->TempProtocolHeader.Opcode    = g_pCB->FunctionParams.pCmdCtrl->Opcode;
    g_pCB->TempProtocolHeader.Len   = _SL_PROTOCOL_CALC_LEN(g_pCB->FunctionParams.pCmdCtrl,
        g_pCB->FunctionParams.pCmdExt);
    /*  */
    if (g_pCB->RelayFlagsViaRxPayload == TRUE)
    {
        g_pCB->TempProtocolHeader.Len = g_pCB->TempProtocolHeader.Len + g_pCB->FunctionParams.pCmdExt->RxPayloadLen;
    }

#ifdef SL_START_WRITE_STAT
    sl_IfStartWriteSequence(g_pCB->FD);
#endif

#ifdef SL_IF_TYPE_UART
    /*  Write long sync pattern */
    NWP_IF_WRITE_CHECK(g_pCB->FD, (_u8 *)&g_H2NSyncPattern.Long, 2*SYNC_PATTERN_LEN);
#else
    /*  Write short sync pattern */
    NWP_IF_WRITE_CHECK(g_pCB->FD, (_u8 *)&g_H2NSyncPattern.Short, SYNC_PATTERN_LEN);
#endif

    /*  Header */
    NWP_IF_WRITE_CHECK(g_pCB->FD, (_u8 *)&g_pCB->TempProtocolHeader, _SL_CMD_HDR_SIZE);

    /*  Descriptors */
    if (g_pCB->FunctionParams.pTxRxDescBuff && g_pCB->FunctionParams.pCmdCtrl->TxDescLen > 0)
    {
        NWP_IF_WRITE_CHECK(g_pCB->FD, g_pCB->FunctionParams.pTxRxDescBuff, 
            _SL_PROTOCOL_ALIGN_SIZE(g_pCB->FunctionParams.pCmdCtrl->TxDescLen));
    }

    /*  A special mode where Rx payload and Rx length are used as Tx as well */
    /*  This mode requires no Rx payload on the response and currently used by fs_Close and sl_Send on */
    /*  transceiver mode */
    if (g_pCB->RelayFlagsViaRxPayload == TRUE )
    {
        g_pCB->RelayFlagsViaRxPayload = FALSE;
        NWP_IF_WRITE_CHECK(g_pCB->FD, g_pCB->FunctionParams.pCmdExt->pRxPayload, 
            _SL_PROTOCOL_ALIGN_SIZE(g_pCB->FunctionParams.pCmdExt->RxPayloadLen));
    }

    /*  Payload */
    if (g_pCB->FunctionParams.pCmdExt && g_pCB->FunctionParams.pCmdExt->TxPayloadLen > 0)
    {
        /*  If the message has payload, it is mandatory that the message's arguments are protocol aligned. */
        /*  Otherwise the aligning of arguments will create a gap between arguments and payload. */
        VERIFY_PROTOCOL(_SL_IS_PROTOCOL_ALIGNED_SIZE(g_pCB->FunctionParams.pCmdCtrl->TxDescLen));

        NWP_IF_WRITE_CHECK(g_pCB->FD, g_pCB->FunctionParams.pCmdExt->pTxPayload, 
            _SL_PROTOCOL_ALIGN_SIZE(g_pCB->FunctionParams.pCmdExt->TxPayloadLen));
    }


    _SL_DBG_CNT_INC(MsgCnt.Write);

#ifdef SL_START_WRITE_STAT
    sl_IfEndWriteSequence(g_pCB->FD);
#endif

    return SL_OS_RET_CODE_OK;
}

/* ******************************************************************************/
/*  _SlDrvMsgRead  */
/* ******************************************************************************/
_SlReturnVal_t _SlDrvMsgRead(void)
{
    /*  alignment for small memory models */
    union
    {      
        _u8                TempBuf[_SL_RESP_HDR_SIZE];
        _u32               DummyBuf[2];
    } uBuf;
    _u8                TailBuffer[4];
    _u16               LengthToCopy;
    _u16               AlignedLengthRecv;
    _u8                AlignSize;

    VERIFY_RET_OK(_SlDrvRxHdrRead((_u8*)(uBuf.TempBuf), &AlignSize));

    /* 'Init Compelete' message bears no valid FlowControl info */
    if(SL_OPCODE_DEVICE_INITCOMPLETE != OPCODE(uBuf.TempBuf))
    {
        g_pCB->FlowContCB.TxPoolCnt = ((_SlResponseHeader_t *)uBuf.TempBuf)->TxPoolCnt;
        g_pCB->SocketNonBlocking = ((_SlResponseHeader_t *)uBuf.TempBuf)->SocketNonBlocking;
        g_pCB->SocketTXFailure = ((_SlResponseHeader_t *)uBuf.TempBuf)->SocketTXFailure;

        if(g_pCB->FlowContCB.TxPoolCnt > FLOW_CONT_MIN)
        {
            OSI_RET_OK_CHECK(sl_SyncObjSignal(&g_pCB->FlowContCB.TxSyncObj));
        }
    }

    _SlDrvClassifyRxMsg(OPCODE(uBuf.TempBuf));

    switch(g_pCB->FunctionParams.AsyncExt.RxMsgClass)
    {
    case ASYNC_EVT_CLASS:

        VERIFY_PROTOCOL(NULL == g_pCB->FunctionParams.AsyncExt.pAsyncBuf);

#if (SL_MEMORY_MGMT == SL_MEMORY_MGMT_STATIC)
        g_pCB->FunctionParams.AsyncExt.pAsyncBuf = g_StatMem.AsyncRespBuf;
#else
        g_pCB->FunctionParams.AsyncExt.pAsyncBuf = sl_Malloc(SL_ASYNC_MAX_MSG_LEN);
#endif
        MALLOC_OK_CHECK(g_pCB->FunctionParams.AsyncExt.pAsyncBuf);

        sl_Memcpy(g_pCB->FunctionParams.AsyncExt.pAsyncBuf, uBuf.TempBuf, _SL_RESP_HDR_SIZE);

        /*  This is an Async message. Read the rest of it. */
        if (_SL_PROTOCOL_ALIGN_SIZE(RSP_PAYLOAD_LEN(uBuf.TempBuf)) <= SL_ASYNC_MAX_PAYLOAD_LEN)
        {
            AlignedLengthRecv = _SL_PROTOCOL_ALIGN_SIZE(RSP_PAYLOAD_LEN(uBuf.TempBuf));
        }
        else
        {
            AlignedLengthRecv = _SL_PROTOCOL_ALIGN_SIZE(SL_ASYNC_MAX_PAYLOAD_LEN);
        }

        if (RSP_PAYLOAD_LEN(uBuf.TempBuf) > 0)
        {
            NWP_IF_READ_CHECK(g_pCB->FD,
                g_pCB->FunctionParams.AsyncExt.pAsyncBuf + _SL_RESP_HDR_SIZE,
                AlignedLengthRecv);
        }
        /* In case ASYNC RX buffer length is smaller then the received data length, dump the rest */
        if ((_SL_PROTOCOL_ALIGN_SIZE(RSP_PAYLOAD_LEN(uBuf.TempBuf)) > SL_ASYNC_MAX_PAYLOAD_LEN))
        {
            AlignedLengthRecv = _SL_PROTOCOL_ALIGN_SIZE(RSP_PAYLOAD_LEN(uBuf.TempBuf)) - SL_ASYNC_MAX_PAYLOAD_LEN;
            while (AlignedLengthRecv > 0)
            {
                NWP_IF_READ_CHECK(g_pCB->FD,TailBuffer,4);
                AlignedLengthRecv = AlignedLengthRecv - 4;
            }
        }
        OSI_RET_OK_CHECK(sl_LockObjLock(&g_pCB->ProtectionLockObj, SL_OS_WAIT_FOREVER)); 

        if ((SL_OPCODE_SOCKET_ACCEPTASYNCRESPONSE == OPCODE(uBuf.TempBuf)) || (SL_OPCODE_SOCKET_ACCEPTASYNCRESPONSE_V6 == OPCODE(uBuf.TempBuf)) || (SL_OPCODE_SOCKET_CONNECTASYNCRESPONSE == OPCODE(uBuf.TempBuf)))
        {
            /* go over the active list if exist to find obj waiting for this Async event */
            _SlFindAndSetActiveObj(OPCODE(uBuf.TempBuf),(((_SocketResponse_t *)(g_pCB->FunctionParams.AsyncExt.pAsyncBuf + _SL_RESP_HDR_SIZE))->sd) & BSD_SOCKET_ID_MASK);
        }
        else
        {
            _SlFindAndSetActiveObj(OPCODE(uBuf.TempBuf),SL_MAX_SOCKETS);
        }
        OSI_RET_OK_CHECK(sl_LockObjUnlock(&g_pCB->ProtectionLockObj));

        break;

    case RECV_RESP_CLASS:
        {
            _u8   ExpArgSize; /*  Expected size of Recv/Recvfrom arguments */

            switch(OPCODE(uBuf.TempBuf))
            {
            case SL_OPCODE_SOCKET_RECVFROMASYNCRESPONSE:
                ExpArgSize = RECVFROM_IPV4_ARGS_SIZE;
                break;
            case SL_OPCODE_SOCKET_RECVFROMASYNCRESPONSE_V6:
                ExpArgSize = RECVFROM_IPV6_ARGS_SIZE;
                break;
            default:
                /* SL_OPCODE_SOCKET_RECVASYNCRESPONSE: */
                ExpArgSize = RECV_ARGS_SIZE;
            }              

            /*  Read first 4 bytes of Recv/Recvfrom response to get SocketId and actual  */
            /*  response data length */
            NWP_IF_READ_CHECK(g_pCB->FD, &uBuf.TempBuf[4], RECV_ARGS_SIZE);

            /*  Validate Socket ID and Received Length value.  */
            VERIFY_PROTOCOL((SD(&uBuf.TempBuf[4])& BSD_SOCKET_ID_MASK) < SL_MAX_SOCKETS);

            OSI_RET_OK_CHECK(sl_LockObjLock(&g_pCB->ProtectionLockObj, SL_OS_WAIT_FOREVER)); 

            /* go over the active list if exist to find obj waiting for this Async event */
            VERIFY_RET_OK(_SlFindAndSetActiveObj(OPCODE(uBuf.TempBuf),SD(&uBuf.TempBuf[4]) & BSD_SOCKET_ID_MASK));

            /*  Verify data is waited on this socket. The pArgs should have been set by _SlDrvDataReadOp(). */
            VERIFY_SOCKET_CB(NULL !=  ((_SlArgsData_t *)(g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].pData))->pArgs);    

            sl_Memcpy( ((_SlArgsData_t *)(g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].pRespArgs))->pArgs, &uBuf.TempBuf[4], RECV_ARGS_SIZE);

            if(ExpArgSize > RECV_ARGS_SIZE)
            {
                NWP_IF_READ_CHECK(g_pCB->FD,
                    ((_SlArgsData_t *)(g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].pRespArgs))->pArgs + RECV_ARGS_SIZE,
                    ExpArgSize - RECV_ARGS_SIZE);
            }

            /*  Here g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].pData contains requested(expected) Recv/Recvfrom DataSize. */
            /*  Overwrite requested DataSize with actual one. */
            /*  If error is received, this information will be read from arguments. */
            if(ACT_DATA_SIZE(&uBuf.TempBuf[4]) > 0)
            {       
                VERIFY_SOCKET_CB(NULL != ((_SlArgsData_t *)(g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].pRespArgs))->pData);

                /*  Read 4 bytes aligned from interface */
                /*  therefore check the requested length and read only  */
                /*  4 bytes aligned data. The rest unaligned (if any) will be read */
                /*  and copied to a TailBuffer  */
                LengthToCopy = ACT_DATA_SIZE(&uBuf.TempBuf[4]) & (3);
                AlignedLengthRecv = ACT_DATA_SIZE(&uBuf.TempBuf[4]) & (~3);
                if( AlignedLengthRecv >= 4)
                {
                    NWP_IF_READ_CHECK(g_pCB->FD,((_SlArgsData_t *)(g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].pRespArgs))->pData,AlignedLengthRecv );                      
                }
                /*  copy the unaligned part, if any */
                if( LengthToCopy > 0) 
                {
                    NWP_IF_READ_CHECK(g_pCB->FD,TailBuffer,4);
                    /*  copy TailBuffer unaligned part (1/2/3 bytes) */
                    sl_Memcpy(((_SlArgsData_t *)(g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].pRespArgs))->pData + AlignedLengthRecv,TailBuffer,LengthToCopy);                    
                }                  
            }
            OSI_RET_OK_CHECK(sl_SyncObjSignal(&(g_pCB->ObjPool[g_pCB->FunctionParams.AsyncExt.ActionIndex].SyncObj)));
            OSI_RET_OK_CHECK(sl_LockObjUnlock(&g_pCB->ProtectionLockObj)); 
        }
        break;

    case CMD_RESP_CLASS:

        /*  Some commands pass a maximum arguments size. */
        /*  In this case Driver will send extra dummy patterns to NWP if */
        /*  the response message is smaller than maximum. */
        /*  When RxDescLen is not exact, using RxPayloadLen is forbidden! */
        /*  If such case cannot be avoided - parse message here to detect */
        /*  arguments/payload border. */
        NWP_IF_READ_CHECK(g_pCB->FD,
            g_pCB->FunctionParams.pTxRxDescBuff,
            _SL_PROTOCOL_ALIGN_SIZE(g_pCB->FunctionParams.pCmdCtrl->RxDescLen));

        if((NULL != g_pCB->FunctionParams.pCmdExt) && (0 != g_pCB->FunctionParams.pCmdExt->RxPayloadLen))
        {
            /*  Actual size of command's response payload: <msg_payload_len> - <rsp_args_len> */
            _i16    ActDataSize = RSP_PAYLOAD_LEN(uBuf.TempBuf) - g_pCB->FunctionParams.pCmdCtrl->RxDescLen;

            g_pCB->FunctionParams.pCmdExt->ActualRxPayloadLen = ActDataSize;

            /* Check that the space prepared by user for the response data is sufficient. */
            if(ActDataSize <= 0)
            {
                g_pCB->FunctionParams.pCmdExt->RxPayloadLen = 0;
            }
            else
            {
                /* In case the user supplied Rx buffer length which is smaller then the received data length, copy according to user length */
                if (ActDataSize > g_pCB->FunctionParams.pCmdExt->RxPayloadLen)
                {
                    LengthToCopy = g_pCB->FunctionParams.pCmdExt->RxPayloadLen & (3);
                    AlignedLengthRecv = g_pCB->FunctionParams.pCmdExt->RxPayloadLen & (~3);
                }
                else
                {
                    LengthToCopy = ActDataSize & (3);
                    AlignedLengthRecv = ActDataSize & (~3);
                }
                /*  Read 4 bytes aligned from interface */
                /*  therefore check the requested length and read only  */
                /*  4 bytes aligned data. The rest unaligned (if any) will be read */
                /*  and copied to a TailBuffer  */

                if( AlignedLengthRecv >= 4)
                {
                    NWP_IF_READ_CHECK(g_pCB->FD,
                        g_pCB->FunctionParams.pCmdExt->pRxPayload,
                        AlignedLengthRecv );

                }
                /*  copy the unaligned part, if any */
                if( LengthToCopy > 0) 
                {
                    NWP_IF_READ_CHECK(g_pCB->FD,TailBuffer,4);
                    /*  copy TailBuffer unaligned part (1/2/3 bytes) */
                    sl_Memcpy(g_pCB->FunctionParams.pCmdExt->pRxPayload + AlignedLengthRecv,
                        TailBuffer,
                        LengthToCopy);
                    ActDataSize = ActDataSize-4;
                }
                /* In case the user supplied Rx buffer length which is smaller then the received data length, dump the rest */
                if (ActDataSize > g_pCB->FunctionParams.pCmdExt->RxPayloadLen)
                {
                    /* calculate the rest of the data size to dump */
                    AlignedLengthRecv = ActDataSize - (g_pCB->FunctionParams.pCmdExt->RxPayloadLen & (~3));
                    while( AlignedLengthRecv > 0)
                    {
                        NWP_IF_READ_CHECK(g_pCB->FD,TailBuffer, 4 );
                        AlignedLengthRecv = AlignedLengthRecv - 4;
                    }
                }
            }
        }
        break;

    default:
        /*  DUMMY_MSG_CLASS: Flow control message has no payload. */
        break;
    }

    if(AlignSize > 0)
    {
        NWP_IF_READ_CHECK(g_pCB->FD, uBuf.TempBuf, AlignSize);
    }

    _SL_DBG_CNT_INC(MsgCnt.Read);

    /*  Unmask Interrupt call */
    sl_IfUnMaskIntHdlr();

    return SL_OS_RET_CODE_OK;
}

/* ******************************************************************************/
/*  _SlAsyncEventGenericHandler */
/* ******************************************************************************/
void _SlAsyncEventGenericHandler(void)
{
    _SlResponseHeader_t      *pHdr       = (_SlResponseHeader_t *)g_pCB->FunctionParams.AsyncExt.pAsyncBuf;
    SlWlanEvent_t            wlanEvent;
    SlNetAppEvent_t          netAppEvent;
    SlSockEvent_t            sockAppEvent;

    if (NULL != g_pCB->FunctionParams.AsyncExt.AsyncEvtHandler)
    {
        switch(pHdr->GenHeader.Opcode)
        {
        case  SL_OPCODE_WLAN_P2P_DEV_FOUND:
            {
                slPeerInfoAsyncResponse_t* pResp = (slPeerInfoAsyncResponse_t*)_SL_RESP_ARGS_START(g_pCB->FunctionParams.AsyncExt.pAsyncBuf);
                wlanEvent.Event = SL_WLAN_P2P_DEV_FOUND_EVENT;
                sl_Memcpy(wlanEvent.EventData.P2PModeDevFound.mac,pResp->mac, 6);
                sl_Memcpy(wlanEvent.EventData.P2PModeDevFound.go_peer_device_name,pResp->go_peer_device_name,pResp->go_peer_device_name_len);
                wlanEvent.EventData.P2PModeDevFound.go_peer_device_name_len = pResp->go_peer_device_name_len;
                g_pCB->FunctionParams.AsyncExt.AsyncEvtHandler(&wlanEvent);
                break;
            }

        case SL_OPCODE_WLAN_P2P_NEG_REQ_RECEIVED:
            {
                slPeerInfoAsyncResponse_t* pResp = (slPeerInfoAsyncResponse_t*)_SL_RESP_ARGS_START(g_pCB->FunctionParams.AsyncExt.pAsyncBuf);

                wlanEvent.Event = SL_WLAN_P2P_NEG_REQ_RECEIVED_EVENT;
                sl_Memcpy(wlanEvent.EventData.P2PModeNegReqReceived.mac,pResp->mac, 6);
                sl_Memcpy(wlanEvent.EventData.P2PModeNegReqReceived.go_peer_device_name,pResp->go_peer_device_name,pResp->go_peer_device_name_len);
                wlanEvent.EventData.P2PModeNegReqReceived.go_peer_device_name_len = pResp->go_peer_device_name_len;
                wlanEvent.EventData.P2PModeNegReqReceived.wps_dev_password_id = pResp->wps_dev_password_id;
                g_pCB->FunctionParams.AsyncExt.AsyncEvtHandler(&wlanEvent);
                break;
            }
        case SL_OPCODE_WLAN_CONNECTION_FAILED:
            {
                slWlanConnFailureAsyncResponse_t    * pResp = (slWlanConnFailureAsyncResponse_t*)_SL_RESP_ARGS_START(g_pCB->FunctionParams.AsyncExt.pAsyncBuf);

                wlanEvent.Event = SL_WLAN_CONNECTION_FAILED_EVENT;
                wlanEvent.EventData.P2PModewlanConnectionFailure.status = pResp->status;

                g_pCB->FunctionParams.AsyncExt.AsyncEvtHandler(&wlanEvent);
                break;
            }

        case SL_OPCODE_WLAN_WLANASYNCCONNECTEDRESPONSE:
            {
                slWlanConnectAsyncResponse_t *pWlanResp = (slWlanConnectAsyncResponse_t*)_SL_RESP_ARGS_START(g_pCB->FunctionParams.AsyncExt.pAsyncBuf);
                sl_Memset(&wlanEvent.EventData.STAandP2PModeWlanConnected,0,sizeof(slWlanConnectAsyncResponse_t));
                wlanEvent.Event = SL_WLAN_CONNECT_EVENT;
                wlanEvent.EventData.STAandP2PModeWlanConnected.connection_type = pWlanResp->connection_type;
                sl_Memcpy(wlanEvent.EventData.STAandP2PModeWlanConnected.bssid, pWlanResp->bssid, 6);
                sl_Memcpy(wlanEvent.EventData.STAandP2PModeWlanConnected.go_peer_device_name,pWlanResp->go_peer_device_name,pWlanResp->go_peer_device_name_len);
                sl_Memcpy(wlanEvent.EventData.STAandP2PModeWlanConnected.ssid_name,       pWlanResp->ssid_name,          pWlanResp->ssid_len);
                wlanEvent.EventData.STAandP2PModeWlanConnected.ssid_len = pWlanResp->ssid_len;
                wlanEvent.EventData.STAandP2PModeWlanConnected.go_peer_device_name_len = pWlanResp->go_peer_device_name_len;

                g_pCB->FunctionParams.AsyncExt.AsyncEvtHandler(&wlanEvent);
                break;
            }
        case SL_OPCODE_WLAN_WLANASYNCDISCONNECTEDRESPONSE:
            {
                slWlanConnectAsyncResponse_t *pWlanResp = (slWlanConnectAsyncResponse_t*)_SL_RESP_ARGS_START(g_pCB->FunctionParams.AsyncExt.pAsyncBuf);
                sl_Memset(&wlanEvent.EventData.STAandP2PModeDisconnected,0,sizeof(slWlanConnectAsyncResponse_t));
                wlanEvent.Event = SL_WLAN_DISCONNECT_EVENT;
                wlanEvent.EventData.STAandP2PModeDisconnected.connection_type = pWlanResp->connection_type;
                sl_Memcpy(wlanEvent.EventData.STAandP2PModeDisconnected.bssid, pWlanResp->bssid, 6);
                sl_Memcpy(wlanEvent.EventData.STAandP2PModeDisconnected.go_peer_device_name,pWlanResp->go_peer_device_name,pWlanResp->go_peer_device_name_len);
                sl_Memcpy(wlanEvent.EventData.STAandP2PModeDisconnected.ssid_name,        pWlanResp->ssid_name,          pWlanResp->ssid_len);
                wlanEvent.EventData.STAandP2PModeDisconnected.ssid_len = pWlanResp->ssid_len;
                wlanEvent.EventData.STAandP2PModeDisconnected.reason_code = pWlanResp->reason_code;
                wlanEvent.EventData.STAandP2PModeDisconnected.go_peer_device_name_len = pWlanResp->go_peer_device_name_len;
                g_pCB->FunctionParams.AsyncExt.AsyncEvtHandler(&wlanEvent);
                break;             
            }
        case SL_OPCODE_NETAPP_IPACQUIRED:
            {
                SlIpV4AcquiredAsync_t *pIpV4 = (SlIpV4AcquiredAsync_t*)_SL_RESP_ARGS_START(g_pCB->FunctionParams.AsyncExt.pAsyncBuf);
                netAppEvent.Event = SL_NETAPP_IPV4_IPACQUIRED_EVENT;
                netAppEvent.EventData.ipAcquiredV4.ip = pIpV4->ip;
                netAppEvent.EventData.ipAcquiredV4.gateway = pIpV4->gateway;
                netAppEvent.EventData.ipAcquiredV4.dns = pIpV4->dns;
                g_pCB->FunctionParams.AsyncExt.AsyncEvtHandler(&netAppEvent);
            }
            break;
        case SL_OPCODE_NETAPP_IPACQUIRED_V6:
            {
                SlIpV6AcquiredAsync_t *pIpV6 = (SlIpV6AcquiredAsync_t*)_SL_RESP_ARGS_START(g_pCB->FunctionParams.AsyncExt.pAsyncBuf);
                netAppEvent.Event =  SL_NETAPP_IPV6_IPACQUIRED_EVENT;
                sl_Memcpy((void *)&netAppEvent.EventData.ipAcquiredV6.ip[0],(void *)&pIpV6->ip[0],sizeof(pIpV6->ip[0])*4);
                g_pCB->FunctionParams.AsyncExt.AsyncEvtHandler(&netAppEvent);
            }
            break;

        case SL_OPCODE_NETAPP_IP_LEASED:
            {
                SlIpLeasedAsync_t *pIpV4 = (SlIpLeasedAsync_t*)_SL_RESP_ARGS_START(g_pCB->FunctionParams.AsyncExt.pAsyncBuf);
                netAppEvent.Event = SL_NETAPP_IP_LEASED_EVENT;
                netAppEvent.EventData.ipLeased.ip_address = pIpV4->ip_address;
                netAppEvent.EventData.ipLeased.lease_time = pIpV4->lease_time;
                sl_Memcpy(netAppEvent.EventData.ipLeased.mac, pIpV4->mac, 6);

                g_pCB->FunctionParams.AsyncExt.AsyncEvtHandler(&netAppEvent);
            }
            break;

        case SL_OPCODE_NETAPP_IP_RELEASED:
            {
                SlIpReleasedAsync_t *pIpV4 = (SlIpReleasedAsync_t *)_SL_RESP_ARGS_START(g_pCB->FunctionParams.AsyncExt.pAsyncBuf);
                netAppEvent.Event = SL_NETAPP_IP_RELEASED_EVENT;
                netAppEvent.EventData.ipReleased.ip_address = pIpV4->ip_address;
                netAppEvent.EventData.ipReleased.reason = pIpV4->reason;
                sl_Memcpy(netAppEvent.EventData.ipReleased.mac, pIpV4->mac, 6);

                g_pCB->FunctionParams.AsyncExt.AsyncEvtHandler(&netAppEvent);
            }
            break;

        case SL_OPCODE_SOCKET_TXFAILEDASYNCRESPONSE:
            {
                SlSockEventData_t *txfailparams = (SlSockEventData_t*)_SL_RESP_ARGS_START(g_pCB->FunctionParams.AsyncExt.pAsyncBuf);            
                sockAppEvent.Event =  SL_SOCKET_TX_FAILED_EVENT;
                sl_Memcpy((void *)&sockAppEvent.EventData,(void *)txfailparams,sizeof(SlSockEventData_t)); 
                g_pCB->FunctionParams.AsyncExt.AsyncEvtHandler(&sockAppEvent);
            }
            break; 

        case SL_OPCODE_SOCKET_SOCKETASYNCEVENT:
            {
                SlSockEventData_t *socketAsyncEvent = (SlSockEventData_t*)_SL_RESP_ARGS_START(g_pCB->FunctionParams.AsyncExt.pAsyncBuf);            
                sockAppEvent.Event =  SL_SOCKET_ASYNC_EVENT;
                sockAppEvent.EventData.socketAsyncEvent.sd = socketAsyncEvent->socketAsyncEvent.sd;
                sockAppEvent.EventData.socketAsyncEvent.type = socketAsyncEvent->socketAsyncEvent.type; /* one of the possible types of socket */
                sockAppEvent.EventData.socketAsyncEvent.val = socketAsyncEvent->socketAsyncEvent.val;
                g_pCB->FunctionParams.AsyncExt.AsyncEvtHandler(&sockAppEvent);
            }
            break; 

        case SL_OPCODE_WLAN_SMART_CONFIG_START_ASYNC_RESPONSE:
            {
                slSmartConfigStartAsyncResponse_t *pResp = (slSmartConfigStartAsyncResponse_t*)_SL_RESP_ARGS_START(g_pCB->FunctionParams.AsyncExt.pAsyncBuf);

                wlanEvent.Event = SL_WLAN_SMART_CONFIG_COMPLETE_EVENT;
                wlanEvent.EventData.smartConfigStartResponse.status = pResp->status;
                wlanEvent.EventData.smartConfigStartResponse.ssid_len = pResp->ssid_len;
                wlanEvent.EventData.smartConfigStartResponse.private_token_len = pResp->private_token_len;

                sl_Memset(wlanEvent.EventData.smartConfigStartResponse.ssid, 0x00, sizeof(wlanEvent.EventData.smartConfigStartResponse.ssid));
                sl_Memcpy(wlanEvent.EventData.smartConfigStartResponse.ssid, pResp->ssid, pResp->ssid_len);
                /* if private data exist */
                if (pResp->private_token_len)
                {
                    sl_Memset(wlanEvent.EventData.smartConfigStartResponse.private_token, 0x00, sizeof(wlanEvent.EventData.smartConfigStartResponse.private_token));
                    sl_Memcpy(wlanEvent.EventData.smartConfigStartResponse.private_token, pResp->private_token, pResp->private_token_len);
                }

                g_pCB->FunctionParams.AsyncExt.AsyncEvtHandler(&wlanEvent);
            }
            break;

        case SL_OPCODE_WLAN_SMART_CONFIG_STOP_ASYNC_RESPONSE:
            {
                slSmartConfigStopAsyncResponse_t *pResp = (slSmartConfigStopAsyncResponse_t*)_SL_RESP_ARGS_START(g_pCB->FunctionParams.AsyncExt.pAsyncBuf);

                wlanEvent.Event = SL_WLAN_SMART_CONFIG_STOP_EVENT;
                wlanEvent.EventData.smartConfigStopResponse.status = pResp->status;

                g_pCB->FunctionParams.AsyncExt.AsyncEvtHandler(&wlanEvent);
            }
            break;

        case SL_OPCODE_WLAN_STA_CONNECTED:
            {
                slPeerInfoAsyncResponse_t* pResp = (slPeerInfoAsyncResponse_t*)_SL_RESP_ARGS_START(g_pCB->FunctionParams.AsyncExt.pAsyncBuf);
                sl_Memset(&wlanEvent.EventData.APModeStaConnected,0,sizeof(slPeerInfoAsyncResponse_t));
                wlanEvent.Event = SL_WLAN_STA_CONNECTED_EVENT;
                sl_Memcpy(wlanEvent.EventData.APModeStaConnected.mac,pResp->mac, 6);
                sl_Memcpy(wlanEvent.EventData.APModeStaConnected.go_peer_device_name,pResp->go_peer_device_name,pResp->go_peer_device_name_len);
                wlanEvent.EventData.APModeStaConnected.go_peer_device_name_len = pResp->go_peer_device_name_len;

                sl_Memcpy(wlanEvent.EventData.APModeStaConnected.own_ssid,pResp->own_ssid,pResp->own_ssid_len);
                wlanEvent.EventData.APModeStaConnected.own_ssid_len = pResp->own_ssid_len;
                g_pCB->FunctionParams.AsyncExt.AsyncEvtHandler(&wlanEvent);
            }
            break;

        case SL_OPCODE_WLAN_STA_DISCONNECTED:
            {
                slPeerInfoAsyncResponse_t* pResp = (slPeerInfoAsyncResponse_t*)_SL_RESP_ARGS_START(g_pCB->FunctionParams.AsyncExt.pAsyncBuf);
                sl_Memset(&wlanEvent.EventData.APModestaDisconnected,0,sizeof(slPeerInfoAsyncResponse_t));
                wlanEvent.Event = SL_WLAN_STA_DISCONNECTED_EVENT;
                sl_Memcpy(wlanEvent.EventData.APModestaDisconnected.mac,pResp->mac, 6);
                sl_Memcpy(wlanEvent.EventData.APModestaDisconnected.go_peer_device_name,pResp->go_peer_device_name,pResp->go_peer_device_name_len);
                wlanEvent.EventData.APModestaDisconnected.go_peer_device_name_len = pResp->go_peer_device_name_len;
                g_pCB->FunctionParams.AsyncExt.AsyncEvtHandler(&wlanEvent);
            }
            break;

        case SL_OPCODE_NETAPP_PINGREPORTREQUESTRESPONSE:
            {
                _sl_HandleAsync_PingResponse((void *)g_pCB->FunctionParams.AsyncExt.pAsyncBuf);
            }
            break;     


        default:
            g_pCB->FunctionParams.AsyncExt.AsyncEvtHandler(g_pCB->FunctionParams.AsyncExt.pAsyncBuf); 
            break;
        } 
    } 
}


/* ******************************************************************************/
/*  _SlDrvMsgReadCmdCtx  */
/* ******************************************************************************/
_SlReturnVal_t _SlDrvMsgReadCmdCtx(void)
{

    /*  after command response is received and isCmdRespWaited */
    /*  flag is set FALSE, it is necessary to read out all */
    /*  Async messages in Commands context, because ssiDma_IsrHandleSignalFromSlave */
    /*  could have dispatched some Async messages to g_NwpIf.CmdSyncObj */
    /*  after command response but before this response has been processed */
    /*  by spi_singleRead and isCmdRespWaited was set FALSE. */
    while (TRUE == g_pCB->IsCmdRespWaited)
    {
        if(_SL_PENDING_RX_MSG(g_pCB))
        {
            g_pCB->FunctionParams.AsyncExt.pAsyncBuf      = NULL;/*  buffer must be allocated by _SlDrvMsgRead */
            g_pCB->FunctionParams.AsyncExt.AsyncEvtHandler= NULL;
            g_pCB->FunctionParams.AsyncExt.RxMsgClass     = (_SlRxMsgClass_e)(-1);/* init to illegal value and verify it's overwritten with the valid one */

            VERIFY_RET_OK(_SlDrvMsgRead());
            g_pCB->RxDoneCnt++;

            if (CMD_RESP_CLASS == g_pCB->FunctionParams.AsyncExt.RxMsgClass)
            {
                g_pCB->IsCmdRespWaited = FALSE;

                /*  In case CmdResp has been read without  waiting on CmdSyncObj -  that */
                /*  Sync object. That to prevent old signal to be processed. */
                sl_SyncObjClear(&g_pCB->CmdSyncObj);
            }
            else if (ASYNC_EVT_CLASS == g_pCB->FunctionParams.AsyncExt.RxMsgClass)
            {
                /*  If Async event has been read in CmdResp context, check whether */
                /*  there is a handler for this event. If there is, spawn specific */
                /*  handler. Otherwise free the event's buffer. */
                /*  This way there will be no "dry shots" from CmdResp context to */
                /*  temporary context, i.e less waste of CPU and faster buffer */
                /*  release. */
                _SlAsyncEventGenericHandler();

#if (SL_MEMORY_MGMT == SL_MEMORY_MGMT_STATIC)
                g_pCB->FunctionParams.AsyncExt.pAsyncBuf = NULL;
#else
                sl_Free(g_pCB->FunctionParams.AsyncExt.pAsyncBuf);
#endif
            }
        }
        else
        {
            /* CmdSyncObj will be signaled by IRQ */
            OSI_RET_OK_CHECK(sl_SyncObjWait(&g_pCB->CmdSyncObj, SL_OS_WAIT_FOREVER));
        }
    }

    /*  If there are more pending Rx Msgs after CmdResp is received, */
    /*  that means that these are Async, Dummy or Read Data Msgs. */
    /*  Spawn _SlDrvMsgReadSpawnCtx to trigger reading these messages from */
    /*  Temporary context. */
    /* sl_Spawn is activated, using a different context */
    OSI_RET_OK_CHECK(sl_LockObjUnlock(&g_pCB->GlobalLockObj));
    if(_SL_PENDING_RX_MSG(g_pCB))
    {
        sl_Spawn((_SlSpawnEntryFunc_t)_SlDrvMsgReadSpawnCtx, NULL, 0);
    }

    return SL_OS_RET_CODE_OK;
}

/* ******************************************************************************/
/*  _SlDrvMsgReadSpawnCtx                                                       */
/* ******************************************************************************/
_SlReturnVal_t _SlDrvMsgReadSpawnCtx(void *pValue)
{
#ifdef SL_POLLING_MODE_USED
    _i16 retCode = OSI_OK;
    /*  for polling based systems */
    do
    {
        retCode = sl_LockObjLock(&g_pCB->GlobalLockObj, 0);
        if ( OSI_OK != retCode )
        {
            if (TRUE == g_pCB->IsCmdRespWaited)
            {
                OSI_RET_OK_CHECK( sl_SyncObjSignal(&g_pCB->CmdSyncObj) );
                return SL_RET_CODE_OK;
            }
        }

    }
    while (OSI_OK != retCode);

#else
    OSI_RET_OK_CHECK(sl_LockObjLock(&g_pCB->GlobalLockObj, SL_OS_WAIT_FOREVER) );
#endif

    g_pCB->FunctionParams.AsyncExt.pAsyncBuf      = NULL;/*  buffer must be allocated by _SlDrvMsgRead */
    g_pCB->FunctionParams.AsyncExt.AsyncEvtHandler= NULL;
    g_pCB->FunctionParams.AsyncExt.RxMsgClass     = CMD_RESP_CLASS;/* init to illegal value and verify it's overwritten with the valid one */

    /*  Messages might have been read by CmdResp context. Therefore after */
    /*  getting LockObj, check again where the Pending Rx Msg is still present. */
    if(FALSE == (_SL_PENDING_RX_MSG(g_pCB)))
    {
        OSI_RET_OK_CHECK(sl_LockObjUnlock(&g_pCB->GlobalLockObj));
        return SL_RET_CODE_OK;
    }

    VERIFY_RET_OK(_SlDrvMsgRead());

    g_pCB->RxDoneCnt++;

    switch(g_pCB->FunctionParams.AsyncExt.RxMsgClass)
    {
    case ASYNC_EVT_CLASS:
        /*  If got here and protected by LockObj a message is waiting  */
        /*  to be read */
        VERIFY_PROTOCOL(NULL != g_pCB->FunctionParams.AsyncExt.pAsyncBuf);

        _SlAsyncEventGenericHandler();

#if (SL_MEMORY_MGMT == SL_MEMORY_MGMT_STATIC)
        g_pCB->FunctionParams.AsyncExt.pAsyncBuf = NULL;
#else
        sl_Free(g_pCB->FunctionParams.AsyncExt.pAsyncBuf);
#endif
        break;
    case DUMMY_MSG_CLASS:
    case RECV_RESP_CLASS:
        /* These types are legal in this context. Do nothing */
        break;
    case CMD_RESP_CLASS:
        /* Command response is illegal in this context. */
        /* No 'break' here: Assert! */
    default:
        VERIFY_PROTOCOL(0);
    }

    OSI_RET_OK_CHECK(sl_LockObjUnlock(&g_pCB->GlobalLockObj));

    return(SL_RET_CODE_OK);
}

/* ******************************************************************************/
/*  _SlDrvClassifyRxMsg */
/* ******************************************************************************/
void _SlDrvClassifyRxMsg(
    _SlOpcode_t         Opcode)
{

    g_pCB->FunctionParams.AsyncExt.AsyncEvtHandler = NULL;

    /* Async event has received */
    if (0 == (SL_OPCODE_SYNC & Opcode))
    {
        if (SL_OPCODE_DEVICE_DEVICEASYNCDUMMY == Opcode)
        { 
            g_pCB->FunctionParams.AsyncExt.RxMsgClass = DUMMY_MSG_CLASS;
        }
        else if ( (SL_OPCODE_SOCKET_RECVASYNCRESPONSE == Opcode) || (SL_OPCODE_SOCKET_RECVFROMASYNCRESPONSE == Opcode) || (SL_OPCODE_SOCKET_RECVFROMASYNCRESPONSE_V6 == Opcode) ) 
        {
            g_pCB->FunctionParams.AsyncExt.RxMsgClass = RECV_RESP_CLASS;
        }
        else
        {
            g_pCB->FunctionParams.AsyncExt.RxMsgClass = ASYNC_EVT_CLASS;

            /* set silo handler */
            if (SL_OPCODE_SILO_DEVICE == (Opcode & SL_OPCODE_SILO_MASK))
            {
                g_pCB->FunctionParams.AsyncExt.AsyncEvtHandler = _SlDrvDeviceEventHandler;
            }
            else if (SL_OPCODE_SILO_WLAN == (Opcode & SL_OPCODE_SILO_MASK))
            {
#ifdef sl_WlanEvtHdlr
                g_pCB->FunctionParams.AsyncExt.AsyncEvtHandler = (_SlSpawnEntryFunc_t)sl_WlanEvtHdlr;
#endif
            }
            else if (SL_OPCODE_SILO_SOCKET == (Opcode & SL_OPCODE_SILO_MASK))
            {

#ifdef sl_SockEvtHdlr
                g_pCB->FunctionParams.AsyncExt.AsyncEvtHandler = (_SlSpawnEntryFunc_t)sl_SockEvtHdlr;
#endif
            }
            else if (SL_OPCODE_SILO_NETAPP == (Opcode & SL_OPCODE_SILO_MASK))
            {

                if ((SL_OPCODE_NETAPP_HTTPGETTOKENVALUE == Opcode) || (SL_OPCODE_NETAPP_HTTPPOSTTOKENVALUE == Opcode))
                {
                    g_pCB->FunctionParams.AsyncExt.AsyncEvtHandler = _SlDrvNetAppEventHandler;
                }
#ifdef sl_NetAppEvtHdlr
                else
                {
                    g_pCB->FunctionParams.AsyncExt.AsyncEvtHandler = (_SlSpawnEntryFunc_t)sl_NetAppEvtHdlr;
                }
#endif
            }
            /*          else if (SL_OPCODE_SILO_NVMEM == (Opcode & SL_OPCODE_SILO_MASK))   */
            /*          { */
            /*          } */
            /*          else if (SL_OPCODE_SILO_NETCFG == (Opcode & SL_OPCODE_SILO_MASK))   */
            /*          { */
            /*          } */
            else
            {
                SL_ERROR_TRACE1(MSG_311, "ASSERT: _SlDrvClassifyRxMsg : invalid opcode = 0x%x", Opcode);
            }
        }
    }
    else
    {
        /* These may be Command responses only */
        g_pCB->FunctionParams.AsyncExt.RxMsgClass = CMD_RESP_CLASS; 
    }    

}

/* ******************************************************************************/
/*  _SlDrvShiftDWord */
/* ******************************************************************************/
void _SlDrvShiftDWord(_u8 *pBuf)
{
    _u8       ShiftIdx;
    for(ShiftIdx = 0; ShiftIdx< 7; ShiftIdx++)
    {
        pBuf[ShiftIdx] = pBuf[ShiftIdx+1];
    }
    pBuf[7] = 0;
}

/* ******************************************************************************/
/*  _SlDrvRxHdrRead  */
/* ******************************************************************************/
_SlReturnVal_t   _SlDrvRxHdrRead(_u8 *pBuf, _u8 *pAlignSize)
{
     _u32       SyncCnt  = 0;

#ifndef SL_IF_TYPE_UART
    /*  1. Write CNYS pattern to NWP when working in SPI mode only  */
    NWP_IF_WRITE_CHECK(g_pCB->FD, (_u8 *)&g_H2NCnysPattern.Short, SYNC_PATTERN_LEN);
#endif

    /*  2. Read 4 bytes (protocol aligned) */
    NWP_IF_READ_CHECK(g_pCB->FD, &pBuf[0], 4);
    _SL_DBG_SYNC_LOG(SyncCnt,pBuf);

    /* Wait for SYNC_PATTERN_LEN from the device */
    while ( ! N2H_SYNC_PATTERN_MATCH(pBuf, g_pCB->TxSeqNum) )
    {
        /*  3. Debug limit of scan */
        VERIFY_PROTOCOL(SyncCnt < SL_SYNC_SCAN_THRESHOLD);

        /*  4. Read next 4 bytes to Low 4 bytes of buffer */
        if(0 == (SyncCnt % (_u32)SYNC_PATTERN_LEN))
        {
            NWP_IF_READ_CHECK(g_pCB->FD, &pBuf[4], 4);
            _SL_DBG_SYNC_LOG(SyncCnt,pBuf);
        }

        /*  5. Shift Buffer Up for checking if the sync is shifted */
        _SlDrvShiftDWord(pBuf);

        SyncCnt++;
    }

    /*  5. Sync pattern found. If needed, complete number of read bytes to multiple of 4 (protocol align) */
    SyncCnt %= SYNC_PATTERN_LEN;

    if(SyncCnt > 0)
    {
        *(_u32 *)&pBuf[0] = *(_u32 *)&pBuf[4];
        NWP_IF_READ_CHECK(g_pCB->FD, &pBuf[SYNC_PATTERN_LEN - SyncCnt], (_u16)SyncCnt);
    }
    else
    {
        NWP_IF_READ_CHECK(g_pCB->FD, &pBuf[0], 4);
    }

    /*  6. Scan for Double pattern. */
    while ( N2H_SYNC_PATTERN_MATCH(pBuf, g_pCB->TxSeqNum) )
    {
        _SL_DBG_CNT_INC(Work.DoubleSyncPattern);
        NWP_IF_READ_CHECK(g_pCB->FD, &pBuf[0], SYNC_PATTERN_LEN);
    }
    g_pCB->TxSeqNum++;

    /*  7. Here we've read Generic Header (4 bytes). Read the Resp Specific header (4 more bytes). */
    NWP_IF_READ_CHECK(g_pCB->FD, &pBuf[SYNC_PATTERN_LEN], _SL_RESP_SPEC_HDR_SIZE);

    /*  8. Here we've read the entire Resp Header. */
    /*     Return number bytes needed to be sent after read for NWP Rx 4-byte alignment (protocol alignment) */
    *pAlignSize = (_u8)((SyncCnt > 0) ? (SYNC_PATTERN_LEN - SyncCnt) : 0);

    return SL_RET_CODE_OK;
}

/* ***************************************************************************** */
/*  _SlDrvBasicCmd */
/* ***************************************************************************** */
typedef union
{
    _BasicResponse_t    Rsp;
}_SlBasicCmdMsg_u;

_i16 _SlDrvBasicCmd(_SlOpcode_t Opcode)
{
    _SlBasicCmdMsg_u       Msg = {{0, 0}};
    _SlCmdCtrl_t           CmdCtrl;

    CmdCtrl.Opcode = Opcode;
    CmdCtrl.TxDescLen = 0;
    CmdCtrl.RxDescLen = sizeof(_BasicResponse_t);


    VERIFY_RET_OK(_SlDrvCmdOp((_SlCmdCtrl_t *)&CmdCtrl, &Msg, NULL));

    return (_i16)Msg.Rsp.status;
}

/* ***************************************************************************** */
/*  _SlDrvWaitForPoolObj */
/* ***************************************************************************** */
_i16 _SlDrvWaitForPoolObj(_u32 ActionID, _u8 SocketID)
{
    _u8 CurrObjIndex = MAX_CONCURRENT_ACTIONS;

    OSI_RET_OK_CHECK(sl_LockObjLock(&g_pCB->ProtectionLockObj, SL_OS_WAIT_FOREVER));

    /* Get free object  */
    if (MAX_CONCURRENT_ACTIONS > g_pCB->FreePoolIdx)
    {
        /* save the current obj index */
        CurrObjIndex = g_pCB->FreePoolIdx;
        /* set the new free index */
        if (MAX_CONCURRENT_ACTIONS > g_pCB->ObjPool[CurrObjIndex].NextIndex)
        {
            g_pCB->FreePoolIdx = g_pCB->ObjPool[CurrObjIndex].NextIndex;
        }
        else
        {
            /* No further free actions available */
            g_pCB->FreePoolIdx = MAX_CONCURRENT_ACTIONS;
        }
    }
    else
    {
        OSI_RET_OK_CHECK(sl_LockObjUnlock(&g_pCB->ProtectionLockObj));
        return CurrObjIndex;
    }
    g_pCB->ObjPool[CurrObjIndex].ActionID = (_u8)ActionID;
    if (SL_MAX_SOCKETS > SocketID)
    {
        g_pCB->ObjPool[CurrObjIndex].AdditionalData = SocketID;
    }
    /*In case this action is socket related, SocketID bit will be on
    In case SocketID is set to SL_MAX_SOCKETS, the socket is not relevant to the action. In that case ActionID bit will be on */
    while ( ( (SL_MAX_SOCKETS > SocketID) && (g_pCB->ActiveActionsBitmap & (1<<SocketID)) ) || ( (g_pCB->ActiveActionsBitmap & (1<<ActionID)) && (SL_MAX_SOCKETS == SocketID) ) )
    {
        //action in progress - move to pending list 
        g_pCB->ObjPool[CurrObjIndex].NextIndex = g_pCB->PendingPoolIdx;
        g_pCB->PendingPoolIdx = CurrObjIndex;
        OSI_RET_OK_CHECK(sl_LockObjUnlock(&g_pCB->ProtectionLockObj));
        //wait for action to be free
        OSI_RET_OK_CHECK(sl_SyncObjWait(&g_pCB->ObjPool[CurrObjIndex].SyncObj, SL_OS_WAIT_FOREVER));
        //set params and move to active (remove from pending list at _SlDrvReleasePoolObj)
        OSI_RET_OK_CHECK(sl_LockObjLock(&g_pCB->ProtectionLockObj, SL_OS_WAIT_FOREVER));
    }
    /*mark as active. Set socket as active if action is on socket, otherwise mark action as active*/
    if (SL_MAX_SOCKETS > SocketID)
    {
        g_pCB->ActiveActionsBitmap |= (1<<SocketID);
    }
    else
    {
        g_pCB->ActiveActionsBitmap |= (1<<ActionID);
    }
    /* move to active list  */
    g_pCB->ObjPool[CurrObjIndex].NextIndex = g_pCB->ActivePoolIdx;
    g_pCB->ActivePoolIdx = CurrObjIndex;    
    /* unlock */
    OSI_RET_OK_CHECK(sl_LockObjUnlock(&g_pCB->ProtectionLockObj));
    return CurrObjIndex;
}

/* ******************************************************************************/
/*  _SlDrvReleasePoolObj */
/* ******************************************************************************/
void _SlDrvReleasePoolObj(_u8 ObjIdx)
{
    _u8 PendingIndex;

    OSI_RET_OK_CHECK(sl_LockObjLock(&g_pCB->ProtectionLockObj, SL_OS_WAIT_FOREVER));

    /* go over the pending list and release other pending action if needed */
    PendingIndex = g_pCB->PendingPoolIdx;
    while(MAX_CONCURRENT_ACTIONS > PendingIndex)
    {
        /* In case this action is socket related, SocketID is in use, otherwise will be set to SL_MAX_SOCKETS */
        if ( (g_pCB->ObjPool[PendingIndex].ActionID == g_pCB->ObjPool[ObjIdx].ActionID) && 
            ( (SL_MAX_SOCKETS == (g_pCB->ObjPool[PendingIndex].AdditionalData & BSD_SOCKET_ID_MASK)) || 
            ((SL_MAX_SOCKETS > (g_pCB->ObjPool[ObjIdx].AdditionalData & BSD_SOCKET_ID_MASK)) && ( (g_pCB->ObjPool[PendingIndex].AdditionalData & BSD_SOCKET_ID_MASK) == (g_pCB->ObjPool[ObjIdx].AdditionalData & BSD_SOCKET_ID_MASK) ))) )
        {
            /* remove from pending list */
            _SlRemoveFromList(&g_pCB->PendingPoolIdx, PendingIndex);
            OSI_RET_OK_CHECK(sl_SyncObjSignal(&(g_pCB->ObjPool[PendingIndex].SyncObj)));
            break;
        }
        PendingIndex = g_pCB->ObjPool[PendingIndex].NextIndex;
    }

    if (SL_MAX_SOCKETS > (g_pCB->ObjPool[ObjIdx].AdditionalData & BSD_SOCKET_ID_MASK))
    {
        /* unset socketID  */
        g_pCB->ActiveActionsBitmap &= ~(1<<(g_pCB->ObjPool[ObjIdx].AdditionalData & BSD_SOCKET_ID_MASK));
    }
    else
    {
        /* unset actionID  */
        g_pCB->ActiveActionsBitmap &= ~(1<<g_pCB->ObjPool[ObjIdx].ActionID);
    }   

    /* delete old data */
    g_pCB->ObjPool[ObjIdx].pRespArgs = NULL;
    g_pCB->ObjPool[ObjIdx].ActionID = 0;
    g_pCB->ObjPool[ObjIdx].AdditionalData = SL_MAX_SOCKETS;

    /* remove from active list */
    _SlRemoveFromList(&g_pCB->ActivePoolIdx, ObjIdx);
    /* move to free list */
    g_pCB->ObjPool[ObjIdx].NextIndex = g_pCB->FreePoolIdx;
    g_pCB->FreePoolIdx = ObjIdx;

    OSI_RET_OK_CHECK(sl_LockObjUnlock(&g_pCB->ProtectionLockObj));
}



/* ******************************************************************************/
/*  _SlDrvObjInit */
/* ******************************************************************************/
void _SlDrvObjInit(void)
{
    _u8 Idx;

    sl_Memset(&g_pCB->ObjPool[0],0,MAX_CONCURRENT_ACTIONS*sizeof(_SlPoolObj_t));
    /* place all Obj in the free list */
    g_pCB->FreePoolIdx = 0;
    for (Idx = 0 ; Idx < MAX_CONCURRENT_ACTIONS ; Idx++)
    {
        g_pCB->ObjPool[Idx].NextIndex = Idx + 1;
        g_pCB->ObjPool[Idx].AdditionalData = SL_MAX_SOCKETS;
    }

    g_pCB->ActivePoolIdx = MAX_CONCURRENT_ACTIONS;
    g_pCB->PendingPoolIdx = MAX_CONCURRENT_ACTIONS;

}

/* ******************************************************************************/
/*  _SlDrvObjDeInit  */
/* ******************************************************************************/
void _SlDrvObjDeInit(void)
{
    g_pCB->FreePoolIdx = 0;
    g_pCB->PendingPoolIdx = MAX_CONCURRENT_ACTIONS;
    g_pCB->ActivePoolIdx = MAX_CONCURRENT_ACTIONS;

}

/* ******************************************************************************/
/* _SlRemoveFromList  */
/* ******************************************************************************/
void _SlRemoveFromList(_u8 *ListIndex, _u8 ItemIndex)
{
    _u8 Idx;
    /* only one item in the list */
    if (MAX_CONCURRENT_ACTIONS == g_pCB->ObjPool[*ListIndex].NextIndex)
    {
        *ListIndex = MAX_CONCURRENT_ACTIONS;
    }
    /* need to remove the first item in the list and therefore update the global which holds this index */
    else if (*ListIndex == ItemIndex)
    {
        *ListIndex = g_pCB->ObjPool[ItemIndex].NextIndex;
    }
    else
    {
        Idx = *ListIndex;
        while(MAX_CONCURRENT_ACTIONS > Idx)
        {
            /* remove from list */
            if (g_pCB->ObjPool[Idx].NextIndex == ItemIndex)
            {
                g_pCB->ObjPool[Idx].NextIndex = g_pCB->ObjPool[ItemIndex].NextIndex;
                break;
            }
            Idx = g_pCB->ObjPool[Idx].NextIndex;        
        }
    }
}


/* ******************************************************************************/
/*  _SlFindAndSetActiveObj                                                     */
/* ******************************************************************************/
_SlReturnVal_t _SlFindAndSetActiveObj(_SlOpcode_t  Opcode, _u8 Sd)
{
    _u8 ActiveIndex;

    ActiveIndex = g_pCB->ActivePoolIdx;
    /* go over the active list if exist to find obj waiting for this Async event */
    while (MAX_CONCURRENT_ACTIONS > ActiveIndex)
    {
        /* unset the Ipv4\IPv6 bit in the opcode if family bit was set  */
        if (g_pCB->ObjPool[ActiveIndex].AdditionalData & SL_NETAPP_FAMILY_MASK)
        {
            Opcode &= ~SL_OPCODE_IPV6;
        }

        if ((g_pCB->ObjPool[ActiveIndex].ActionID == RECV_ID) && (Sd == g_pCB->ObjPool[ActiveIndex].AdditionalData) && 
            ( (SL_OPCODE_SOCKET_RECVASYNCRESPONSE == Opcode) || (SL_OPCODE_SOCKET_RECVFROMASYNCRESPONSE == Opcode) || (SL_OPCODE_SOCKET_RECVFROMASYNCRESPONSE_V6 == Opcode) ) )
        {
            g_pCB->FunctionParams.AsyncExt.ActionIndex = ActiveIndex;
            return SL_RET_CODE_OK;
        }
        /* In case this action is socket related, SocketID is in use, otherwise will be set to SL_MAX_SOCKETS */
        if ( (_SlActionLookupTable[ g_pCB->ObjPool[ActiveIndex].ActionID - MAX_SOCKET_ENUM_IDX].ActionAsyncOpcode == Opcode) && 
            ( ((Sd == (g_pCB->ObjPool[ActiveIndex].AdditionalData & BSD_SOCKET_ID_MASK) ) && (SL_MAX_SOCKETS > Sd)) || (SL_MAX_SOCKETS == (g_pCB->ObjPool[ActiveIndex].AdditionalData & BSD_SOCKET_ID_MASK)) ) )
        {
            /* set handler */
            g_pCB->FunctionParams.AsyncExt.AsyncEvtHandler = _SlActionLookupTable[ g_pCB->ObjPool[ActiveIndex].ActionID - MAX_SOCKET_ENUM_IDX].AsyncEventHandler;
            g_pCB->FunctionParams.AsyncExt.ActionIndex = ActiveIndex;
            return SL_RET_CODE_OK;
        }
        ActiveIndex = g_pCB->ObjPool[ActiveIndex].NextIndex;
    }

    return SL_RET_CODE_SELF_ERROR;
}
