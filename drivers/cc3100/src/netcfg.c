/*
* netcfg.c - CC31xx/CC32xx Host Driver Implementation
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

/*****************************************************************************/
/* sl_NetCfgSet */
/*****************************************************************************/
typedef union
{
    _NetCfgSetGet_t    Cmd;
    _BasicResponse_t   Rsp;
}_SlNetCfgMsgSet_u;

const _SlCmdCtrl_t _SlNetCfgSetCmdCtrl =
{
    SL_OPCODE_DEVICE_NETCFG_SET_COMMAND,
    sizeof(_NetCfgSetGet_t),
    sizeof(_BasicResponse_t)
};

#if _SL_INCLUDE_FUNC(sl_NetCfgSet)
_i32 sl_NetCfgSet(_u8 ConfigId ,_u8 ConfigOpt,_u8 ConfigLen, _u8 *pValues)
{
    _SlNetCfgMsgSet_u         Msg;
    _SlCmdExt_t               CmdExt;

    CmdExt.TxPayloadLen = (ConfigLen+3) & (~3);
    CmdExt.RxPayloadLen = 0;
    CmdExt.pTxPayload = (_u8 *)pValues;
    CmdExt.pRxPayload = NULL;


    Msg.Cmd.ConfigId    = ConfigId;
    Msg.Cmd.ConfigLen   = ConfigLen;
    Msg.Cmd.ConfigOpt   = ConfigOpt;

    VERIFY_RET_OK(_SlDrvCmdOp((_SlCmdCtrl_t *)&_SlNetCfgSetCmdCtrl, &Msg, &CmdExt));

    return (_i16)Msg.Rsp.status;
}
#endif


/*****************************************************************************/
/* sl_NetCfgGet */
/*****************************************************************************/
typedef union
{
    _NetCfgSetGet_t	    Cmd;
    _NetCfgSetGet_t	    Rsp;
}_SlNetCfgMsgGet_u;

const _SlCmdCtrl_t _SlNetCfgGetCmdCtrl =
{
    SL_OPCODE_DEVICE_NETCFG_GET_COMMAND,
    sizeof(_NetCfgSetGet_t),
    sizeof(_NetCfgSetGet_t)
};

#if _SL_INCLUDE_FUNC(sl_NetCfgGet)
_i32 sl_NetCfgGet(_u8 ConfigId, _u8 *pConfigOpt,_u8 *pConfigLen, _u8 *pValues)
{
    _SlNetCfgMsgGet_u         Msg;
    _SlCmdExt_t               CmdExt;

    if (*pConfigLen == 0)
    {
        return SL_EZEROLEN;
    }
    CmdExt.TxPayloadLen = 0;
    CmdExt.RxPayloadLen = *pConfigLen;
    CmdExt.pTxPayload = NULL;
    CmdExt.pRxPayload = (_u8 *)pValues;
    CmdExt.ActualRxPayloadLen = 0;
    Msg.Cmd.ConfigLen    = *pConfigLen;
    Msg.Cmd.ConfigId     = ConfigId;

    if( pConfigOpt )
    {
        Msg.Cmd.ConfigOpt   = (_u16)*pConfigOpt;
    }
    VERIFY_RET_OK(_SlDrvCmdOp((_SlCmdCtrl_t *)&_SlNetCfgGetCmdCtrl, &Msg, &CmdExt));

    if( pConfigOpt )
    {
        *pConfigOpt = (_u8)Msg.Rsp.ConfigOpt;
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
#endif

