/*
 * trace.h - CC31xx/CC32xx Host Driver Implementation
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
 


#include "simplelink.h"

#ifndef __SIMPLELINK_TRACE_H__
#define __SIMPLELINK_TRACE_H__


#ifdef  __cplusplus
extern "C" {
#endif

/*****************************************************************************/
/* Macro declarations                                                        */
/*****************************************************************************/

#define SL_SYNC_SCAN_THRESHOLD  (( _u32 )2000)

#ifdef SL_TINY_EXT
#define _SlDrvAssert(line )  { while(1); }  
#else
#define _SlDrvAssert( )  { _SlDriverHandleError(SL_DEVICE_DRIVER_ASSERT_ERROR_EVENT, 0, 0); }
#endif

#define _SL_ASSERT(expr)            { if(!(expr)){ \
	_SlDrvAssert(); } \
}
#define _SL_ERROR(expr, error)      { if(!(expr)){return (error); } }

#define SL_HANDLING_ASSERT          2
#define SL_HANDLING_ERROR           1
#define SL_HANDLING_NONE            0


#ifndef SL_TINY_EXT

#if 1
#define SL_SELF_COND_HANDLING       SL_HANDLING_ERROR
#define SL_PROTOCOL_HANDLING        SL_HANDLING_ERROR
#define SL_DRV_RET_CODE_HANDLING    SL_HANDLING_ERROR
#define SL_NWP_IF_HANDLING          SL_HANDLING_ERROR
#define SL_OSI_RET_OK_HANDLING      SL_HANDLING_ERROR
#define SL_MALLOC_OK_HANDLING       SL_HANDLING_ERROR
#define SL_USER_ARGS_HANDLING       SL_HANDLING_ERROR
#define SL_ERR_IN_PROGRESS_HANDLING SL_HANDLING_ERROR
#endif

#else
#define SL_SELF_COND_HANDLING       SL_HANDLING_NONE
#define SL_PROTOCOL_HANDLING        SL_HANDLING_NONE
#define SL_DRV_RET_CODE_HANDLING    SL_HANDLING_NONE
#define SL_NWP_IF_HANDLING          SL_HANDLING_NONE
#define SL_OSI_RET_OK_HANDLING      SL_HANDLING_NONE
#define SL_MALLOC_OK_HANDLING       SL_HANDLING_NONE
#define SL_USER_ARGS_HANDLING       SL_HANDLING_NONE
#define SL_ERR_IN_PROGRESS_HANDLING SL_HANDLING_NONE
#endif


#if (SL_ERR_IN_PROGRESS_HANDLING == SL_HANDLING_ERROR)
#define VERIFY_NO_ERROR_HANDLING_IN_PROGRESS() { \
	    if ( g_bDeviceRestartIsRequired == (_u8)TRUE) return SL_API_ABORTED; }
#else
#define VERIFY_NO_ERROR_HANDLING_IN_PROGRESS()
#endif
#if (SL_DRV_RET_CODE_HANDLING == SL_HANDLING_ASSERT)
#define VERIFY_RET_OK(Func)                     {_SlReturnVal_t _RetVal = (Func); _SL_ASSERT((_SlReturnVal_t)SL_OS_RET_CODE_OK == _RetVal)}
#elif (SL_DRV_RET_CODE_HANDLING == SL_HANDLING_ERROR)
#define VERIFY_RET_OK(Func)                     {_SlReturnVal_t _RetVal = (Func); if (SL_OS_RET_CODE_OK != _RetVal) return  _RetVal;}
#else
#define VERIFY_RET_OK(Func)                     (Func);
#endif

#if (SL_PROTOCOL_HANDLING == SL_HANDLING_ASSERT)
#define VERIFY_PROTOCOL(expr)                   _SL_ASSERT(expr)
#elif (SL_PROTOCOL_HANDLING == SL_HANDLING_ERROR)
#define VERIFY_PROTOCOL(expr)                   _SL_ERROR(expr, SL_RET_CODE_PROTOCOL_ERROR)
#else
#define VERIFY_PROTOCOL(expr)
#endif

#if (defined(PROTECT_SOCKET_ASYNC_RESP) && (SL_SELF_COND_HANDLING == SL_HANDLING_ASSERT))
#define VERIFY_SOCKET_CB(expr)                  _SL_ASSERT(expr)
#elif (defined(PROTECT_SOCKET_ASYNC_RESP) && (SL_SELF_COND_HANDLING == SL_HANDLING_ERROR))
#define VERIFY_SOCKET_CB(expr)                  _SL_ERROR(expr, SL_RET_CODE_SELF_ERROR)
#else
#define VERIFY_SOCKET_CB(expr)
#endif

#if (SL_NWP_IF_HANDLING == SL_HANDLING_ASSERT)
#define NWP_IF_WRITE_CHECK(fd,pBuff,len)       { _i16 RetSize, ExpSize = (_i16)(len); RetSize = sl_IfWrite((fd),(pBuff),ExpSize); _SL_ASSERT(ExpSize == RetSize)}
#define NWP_IF_READ_CHECK(fd,pBuff,len)        { _i16 RetSize, ExpSize = (_i16)(len); RetSize = sl_IfRead((fd),(pBuff),ExpSize);  _SL_ASSERT(ExpSize == RetSize)}
#elif (SL_NWP_IF_HANDLING == SL_HANDLING_ERROR)
#define NWP_IF_WRITE_CHECK(fd,pBuff,len)       { _SL_ERROR((len == sl_IfWrite((fd),(pBuff),(len))), SL_RET_CODE_NWP_IF_ERROR);}
#define NWP_IF_READ_CHECK(fd,pBuff,len)        { _SL_ERROR((len == sl_IfRead((fd),(pBuff),(len))),  SL_RET_CODE_NWP_IF_ERROR);}
#else
#define NWP_IF_WRITE_CHECK(fd,pBuff,len)       { sl_IfWrite((fd),(pBuff),(len));}
#define NWP_IF_READ_CHECK(fd,pBuff,len)        { sl_IfRead((fd),(pBuff),(len));}
#endif

#if (SL_OSI_RET_OK_HANDLING == SL_HANDLING_ASSERT)
#define OSI_RET_OK_CHECK(Func)                  {_SlReturnVal_t _RetVal = (Func); _SL_ASSERT((_SlReturnVal_t)SL_OS_RET_CODE_OK == _RetVal)}
#elif (SL_OSI_RET_OK_HANDLING == SL_HANDLING_ERROR)
#define OSI_RET_OK_CHECK(Func)                  {_SlReturnVal_t _RetVal = (Func); if (SL_OS_RET_CODE_OK != _RetVal) return  _RetVal;}
#else
#define OSI_RET_OK_CHECK(Func)                  (Func);
#endif

#if (SL_MALLOC_OK_HANDLING == SL_HANDLING_ASSERT)
#define MALLOC_OK_CHECK(Ptr)                    _SL_ASSERT(NULL != Ptr)
#elif (SL_MALLOC_OK_HANDLING == SL_HANDLING_ERROR)
#define MALLOC_OK_CHECK(Ptr)                    _SL_ERROR((NULL != Ptr), SL_RET_CODE_MALLOC_ERROR)
#else
#define MALLOC_OK_CHECK(Ptr)
#endif

#ifdef SL_INC_ARG_CHECK

#if (SL_USER_ARGS_HANDLING == SL_HANDLING_ASSERT)
#define ARG_CHECK_PTR(Ptr)                      _SL_ASSERT(NULL != Ptr)
#elif (SL_USER_ARGS_HANDLING == SL_HANDLING_ERROR)
#define ARG_CHECK_PTR(Ptr)                      _SL_ERROR((NULL != Ptr), SL_RET_CODE_INVALID_INPUT)
#else
#define ARG_CHECK_PTR(Ptr)
#endif

#else
#define ARG_CHECK_PTR(Ptr)
#endif

/*#define SL_DBG_TRACE_ENABLE*/
#ifdef SL_DBG_TRACE_ENABLE
#define SL_TRACE0(level,msg_id,str)                     printf(str)
#define SL_TRACE1(level,msg_id,str,p1)                  printf(str,(p1))
#define SL_TRACE2(level,msg_id,str,p1,p2)               printf(str,(p1),(p2))
#define SL_TRACE3(level,msg_id,str,p1,p2,p3)            printf(str,(p1),(p2),(p3))
#define SL_TRACE4(level,msg_id,str,p1,p2,p3,p4)         printf(str,(p1),(p2),(p3),(p4))
#define SL_ERROR_TRACE(msg_id,str)                      printf(str)
#define SL_ERROR_TRACE1(msg_id,str,p1)                  printf(str,(p1))
#define SL_ERROR_TRACE2(msg_id,str,p1,p2)               printf(str,(p1),(p2))
#define SL_ERROR_TRACE3(msg_id,str,p1,p2,p3)            printf(str,(p1),(p2),(p3))
#define SL_ERROR_TRACE4(msg_id,str,p1,p2,p3,p4)         printf(str,(p1),(p2),(p3),(p4))
#define SL_TRACE_FLUSH()
#else
#define SL_TRACE0(level,msg_id,str)
#define SL_TRACE1(level,msg_id,str,p1)
#define SL_TRACE2(level,msg_id,str,p1,p2)
#define SL_TRACE3(level,msg_id,str,p1,p2,p3)
#define SL_TRACE4(level,msg_id,str,p1,p2,p3,p4)
#define SL_ERROR_TRACE(msg_id,str)
#define SL_ERROR_TRACE1(msg_id,str,p1)
#define SL_ERROR_TRACE2(msg_id,str,p1,p2)
#define SL_ERROR_TRACE3(msg_id,str,p1,p2,p3)
#define SL_ERROR_TRACE4(msg_id,str,p1,p2,p3,p4)
#define SL_TRACE_FLUSH()
#endif

/* #define SL_DBG_CNT_ENABLE */
#ifdef SL_DBG_CNT_ENABLE
#define _SL_DBG_CNT_INC(Cnt)            g_DbgCnt. ## Cnt++
#define _SL_DBG_SYNC_LOG(index,value)   {if(index < SL_DBG_SYNC_LOG_SIZE){*(_u32 *)&g_DbgCnt.SyncLog[index] = *(_u32 *)(value);}}

#else
#define _SL_DBG_CNT_INC(Cnt)
#define _SL_DBG_SYNC_LOG(index,value)
#endif

#define SL_DBG_LEVEL_1                  1
#define SL_DBG_LEVEL_2                  2
#define SL_DBG_LEVEL_3                  4
#define SL_DBG_LEVEL_MASK               (SL_DBG_LEVEL_2|SL_DBG_LEVEL_3)

#define SL_INCLUDE_DBG_FUNC(Name)		((Name ## _DBG_LEVEL) & SL_DBG_LEVEL_MASK)

#define _SlDrvPrintStat_DBG_LEVEL       SL_DBG_LEVEL_3
#define _SlDrvOtherFunc_DBG_LEVEL       SL_DBG_LEVEL_1

#ifdef  __cplusplus
}
#endif


#endif /*__SIMPLELINK_TRACE_H__*/

