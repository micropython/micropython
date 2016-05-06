/*
 * user.h - CC31xx/CC32xx Host Driver Implementation
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



#ifndef __USER_H__
#define __USER_H__

#ifdef  __cplusplus
extern "C" {
#endif


#include <string.h>
#include "modnwcc3100.h"

/*!
	\def		MAX_CONCURRENT_ACTIONS

    \brief      Defines the maximum number of concurrent action in the system
				Min:1 , Max: 32

                Actions which has async events as return, can be

    \sa

    \note       In case there are not enough resources for the actions needed in the system,
	        	error is received: POOL_IS_EMPTY
			    one option is to increase MAX_CONCURRENT_ACTIONS
				(improves performance but results in memory consumption)
		     	Other option is to call the API later (decrease performance)

    \warning    In case of setting to one, recommend to use non-blocking recv\recvfrom to allow
				multiple socket recv
*/
#define MAX_CONCURRENT_ACTIONS 10
/*!
	\def		CPU_FREQ_IN_MHZ
    \brief      Defines CPU frequency for Host side, for better accuracy of busy loops, if any
    \sa
    \note

    \warning    If not set the default CPU frequency is set to 200MHz
                This option will be deprecated in future release
*/

#define CPU_FREQ_IN_MHZ        84


/*!
 ******************************************************************************

    \defgroup       configuration_capabilities        Configuration - Capabilities Set

    This section IS NOT REQUIRED in case one of the following pre defined
    capabilities set is in use:
    - SL_TINY
    - SL_SMALL
    - SL_FULL

    PORTING ACTION:
        - Define one of the pre-defined capabilities set or uncomment the
          relevant definitions below to select the required capabilities

    @{

 *******************************************************************************
*/

/*!
	\def		SL_INC_ARG_CHECK

    \brief      Defines whether the SimpleLink driver perform argument check
                or not

                When defined, the SimpleLink driver perform argument check on
                function call. Removing this define could reduce some code
                size and improve slightly the performances but may impact in
                unpredictable behavior in case of invalid arguments

    \sa

    \note       belongs to \ref configuration_sec

    \warning    Removing argument check may cause unpredictable behavior in
                case of invalid arguments.
                In this case the user is responsible to argument validity
                (for example all handlers must not be NULL)
*/
#define SL_INC_ARG_CHECK


/*!
    \def		SL_INC_STD_BSD_API_NAMING

    \brief      Defines whether SimpleLink driver should expose standard BSD
                APIs or not

                When defined, the SimpleLink driver in addtion to its alternative
                BSD APIs expose also standard BSD APIs.
                Stadrad BSD API includs the following functions:
                socket , close , accept , bind , listen	, connect , select ,
                setsockopt	, getsockopt , recv , recvfrom , write , send , sendto ,
                gethostbyname

    \sa

    \note       belongs to \ref configuration_sec

    \warning
*/
#undef SL_INC_STD_BSD_API_NAMING


/*!
    \brief      Defines whether to include extended API in SimpleLink driver
                or not

                When defined, the SimpleLink driver will include also all
                exteded API of the included packages

    \sa         ext_api

    \note       belongs to \ref configuration_sec

    \warning
*/
#define SL_INC_EXT_API

/*!
    \brief      Defines whether to include WLAN package in SimpleLink driver
                or not

                When defined, the SimpleLink driver will include also
                the WLAN package

    \sa

    \note       belongs to \ref configuration_sec

    \warning
*/
#define SL_INC_WLAN_PKG

/*!
    \brief      Defines whether to include SOCKET package in SimpleLink
                driver or not

                When defined, the SimpleLink driver will include also
                the SOCKET package

    \sa

    \note       belongs to \ref configuration_sec

    \warning
*/
#define SL_INC_SOCKET_PKG

/*!
    \brief      Defines whether to include NET_APP package in SimpleLink
                driver or not

                When defined, the SimpleLink driver will include also the
                NET_APP package

    \sa

    \note       belongs to \ref configuration_sec

    \warning
*/
#define SL_INC_NET_APP_PKG

/*!
    \brief      Defines whether to include NET_CFG package in SimpleLink
                driver or not

                When defined, the SimpleLink driver will include also
                the NET_CFG package

    \sa

    \note       belongs to \ref configuration_sec

    \warning
*/
#define SL_INC_NET_CFG_PKG

/*!
    \brief      Defines whether to include NVMEM package in SimpleLink
                driver or not

                When defined, the SimpleLink driver will include also the
                NVMEM package

    \sa

    \note       belongs to \ref configuration_sec

    \warning
*/
#define SL_INC_NVMEM_PKG

/*!
    \brief      Defines whether to include socket server side APIs
                in SimpleLink driver or not

                When defined, the SimpleLink driver will include also socket
                server side APIs

    \sa         server_side

    \note

    \warning
*/
#define SL_INC_SOCK_SERVER_SIDE_API

/*!
    \brief      Defines whether to include socket client side APIs in SimpleLink
                driver or not

                When defined, the SimpleLink driver will include also socket
                client side APIs

    \sa         client_side

    \note       belongs to \ref configuration_sec

    \warning
*/
#define SL_INC_SOCK_CLIENT_SIDE_API

/*!
    \brief      Defines whether to include socket receive APIs in SimpleLink
                driver or not

                When defined, the SimpleLink driver will include also socket
                receive side APIs

    \sa         recv_api

    \note       belongs to \ref configuration_sec

    \warning
*/
#define SL_INC_SOCK_RECV_API

/*!
    \brief      Defines whether to include socket send APIs in SimpleLink
                driver or not

                When defined, the SimpleLink driver will include also socket
                send side APIs

    \sa         send_api

    \note       belongs to \ref configuration_sec

    \warning
*/
#define SL_INC_SOCK_SEND_API

/*!

 Close the Doxygen group.
 @}

 */


/*!
 ******************************************************************************

    \defgroup   configuration_enable_device       Configuration - Device Enable/Disable

    The enable/disable API provide mechanism to enable/disable the network processor


    porting ACTION:
        - None
    @{

 ******************************************************************************
 */

/*!
    \brief		Preamble to the enabling the Network Processor.
                        Placeholder to implement any pre-process operations
                        before enabling networking operations.

    \sa			sl_DeviceEnable

    \note       belongs to \ref configuration_sec

*/
#ifndef DISABLE_DEBUGGER_RECONNECT
#define sl_DeviceEnablePreamble()		NwpPowerOnPreamble()
#else
#define sl_DeviceEnablePreamble()
#endif

/*!
    \brief		Enable the Network Processor

    \sa			sl_DeviceDisable

    \note       belongs to \ref configuration_sec

*/
#define sl_DeviceEnable()			NwpPowerOn()

/*!
    \brief		Disable the Network Processor

    \sa			sl_DeviceEnable

    \note       belongs to \ref configuration_sec
*/
#define sl_DeviceDisable() 			NwpPowerOff()

/*!

 Close the Doxygen group.
 @}

 */

/*!
 ******************************************************************************

    \defgroup   configuration_interface         Configuration - Communication Interface

	The SimpleLink device supports several standard communication protocol among SPI and
	UART. CC32XX Host Driver implements SPI Communication Interface


    \note       	In CC32XX, SPI implementation uses DMA in order to increase the utilization
 			of the communication channel. If user prefers to user UART, these interfaces
 			need to be redefined


    porting ACTION:
        - None

    @{

 ******************************************************************************
*/

#define _SlFd_t					Fd_t

/*!
    \brief      Opens an interface communication port to be used for communicating
                with a SimpleLink device

	            Given an interface name and option flags, this function opens
                the communication port and creates a file descriptor.
                This file descriptor is used afterwards to read and write
                data from and to this specific communication channel.
	            The speed, clock polarity, clock phase, chip select and all other
                specific attributes of the channel are all should be set to hardcoded
                in this function.

	\param	 	ifName  -   points to the interface name/path. The interface name is an
                            optional attributes that the simple link driver receives
                            on opening the driver (sl_Start).
                            In systems that the spi channel is not implemented as
                            part of the os device drivers, this parameter could be NULL.

	\param      flags   -   optional flags parameters for future use

	\return     upon successful completion, the function shall open the channel
                and return a non-negative integer representing the file descriptor.
                Otherwise, -1 shall be returned

    \sa         sl_IfClose , sl_IfRead , sl_IfWrite

	\note       The prototype of the function is as follow:
                    Fd_t xxx_IfOpen(char* pIfName , unsigned long flags);

    \note       belongs to \ref configuration_sec

    \warning
*/
#define sl_IfOpen                           spi_Open

/*!
    \brief      Closes an opened interface communication port

	\param	 	fd  -   file descriptor of opened communication channel

	\return		upon successful completion, the function shall return 0.
			    Otherwise, -1 shall be returned

    \sa         sl_IfOpen , sl_IfRead , sl_IfWrite

	\note       The prototype of the function is as follow:
                    int xxx_IfClose(Fd_t Fd);

    \note       belongs to \ref configuration_sec

    \warning
*/
#define sl_IfClose                          spi_Close

/*!
    \brief      Attempts to read up to len bytes from an opened communication channel
                into a buffer starting at pBuff.

	\param	 	fd      -   file descriptor of an opened communication channel

	\param		pBuff   -   pointer to the first location of a buffer that contains enough
                            space for all expected data

	\param      len     -   number of bytes to read from the communication channel

	\return     upon successful completion, the function shall return the number of read bytes.
                Otherwise, 0 shall be returned

    \sa         sl_IfClose , sl_IfOpen , sl_IfWrite


	\note       The prototype of the function is as follow:
                    int xxx_IfRead(Fd_t Fd , char* pBuff , int Len);

    \note       belongs to \ref configuration_sec

    \warning
*/
#define sl_IfRead                           spi_Read

/*!
    \brief attempts to write up to len bytes to the SPI channel

	\param	 	fd      -   file descriptor of an opened communication channel

	\param		pBuff   -   pointer to the first location of a buffer that contains
                            the data to send over the communication channel

	\param      len     -   number of bytes to write to the communication channel

	\return     upon successful completion, the function shall return the number of sent bytes.
				therwise, 0 shall be returned

    \sa         sl_IfClose , sl_IfOpen , sl_IfRead

	\note       This function could be implemented as zero copy and return only upon successful completion
                of writing the whole buffer, but in cases that memory allocation is not too tight, the
                function could copy the data to internal buffer, return back and complete the write in
                parallel to other activities as long as the other SPI activities would be blocked until
                the entire buffer write would be completed

               The prototype of the function is as follow:
                    int xxx_IfWrite(Fd_t Fd , char* pBuff , int Len);

    \note       belongs to \ref configuration_sec

    \warning
*/
#define sl_IfWrite                          spi_Write

/*!
    \brief 		register an interrupt handler routine for the host IRQ

	\param	 	InterruptHdl	-	pointer to interrupt handler routine

	\param 		pValue			-	pointer to a memory structure that is passed
									to the interrupt handler.

	\return		upon successful registration, the function shall return 0.
				Otherwise, -1 shall be returned

    \sa

	\note		If there is already registered interrupt handler, the function
				should overwrite the old handler with the new one

	\note       If the handler is a null pointer, the function should un-register the
	            interrupt handler, and the interrupts can be disabled.

    \note       belongs to \ref configuration_sec

    \warning
*/
#define sl_IfRegIntHdlr(InterruptHdl , pValue)          NwpRegisterInterruptHandler(InterruptHdl , pValue)

/*!
    \brief 		Masks the Host IRQ

    \sa		sl_IfUnMaskIntHdlr



    \note       belongs to \ref configuration_sec

    \warning
*/


#define sl_IfMaskIntHdlr()								NwpMaskInterrupt()

/*!
    \brief 		Unmasks the Host IRQ

    \sa		sl_IfMaskIntHdlr



    \note       belongs to \ref configuration_sec

    \warning
*/

#define sl_IfUnMaskIntHdlr()								NwpUnMaskInterrupt()

/*!
    \brief 		Write Handers for statistics debug on write

	\param	 	interface handler	-	pointer to interrupt handler routine


	\return		no return value

    \sa

	\note		An optional hooks for monitoring before and after write info

    \note       belongs to \ref configuration_sec

    \warning
*/
/* #define SL_START_WRITE_STAT */

/*!

 Close the Doxygen group.
 @}

*/



/*!
 ******************************************************************************

    \defgroup   configuration_os          Configuration - Operating System

	The SimpleLink driver could run on two kind of platforms:
	   -# Non-Os / Single Threaded (default)
	   -# Multi-Threaded

	CC32XX SimpleLink Host Driver is ported on both Non-Os and Multi Threaded OS enviroment.
	The Host driver is made OS independent by implementing an OS Abstraction layer.
	Reference implementation for OS Abstraction is available for FreeRTOS and TI-RTOS.


	If you choose to work in multi-threaded environment under different operating system you
	will have to provide some basic adaptation routines to allow the driver to protect access to
	resources for different threads (locking object) and to allow synchronization between threads
	(sync objects). In additional the driver support running without dedicated thread allocated solely
	to the simple link driver. If you choose to work in this mode, you should also supply a spawn
	method that will enable to run function on a temporary context.

	\note - This Macro is defined in the IDE to generate Driver for both OS and Non-OS

	 porting ACTION:
		 - None

	 @{

 ******************************************************************************
*/

/*
#define SL_PLATFORM_MULTI_THREADED
*/

#ifdef SL_PLATFORM_MULTI_THREADED
#include "osi.h"


/*!
    \brief
    \sa
    \note           belongs to \ref configuration_sec
    \warning
*/
#define SL_OS_RET_CODE_OK                       ((int)OSI_OK)

/*!
    \brief
    \sa
    \note           belongs to \ref configuration_sec
    \warning
*/
#define SL_OS_WAIT_FOREVER                      ((OsiTime_t)OSI_WAIT_FOREVER)

/*!
    \brief
    \sa
    \note           belongs to \ref configuration_sec
    \warning
*/
#define SL_OS_NO_WAIT	                        ((OsiTime_t)OSI_NO_WAIT)

/*!
	\brief type definition for a time value

	\note	On each configuration or platform the type could be whatever is needed - integer, pointer to structure etc.

    \note       belongs to \ref configuration_sec
*/
#define _SlTime_t				OsiTime_t

/*!
	\brief 	type definition for a sync object container

	Sync object is object used to synchronize between two threads or thread and interrupt handler.
	One thread is waiting on the object and the other thread send a signal, which then
	release the waiting thread.
	The signal must be able to be sent from interrupt context.
	This object is generally implemented by binary semaphore or events.

	\note	On each configuration or platform the type could be whatever is needed - integer, structure etc.

    \note       belongs to \ref configuration_sec
*/
typedef OsiSyncObj_t                            _SlSyncObj_t;


/*!
	\brief 	This function creates a sync object

	The sync object is used for synchronization between diffrent thread or ISR and
	a thread.

	\param	pSyncObj	-	pointer to the sync object control block

	\return upon successful creation the function should return 0
			Otherwise, a negative value indicating the error code shall be returned

    \note       belongs to \ref configuration_sec
    \warning
*/
#define sl_SyncObjCreate(pSyncObj,pName)            osi_SyncObjCreate(pSyncObj)


/*!
	\brief 	This function deletes a sync object

	\param	pSyncObj	-	pointer to the sync object control block

	\return upon successful deletion the function should return 0
			Otherwise, a negative value indicating the error code shall be returned
    \note       belongs to \ref configuration_sec
    \warning
*/
#define sl_SyncObjDelete(pSyncObj)                  osi_SyncObjDelete(pSyncObj)


/*!
	\brief 		This function generates a sync signal for the object.

	All suspended threads waiting on this sync object are resumed

	\param		pSyncObj	-	pointer to the sync object control block

	\return 	upon successful signaling the function should return 0
				Otherwise, a negative value indicating the error code shall be returned
	\note		the function could be called from ISR context
    \warning
*/
#define sl_SyncObjSignal(pSyncObj)                osi_SyncObjSignal(pSyncObj)

/*!
	\brief 		This function generates a sync signal for the object from Interrupt

	This is for RTOS that should signal from IRQ using a dedicated API

	\param		pSyncObj	-	pointer to the sync object control block

	\return 	upon successful signaling the function should return 0
				Otherwise, a negative value indicating the error code shall be returned
	\note		the function could be called from ISR context
	\warning
*/
#define sl_SyncObjSignalFromIRQ(pSyncObj)           osi_SyncObjSignalFromISR(pSyncObj)

/*!
	\brief 	This function waits for a sync signal of the specific sync object

	\param	pSyncObj	-	pointer to the sync object control block
	\param	Timeout		-	numeric value specifies the maximum number of mSec to
							stay suspended while waiting for the sync signal
							Currently, the simple link driver uses only two values:
								- OSI_WAIT_FOREVER
								- OSI_NO_WAIT

	\return upon successful reception of the signal within the timeout window return 0
			Otherwise, a negative value indicating the error code shall be returned
    \note       belongs to \ref configuration_sec
    \warning
*/
#define sl_SyncObjWait(pSyncObj,Timeout)            osi_SyncObjWait(pSyncObj,Timeout)

/*!
	\brief 	type definition for a locking object container

	Locking object are used to protect a resource from mutual accesses of two or more threads.
	The locking object should suppurt reentrant locks by a signal thread.
	This object is generally implemented by mutex semaphore

	\note	On each configuration or platform the type could be whatever is needed - integer, structure etc.
    \note       belongs to \ref configuration_sec
*/
typedef OsiLockObj_t                            _SlLockObj_t;

/*!
	\brief 	This function creates a locking object.

	The locking object is used for protecting a shared resources between different
	threads.

	\param	pLockObj	-	pointer to the locking object control block

	\return upon successful creation the function should return 0
			Otherwise, a negative value indicating the error code shall be returned
    \note       belongs to \ref configuration_sec
    \warning
*/
#define sl_LockObjCreate(pLockObj,pName)            osi_LockObjCreate(pLockObj)

/*!
	\brief 	This function deletes a locking object.

	\param	pLockObj	-	pointer to the locking object control block

	\return upon successful deletion the function should return 0
			Otherwise, a negative value indicating the error code shall be returned
    \note       belongs to \ref configuration_sec
    \warning
*/
#define sl_LockObjDelete(pLockObj)                  osi_LockObjDelete(pLockObj)

/*!
	\brief 	This function locks a locking object.

	All other threads that call this function before this thread calls
	the osi_LockObjUnlock would be suspended

	\param	pLockObj	-	pointer to the locking object control block
	\param	Timeout		-	numeric value specifies the maximum number of mSec to
							stay suspended while waiting for the locking object
							Currently, the simple link driver uses only two values:
								- OSI_WAIT_FOREVER
								- OSI_NO_WAIT


	\return upon successful reception of the locking object the function should return 0
			Otherwise, a negative value indicating the error code shall be returned
    \note       belongs to \ref configuration_sec
    \warning
*/
#define sl_LockObjLock(pLockObj,Timeout)           osi_LockObjLock(pLockObj,Timeout)

/*!
	\brief 	This function unlock a locking object.

	\param	pLockObj	-	pointer to the locking object control block

	\return upon successful unlocking the function should return 0
			Otherwise, a negative value indicating the error code shall be returned
    \note       belongs to \ref configuration_sec
    \warning
*/
#define sl_LockObjUnlock(pLockObj)                   osi_LockObjUnlock(pLockObj)
#else
#ifdef NON_OS_PM
#define _SlSyncWaitLoopCallback 		             SimpleLinkSyncWaitLoopCallback
#endif
#endif
/*!
	\brief 	This function call the pEntry callback from a different context

	\param	pEntry		-	pointer to the entry callback function

	\param	pValue		- 	pointer to any type of memory structure that would be
							passed to pEntry callback from the execution thread.

	\param	flags		- 	execution flags - reserved for future usage

	\return upon successful registration of the spawn the function should return 0
			(the function is not blocked till the end of the execution of the function
			and could be returned before the execution is actually completed)
			Otherwise, a negative value indicating the error code shall be returned
    \note       belongs to \ref configuration_sec
    \warning
*/
//#define SL_PLATFORM_EXTERNAL_SPAWN

#ifdef SL_PLATFORM_EXTERNAL_SPAWN
#define sl_Spawn(pEntry,pValue,flags)       osi_Spawn(pEntry,pValue,flags)
#endif

/*!

 Close the Doxygen group.
 @}

 */
/*!
 ******************************************************************************

    \defgroup   configuration_mem_mgm             Configuration - Memory Management

    This section declare in which memory management model the SimpleLink driver
    will run:
        -# Static
        -# Dynamic

    This section IS NOT REQUIRED in case Static model is selected.

    The default memory model is Static


    @{

 *****************************************************************************
*/

/*!
    \brief      Defines whether the SimpleLink driver is working in dynamic
                memory model or not

                When defined, the SimpleLink driver use dynamic allocations
                if dynamic allocation is selected malloc and free functions
                must be retrieved

    \sa

    \note       belongs to \ref configuration_sec

    \warning
*/

#define SL_MEMORY_MGMT_DYNAMIC 	1
#define SL_MEMORY_MGMT_STATIC  0

#define SL_MEMORY_MGMT  SL_MEMORY_MGMT_DYNAMIC

#ifdef SL_MEMORY_MGMT_DYNAMIC

#ifdef SL_PLATFORM_MULTI_THREADED

/*!
    \brief
    \sa
    \note           belongs to \ref configuration_sec
    \warning
*/
#define sl_Malloc(Size)                                 mem_Malloc(Size)

/*!
    \brief
    \sa
    \note           belongs to \ref configuration_sec
    \warning
*/
#define sl_Free(pMem)                                   mem_Free(pMem)

#else
#include <stdlib.h>
/*!
    \brief
    \sa
    \note           belongs to \ref configuration_sec
    \warning
*/
#define sl_Malloc(Size)                                 malloc(Size)

/*!
    \brief
    \sa
    \note           belongs to \ref configuration_sec
    \warning
*/
#define sl_Free(pMem)                                   free(pMem)
#endif

#endif

/*!

 Close the Doxygen group.
 @}

*/

/*!
 ******************************************************************************

    \defgroup       configuration_events      Configuration - Event Handlers

    This section includes the asynchronous event handlers routines

    porting ACTION:
        -define your routine as the value of this handler

    @{

 ******************************************************************************
 */

/*!
    \brief General async event for inspecting general events

    \param[out]      pSlDeviceEvent   pointer to SlDeviceEvent_t

    \par
          Parameters: \n
          <b>pSlDeviceEvent->Event = SL_DEVICE_FATAL_ERROR_EVENT </b>
              - pSlDeviceEvent->EventData.deviceEvent fields:
                  - status: An error code indication from the device
                  - sender: The sender originator which is based on SlErrorSender_e enum

    \par  Example:
    \code
    printf(General Event Handler - ID=%d Sender=%d\n\n",
           pSlDeviceEvent->EventData.deviceEvent.status,  // status of the general event
           pSlDeviceEvent->EventData.deviceEvent.sender); // sender type
    \endcode


    \sa

    \note       belongs to \ref configuration_sec

    \warning
*/


#define sl_GeneralEvtHdlr SimpleLinkGeneralEventHandler


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

    \sa

    \note           belongs to \ref configuration_sec

    \warning
*/

#define sl_WlanEvtHdlr                     SimpleLinkWlanEventHandler


/*!
    \brief NETAPP Async event handler

    \param[out]      pSlNetApp   pointer to SlNetAppEvent_t data

    \par
             Parameters:
              - <b>pSlWlanEvent->Event = SL_NETAPP_IPV4_IPACQUIRED_EVENT</b>, IPV4 acquired event
                  - pSlWlanEvent->EventData.ipAcquiredV4 fields:
                       - ip
                       - gateway
                       - dns

              - <b>pSlWlanEvent->Event = SL_NETAPP_IP_LEASED_EVENT</b>, AP or P2P go dhcp lease event
                  - pSlWlanEvent->EventData.ipLeased  fields:
                       - ip_address
                       - lease_time
                       - mac

              - <b>pSlWlanEvent->Event = SL_NETAPP_IP_RELEASED_EVENT</b>, AP or P2P go dhcp ip release event
                   - pSlWlanEvent->EventData.ipReleased fields
                       - ip_address
                       - mac
                       - reason


    \sa

    \note           belongs to \ref configuration_sec

    \warning
*/

#define sl_NetAppEvtHdlr              		SimpleLinkNetAppEventHandler

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


    \sa

    \note           belongs to \ref configuration_sec

    \warning
*/

#define sl_HttpServerCallback   SimpleLinkHttpServerCallback
/*!
    \brief Socket Async event handler

    \param[out]      pSlSockEvent   pointer to SlSockEvent_t data

    \par
             Parameters:\n
             - <b>pSlSockEvent->Event = SL_SOCKET_TX_FAILED_EVENT</b>
                 - pSlSockEvent->EventData fields:
                     - sd
                     - status
             - <b>pSlSockEvent->Event = SL_SOCKET_ASYNC_EVENT</b>
                - pSlSockEvent->EventData fields:
                     - sd
                     - type: SSL_ACCEPT  or RX_FRAGMENTATION_TOO_BIG or OTHER_SIDE_CLOSE_SSL_DATA_NOT_ENCRYPTED
                     - val

    \sa

    \note           belongs to \ref configuration_sec

    \warning
*/

#define sl_SockEvtHdlr         SimpleLinkSockEventHandler


/*!

 Close the Doxygen group.
 @}

 */


#ifdef  __cplusplus
}
#endif // __cplusplus

#endif // __USER_H__
