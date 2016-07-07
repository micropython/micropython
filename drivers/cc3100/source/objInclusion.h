/*
 * objInclusion.h - CC31xx/CC32xx Host Driver Implementation
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

#include <simplelink.h>

#ifndef OBJINCLUSION_H_
#define OBJINCLUSION_H_

#ifdef	__cplusplus
extern "C" {
#endif

/******************************************************************************
 
 For future use
	
*******************************************************************************/

#define __inln        	/* if inline functions requiered: #define __inln inline */

#define SL_DEVICE      	/* Device silo is currently always mandatory */



/******************************************************************************

 Qualifiers for package customizations 
	
*******************************************************************************/

#if defined (SL_DEVICE)
#define __dev   1
#else
#define __dev   0
#endif

#if defined (SL_DEVICE) && defined (SL_INC_EXT_API)
#define __dev__ext   1
#else
#define __dev__ext   0
#endif


#if (!defined (SL_PLATFORM_MULTI_THREADED)) || (!defined (SL_PLATFORM_EXTERNAL_SPAWN))
#define __int__spwn     1
#else
#define __int__spwn     0
#endif

#if defined (SL_INC_NET_APP_PKG)
#define __nap    1
#else
#define __nap    0
#endif

#if defined  (SL_INC_NET_APP_PKG) && defined (SL_INC_SOCK_CLIENT_SIDE_API)
#define __nap__clt  1
#else
#define __nap__clt  0
#endif

#if defined  (SL_INC_NET_APP_PKG) && defined (SL_INC_EXT_API)
#define __nap__ext   1
#else
#define __nap__ext   0
#endif
  
#if defined (SL_INC_NET_CFG_PKG)
#define __ncg        1
#else
#define __ncg        0
#endif

#if defined (SL_INC_NET_CFG_PKG) && defined (SL_INC_EXT_API)
#define __ncg__ext        1
#else
#define __ncg__ext        0
#endif

#if defined (SL_INC_NVMEM_PKG)
#define __nvm       1
#else
#define __nvm       0
#endif

#if defined (SL_INC_SOCKET_PKG)
#define __sck        1
#else
#define __sck        0
#endif

#if defined  (SL_INC_SOCKET_PKG) && defined (SL_INC_EXT_API)
#define __sck__ext    1
#else
#define __sck__ext    0
#endif

#if defined  (SL_INC_SOCKET_PKG) && defined (SL_INC_SOCK_SERVER_SIDE_API)
#define __sck__srv     1
#else
#define __sck__srv     0
#endif

#if defined  (SL_INC_SOCKET_PKG) && defined (SL_INC_SOCK_CLIENT_SIDE_API)
#define __sck__clt      1
#else
#define __sck__clt      0
#endif

#if defined  (SL_INC_SOCKET_PKG) && defined (SL_INC_SOCK_RECV_API)
#define __sck__rcv     1
#else
#define __sck__rcv     0
#endif

#if defined  (SL_INC_SOCKET_PKG) && defined (SL_INC_SOCK_SEND_API)
#define __sck__snd      1
#else
#define __sck__snd      0
#endif

#if defined (SL_INC_WLAN_PKG)
#define __wln           1
#else
#define __wln           0
#endif

#if defined  (SL_INC_WLAN_PKG) && defined (SL_INC_EXT_API)
#define __wln__ext      1
#else
#define __wln__ext      0
#endif

/* The return 1 is the function need to be included in the output */
#define _SL_INCLUDE_FUNC(Name)		(_SL_INC_##Name)

/* Driver */
#define _SL_INC_sl_NetAppStart          __nap__ext
#define _SL_INC_sl_NetAppStop           __nap__ext

#define _SL_INC_sl_NetAppDnsGetHostByName   __nap__clt


#define _SL_INC_sl_NetAppDnsGetHostByService			__nap__ext
#define _SL_INC_sl_NetAppMDNSRegisterService		    __nap__ext
#define _SL_INC_sl_NetAppMDNSUnRegisterService		    __nap__ext
#define _SL_INC_sl_NetAppMDNSRegisterUnregisterService	__nap__ext
#define _SL_INC_sl_NetAppGetServiceList	                __nap__ext
	

#define _SL_INC_sl_DnsGetHostByAddr     __nap__ext
#define _SL_INC_sl_NetAppPingStart      __nap__ext
#define _SL_INC_sl_NetAppPingReport     __nap__ext
#define _SL_INC_sl_NetAppSet            __nap__ext
#define _SL_INC_sl_NetAppGet            __nap__ext


/* FS */
#define _SL_INC_sl_FsOpen            __nvm

#define _SL_INC_sl_FsClose           __nvm

#define _SL_INC_sl_FsRead            __nvm

#define _SL_INC_sl_FsWrite           __nvm

#define _SL_INC_sl_FsGetInfo         __nvm

#define _SL_INC_sl_FsDel             __nvm

/* netcfg */
#define _SL_INC_sl_MacAdrrSet           __ncg

#define _SL_INC_sl_MacAdrrGet           __ncg

#define _SL_INC_sl_NetCfgGet          __ncg

#define _SL_INC_sl_NetCfgSet          __ncg


/* socket */
#define _SL_INC_sl_Socket               __sck

#define _SL_INC_sl_Close                __sck

#define _SL_INC_sl_Accept               __sck__srv

#define _SL_INC_sl_Bind                 __sck

#define _SL_INC_sl_Listen               __sck__srv

#define _SL_INC_sl_Connect              __sck__clt

#define _SL_INC_sl_Select               __sck

#define _SL_INC_sl_SetSockOpt           __sck

#define _SL_INC_sl_GetSockOpt           __sck__ext

#define _SL_INC_sl_Recv                 __sck__rcv

#define _SL_INC_sl_RecvFrom             __sck__rcv

#define _SL_INC_sl_Write                __sck__snd

#define _SL_INC_sl_Send                 __sck__snd

#define _SL_INC_sl_SendTo               __sck__snd

#define _SL_INC_sl_Htonl                __sck

#define _SL_INC_sl_Htons                __sck
  
/* wlan */
#define _SL_INC_sl_WlanConnect          __wln__ext

#define _SL_INC_sl_WlanDisconnect           __wln__ext

#define _SL_INC_sl_WlanProfileAdd           __wln__ext

#define _SL_INC_sl_WlanProfileGet           __wln__ext

#define _SL_INC_sl_WlanProfileDel           __wln__ext

#define _SL_INC_sl_WlanPolicySet            __wln__ext

#define _SL_INC_sl_WlanPolicyGet            __wln__ext

#define _SL_INC_sl_WlanGetNetworkList       __wln__ext

#define _SL_INC_sl_WlanRxFilterAdd      __wln__ext

#define _SL_INC_sl_WlanRxFilterSet   __wln__ext

#define _SL_INC_sl_WlanRxFilterGet   __wln__ext

#define _SL_INC_sl_SmartConfigStart     __wln

#define _SL_INC_sl_SmartConfigOptSet    __wln__ext


#define _SL_INC_sl_WlanSmartConfigStart  __wln

#define _SL_INC_sl_WlanSmartConfigStop   __wln

#define _SL_INC_sl_WlanSetMode			 __wln

#define _SL_INC_sl_WlanSet			 __wln

#define _SL_INC_sl_WlanGet			 __wln

#define _SL_INC_sl_SmartConfigOptSet    __wln__ext

#define _SL_INC_sl_SmartConfigOptGet    __wln__ext

#define  _SL_INC_sl_WlanRxStatStart      __wln__ext

#define _SL_INC_sl_WlanRxStatStop       __wln__ext

#define _SL_INC_sl_WlanRxStatGet        __wln__ext


/* device */
#define _SL_INC_sl_Task                 __int__spwn

#define _SL_INC_sl_Start                __dev

#define _SL_INC_sl_Stop                 __dev

#define _SL_INC_sl_StatusGet            __dev

#ifdef SL_IF_TYPE_UART  
#define _SL_INC_sl_UartSetMode	        __dev__ext
#endif

#define _SL_INC_sl_EventMaskGet         __dev__ext

#define _SL_INC_sl_EventMaskSet         __dev__ext

#define _SL_INC_sl_DevGet			    __dev__ext

#define _SL_INC_sl_DevSet			    __dev__ext


#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif /*OBJINCLUSION_H_  */
