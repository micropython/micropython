/*
 * simplelink.h - CC31xx/CC32xx Host Driver Implementation
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


/*!
    \mainpage SimpleLink Driver

    \section intro_sec Introduction

 The SimpleLink CC31xx/CC2xx family allows to add Wi-Fi and networking capabilities
 to low-cost embedded products without having prior Wi-Fi, RF or networking expertise.
 The CC31xx/CC32xx is an ideal solution for microcontroller-based sensor and control
 applications such as home appliances, home automation and smart metering.
 The CC31xx/CC32xx has integrated a comprehensive TCP/IP network stack, Wi-Fi driver and
 security supplicant leading to easier portability to microcontrollers, to an
 ultra-low memory footprint, all without compromising the capabilities and robustness
 of the final application.



 \section modules_sec Module Names
 To make it simple, TI's SimpleLink CC31xx/CC32xx platform capabilities were divided into modules by topic (Silo). 
 These capabilities range from basic device management through wireless
 network configuration, standard BSD socket and much more.
 Listed below are the various modules in the SimpleLink CC31xx/CC32xx driver:
     -# \ref device - controls the behaviour of the CC31xx/CC32xx device (start/stop, events masking and obtaining specific device status)
     -# \ref wlan - controls the use of the WiFi WLAN module including:
       - Connection features, such as: profiles, policies, SmartConfig�
       - Advanced WLAN features, such as: scans, rx filters and rx statistics collection
     -# \ref socket - controls standard client/server sockets programming options and capabilities 
     -# \ref netapp - activates networking applications, such as: HTTP Server, DHCP Server, Ping, DNS and mDNS.
     -# \ref netcfg - controls the configuration of the device addresses (i.e. IP and MAC addresses)
     -# \ref FileSystem     - provides file system capabilities to TI's CC31XX that can be used by both the CC31XX device and the user.


 \section         proting_sec     Porting Guide

 The porting of the SimpleLink driver to any new platform is based on few simple steps.
 This guide takes you through this process step by step. Please follow the instructions
 carefully to avoid any problems during this process and to enable efficient and proper
 work with the device.
 Please notice that all modifications and porting adjustments of the driver should be
 made in the user.h header file only.
 Keep making any of the changes only in this file will ensure smoothly transaction to
 new versions of the driver at the future!


 \subsection     porting_step1   Step 1 - Create your own user.h file

 The first step is to create a user.h file that will include your configurations and
 adjustments. You can use the empty template provided as part of this driver or
 you can choose to base your file on file from one of the wide range of examples
 applications provided by Texas Instruments


 \subsection    porting_step2   Step 2 - Select the capabilities set required for your application

 Texas Instruments made a lot of efforts to build set of predefined capability sets that would
 fit most of the target application.
 It is recommended to try and choose one of this predefined capabilities set before going to
 build your own customized set. If you find compatible set you can skip the rest of this step.

 The available sets are:
     -# SL_TINY     -   Compatible to be used on platforms with very limited resources. Provides
                        the best in class foot print in terms of Code and Data consumption.
     -# SL_SMALL    -   Compatible to most common networking applications. Provide the most
                        common APIs with decent balance between code size, data size, functionality
                        and performances
     -# SL_FULL     -   Provide access to all SimpleLink functionalities


 \subsection    porting_step3   Step 3 - Bind the device enable/disable output line

 The enable/disable line (nHib) provide mechanism to enter the device into the least current
 consumption mode. This mode could be used when no traffic is required (tx/rx).
 when this line is not connected to any IO of the host this define should be left empty.
 Not connecting this line results in ability to start the driver only once.


 \subsection    porting_step4   Step 4 - Writing your interface communication driver

 The SimpleLink device support several standard communication protocol among SPI and
 UART. Depending on your needs and your hardware design, you should choose the
 communication channel type.
 The interface for this communication channel should include 4 simple access functions:
 -# open
 -# close
 -# read
 -# write

 The way this driver would be implemented is directly effecting the efficiency and
 the performances of the SimpleLink device on this platform.
 If your system has DMA you should consider to use it in order to increase the utilization
 of the communication channel
 If you have enough memory resources you should consider using a buffer to increase the
 efficiency of the write operations.


 \subsection     porting_step5   Step 5 - Choose your memory management model

 The SimpleLink driver support two memory models:
     -# Static (default)
     -# Dynamic

 If you choose to work in dynamic model you will have to provide alloc and free functions
 to be used by the Simple Link driver otherwise nothing need to be done.


 \subsection     porting_step6   Step 6 - OS adaptation

 The SimpleLink driver could run on two kind of platforms:
     -# Non-Os / Single Threaded (default)
     -# Multi-Threaded

 If you choose to work in multi-threaded environment under operating system you will have to
 provide some basic adaptation routines to allow the driver to protect access to resources
 for different threads (locking object) and to allow synchronization between threads (sync objects).
 In additional the driver support running without dedicated thread allocated solely to the simple
 link driver. If you choose to work in this mode, you should also supply a spawn method that
 will enable to run function on a temporary context.


 \subsection     porting_step7   Step 7 - Set your asynchronous event handlers routines

 The SimpleLink device generate asynchronous events in several situations.
 These asynchronous events could be masked.
 In order to catch these events you have to provide handler routines.
 Please notice that if you not provide a handler routine and the event is received,
 the driver will drop this event without any indication of this drop.


 \subsection     porting_step8   Step 8 - Run diagnostic tools to validate the correctness of your porting

 The driver is delivered with some porting diagnostic tools to simplify the porting validation process
 and to reduce issues latter. It is very important to follow carefully this process.

 The diagnostic process include:
     -# Validating Interface Communication Driver
     -# Validating OS adaptation layer
     -# Validating HW integrity
     -# Validating basic work with the device


    \section sw_license License

 *
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
    
    

#ifndef __SIMPLELINK_H__
#define    __SIMPLELINK_H__

#include "user.h"

#ifdef    __cplusplus
extern "C"
{
#endif


/*! \attention  Async event activation notes
    Function prototypes for event callback handlers                               
    Event handler function names should be defined in the user.h file             
    e.g.                                                                          
    "#define sl_WlanEvtHdlr   SLWlanEventHandler"                               
    Indicates all WLAN events are handled by User func "SLWlanEventHandler"                                                                       
    Important notes:                                                              
    1. Event handlers cannot activate another SimpleLink API from the event's context                                                                    
    2. Event's data is valid during event's context. Any application data         
       which is required for the user application should be copied or marked      
       into user's variables                                                      
    3. It is not recommended to delay the execution of the event callback handler 

*/

/*!

    \addtogroup UserEvents
    @{

*/


/*****************************************************************************/
/* Macro declarations for Host Driver version                                */
/*****************************************************************************/
#define SL_DRIVER_VERSION   "1.0.0.10"
#define SL_MAJOR_VERSION_NUM    1L
#define SL_MINOR_VERSION_NUM    0L
#define SL_VERSION_NUM          0L
#define SL_SUB_VERSION_NUM      10L


/*****************************************************************************/
/* Macro declarations for predefined configurations                          */
/*****************************************************************************/

#ifdef SL_TINY
#undef SL_INC_ARG_CHECK
#undef SL_INC_EXT_API
#undef SL_INC_SOCK_SERVER_SIDE_API
#undef SL_INC_WLAN_PKG
#undef SL_INC_NET_CFG_PKG
#undef SL_INC_FS_PKG
#undef SL_INC_SET_UART_MODE
#undef SL_INC_STD_BSD_API_NAMING
#undef SL_INC_SOCK_CLIENT_SIDE_API
#undef SL_INC_NET_APP_PKG
#undef SL_INC_SOCK_RECV_API
#undef SL_INC_SOCK_SEND_API
#undef SL_INC_SOCKET_PKG
#endif

#ifdef SL_SMALL
#undef SL_INC_EXT_API
#undef SL_INC_NET_APP_PKG
#undef SL_INC_NET_CFG_PKG
#undef SL_INC_FS_PKG
#define SL_INC_ARG_CHECK
#define SL_INC_WLAN_PKG
#define SL_INC_SOCKET_PKG
#define SL_INC_SOCK_CLIENT_SIDE_API
#define SL_INC_SOCK_SERVER_SIDE_API
#define SL_INC_SOCK_RECV_API
#define SL_INC_SOCK_SEND_API
#define SL_INC_SET_UART_MODE
#endif

#ifdef SL_FULL
#define SL_INC_EXT_API
#define SL_INC_NET_APP_PKG
#define SL_INC_NET_CFG_PKG
#define SL_INC_FS_PKG
#define SL_INC_ARG_CHECK
#define SL_INC_WLAN_PKG
#define SL_INC_SOCKET_PKG
#define SL_INC_SOCK_CLIENT_SIDE_API
#define SL_INC_SOCK_SERVER_SIDE_API
#define SL_INC_SOCK_RECV_API
#define SL_INC_SOCK_SEND_API
#define SL_INC_SET_UART_MODE
#endif

#define SL_RET_CODE_OK                          (0)
#define SL_RET_CODE_INVALID_INPUT               (-2)
#define SL_RET_CODE_SELF_ERROR                  (-3)
#define SL_RET_CODE_NWP_IF_ERROR                (-4)
#define SL_RET_CODE_MALLOC_ERROR                (-5)

#define sl_Memcpy       memcpy
#define sl_Memset       memset

#define sl_SyncObjClear(pObj)     sl_SyncObjWait(pObj,SL_OS_NO_WAIT)

#ifndef SL_TINY_EXT
#define SL_MAX_SOCKETS      (8)
#else
#define SL_MAX_SOCKETS      (2)
#endif


/*****************************************************************************/
/* Types definitions                                                          */
/*****************************************************************************/
typedef void (*_SlSpawnEntryFunc_t)(void* pValue);

#ifndef NULL
#define NULL        (0)
#endif

#ifndef FALSE
#define FALSE       (0)
#endif

#ifndef TRUE
#define TRUE        (!FALSE)
#endif

#ifndef OK
#define OK          (0)
#endif

#ifndef _SL_USER_TYPES
      typedef unsigned char _u8;
      typedef signed char   _i8;
 
      typedef unsigned short _u16;
      typedef signed short   _i16;
 
      typedef unsigned long  _u32;
      typedef signed long    _i32;
      #define _volatile volatile
      #define _const    const
#endif

typedef _u16  _SlOpcode_t;
typedef _u8   _SlArgSize_t;
typedef _i16   _SlDataSize_t;
typedef _i16   _SlReturnVal_t;

#ifdef    __cplusplus
}
#endif /*  __cplusplus */



/*
 * This event status used to  block or continue the event propagation
 * through all the registered external libs/user application
 *
 */

 typedef enum {
 	EVENT_PROPAGATION_BLOCK = 0,
 	EVENT_PROPAGATION_CONTINUE

 } _SlEventPropogationStatus_e;






/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/

#ifdef SL_PLATFORM_MULTI_THREADED
    #include "spawn.h"
#else
    #include "nonos.h"
#endif


/* 
   objInclusion.h and user.h must be included before all api header files 
   objInclusion.h must be the last arrangement just before including the API header files 
   since it based on the other configurations to decide which object should be included 
*/
#include "objInclusion.h"
#include "trace.h"
#include "fs.h"
#include "socket.h"
#include "netapp.h"
#include "wlan.h"
#include "device.h"
#include "netcfg.h"
#include "wlan_rx_filters.h"


 /* The general events dispatcher which is
  * initialized to the user handler */
#ifdef sl_GeneralEvtHdlr
#define _SlDrvHandleGeneralEvents sl_GeneralEvtHdlr
#endif

 /* The wlan events dispatcher which is
  * initialized to the user handler */
#ifdef sl_WlanEvtHdlr
#define _SlDrvHandleWlanEvents sl_WlanEvtHdlr
#endif

 /* The NetApp events dispatcher which is
  * initialized to the user handler */
#ifdef sl_NetAppEvtHdlr
#define _SlDrvHandleNetAppEvents sl_NetAppEvtHdlr
#endif

 /* The http server events dispatcher which is
  * initialized to the user handler if exists */
#ifdef sl_HttpServerCallback
#define _SlDrvHandleHttpServerEvents sl_HttpServerCallback
#endif

 /* The socket events dispatcher which is
  * initialized to the user handler */
#ifdef sl_SockEvtHdlr
#define _SlDrvHandleSockEvents sl_SockEvtHdlr
#endif


#ifndef __CONCAT
#define __CONCAT(x,y)	x ## y
#endif
#define __CONCAT2(x,y)	__CONCAT(x,y)


/*
 * The section below handles the external lib event registration
 * according to the desired events it specified in its API header file.
 * The external lib should be first installed by the user (see user.h)
 */
#ifdef SL_EXT_LIB_1

    /* General Event Registration */
	#if __CONCAT2(SL_EXT_LIB_1, _NOTIFY_GENERAL_EVENT)
	extern _SlEventPropogationStatus_e __CONCAT2(SL_EXT_LIB_1, _GeneralEventHdl) (SlDeviceEvent_t *);
	#define SlExtLib1GeneralEventHandler   __CONCAT2(SL_EXT_LIB_1, _GeneralEventHdl)

	#undef EXT_LIB_REGISTERED_GENERAL_EVENTS
    #define EXT_LIB_REGISTERED_GENERAL_EVENTS
	#endif

	/* Wlan Event Registration */
	#if __CONCAT2(SL_EXT_LIB_1, _NOTIFY_WLAN_EVENT)
	extern _SlEventPropogationStatus_e __CONCAT2(SL_EXT_LIB_1, _WlanEventHdl) (SlWlanEvent_t *);
	#define SlExtLib1WlanEventHandler   __CONCAT2(SL_EXT_LIB_1, _WlanEventHdl)

	#undef EXT_LIB_REGISTERED_WLAN_EVENTS
    #define EXT_LIB_REGISTERED_WLAN_EVENTS
	#endif

	/* NetApp Event Registration */
	#if __CONCAT2(SL_EXT_LIB_1, _NOTIFY_NETAPP_EVENT)
	extern _SlEventPropogationStatus_e __CONCAT2(SL_EXT_LIB_1, _NetAppEventHdl) (SlNetAppEvent_t *);
	#define SlExtLib1NetAppEventHandler __CONCAT2(SL_EXT_LIB_1, _NetAppEventHdl)

	#undef EXT_LIB_REGISTERED_NETAPP_EVENTS
    #define EXT_LIB_REGISTERED_NETAPP_EVENTS
	#endif

	/* Http Server Event Registration */
	#if __CONCAT2(SL_EXT_LIB_1, _NOTIFY_HTTP_SERVER_EVENT)
	extern _SlEventPropogationStatus_e __CONCAT2(SL_EXT_LIB_1, _HttpServerEventHdl) (SlHttpServerEvent_t* , SlHttpServerResponse_t*);
	#define SlExtLib1HttpServerEventHandler __CONCAT2(SL_EXT_LIB_1, _HttpServerEventHdl)

	#undef EXT_LIB_REGISTERED_HTTP_SERVER_EVENTS
    #define EXT_LIB_REGISTERED_HTTP_SERVER_EVENTS
	#endif

	/* Socket Event Registration */
	#if __CONCAT2(SL_EXT_LIB_1, _NOTIFY_SOCK_EVENT)
	extern _SlEventPropogationStatus_e __CONCAT2(SL_EXT_LIB_1, _SockEventHdl) (SlSockEvent_t *);
	#define SlExtLib1SockEventHandler __CONCAT2(SL_EXT_LIB_1, _SockEventHdl)

	#undef EXT_LIB_REGISTERED_SOCK_EVENTS
    #define EXT_LIB_REGISTERED_SOCK_EVENTS
	#endif

#endif


#ifdef SL_EXT_LIB_2

    /* General Event Registration */
	#if __CONCAT2(SL_EXT_LIB_2, _NOTIFY_GENERAL_EVENT)
	extern _SlEventPropogationStatus_e __CONCAT2(SL_EXT_LIB_2, _GeneralEventHdl) (SlDeviceEvent_t *);
	#define SlExtLib2GeneralEventHandler   __CONCAT2(SL_EXT_LIB_2, _GeneralEventHdl)

	#undef EXT_LIB_REGISTERED_GENERAL_EVENTS
    #define EXT_LIB_REGISTERED_GENERAL_EVENTS
	#endif

	/* Wlan Event Registration */
	#if __CONCAT2(SL_EXT_LIB_2, _NOTIFY_WLAN_EVENT)
	extern _SlEventPropogationStatus_e __CONCAT2(SL_EXT_LIB_2, _WlanEventHdl) (SlWlanEvent_t *);
	#define SlExtLib2WlanEventHandler   __CONCAT2(SL_EXT_LIB_2, _WlanEventHdl)

	#undef EXT_LIB_REGISTERED_WLAN_EVENTS
    #define EXT_LIB_REGISTERED_WLAN_EVENTS
	#endif

	/* NetApp Event Registration */
	#if __CONCAT2(SL_EXT_LIB_2, _NOTIFY_NETAPP_EVENT)
	extern _SlEventPropogationStatus_e __CONCAT2(SL_EXT_LIB_2, _NetAppEventHdl) (SlNetAppEvent_t *);
	#define SlExtLib2NetAppEventHandler __CONCAT2(SL_EXT_LIB_2, _NetAppEventHdl)

	#undef EXT_LIB_REGISTERED_NETAPP_EVENTS
    #define EXT_LIB_REGISTERED_NETAPP_EVENTS
	#endif

	/* Http Server Event Registration */
	#if __CONCAT2(SL_EXT_LIB_2, _NOTIFY_HTTP_SERVER_EVENT)
	extern _SlEventPropogationStatus_e __CONCAT2(SL_EXT_LIB_2, _HttpServerEventHdl) (SlHttpServerEvent_t* , SlHttpServerResponse_t*);
	#define SlExtLib2HttpServerEventHandler __CONCAT2(SL_EXT_LIB_2, _HttpServerEventHdl)

	#undef EXT_LIB_REGISTERED_HTTP_SERVER_EVENTS
    #define EXT_LIB_REGISTERED_HTTP_SERVER_EVENTS
	#endif

	/* Socket Event Registration */
	#if __CONCAT2(SL_EXT_LIB_2, _NOTIFY_SOCK_EVENT)
	extern _SlEventPropogationStatus_e __CONCAT2(SL_EXT_LIB_2, _SockEventHdl) (SlSockEvent_t *);
	#define SlExtLib2SockEventHandler __CONCAT2(SL_EXT_LIB_2, _SockEventHdl)

	#undef EXT_LIB_REGISTERED_SOCK_EVENTS
    #define EXT_LIB_REGISTERED_SOCK_EVENTS
	#endif

#endif


#ifdef SL_EXT_LIB_3

    /* General Event Registration */
	#if __CONCAT2(SL_EXT_LIB_3, _NOTIFY_GENERAL_EVENT)
	extern _SlEventPropogationStatus_e __CONCAT2(SL_EXT_LIB_3, _GeneralEventHdl) (SlDeviceEvent_t *);
	#define SlExtLib3GeneralEventHandler   __CONCAT2(SL_EXT_LIB_3, _GeneralEventHdl)

	#undef EXT_LIB_REGISTERED_GENERAL_EVENTS
    #define EXT_LIB_REGISTERED_GENERAL_EVENTS
	#endif

	/* Wlan Event Registration */
	#if __CONCAT2(SL_EXT_LIB_3, _NOTIFY_WLAN_EVENT)
	extern _SlEventPropogationStatus_e __CONCAT2(SL_EXT_LIB_3, _WlanEventHdl) (SlWlanEvent_t *);
	#define SlExtLib3WlanEventHandler   __CONCAT2(SL_EXT_LIB_3, _WlanEventHdl)

	#undef EXT_LIB_REGISTERED_WLAN_EVENTS
    #define EXT_LIB_REGISTERED_WLAN_EVENTS
	#endif

	/* NetApp Event Registration */
	#if __CONCAT2(SL_EXT_LIB_3, _NOTIFY_NETAPP_EVENT)
	extern _SlEventPropogationStatus_e __CONCAT2(SL_EXT_LIB_3, _NetAppEventHdl) (SlNetAppEvent_t *);
	#define SlExtLib3NetAppEventHandler __CONCAT2(SL_EXT_LIB_3, _NetAppEventHdl)

	#undef EXT_LIB_REGISTERED_NETAPP_EVENTS
    #define EXT_LIB_REGISTERED_NETAPP_EVENTS
	#endif

	/* Http Server Event Registration */
	#if __CONCAT2(SL_EXT_LIB_3, _NOTIFY_HTTP_SERVER_EVENT)
	extern _SlEventPropogationStatus_e __CONCAT2(SL_EXT_LIB_3, _HttpServerEventHdl) (SlHttpServerEvent_t* , SlHttpServerResponse_t*);
	#define SlExtLib3HttpServerEventHandler __CONCAT2(SL_EXT_LIB_3, _HttpServerEventHdl)

	#undef EXT_LIB_REGISTERED_HTTP_SERVER_EVENTS
    #define EXT_LIB_REGISTERED_HTTP_SERVER_EVENTS
	#endif

	/* Socket Event Registration */
	#if __CONCAT2(SL_EXT_LIB_3, _NOTIFY_SOCK_EVENT)
	extern _SlEventPropogationStatus_e __CONCAT2(SL_EXT_LIB_3, _SockEventHdl) (SlSockEvent_t *);
	#define SlExtLib3SockEventHandler __CONCAT2(SL_EXT_LIB_3, _SockEventHdl)

	#undef EXT_LIB_REGISTERED_SOCK_EVENTS
    #define EXT_LIB_REGISTERED_SOCK_EVENTS
	#endif

#endif


#ifdef SL_EXT_LIB_4

    /* General Event Registration */
	#if __CONCAT2(SL_EXT_LIB_4, _NOTIFY_GENERAL_EVENT)
	extern _SlEventPropogationStatus_e __CONCAT2(SL_EXT_LIB_4, _GeneralEventHdl) (SlDeviceEvent_t *);
	#define SlExtLib4GeneralEventHandler   __CONCAT2(SL_EXT_LIB_4, _GeneralEventHdl)

	#undef EXT_LIB_REGISTERED_GENERAL_EVENTS
    #define EXT_LIB_REGISTERED_GENERAL_EVENTS
	#endif

	/* Wlan Event Registration */
	#if __CONCAT2(SL_EXT_LIB_4, _NOTIFY_WLAN_EVENT)
	extern _SlEventPropogationStatus_e __CONCAT2(SL_EXT_LIB_4, _WlanEventHdl) (SlWlanEvent_t *);
	#define SlExtLib4WlanEventHandler   __CONCAT2(SL_EXT_LIB_4, _WlanEventHdl)

	#undef EXT_LIB_REGISTERED_WLAN_EVENTS
    #define EXT_LIB_REGISTERED_WLAN_EVENTS
	#endif

	/* NetApp Event Registration */
	#if __CONCAT2(SL_EXT_LIB_4, _NOTIFY_NETAPP_EVENT)
	extern _SlEventPropogationStatus_e __CONCAT2(SL_EXT_LIB_4, _NetAppEventHdl) (SlNetAppEvent_t *);
	#define SlExtLib4NetAppEventHandler __CONCAT2(SL_EXT_LIB_4, _NetAppEventHdl)

	#undef EXT_LIB_REGISTERED_NETAPP_EVENTS
    #define EXT_LIB_REGISTERED_NETAPP_EVENTS
	#endif

	/* Http Server Event Registration */
	#if __CONCAT2(SL_EXT_LIB_4, _NOTIFY_HTTP_SERVER_EVENT)
	extern _SlEventPropogationStatus_e __CONCAT2(SL_EXT_LIB_4, _HttpServerEventHdl) (SlHttpServerEvent_t* , SlHttpServerResponse_t*);
	#define SlExtLib4HttpServerEventHandler __CONCAT2(SL_EXT_LIB_4, _HttpServerEventHdl)

	#undef EXT_LIB_REGISTERED_HTTP_SERVER_EVENTS
    #define EXT_LIB_REGISTERED_HTTP_SERVER_EVENTS
	#endif

	/* Socket Event Registration */
	#if __CONCAT2(SL_EXT_LIB_4, _NOTIFY_SOCK_EVENT)
	extern _SlEventPropogationStatus_e __CONCAT2(SL_EXT_LIB_4, _SockEventHdl) (SlSockEvent_t *);
	#define SlExtLib4SockEventHandler __CONCAT2(SL_EXT_LIB_4, _SockEventHdl)

	#undef EXT_LIB_REGISTERED_SOCK_EVENTS
    #define EXT_LIB_REGISTERED_SOCK_EVENTS
	#endif

#endif


#ifdef SL_EXT_LIB_5

    /* General Event Registration */
	#if __CONCAT2(SL_EXT_LIB_5, _NOTIFY_GENERAL_EVENT)
	extern _SlEventPropogationStatus_e __CONCAT2(SL_EXT_LIB_5, _GeneralEventHdl) (SlDeviceEvent_t *);
	#define SlExtLib5GeneralEventHandler   __CONCAT2(SL_EXT_LIB_5, _GeneralEventHdl)

	#undef EXT_LIB_REGISTERED_GENERAL_EVENTS
    #define EXT_LIB_REGISTERED_GENERAL_EVENTS
	#endif

	/* Wlan Event Registration */
	#if __CONCAT2(SL_EXT_LIB_5, _NOTIFY_WLAN_EVENT)
	extern _SlEventPropogationStatus_e __CONCAT2(SL_EXT_LIB_5, _WlanEventHdl) (SlWlanEvent_t *);
	#define SlExtLib5WlanEventHandler   __CONCAT2(SL_EXT_LIB_5, _WlanEventHdl)

	#undef EXT_LIB_REGISTERED_WLAN_EVENTS
    #define EXT_LIB_REGISTERED_WLAN_EVENTS
	#endif

	/* NetApp Event Registration */
	#if __CONCAT2(SL_EXT_LIB_5, _NOTIFY_NETAPP_EVENT)
	extern _SlEventPropogationStatus_e __CONCAT2(SL_EXT_LIB_5, _NetAppEventHdl) (SlNetAppEvent_t *);
	#define SlExtLib5NetAppEventHandler __CONCAT2(SL_EXT_LIB_5, _NetAppEventHdl)

	#undef EXT_LIB_REGISTERED_NETAPP_EVENTS
    #define EXT_LIB_REGISTERED_NETAPP_EVENTS
	#endif

	/* Http Server Event Registration */
	#if __CONCAT2(SL_EXT_LIB_5, _NOTIFY_HTTP_SERVER_EVENT)
	extern _SlEventPropogationStatus_e __CONCAT2(SL_EXT_LIB_5, _HttpServerEventHdl) (SlHttpServerEvent_t* , SlHttpServerResponse_t*);
	#define SlExtLib5HttpServerEventHandler __CONCAT2(SL_EXT_LIB_5, _HttpServerEventHdl)

	#undef EXT_LIB_REGISTERED_HTTP_SERVER_EVENTS
    #define EXT_LIB_REGISTERED_HTTP_SERVER_EVENTS
	#endif

	/* Socket Event Registration */
	#if __CONCAT2(SL_EXT_LIB_5, _NOTIFY_SOCK_EVENT)
	extern _SlEventPropogationStatus_e __CONCAT2(SL_EXT_LIB_5, _SockEventHdl) (SlSockEvent_t *);
	#define SlExtLib5SockEventHandler __CONCAT2(SL_EXT_LIB_5, _SockEventHdl)

	#undef EXT_LIB_REGISTERED_SOCK_EVENTS
    #define EXT_LIB_REGISTERED_SOCK_EVENTS
	#endif

#endif



#if defined(EXT_LIB_REGISTERED_GENERAL_EVENTS)
extern void _SlDrvHandleGeneralEvents(SlDeviceEvent_t *slGeneralEvent);
#endif

#if defined(EXT_LIB_REGISTERED_WLAN_EVENTS)
extern void _SlDrvHandleWlanEvents(SlWlanEvent_t *slWlanEvent);
#endif

#if defined (EXT_LIB_REGISTERED_NETAPP_EVENTS)
extern void _SlDrvHandleNetAppEvents(SlNetAppEvent_t *slNetAppEvent);
#endif

#if defined(EXT_LIB_REGISTERED_HTTP_SERVER_EVENTS)
extern void _SlDrvHandleHttpServerEvents(SlHttpServerEvent_t *slHttpServerEvent, SlHttpServerResponse_t *slHttpServerResponse);
#endif


#if defined(EXT_LIB_REGISTERED_SOCK_EVENTS)
extern void _SlDrvHandleSockEvents(SlSockEvent_t *slSockEvent);
#endif


typedef void (*_SlSpawnEntryFunc_t)(void* pValue);


/* Async functions description*/

/*!
    \brief General async event for inspecting general events
    
    \param[out]      pSlDeviceEvent   pointer to SlDeviceEvent_t 
    
    \par
          Parameters: \n
          - <b>pSlDeviceEvent->Event = SL_DEVICE_FATAL_ERROR_EVENT </b>
              - pSlDeviceEvent->EventData.deviceEvent fields:      
                  - status: An error code indication from the device
                  - sender: The sender originator which is based on SlErrorSender_e enum 

        - <b>pSlDeviceEvent->Event = SL_DEVICE_ABORT_ERROR_EVENT </b>
              Indicates a severe error occured and the device stopped 
              - pSlDeviceEvent->EventData.deviceReport fields:      
                  - AbortType: An idication of the event type
                  - AbortData: Additional info about the data error 
                  
      
    \par  Example for fatal error:           
    \code         
    printf(General Event Handler - ID=%d Sender=%d\n\n",
           pSlDeviceEvent->EventData.deviceEvent.status,  // status of the general event
           pSlDeviceEvent->EventData.deviceEvent.sender); // sender type
    \endcode
    \par Example for abort request:
    \code
     printf(Abort type =%d Abort Data=%d\n\n",
           pSlDeviceEvent->EventData.deviceReport.AbortType,  
           pSlDeviceEvent->EventData.deviceReport.AbortData); 

    \endcode
*/
#if (defined(sl_GeneralEvtHdlr))
extern void sl_GeneralEvtHdlr(SlDeviceEvent_t *pSlDeviceEvent);
#endif


/*!
    \brief WLAN Async event handler
    
    \param[out]      pSlWlanEvent   pointer to SlWlanEvent_t data 
    
    \par
             Parameters:
             
             - <b>pSlWlanEvent->Event = SL_WLAN_CONNECT_EVENT </b>, STA or P2P client connection indication event
                 - pSlWlanEvent->EventData.STAandP2PModeWlanConnected main fields:
                      - ssid_name
                      - ssid_len
                      - bssid
                      - go_peer_device_name
                      - go_peer_device_name_len
                       
             - <b>pSlWlanEvent->Event = SL_WLAN_DISCONNECT_EVENT </b>, STA or P2P client disconnection event                          
                 - pSlWlanEvent->EventData.STAandP2PModeDisconnected main fields:
                      - ssid_name
                      - ssid_len
                      - reason_code

             - <b>pSlWlanEvent->Event = SL_WLAN_STA_CONNECTED_EVENT </b>, AP/P2P(Go) connected STA/P2P(Client)                  
                  - pSlWlanEvent->EventData.APModeStaConnected fields:
                      - go_peer_device_name
                      - mac
                      - go_peer_device_name_len
                      - wps_dev_password_id
                      - own_ssid:  relevant for event sta-connected only
                      - own_ssid_len:  relevant for event sta-connected only
                      
             - <b>pSlWlanEvent->Event = SL_WLAN_STA_DISCONNECTED_EVENT </b>, AP/P2P(Go) disconnected STA/P2P(Client)                        
                  - pSlWlanEvent->EventData.APModestaDisconnected fields:
                      - go_peer_device_name
                      - mac
                      - go_peer_device_name_len
                      - wps_dev_password_id
                      - own_ssid:  relevant for event sta-connected only
                      - own_ssid_len:  relevant for event sta-connected only

             - <b>pSlWlanEvent->Event = SL_WLAN_SMART_CONFIG_COMPLETE_EVENT </b>                             
                  - pSlWlanEvent->EventData.smartConfigStartResponse fields:
                     - status
                     - ssid_len
                     - ssid
                     - private_token_len
                     - private_token
                     
             - <b>pSlWlanEvent->Event = SL_WLAN_SMART_CONFIG_STOP_EVENT </b>                 
                     - pSlWlanEvent->EventData.smartConfigStopResponse fields:       
                         - status
                         
             - <b>pSlWlanEvent->Event = SL_WLAN_P2P_DEV_FOUND_EVENT </b>         
                     - pSlWlanEvent->EventData.P2PModeDevFound fields:
                         - go_peer_device_name
                         - mac
                         - go_peer_device_name_len
                         - wps_dev_password_id
                         - own_ssid:  relevant for event sta-connected only
                         - own_ssid_len:  relevant for event sta-connected only
                         
             - <b>pSlWlanEvent->Event = SL_WLAN_P2P_NEG_REQ_RECEIVED_EVENT </b>                             
                      - pSlWlanEvent->EventData.P2PModeNegReqReceived fields
                          - go_peer_device_name
                          - mac
                          - go_peer_device_name_len
                          - wps_dev_password_id
                          - own_ssid:  relevant for event sta-connected only
                           
             - <b>pSlWlanEvent->Event = SL_WLAN_CONNECTION_FAILED_EVENT </b>, P2P only
                       - pSlWlanEvent->EventData.P2PModewlanConnectionFailure fields:
                           - status                  
*/
#if (defined(sl_WlanEvtHdlr))
extern void sl_WlanEvtHdlr(SlWlanEvent_t* pSlWlanEvent);
#endif


/*!
    \brief NETAPP Async event handler
    
    \param[out]      pSlNetApp   pointer to SlNetAppEvent_t data    
    
    \par
             Parameters:
              - <b>pSlNetApp->Event = SL_NETAPP_IPV4_IPACQUIRED_EVENT</b>, IPV4 acquired event
                  - pSlNetApp->EventData.ipAcquiredV4 fields:
                       - ip
                       - gateway
                       - dns
                           
              - <b>pSlNetApp->Event = SL_NETAPP_IP_LEASED_EVENT</b>, AP or P2P go dhcp lease event
                  - pSlNetApp->EventData.ipLeased  fields:
                       - ip_address
                       - lease_time
                       - mac

              - <b>pSlNetApp->Event = SL_NETAPP_IP_RELEASED_EVENT</b>, AP or P2P go dhcp ip release event
                   - pSlNetApp->EventData.ipReleased fields
                       - ip_address
                       - mac
                       - reason

*/
#if (defined(sl_NetAppEvtHdlr))
extern void sl_NetAppEvtHdlr(SlNetAppEvent_t* pSlNetApp);
#endif

/*!
    \brief Socket Async event handler
    
    \param[out]      pSlSockEvent   pointer to SlSockEvent_t data 
    
    \par
             Parameters:\n
             - <b>pSlSockEvent->Event = SL_SOCKET_TX_FAILED_EVENT</b>
                 - pSlSockEvent->SockTxFailData fields:
                     - sd
                     - status
             - <b>pSlSockEvent->Event = SL_SOCKET_ASYNC_EVENT</b>
                - pSlSockEvent->SockAsyncData fields:
                     - sd
                     - type: SSL_ACCEPT  or RX_FRAGMENTATION_TOO_BIG or OTHER_SIDE_CLOSE_SSL_DATA_NOT_ENCRYPTED 
                     - val

*/
#if (defined(sl_SockEvtHdlr))
extern void sl_SockEvtHdlr(SlSockEvent_t* pSlSockEvent);
#endif

/*!
    \brief HTTP server async event

    \param[out] pSlHttpServerEvent   pointer to SlHttpServerEvent_t
    \param[in] pSlHttpServerResponse pointer to SlHttpServerResponse_t

    \par
          Parameters: \n

          - <b>pSlHttpServerEvent->Event = SL_NETAPP_HTTPGETTOKENVALUE_EVENT</b>
             - pSlHttpServerEvent->EventData fields:
                 - httpTokenName
                     - data
                     - len
             - pSlHttpServerResponse->ResponseData fields:
                     - data
                     - len
             
          - <b>pSlHttpServerEvent->Event = SL_NETAPP_HTTPPOSTTOKENVALUE_EVENT</b>
              - pSlHttpServerEvent->EventData.httpPostData fields:
                     - action
                     - token_name
                     - token_value                     
              - pSlHttpServerResponse->ResponseData fields:
                     - data
                     - len         
 
*/
#if (defined(sl_HttpServerCallback))
extern void sl_HttpServerCallback(SlHttpServerEvent_t *pSlHttpServerEvent, SlHttpServerResponse_t *pSlHttpServerResponse);
#endif
/*!

 Close the Doxygen group.
 @}

 */
 
#ifdef  __cplusplus
}
#endif /* __cplusplus */

#endif    /*  __SIMPLELINK_H__ */

